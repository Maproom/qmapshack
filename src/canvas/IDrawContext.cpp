/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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
#include "canvas/CCanvas.h"

#include <QtWidgets>

//#define N_ZOOM_LEVELS 20
//const qreal CMapDraw::scales[N_ZOOM_LEVELS] =
//{
//     0.10
//    ,0.20
//    ,0.40
//    ,0.80
//    ,1.60
//    ,3.20
//    ,6.40
//    ,12.8
//    ,25.6
//    ,51.2
//    ,102.4
//    ,204.8
//    ,409.6
//    ,819.2
//    ,1638.4
//    ,3276.8
//    ,6553.6
//    ,13107.2
//    ,26214.4
//    ,52428.8
//};

#define BUFFER_BORDER 50
#define N_ZOOM_LEVELS 31
const qreal IDrawContext::scales[N_ZOOM_LEVELS] =
{
      0.10
    , 0.15
    , 0.20
    , 0.30
    , 0.50
    , 0.70
    , 1.0
    , 1.5
    , 2.0
    , 3.0
    , 5.0
    , 7.0
    , 10.0
    , 15.0
    , 20.0
    , 30.0
    , 50.0
    , 70.0
    , 100.0
    , 150.0
    , 200.0
    , 300.0
    , 500.0
    , 700.0
    , 1000.0
    , 1500.0
    , 2000.0
    , 3000.0
    , 5000.0
    , 7000.0
    , 10000.0
//    , 15000.0
//    , 20000.0
//    , 30000.0
//    , 50000.0
//    , 70000.0
};

QPointF operator*(const QPointF& p1, const QPointF& p2)
{
    return QPointF(p1.x() * p2.x(), p1.y() * p2.y());
}

QPointF operator/(const QPointF& p1, const QPointF& p2)
{
    return QPointF(p1.x() / p2.x(), p1.y() / p2.y());
}


IDrawContext::IDrawContext(const QString& name, CCanvas::redraw_e maskRedraw, CCanvas *parent)
    : QThread(parent)
    , canvas(parent)
    , maskRedraw(maskRedraw)
    , bufIndex(false)
    , bufWidth(100)
    , bufHeight(100)
    , viewWidth(100)
    , viewHeight(100)
    , scale(1.0,-1.0)
    , zoomIndex(0)

{
    setObjectName(name);
    // setup map parameters and connect to canvas
    pjsrc = pj_init_plus("+proj=merc +a=6378137.0000 +b=6356752.3142 +towgs84=0,0,0,0,0,0,0,0 +units=m  +no_defs");
    pjtar = pj_init_plus("+proj=longlat +a=6378137.0000 +b=6356752.3142 +towgs84=0,0,0,0,0,0,0,0 +units=m  +no_defs");


    zoom(5);

    resize(canvas->size());
    connect(this, SIGNAL(finished()), canvas, SLOT(update()));
    connect(this, SIGNAL(finished()), SIGNAL(sigStopThread()));

}

IDrawContext::~IDrawContext()
{
    pj_free(pjtar);
    pj_free(pjsrc);

}

void IDrawContext::emitSigCanvasUpdate()
{
    emit sigCanvasUpdate(maskRedraw);
}


void IDrawContext::resize(const QSize& size)
{
    if(isRunning())
    {
        wait();
    }
    mutex.lock(); // --------- start serialize with thread
    viewWidth   = size.width();
    viewHeight  = size.height();

    center      = QPointF(viewWidth/2.0, viewHeight/2.0);
    bufWidth    = viewWidth  + 2 * BUFFER_BORDER;
    bufHeight   = viewHeight + 2 * BUFFER_BORDER;

    buffer[0].image = QImage(bufWidth, bufHeight, QImage::Format_ARGB32);
    buffer[1].image = QImage(bufWidth, bufHeight, QImage::Format_ARGB32);
    mutex.unlock(); // --------- stop serialize with thread
}

QString IDrawContext::getProjection()
{
    if(pjsrc == 0)
    {
        return QString::Null();
    }
    return pj_get_def(pjsrc,0);
}

void IDrawContext::setProjection(const QString& proj)
{
    if(pjsrc != 0)
    {
        pj_free(pjsrc);
    }

    pjsrc = pj_init_plus(proj.toLatin1());
}

bool IDrawContext::needsRedraw()
{
    bool res = false;
    mutex.lock();
    res = intNeedsRedraw;
    mutex.unlock();
    return res;
}

void IDrawContext::zoom(const QRectF& rect)
{
    if(pjsrc == 0) return;

    // special case for elements with no extent
    if(rect.width() == 0 || rect.height() == 0)
    {
        zoom(8);
        return;
    }

    // zoom out from closest zoom level until a match is found
    for(int i = 0; i < N_ZOOM_LEVELS; i++)
    {
        zoom(i);
        QPointF pt1 = rect.topLeft();
        QPointF pt2 = rect.bottomRight();

        convertRad2Px(pt1);
        convertRad2Px(pt2);

        QPointF pt = pt2 - pt1;
        if(qAbs(pt.x()) < (bufWidth - 2 * BUFFER_BORDER) && qAbs(pt.y() < (bufHeight - 2 * BUFFER_BORDER)))
        {
            break;
        }
    }
}

void IDrawContext::zoom(bool in, CCanvas::redraw_e& needsRedraw)
{
    if(pjsrc == 0) return;
    zoom(zoomIndex + (in ? -1 : 1));
    needsRedraw = CCanvas::eRedrawAll;
}

