/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "canvas/IDrawContext.h"

#include <QtWidgets>

#define N_DEFAULT_ZOOM_LEVELS 31
const qreal IDrawContext::scales[N_DEFAULT_ZOOM_LEVELS] =
{
    0.10,  0.15,  0.20,  0.30,   0.50,   0.70,   1.0,    1.5,    2.0,    3.0,
    5.0,   7.0,  10.0,  15.0,   20.0,   30.0,   50.0,   70.0,  100.0,  150.0,
    200.0, 300.0, 500.0, 700.0, 1000.0, 1500.0, 2000.0, 3000.0, 5000.0, 7000.0,
    10000.0
    //, 15000.0, 20000.0, 30000.0, 50000.0, 70000.0
};

QPointF operator*(const QPointF& p1, const QPointF& p2)
{
    return QPointF(p1.x() * p2.x(), p1.y() * p2.y());
}

QPointF operator/(const QPointF& p1, const QPointF& p2)
{
    return QPointF(p1.x() / p2.x(), p1.y() / p2.y());
}

QMutex IDrawContext::mutex(QMutex::Recursive);


IDrawContext::IDrawContext(CCanvas * canvas, QObject * parent)
    : QThread(parent)
    , canvas(canvas)
{
    init();
}

void IDrawContext::init()
{
    connect(this, &IDrawContext::finished, canvas, static_cast<void (CCanvas::*)()>(&CCanvas::update));
    connect(this, &IDrawContext::started, canvas, &CCanvas::slotShowLoadIndicator);
    connect(this, &IDrawContext::finished, canvas, &CCanvas::slotHideLoadIndicator);
    connect(canvas, &CCanvas::sigChangedSize, this, &IDrawContext::slotResize);

    slotResize(canvas->size());
}


void IDrawContext::saveSettings(QSettings& cfg) const
{
    QMutexLocker lock(&mutex);

    cfg.beginGroup("DrawContext");
    cfg.setValue("focus", focus);
    cfg.setValue("zoomFactorIdx", zoomFactorIdx);
    cfg.endGroup();
}

void IDrawContext::loadSettings(QSettings& cfg)
{
    QMutexLocker lock(&mutex);

    cfg.beginGroup("DrawContext");
    focus = cfg.value("focus", focus).toPointF();
    zoomFactorIdx = cfg.value("zoomFactorIdx", zoomFactorIdx).toInt();
    cfg.endGroup();

    zoom(zoomFactorIdx);
}


void IDrawContext::slotResize(const QSize& size)
{
    if(isRunning())
    {
        wait();
    }

    QMutexLocker lock(&mutex);
    viewWidth  = size.width();
    viewHeight = size.height();

    bufWidth   = viewWidth;
    bufHeight  = viewHeight;

    buffer[0].image = QImage(qRound(bufWidth), qRound(bufHeight), QImage::Format_ARGB32);
    buffer[1].image = QImage(qRound(bufWidth), qRound(bufHeight), QImage::Format_ARGB32);
}

bool IDrawContext::needsRedraw() const
{
    mutex.lock(); // --------- start serialize with thread
    bool res = intNeedsRedraw;
    mutex.unlock(); // --------- stop serialize with thread
    return res;
}

