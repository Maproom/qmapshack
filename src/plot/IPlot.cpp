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

#include "plot/IPlot.h"
#include "plot/CPlotAxis.h"

#include "canvas/CCanvas.h"
#include "CMainWindow.h"

#include <QtWidgets>

QPen IPlot::pens[] =
{
    QPen(Qt::darkBlue,4)
    , QPen(Qt::darkRed,2)
    , QPen(Qt::darkYellow,2)
    , QPen(Qt::darkGreen,2)

};

QPen IPlot::pensThin[] =
{
    QPen(Qt::darkBlue,2)
    , QPen(Qt::darkRed,1)
    , QPen(Qt::darkYellow,1)
    , QPen(Qt::darkGreen,1)

};

QColor IPlot::colors[] =
{
    QColor(Qt::blue)
    , QColor(0,0,0,0)
    , QColor(0,0,0,0)
    , QColor(0,0,0,0)

};


IPlot::IPlot(CGisItemTrk *trk, CPlotData::axistype_e type, mode_e mode, QWidget *parent)
    : QWidget(parent)
    , mode(mode)
    , needsRedraw(true)
    , showScale(true)
    , thinLine(false)
    , posMouse(NOPOINT)
    , trk(trk)
    , fm(font())
    , left(0)
    , right(0)
    , top(0)
    , bottom(0)
    , deadAreaX(0)
    , deadAreaY(0)
    , fontWidth(0)
    , fontHeight(0)
    , scaleWidthX1(0)
    , scaleWidthY1(0)

{
    setMouseTracking(true);

    if(trk) trk->registerPlot(this);

    data = new CPlotData(type, this);

    if(mode == eModeIcon)
    {
        showScale = false;
        thinLine = true;
    }

}

IPlot::~IPlot()
{
    if(trk) trk->unregisterPlot(this);
}

void IPlot::clear()
{
    data->lines.clear();
    data->marks.points.clear();
    data->tags.clear();
    data->badData = true;
    update();
}


void IPlot::setYLabel(const QString& str)
{
    data->ylabel = str;
    setSizes();
    update();
}


void IPlot::setXLabel(const QString& str)
{
    data->xlabel = str;
    setSizes();
    update();
}


void IPlot::newLine(const QPolygonF& line, const QString& label)
{
    data->lines.clear();

    QRectF r = line.boundingRect();
    if((r.height() < 0) || (r.width() < 0))
    {
        data->badData = true;
        return;
    }

    CPlotData::line_t l;
    l.points    = line;
    l.label     = label;

    data->badData = false;
    data->lines << l;
    setSizes();
    data->x().setScale( rectGraphArea.width() );
    data->y().setScale( rectGraphArea.height() );

    needsRedraw = true;
    update();

}

void IPlot::setLimits()
{
    data->setLimits();
}

void IPlot::resetZoom()
{
    data->x().resetZoom();
    data->y().resetZoom();
    setSizes();

//    initialYMin = data->y().min();
//    initialYMax = data->y().max();

    needsRedraw = true;
    update();
}


void IPlot::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    draw(p);
}

void IPlot::resizeEvent(QResizeEvent * e)
{
    setSizes();

//    initialYMin = data->y().min();
//    initialYMax = data->y().max();

    buffer = QImage(e->size(), QImage::Format_ARGB32);

    needsRedraw = true;
    update();
}

void IPlot::leaveEvent(QEvent * e)
{
    needsRedraw = true;
    posMouse    = NOPOINT;

    if(trk) trk->setPointOfFocusByDistance(NOFLOAT);

    QApplication::restoreOverrideCursor();
    update();
}


void IPlot::enterEvent(QEvent * e)
{
    needsRedraw = true;
    QApplication::setOverrideCursor(Qt::PointingHandCursor);
    update();
}

void IPlot::draw(QPainter& p)
{
    if(needsRedraw)
    {
        draw();
        needsRedraw = false;
    }

    p.drawImage(0,0,buffer);

    drawDecoration(p);
}

