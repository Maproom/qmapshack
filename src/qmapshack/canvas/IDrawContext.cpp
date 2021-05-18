/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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


#define BUFFER_BORDER 50


#define N_DEFAULT_ZOOM_LEVELS 31
const qreal IDrawContext::scalesDefault[N_DEFAULT_ZOOM_LEVELS] =
{
    0.10, 0.15, 0.20, 0.30, 0.50, 0.70, 1.0, 1.5, 2.0, 3.0,
    5.0, 7.0, 10.0, 15.0, 20.0, 30.0, 50.0, 70.0, 100.0, 150.0,
    200.0, 300.0, 500.0, 700.0, 1000.0, 1500.0, 2000.0, 3000.0, 5000.0, 7000.0,
    10000.0
    //, 15000.0, 20000.0, 30000.0, 50000.0, 70000.0
};

#define N_SQUARE_ZOOM_LEVELS 17
const qreal IDrawContext::scalesSquare[N_SQUARE_ZOOM_LEVELS] =
{
    0.1492910709, 0.2985821417, 0.5971642834, 1.1943285668,
    2.3886571336, 4.7773142672, 9.5546285344, 19.1092570688,
    38.2185141376, 76.4370282752, 152.8740565504, 305.7481131008,
    611.4962262016, 1222.9924524032, 2445.9849048064, 4891.9698096128,
    9783.9396192256
};

QPointF operator*(const QPointF& p1, const QPointF& p2)
{
    return QPointF(p1.x() * p2.x(), p1.y() * p2.y());
}

QPointF operator/(const QPointF& p1, const QPointF& p2)
{
    return QPointF(p1.x() / p2.x(), p1.y() / p2.y());
}


IDrawContext::IDrawContext(const QString& name, CCanvas::redraw_e maskRedraw, CCanvas* parent)
    : QThread(parent)
    , canvas(parent)
    , maskRedraw(maskRedraw)
{
    setObjectName(name);

    IDrawContext::setScales(CCanvas::eScalesDefault);

    zoom(5);

    resize(canvas->size());
    connect(this, &IDrawContext::finished, canvas, static_cast<void (CCanvas::*)()>(&CCanvas::update));
    connect(this, &IDrawContext::finished, this, &IDrawContext::sigStopThread);
}

IDrawContext::~IDrawContext()
{
}

void IDrawContext::emitSigCanvasUpdate()
{
    emit sigCanvasUpdate(maskRedraw);
}


bool IDrawContext::resize(const QSize& size)
{
    if(lastSize == size)
    {
        // nothing to do
        return true;
    }

    if(isRunning() && !wait(100))
    {
        // blocked by thread, reschedule
        return false;
    }

    QMutexLocker lock(&mutex);

    lastSize = size;
    viewWidth = size.width();
    viewHeight = size.height();

    center = QPointF(viewWidth / 2.0, viewHeight / 2.0);
    bufWidth = viewWidth + 2 * BUFFER_BORDER;
    bufHeight = viewHeight + 2 * BUFFER_BORDER;

    buffer[0].image = QImage(bufWidth, bufHeight, QImage::Format_ARGB32);
    buffer[0].image.fill(Qt::transparent);

    buffer[1].image = QImage(bufWidth, bufHeight, QImage::Format_ARGB32);
    buffer[1].image.fill(Qt::transparent);

    return true;
}

QString IDrawContext::getProjection() const
{
    return proj.getProjSrc();
}

bool IDrawContext::setProjection(const QString& projStr)
{
    proj.init(projStr.toLatin1(), "EPSG:4326");
    return proj.isValid();
}

void IDrawContext::setScales(const CCanvas::scales_type_e type)
{
    switch (type)
    {
    case CCanvas::eScalesDefault:
        scales = scalesDefault;
        zoomLevels = N_DEFAULT_ZOOM_LEVELS;
        scalesType = type;
        break;

    case CCanvas::eScalesSquare:
        scales = scalesSquare;
        zoomLevels = N_SQUARE_ZOOM_LEVELS;
        scalesType = type;
        break;

    default:
        qDebug() << "Invalid type of scales table" << scalesType;
    }
}

bool IDrawContext::needsRedraw() const
{
    mutex.lock();
    bool res = intNeedsRedraw;
    mutex.unlock();
    return res;
}