void IDrawContext::convertScreen2Map(QPointF& pt) const
{
    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertCoord2Map(f);

    pt = f + pt * scale * zoomFactor;

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertMap2Screen(QPointF& pt) const
{
    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertCoord2Map(f);

    pt = (pt - f) / (scale * zoomFactor);

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertMap2Screen(QPolygonF& line) const
{
    mutex.lock(); // --------- start serialize with thread

    const int N = line.size();
    for(int n = 0; n < N; n++)
    {
        line[n] = (line[n] - focus) / (scale * zoomFactor);
    }

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertMap2Screen(QRectF& rect) const
{
    mutex.lock(); // --------- start serialize with thread

    QPointF topLeft = rect.topLeft();
    convertMap2Screen(topLeft);
    rect.setTopLeft(topLeft);

    QPointF bottomRight = rect.bottomRight();
    convertMap2Screen(bottomRight);
    rect.setBottomRight(bottomRight);

    mutex.unlock(); // --------- stop serialize with thread
}


void IDrawContext::move(const QPointF& delta)
{
    mutex.lock(); // --------- start serialize with thread
    QPointF f = focus;

    convertCoord2Map(f);
    convertMap2Screen(f);
    f -= delta;
    convertScreen2Map(f);
    convertMap2Coord(f);

    focus = f;
    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::zoom(bool in, const QPointF& pt)
{
    mutex.lock(); // --------- start serialize with thread

    QPointF pt2 = pt;

    convertScreen2Map(pt2);
    zoom(zoomFactorIdx + (in ? -1 : 1));
    convertMap2Screen(pt2);

    move(pt - pt2);

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::zoom(int idx)
{
    idx = qMax(idx, 0);
    idx = qMin(idx, N_DEFAULT_ZOOM_LEVELS - 1);

    mutex.lock(); // --------- start serialize with thread
    if((zoomFactorIdx != idx) || (zoomFactor.x() != scales[idx]))
    {
        zoomFactorIdx   = idx;
        zoomFactor.rx() = scales[idx];
        zoomFactor.ry() = scales[idx];
        intNeedsRedraw  = true;
    }
    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::draw(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    mutex.lock(); // --------- start serialize with thread
    // derive map reference points for all corners coordinate of map buffer
    ref1 = QPointF( 0,         0);
    ref2 = QPointF( bufWidth,  0);
    ref3 = QPointF( bufWidth,  bufHeight);
    ref4 = QPointF( 0,         bufHeight);

    convertScreen2Map(ref1);
    convertScreen2Map(ref2);
    convertScreen2Map(ref3);
    convertScreen2Map(ref4);

    convertMap2Coord(ref1);
    convertMap2Coord(ref2);
    convertMap2Coord(ref3);
    convertMap2Coord(ref4);

    // get current active buffer
    const buffer_t& currentBuffer   = buffer[bufIndex];

    // calculate screen offset of current buffer
    QPointF offset = currentBuffer.ref1;
    convertCoord2Map(offset);
    convertMap2Screen(offset);

    p.save();
    // add offset
    p.translate(offset);
    // scale image if current zoomfactor does not match buffer's zoomfactor
    p.scale(currentBuffer.zoomFactor.x() / zoomFactor.x(), currentBuffer.zoomFactor.y() / zoomFactor.y());
    // draw buffer to painter
    p.drawImage(0, 0, currentBuffer.image);
    p.restore();

    emit sigDraw(p);

    // intNeedsRedraw is reset by the thread
    if(needsRedraw & maskRedraw)
    {
        intNeedsRedraw = true;
    }
    mutex.unlock(); // --------- stop serialize with thread

    if((needsRedraw & maskRedraw) && !isRunning())
    {
        start();
    }
}


void IDrawContext::run()
{
    mutex.lock();

    QTime t;
    t.start();
    qDebug() << "start thread" << objectName();

    IDrawContext::buffer_t& currentBuffer = buffer[!bufIndex];
    while(intNeedsRedraw)
    {
        // copy all projection information need by the
        // map render objects to buffer structure
        currentBuffer.zoomFactor = zoomFactor;
        currentBuffer.scale      = scale;
        currentBuffer.ref1       = ref1;
        currentBuffer.ref2       = ref2;
        currentBuffer.ref3       = ref3;
        currentBuffer.ref4       = ref4;
        currentBuffer.focus      = focus;
        intNeedsRedraw           = false;

        mutex.unlock();
        // ----- reset buffer -----
        currentBuffer.image.fill(Qt::transparent);

        drawt(currentBuffer);

        mutex.lock();
    }
    // ----- switch buffer ------
    bufIndex = !bufIndex;
    qDebug() << "stop thread" << objectName() << "after" << t.elapsed() << "ms";

    mutex.unlock();
}