void IPlot::mouseMoveEvent(QMouseEvent * e)
{
    posMouse = NOPOINT;
    if(rectGraphArea.contains(e->pos()))
    {
        posMouse = e->pos();

        // set point of focus at track object
        qreal x = data->x().pt2val(posMouse.x() - left);
        if(data->axisType == CPlotData::eAxisLinear)
        {
            if(trk) trk->setPointOfFocusByDistance(x);
        }
        else if(data->axisType == CPlotData::eAxisTime)
        {

        }

        // update canvas if visible
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas)
        {
            canvas->update();
        }
        e->accept();
    }
    update();
}

void IPlot::setSizes()
{
    fm = QFontMetrics(CMainWindow::self().getMapFont());
    left = 0;

    scaleWidthX1    = showScale ? data->x().getScaleWidth( fm ) : 0;
    scaleWidthY1    = showScale ? data->y().getScaleWidth( fm ) : 0;

    scaleWidthY1    = scaleWidthX1 > scaleWidthY1 ? scaleWidthX1 : scaleWidthY1;

    fontWidth       = fm.maxWidth();
    fontHeight      = fm.height();
    deadAreaX       = fontWidth >> 1;
    deadAreaY       = ( fontHeight + 1 ) >> 1;

    setLRTB();
    setSizeIconArea();
    setSizeXLabel();
    setSizeYLabel();
    setSizeTrackInfo();
    setSizeDrawArea();

}

void IPlot::setLRTB()
{
    left = 0;

    left += data->ylabel.isEmpty() ? 0 : fontHeight;
    left += scaleWidthY1;
    left += deadAreaX;

    right = size().width();
    right -= deadAreaX;
    right -= scaleWidthX1 / 2;

    top = 0;
    if(!data->tags.isEmpty())
    {
        top += fontHeight;
        top += 16;
    }
    top += deadAreaY;

    bottom = size().height();
    bottom -= data->xlabel.isEmpty() ? 0 : fontHeight;
    // tick marks
    if(scaleWidthX1)
    {
        bottom -= fontHeight;
    }
    bottom -= deadAreaY;

    if(!data->xlabel.isEmpty())
    {
        bottom -= deadAreaY;
    }

    if(!data->tags.isEmpty() /*&& CResources::self().showTrackProfileEleInfo()*/)
    {
        bottom -= fontHeight;
    }
}


void IPlot::setSizeIconArea()
{
    rectIconArea = QRect(left, deadAreaY, right - left, 16 + fontHeight + deadAreaY);
}

void IPlot::setSizeXLabel()
{
    int y;
    if ( data->xlabel.isEmpty() )
    {
        rectX1Label = QRect( 0, 0, 0, 0 );
    }
    else
    {
        rectX1Label.setWidth( right - left );
        rectX1Label.setHeight( fontHeight );
        y = ( size().height() - rectX1Label.height()) - deadAreaY;
        if(!data->tags.isEmpty() /*&& CResources::self().showTrackProfileEleInfo()*/)
        {
            y -= fontHeight;
        }

        rectX1Label.moveTopLeft( QPoint( left, y ) );
    }
}

void IPlot::setSizeYLabel()
{
    if ( data->ylabel.isEmpty() )
    {
        rectY1Label = QRect( 0, 0, 0, 0 );
    }
    else
    {
        rectY1Label.setWidth( bottom - top );
        rectY1Label.setHeight( fontHeight );
        rectY1Label.moveTopLeft( QPoint( size().height() - bottom, 0 ) );
    }
}

void IPlot::setSizeTrackInfo()
{
    if(data->tags.isEmpty() /*|| !CResources::self().showTrackProfileEleInfo()*/)
    {
        rectTrackInfo = QRect();
        return;
    }

    rectTrackInfo.setWidth(right - left);
    rectTrackInfo.setHeight(fontHeight);
    rectTrackInfo.moveLeft(left);
    rectTrackInfo.moveTop(size().height() - fontHeight);
}