void IDrawContext::zoom(const QRectF& rect)
{
    if(!proj.isValid())
    {
        return;
    }

    // special case for elements with no extent
    if(rect.width() == 0 || rect.height() == 0)
    {
        zoom(scalesType == CCanvas::eScalesDefault ? 8 : 4);
        return;
    }

    // zoom out from closest zoom level until a match is found
    for(int i = 0; i < zoomLevels; i++)
    {
        zoom(i);
        QPointF pt1 = rect.topLeft();
        QPointF pt2 = rect.bottomRight();

        convertRad2Px(pt1);
        convertRad2Px(pt2);

        QPointF pt = pt2 - pt1;
        if(qAbs(pt.x()) < (bufWidth - 2 * BUFFER_BORDER) && (qAbs(pt.y()) < (bufHeight - 2 * BUFFER_BORDER)))
        {
            break;
        }
    }
}

void IDrawContext::zoom(bool in, CCanvas::redraw_e& needsRedraw)
{
    if(!proj.isValid())
    {
        return;
    }
    zoom(zoomIndex + (in ? -1 : 1));
    needsRedraw = CCanvas::eRedrawAll;
}

void IDrawContext::zoom(int idx)
{
    idx = qMax(idx, 0);
    idx = qMin(idx, zoomLevels - 1);

    mutex.lock(); // --------- start serialize with thread
    if((zoomIndex != idx) || (zoomFactor.x() != scales[idx]))
    {
        zoomIndex = idx;
        zoomFactor.rx() = scales[idx];
        zoomFactor.ry() = scales[idx];
        intNeedsRedraw = true;

        emit sigScaleChanged(scale* zoomFactor);
    }
    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertRad2M(QPointF& p) const
{
    if(!proj.isValid())
    {
        return;
    }

    qreal y = p.y();
    /*
        Proj4 makes a wrap around for values outside the
        range of -180..180°. But the draw context has no
        turnaround. It exceeds the values. We have to
        apply fixes in that case.
     */
    bool fixWest = p.x() < (-180 * DEG_TO_RAD);
    bool fixEast = p.x() > ( 180 * DEG_TO_RAD);

    proj.transform(p, PJ_INV);

    /*
        The idea of the fix is to calculate a point
        at the boundary with the same latitude and use it
        as offset.
     */
    if(fixWest)
    {
        QPointF o(-180 * DEG_TO_RAD, y);
        convertRad2M(o);
        p.rx() = 2 * o.x() + p.x();
    }

    if(fixEast)
    {
        QPointF o(180 * DEG_TO_RAD, y);
        convertRad2M(o);
        p.rx() = 2 * o.x() + p.x();
    }
}

void IDrawContext::convertM2Rad(QPointF& p) const
{
    if(!proj.isValid())
    {
        return;
    }

    proj.transform(p, PJ_FWD);
}

void IDrawContext::convertPx2Rad(QPointF& p) const
{
    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertRad2M(f);

    p = f + (p - center) * scale * zoomFactor;

    convertM2Rad(p);

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertRad2Px(QPointF& p) const
{
    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertRad2M(f);
    convertRad2M(p);

    p = (p - f) / (scale * zoomFactor) + center;

    mutex.unlock(); // --------- stop serialize with thread
}


void IDrawContext::convertRad2Px(QPolygonF& poly) const
{
    if(!proj.isValid())
    {
        return;
    }

    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertRad2M(f);

    const int N = poly.size();

    struct p_t
    {
        qreal fixWest;
        qreal fixEast;
    };

    QVector<p_t> fixes(N, {NOFLOAT, NOFLOAT});

    qreal* pY = &poly.data()->ry();
    p_t* pFix = fixes.data();

    /*
        Proj4 makes a wrap around for values outside the
        range of -180..180°. But the draw context has no
        turnaround. It exceeds the values. We have to
        apply fixes in that case.
     */
    for(int i = 0; i < N; ++i, ++pFix, pY += 2)
    {
        if(*pY < (-180 * DEG_TO_RAD))
        {
            pFix->fixWest = *pY;
        }
        if(*pY > ( 180 * DEG_TO_RAD))
        {
            pFix->fixEast = *pY;
        }
    }

    proj.transform(poly, PJ_INV);

    QPointF* pPt = poly.data();
    pFix = fixes.data();
    for(int i = 0; i < N; ++i, ++pFix, ++pPt)
    {
        /*
            The idea of the fix is to calculate a point
            at the boundary with the same latitude and use it
            as offset.
         */
        if(pFix->fixWest != NOFLOAT)
        {
            QPointF o(-180 * DEG_TO_RAD, pFix->fixWest);
            convertRad2M(o);
            pPt->rx() = 2 * o.x() + pPt->x();
        }
        if(pFix->fixEast != NOFLOAT)
        {
            QPointF o(180 * DEG_TO_RAD, pFix->fixEast);
            convertRad2M(o);
            pPt->rx() = 2 * o.x() + pPt->x();
        }

        *pPt = (*pPt - f) / (scale * zoomFactor) + center;
    }

    mutex.unlock(); // --------- stop serialize with thread
}


void IDrawContext::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QPointF& f)
{
    if(!proj.isValid())
    {
        return;
    }

    // convert global coordinate of focus into point of map
    focus = f;

    QPointF f1 = focus;
    convertRad2M(f1);

    QPointF bufferScale = scale * zoomFactor;

    mutex.lock(); // --------- start serialize with thread

    // derive references for all corners coordinate of map buffer
    ref1 = f1 + QPointF(-bufWidth / 2, -bufHeight / 2) * bufferScale;
    ref2 = f1 + QPointF( bufWidth / 2, -bufHeight / 2) * bufferScale;
    ref3 = f1 + QPointF( bufWidth / 2, bufHeight / 2) * bufferScale;
    ref4 = f1 + QPointF(-bufWidth / 2, bufHeight / 2) * bufferScale;
    convertM2Rad(ref1);
    convertM2Rad(ref2);
    convertM2Rad(ref3);
    convertM2Rad(ref4);

    // adjust west <-> east boundaries
    if(ref1.x() > ref2.x())
    {
        if(qAbs(ref1.x()) > qAbs(ref2.x()))
        {
            ref1.rx() = -2 * (180 * DEG_TO_RAD) + ref1.rx();
        }
        if(qAbs(ref4.x()) > qAbs(ref3.x()))
        {
            ref4.rx() = -2 * (180 * DEG_TO_RAD) + ref4.rx();
        }

        if(qAbs(ref1.x()) < qAbs(ref2.x()))
        {
            ref2.rx() = 2 * (180 * DEG_TO_RAD) + ref2.rx();
        }
        if(qAbs(ref4.x()) < qAbs(ref3.x()))
        {
            ref3.rx() = 2 * (180 * DEG_TO_RAD) + ref3.rx();
        }
    }

//    qDebug() << (ref1 * RAD_TO_DEG) << (ref2 * RAD_TO_DEG) << (ref3 * RAD_TO_DEG) << (ref4 * RAD_TO_DEG);

    // get current active buffer
    buffer_t& currentBuffer = buffer[bufIndex];

    // convert buffers top left reference point to local coordinate system
    QPointF ref = currentBuffer.ref1;
    convertRad2M(ref);

    // derive offset to show coordinate of focus right in the middle of the draw
    // context. NOTE: the draw context's coordinate system has been moved into the
    // middle of the view port.
    QPointF off = (ref - f1) / (currentBuffer.scale * currentBuffer.zoomFactor);

    p.save();
    // scale image if current zoomfactor does not match buffer's zoomfactor
    p.scale(currentBuffer.zoomFactor.x() / zoomFactor.x(), currentBuffer.zoomFactor.y() / zoomFactor.y());
    // add offset
    p.translate(off);
    // draw buffer to painter
    p.drawImage(0, 0, currentBuffer.image);
    p.restore();

    // intNeedsRedraw is reset by the thread
    if(needsRedraw & maskRedraw)
    {
        intNeedsRedraw = true;
    }
    mutex.unlock(); // --------- stop serialize with thread

    if((needsRedraw & maskRedraw) && !isRunning())
    {
        emit sigStartThread();
        start();
    }
}

void IDrawContext::run()
{
    mutex.lock();
    QTime t;
    t.start();
//    qDebug() << "start thread" << objectName();

    IDrawContext::buffer_t& currentBuffer = buffer[!bufIndex];
    while(intNeedsRedraw)
    {
        // copy all projection information need by the
        // map render objects to buffer structure
        currentBuffer.zoomFactor = zoomFactor;
        currentBuffer.scale = scale;
        currentBuffer.ref1 = ref1;
        currentBuffer.ref2 = ref2;
        currentBuffer.ref3 = ref3;
        currentBuffer.ref4 = ref4;
        currentBuffer.focus = focus;
        intNeedsRedraw = false;

        mutex.unlock();

//        qDebug() << "bufferScale" << (currentBuffer.scale * currentBuffer.zoomFactor);
        // ----- reset buffer -----
        currentBuffer.image.fill(Qt::transparent);

        drawt(currentBuffer);

        mutex.lock();
    }
    // ----- switch buffer ------
    bufIndex = !bufIndex;
//    qDebug() << "stop thread" << objectName() << "after" << t.elapsed() << "ms";

    mutex.unlock();
}