void IDrawContext::zoom(int idx)
{
    if(idx < 0)
    {
        idx = 0;
    }

    if(idx == N_ZOOM_LEVELS)
    {
        idx = N_ZOOM_LEVELS -1;
    }

    mutex.lock(); // --------- start serialize with thread
    if(zoomIndex != idx)
    {
        zoomIndex       = idx;
        zoomFactor.rx() = scales[idx];
        zoomFactor.ry() = scales[idx];
        intNeedsRedraw  = true;

        emit  sigScaleChanged(scale*zoomFactor);
    }
    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertRad2M(QPointF &p)
{
    if(pjsrc == 0)
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
    bool fixWest = p.x() < (-180*DEG_TO_RAD);
    bool fixEast = p.x() > ( 180*DEG_TO_RAD);

    pj_transform(pjtar,pjsrc,1,0,&p.rx(),&p.ry(),0);

    /*
        The idea of the fix is to calculate a point
        at the boundary with the same latitude and use it
        as offset.
    */
    if(fixWest)
    {
        QPointF o(-180*DEG_TO_RAD,y);
        convertRad2M(o);
        p.rx() = 2*o.x() + p.x();
    }

    if(fixEast)
    {
        QPointF o(180*DEG_TO_RAD,y);
        convertRad2M(o);
        p.rx() = 2*o.x() + p.x();
    }

}

void IDrawContext::convertM2Rad(QPointF &p)
{
    if(pjsrc == 0)
    {
        return;
    }

    pj_transform(pjsrc,pjtar,1,0,&p.rx(),&p.ry(),0);

}

void IDrawContext::convertPx2Rad(QPointF &p)
{
    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertRad2M(f);

    p = f + (p - center) * scale * zoomFactor;

    convertM2Rad(p);

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertRad2Px(QPointF &p)
{
    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertRad2M(f);
    convertRad2M(p);

    p = (p - f) / (scale * zoomFactor) + center;

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::convertRad2Px(QPolygonF& poly)
{
    mutex.lock(); // --------- start serialize with thread

    QPointF f = focus;
    convertRad2M(f);

    for(int i = 0; i < poly.size(); i++)
    {
        QPointF& p = poly[i];
        convertRad2M(p);
        p = (p - f) / (scale * zoomFactor) + center;
    }

    mutex.unlock(); // --------- stop serialize with thread
}

void IDrawContext::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QPointF& f, const QRectF &r)
{
    if(pjsrc == 0) return;

    // convert global coordinate of focus into point of map
    focus = f;

    QPointF f1 = focus;
    convertRad2M(f1);

    QPointF bufferScale = scale * zoomFactor;

    mutex.lock(); // --------- start serialize with thread
    // derive top left reference coordinate of map buffer
    ref1 = f1 + QPointF(-bufWidth/2, -bufHeight/2) * bufferScale;
    convertM2Rad(ref1);
    // derive top right reference coordinate of map buffer
    ref2 = f1 + QPointF( bufWidth/2, -bufHeight/2) * bufferScale;
    convertM2Rad(ref2);
    // derive bottom right reference coordinate of map buffer
    ref3 = f1 + QPointF( bufWidth/2,  bufHeight/2) * bufferScale;
    convertM2Rad(ref3);
    // derive bottom left reference coordinate of map buffer
    ref4 = f1 + QPointF(-bufWidth/2,  bufHeight/2) * bufferScale;
    convertM2Rad(ref4);

    // adjust west <-> east boundaries
    if(ref1.x() > ref2.x())
    {
        if(qAbs(ref1.x()) > qAbs(ref2.x()))
        {
            ref1.rx() = -2*(180*DEG_TO_RAD) + ref1.rx();
        }
        if(qAbs(ref4.x()) > qAbs(ref3.x()))
        {
            ref4.rx() = -2*(180*DEG_TO_RAD) + ref4.rx();
        }

        if(qAbs(ref1.x()) < qAbs(ref2.x()))
        {
            ref2.rx() = 2*(180*DEG_TO_RAD) + ref2.rx();
        }
        if(qAbs(ref4.x()) < qAbs(ref3.x()))
        {
            ref3.rx() = 2*(180*DEG_TO_RAD) + ref3.rx();
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
    p.scale(currentBuffer.zoomFactor.x()/zoomFactor.x(), currentBuffer.zoomFactor.y()/zoomFactor.y());
    // add offset
    p.translate(off);
    // draw buffer to painter
    p.drawImage(0,0, currentBuffer.image);
    p.restore();

    // intNeedsRedraw is reset by the thread
    if(needsRedraw & maskRedraw) intNeedsRedraw = true;
    mutex.unlock(); // --------- stop serialize with thread

    if((needsRedraw  & maskRedraw) && !isRunning())
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
    qDebug() << "start thread";

    IDrawContext::buffer_t& currentBuffer = buffer[!bufIndex];
    while(intNeedsRedraw)
    {

        // copy all projection information need by the
        // map render objects to buffer structure
        currentBuffer.pjsrc         = pjsrc;
        currentBuffer.zoomFactor    = zoomFactor;
        currentBuffer.scale         = scale;
        currentBuffer.ref1          = ref1;
        currentBuffer.ref2          = ref2;
        currentBuffer.ref3          = ref3;
        currentBuffer.ref4          = ref4;
        currentBuffer.focus         = focus;
        intNeedsRedraw              = false;

        mutex.unlock();        

        qDebug() << "bufferScale" << (currentBuffer.scale * currentBuffer.zoomFactor);
        // ----- reset buffer -----
        currentBuffer.image.fill(Qt::transparent);

        drawt(currentBuffer);

        mutex.lock();
    }
    // ----- switch buffer ------
    bufIndex = !bufIndex;
    qDebug() << objectName() << "stop thread" << t.elapsed();
    mutex.unlock();

}