void IPlot::setSizeDrawArea()
{
    rectGraphArea.setWidth( right - left );
    rectGraphArea.setHeight( bottom - top );
    rectGraphArea.moveTopLeft( QPoint( left, top ) );

    data->x().setScale( rectGraphArea.width() );
    data->y().setScale( rectGraphArea.height() );
}


void IPlot::draw()
{
    buffer.fill(Qt::transparent);
    QPainter p(&buffer);
    USE_ANTI_ALIASING(p, true);

    if(mode == eModeNormal)
    {
        p.fillRect(rect(),Qt::white);
    }
    else if(mode == eModeIcon)
    {
        QRect r = rect();
        r.adjust(2,2,-2,-2);
        if(underMouse() || posMouse != NOPOINT)
        {
            p.setPen(CCanvas::penBorderBlue);
            p.setOpacity(1.0);
        }
        else
        {
            p.setPen(CCanvas::penBorderBlack);
            p.setOpacity(0.6);
        }
        p.setBrush(QColor(255,255,255,255));

        PAINT_ROUNDED_RECT(p,r);
    }

    if(data->lines.isEmpty() || data->badData)
    {
        p.drawText(rect(), Qt::AlignCenter, tr("No or bad data."));
        return;
    }

    p.setFont(CMainWindow::self().getMapFont());
//    drawTags(p);
    p.setClipping(true);
    p.setClipRect(rectGraphArea);
    drawData(p);
    p.setClipping(false);
    drawLabels(p);
    if(showScale)
    {
        drawXScale(p);
        drawYScale(p);
    }
    drawGridX(p);
    drawGridY(p);
    drawXTic(p);
    drawYTic(p);
    p.setPen(QPen(Qt::black,2));
    p.drawRect(rectGraphArea);

//    drawLegend(p);
}

void IPlot::drawData(QPainter& p)
{
    int penIdx = 0;
    int ptx, pty, oldPtx;
    QList<CPlotData::line_t> lines                  = data->lines;
    QList<CPlotData::line_t>::const_iterator line   = lines.begin();

    CPlotAxis& xaxis = data->x();
    CPlotAxis& yaxis = data->y();

    while(line != lines.end())
    {
        QPolygonF background;
        QPolygonF foreground;

        const QPolygonF& polyline       = line->points;
        QPolygonF::const_iterator point = polyline.begin();

        ptx = left   + xaxis.val2pt( point->x() );
        pty = bottom - yaxis.val2pt( point->y() );
        oldPtx = ptx;

        background << QPointF(left,bottom);
        background << QPointF(left,pty);
        background << QPointF(ptx,pty);

        while(point != polyline.end())
        {
            ptx = left   + xaxis.val2pt( point->x() );
            pty = bottom - yaxis.val2pt( point->y() );

            if(oldPtx == ptx)
            {
                ++point;
                continue;
            }
            oldPtx = ptx;

            if(ptx >= left && ptx <= right)
            {
                background << QPointF(ptx,pty);
                foreground << QPointF(ptx,pty);
            }
            ++point;
        }

        background << QPointF(right,pty);
        background << QPointF(right,bottom);

//        QLinearGradient gradient(0, bottom - yaxis.val2pt(initialYMin), 0, bottom - yaxis.val2pt(initialYMax));
//        gradient.setColorAt(0, colors[penIdx]);
//        gradient.setColorAt(1, QColor(0,0,0,0));
        p.setPen(Qt::NoPen);
//        p.setBrush(gradient);
        p.setBrush(colors[penIdx]);
        p.drawPolygon(background);

        p.setPen(thinLine ? pensThin[penIdx++] : pens[penIdx++]);
        p.setBrush(Qt::NoBrush);
        p.drawPolyline(foreground);

        ++line;
    }
}

void IPlot::drawLabels( QPainter &p )
{
    p.setPen(Qt::darkBlue);

    if ( rectX1Label.isValid() )
    {
        p.drawText( rectX1Label, Qt::AlignCenter, data->xlabel );
    }

    p.save();
    QMatrix m = p.matrix();
    m.translate( 0, size().height() );
    m.rotate( -90 );
    p.setMatrix( m );

    if ( rectY1Label.isValid() )
    {
        p.drawText( rectY1Label, Qt::AlignCenter, data->ylabel );
    }
    p.restore();
}

void IPlot::drawXScale( QPainter &p )
{
    QRect recText;

    if ( data->x().getTicType() == CPlotAxis::eNoTic )
    {
        return ;
    }

    p.setPen(Qt::darkBlue);
    recText.setHeight( fontHeight );
    recText.setWidth( scaleWidthX1 );

    int ix;
    int ix_ = -1;
    int iy;

    iy = bottom + deadAreaY;
    const CPlotAxis::tic_t * t = data->x().ticmark();
    while ( t )
    {
        ix = left + data->x().val2pt( t->val ) - ( scaleWidthX1 + 1 ) / 2;
        if ( ( ( ix_ < 0 ) || ( ( ix - ix_ ) > scaleWidthX1 + 5 ) ) && !t->lbl.isEmpty() )
        {
            recText.moveTopLeft( QPoint( ix, iy ) );
            p.drawText( recText, Qt::AlignCenter, t->lbl );
            ix_ = ix;
        }
        t = data->x().ticmark( t );
    }

    qreal limMin, limMax, useMin, useMax;
    data->x().getLimits(limMin, limMax, useMin, useMax);

    if((limMax - limMin) <= (useMax - useMin)) return;

    qreal scale = (right - left) / (limMax - limMin);
    //     qreal val   = data->x().pt2val(0);

    int x = left + (useMin - limMin) * scale;
    int y = bottom + 5;
    int w = (useMax - useMin) * scale;

    p.setPen(QPen(Qt::red,3));
    p.drawLine(x,y, x + w, y);

}


void IPlot::drawYScale( QPainter &p )
{
    QString format_single_prec;
    QRect recText;
    if ( data->y().getTicType() == CPlotAxis::eNoTic )
        return ;

    p.setPen(Qt::darkBlue);
    recText.setHeight( fontHeight );
    recText.setWidth( scaleWidthY1 );

    int ix;
    int iy;

    ix = left - scaleWidthY1 - deadAreaX;

    qreal limMin, limMax, useMin, useMax;
    data->y().getLimits(limMin, limMax, useMin, useMax);

    // draw min/max lables 1st;
    QRect recTextMin;
    QRect recTextMax;

    format_single_prec = data->y().fmtsgl(data->ymin);
    if(data->ymin >= useMin)
    {
        iy = bottom - data->y().val2pt( data->ymin ) - fontHeight / 2;
        recText.moveTopLeft( QPoint( ix, iy ) );
        p.drawText( recText, Qt::AlignRight, QString().sprintf( format_single_prec.toLatin1().data(), data->ymin  ));
        recTextMin = recText;
    }
    format_single_prec = data->y().fmtsgl(data->ymax);
    if(data->ymax <= useMax)
    {
        iy = bottom - data->y().val2pt( data->ymax ) - fontHeight / 2;
        recText.moveTopLeft( QPoint( ix, iy ) );
        p.drawText( recText, Qt::AlignRight, QString().sprintf( format_single_prec.toLatin1().data(), data->ymax  ));
        recTextMax = recText;
    }

    // draw tic marks
    const CPlotAxis::tic_t * t = data->y().ticmark();
    while ( t )
    {
        iy = bottom - data->y().val2pt( t->val ) - fontHeight / 2;

        recText.moveTopLeft( QPoint( ix, iy ) );

        if(!recTextMin.intersects(recText) && !recTextMax.intersects(recText))
        {
            p.drawText( recText, Qt::AlignRight, t->lbl );
        }

        t = data->y().ticmark( t );
    }

    if((limMax - limMin) <= (useMax - useMin)) return;

    qreal scale = (top - bottom) / (limMax - limMin);
    //     qreal val   = data->y().pt2val(0);

    int x = left - 5;
    int y = bottom + (useMin - limMin) * scale;
    int h = (useMax - useMin) * scale;

    p.setPen(QPen(Qt::red,3));
    p.drawLine(x,y, x, y + h);

}


void IPlot::drawGridX( QPainter &p )
{
    int ix;
    int iy, dy;

    CPlotAxis::tictype_e oldtic = data->x().setTicType( CPlotAxis::eTicNorm );

    dy = rectGraphArea.height();
    const CPlotAxis::tic_t * t = data->x().ticmark();

    QPen oldpen = p.pen();
    p.setPen( QPen( QColor(0,150,0,128), 1, Qt::DotLine ) );

    iy = rectGraphArea.top();
    while ( t )
    {
        ix = left + data->x().val2pt( t->val );
        p.drawLine( ix, iy, ix, iy + dy );
        t = data->x().ticmark( t );
    }
    p.setPen( oldpen );
    data->x().setTicType( oldtic );
}


void IPlot::drawGridY( QPainter &p )
{
    int ix, dx;
    int iy;

    CPlotAxis::tictype_e oldtic = data->y().setTicType( CPlotAxis::eTicNorm );
    dx = rectGraphArea.width();
    const CPlotAxis::tic_t * t = data->y().ticmark();

    QPen oldpen = p.pen();
    p.setPen( QPen( QColor(0,150,0,128), 1, Qt::DotLine ) );

    ix = rectGraphArea.left();
    while ( t )
    {
        iy = bottom - data->y().val2pt( t->val );
        p.drawLine( ix, iy, ix + dx, iy );
        t = data->y().ticmark( t );
    }

    // draw min/max lines
    qreal limMin, limMax, useMin, useMax;
    data->y().getLimits(limMin, limMax, useMin, useMax);

    if(data->ymin > useMin)
    {
        iy = bottom - data->y().val2pt( data->ymin );
        p.drawLine( ix, iy, ix + dx, iy );
    }
    if(data->ymax < useMax)
    {
        iy = bottom - data->y().val2pt( data->ymax );
        p.drawLine( ix, iy, ix + dx, iy );
    }

    p.setPen( oldpen );
    data->y().setTicType( oldtic );
}

void IPlot::drawXTic( QPainter & p )
{
    int ix;
    int iyb, iyt;
    const CPlotAxis::tic_t * t = data->x().ticmark();

    p.setPen(QPen(Qt::black,2));
    iyb = rectGraphArea.bottom();
    iyt = rectGraphArea.top();
    while ( t )
    {
        ix = left + data->x().val2pt( t->val );
        p.drawLine( ix, iyb, ix, iyb - 5 );
        p.drawLine( ix, iyt, ix, iyt + 5 );
        t = data->x().ticmark( t );
    }
}


void IPlot::drawYTic( QPainter &p )
{
    int ixl, ixr;
    int iy;
    const CPlotAxis::tic_t * t = data->y().ticmark();

    p.setPen(QPen(Qt::black,2));
    ixl = rectGraphArea.left();
    ixr = rectGraphArea.right();
    while ( t )
    {
        iy = bottom - data->y().val2pt( t->val );
        p.drawLine( ixl, iy, ixl + 5, iy );
        p.drawLine( ixr, iy, ixr - 5, iy );
        t = data->y().ticmark( t );
    }
}

void IPlot::drawDecoration( QPainter &p )
{

    if(posMouse != NOPOINT)
    {
        p.setPen(QPen(Qt::red,2));
        p.drawLine(posMouse.x(), top, posMouse.x(), bottom);
    }
}
