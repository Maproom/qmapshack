/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

#include "plot/CPlotAxis.h"
#include "plot/IPlot.h"

#include "CMainWindow.h"
#include "gis/CGisWidget.h"
#include "gis/trk/CActivityTrk.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "mouse/CScrOptRangeTrk.h"
#include "widgets/CFadingIcon.h"

#include <QKeyEvent>
#include <QtWidgets>

const QPen IPlot::pens[] =
{
    QPen(Qt::darkBlue,      3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , QPen(QColor("#C00000"), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , QPen(Qt::yellow,        3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , QPen(Qt::green,         3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
};

const QPen IPlot::pensThin[] =
{
    QPen(Qt::darkBlue,   2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , QPen(Qt::darkRed,    2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , QPen(Qt::darkYellow, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , QPen(Qt::darkGreen,  2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
};

const QColor IPlot::colors[] =
{
    QColor(Qt::blue)
    , QColor(0, 0, 0, 0)
    , QColor(0, 0, 0, 0)
    , QColor(Qt::darkGreen)
};

int IPlot::cnt = 0;

IPlot::IPlot(CGisItemTrk *trk, CPlotData::axistype_e type, mode_e mode, QWidget *parent)
    : QWidget(parent)
    , INotifyTrk(CGisItemTrk::eVisualPlot)
    , mode(mode)
    , trk(trk)
    , fm(font())
{
    cnt++;
    setObjectName(QString("IPlot%1").arg(cnt));

    setContextMenuPolicy(Qt::CustomContextMenu);
    setMouseTracking(true);

    if(trk)
    {
        trk->registerVisual(this);
    }

    data = new CPlotData(type, this);

    if(mode == eModeIcon)
    {
        showScale = false;
        thinLine = true;
    }

    if(mode == eModeWindow)
    {
        overrideWindowFlags(Qt::Tool);
        setAttribute(Qt::WA_DeleteOnClose, true);
    }

    menu = new QMenu(this);
    actionResetZoom = menu->addAction(QIcon("://icons/32x32/Zoom.png"),        tr("Reset Zoom"), this, SLOT(slotResetZoom()));
    actionStopRange = menu->addAction(QIcon("://icons/32x32/SelectRange.png"), tr("Stop Range"), this, SLOT(slotStopRange()));
    actionPrint     = menu->addAction(QIcon("://icons/32x32/Save.png"),        tr("Save..."),    this, SLOT(slotSave()));

    connect(this, &IPlot::customContextMenuRequested, this, &IPlot::slotContextMenu);
}

IPlot::~IPlot()
{
    cnt--;

    if(trk)
    {
        trk->unregisterVisual(this);
        /*
            Always set the mode to normal. If the object is not owner
            of the current mode, the request will be ignored.
         */
        trk->setMode(CGisItemTrk::eModeNormal, objectName());

        /*
            As having the user focus will always display an on screen plot, closing
            the plot has to result into the track loosing the focus.
         */
        if(mode == eModeWindow)
        {
            trk->looseUserFocus();
            CCanvas * canvas = dynamic_cast<CCanvas*>(parent());
            if(canvas)
            {
                canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
            }
        }
    }
}

void IPlot::clear()
{
    data->lines.clear();
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
    if((r.height() < 0) || (r.width() < 0) || line.isEmpty())
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

void IPlot::addLine(const QPolygonF& line, const QString& label)
{
    QRectF r = line.boundingRect();
    if(!r.isValid() || line.isEmpty())
    {
        return;
    }

    CPlotData::line_t l;
    l.points    = line;
    l.label     = label;

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

    buffer = QImage(e->size(), QImage::Format_ARGB32);

    needsRedraw = true;
    update();
}

void IPlot::leaveEvent(QEvent * e)
{
    needsRedraw = true;
    posMouse    = NOPOINT;

    CCanvas::restoreOverrideCursor("IPlot::leaveEvent");
    update();
}


void IPlot::enterEvent(QEvent * e)
{
    needsRedraw = true;
    CCanvas::setOverrideCursor(Qt::PointingHandCursor,"IPlot::enterEvent");
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

void IPlot::keyPressEvent(QKeyEvent *e)
{
    // close the current window if `Esc` was pressed
    if(Qt::Key_Escape == e->key())
    {
        e->accept();
        deleteLater();
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}

void IPlot::mouseMoveEvent(QMouseEvent * e)
{
    if(data->lines.isEmpty() || data->badData || !data->x().isValid() || !data->y().isValid())
    {
        return;
    }

    posMouse = NOPOINT;
    if(rectGraphArea.contains(e->pos()))
    {
        posMouse = e->pos();

        // set point of focus at track object
        qreal x = data->x().pt2val(posMouse.x() - left);
        setMouseFocus(x, CGisItemTrk::eFocusMouseMove);

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

void IPlot::setMouseFocus(qreal pos, enum CGisItemTrk::focusmode_e fm)
{
    if(nullptr == trk)
    {
        return;
    }

    if(data->axisType == CPlotData::eAxisLinear)
    {
        trk->setMouseFocusByDistance(pos, fm, objectName());
    }
    else if(data->axisType == CPlotData::eAxisTime)
    {
        trk->setMouseFocusByTime(pos, fm, objectName());
    }
}

void IPlot::mousePressEvent(QMouseEvent * e)
{
    if(data->lines.isEmpty() || data->badData || !data->x().isValid() || !data->y().isValid())
    {
        return;
    }

    bool wasProcessed = true;

    posMouse = NOPOINT;
    if((e->button() == Qt::LeftButton) && rectGraphArea.contains(e->pos()))
    {
        posMouse = e->pos();

        if(mode == eModeIcon)
        {
            trk->edit();
        }
        else
        {
            // set point of focus at track object
            qreal x = data->x().pt2val(posMouse.x() - left);

            switch(mouseClickState)
            {
            case eMouseClickIdle:
            {
                // In idle state a mouse click will select the first point of a range
                if(trk->setMode(CGisItemTrk::eModeRange, objectName()))
                {
                    setMouseFocus(x, CGisItemTrk::eFocusMouseClick);
                    mouseClickState = eMouseClick1st;
                }
                else
                {
                    /*
                        If the object is not the owner of the range selection, no action has to be taken.
                        However the user has to be informed, that he clicked on the wrong widget.
                     */
                    new CFadingIcon(posMouse, "://icons/48x48/NoGo.png", this);
                    wasProcessed = false;
                }
                break;
            }

            case eMouseClick1st:
            {
                // In 1st click state a mouse click will select the second point of a range and display options
                setMouseFocus(x, CGisItemTrk::eFocusMouseClick);
                /*
                    As the screen option is created on the fly it has to be connected to all slots,too.
                    Later, when destroyed the slots will be disconnected automatically.
                 */
                delete scrOptRange;
                scrOptRange = new CScrOptRangeTrk(e->pos(), trk, &dummyMouse, this);
                connect(scrOptRange->toolHidePoints, &QToolButton::clicked, this, &IPlot::slotHidePoints);
                connect(scrOptRange->toolShowPoints, &QToolButton::clicked, this, &IPlot::slotShowPoints);
                connect(scrOptRange->toolActivity,   &QToolButton::clicked, this, &IPlot::slotActivity);
                connect(scrOptRange->toolCopy,       &QToolButton::clicked, this, &IPlot::slotCopy);

                /* Adjust position of screen option widget if the widget is out of the visible area*/
                QRect r1 = scrOptRange->geometry();
                QRect r2 = geometry();
                r1.moveTopLeft(mapToParent(r1.topLeft()));
                if(!r2.contains(r1))
                {
                    // test if screen option is out of area on the right side
                    if(!r2.contains(r1.topRight()))
                    {
                        QPoint pt = QPoint(r2.width(), r2.height()) - QPoint(r1.width(), r1.height());
                        scrOptRange->move(pt);
                    }
                    // test if screen option is out of area on the left side
                    else if(!r2.contains(r1.topLeft()))
                    {
                        QPoint pt = QPoint(0, r2.height()) - QPoint(0, r1.height());
                        scrOptRange->move(pt);
                    }
                    // test if screen option is out of area on the bottom
                    else if(!r2.contains(r1.bottomLeft()))
                    {
                        QPoint pt = QPoint(r1.left(), r2.height()) - QPoint(r2.left(), r1.height());
                        scrOptRange->move(pt);
                    }
                }

                mouseClickState = eMouseClick2nd;
                break;
            }

            case eMouseClick2nd:
            {
                // In second click state a mouse click will reset the range selection
                delete scrOptRange;
                trk->setMode(CGisItemTrk::eModeNormal, objectName());
                idxSel1 = idxSel2 = NOIDX;
                mouseClickState = eMouseClickIdle;
                break;
            }
            }


            // Update canvas only if the object is the owner of the range selection
            if(wasProcessed)
            {
                emit sigMouseClickState(mouseClickState);

                // update canvas if visible
                CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
                if(canvas)
                {
                    canvas->update();
                }
            }
        }
        e->accept();
    }
    update();
}

void IPlot::wheelEvent(QWheelEvent * e)
{
    bool in = CMainWindow::self().flipMouseWheel() ? (e->delta() < 0) : (e->delta() > 0);

    data->x().zoom(in, e->pos().x() - left);
    setSizes();
    data->x().setScale(rectGraphArea.width());

    needsRedraw = true;
    update();
}

void IPlot::setSizes()
{
    fm = QFontMetrics(CMainWindow::self().getMapFont());
    left = 0;

    scaleWidthX1 = showScale ? data->x().getScaleWidth( fm ) : 0;
    scaleWidthY1 = showScale ? data->y().getScaleWidth( fm ) : 0;

    scaleWidthY1 = scaleWidthX1 > scaleWidthY1 ? scaleWidthX1 : scaleWidthY1;

    fontWidth    = fm.maxWidth();
    fontHeight   = fm.height();
    deadAreaX    = fontWidth >> 1;
    deadAreaY    = ( fontHeight + 1 ) >> 1;

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
        top += 9;
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
        if(underMouse() || posMouse != NOPOINT || solid)
        {
            p.setPen(solid ? CDraw::penBorderBlack : CDraw::penBorderBlue);
            p.setOpacity(1.0);
        }
        else
        {
            p.setPen(CDraw::penBorderBlack);
            p.setOpacity(0.6);
        }
        p.setBrush(QColor(255,255,255,255));

        PAINT_ROUNDED_RECT(p,r);
    }

    if(data->lines.isEmpty() || data->badData || !data->x().isValid() || !data->y().isValid())
    {
        p.drawText(rect(), Qt::AlignCenter, tr("No or bad data."));
        return;
    }

    p.setFont(CMainWindow::self().getMapFont());
    drawTags(p);
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
    drawActivities(p);
    drawXTic(p);
    drawYTic(p);
    p.setPen(QPen(Qt::black,2));
    p.drawRect(rectGraphArea);

    drawLegend(p);
}

QPointF IPlot::getBasePoint(int ptx) const
{
    CPlotAxis& yaxis = data->y();

    if(0 >= data->ymin && 0 <= data->ymax)
    {
        return QPointF(ptx, bottom - yaxis.val2pt(0));
    }
    else if(data->ymin >= 0)
    {
        return QPointF(ptx, bottom - yaxis.val2pt(data->ymin));
    }
    else if(data->ymax <= 0)
    {
        return QPointF(ptx, bottom - yaxis.val2pt(data->ymax));
    }

    qWarning() << "Requesting basePoint for ptx = " << ptx << "; data->ymin/max = {" << data->ymin << ",  " << data->ymax << "}";
    return QPointF(ptx, bottom);
}

QPolygonF IPlot::getVisiblePolygon(const QPolygonF &polyline, QPolygonF &line) const
{
    const CPlotAxis &xaxis = data->x();
    const CPlotAxis &yaxis = data->y();

    int ptx = NOINT;
    int pty = NOINT;

    foreach(const QPointF &pt, polyline)
    {
        int oldPtx = ptx;
        int oldPty = pty;
        ptx = left   + xaxis.val2pt( pt.x() );
        pty = bottom - yaxis.val2pt( pt.y() );

        if(ptx >= left && ptx <= right)
        {
            // if oldPtx is < left, then ptx is the first visible point
            if(NOINT == oldPtx || oldPtx < left)
            {
                // we may need to interpolate things if we just found the first visible point
                if(NOINT != oldPtx && ptx > left)
                {
                    line << getBasePoint(left);

                    int intPty = oldPty + ((oldPty - pty) * (left - oldPtx)) / (oldPtx - ptx);
                    line << QPointF(left, intPty);
                }
                else
                {
                    line << getBasePoint(ptx);
                }
            }

            line << QPointF(ptx, pty);
        }
        else if(ptx > right)
        {
            // handle the special case `no point in the visible interval`
            // -> add interpolated left point
            if(oldPtx < left)
            {
                oldPty = oldPty + (pty - oldPty) / (left - oldPtx);
                oldPtx = left;

                line << getBasePoint(oldPtx);
                line << QPointF(oldPtx, oldPty);
            }

            // interpolate the value at `right`
            pty = oldPty + ((pty - oldPty) * (right - oldPtx)) / (ptx - oldPtx);
            ptx = right;
            line << QPointF(ptx, pty);
        }

        if(ptx >= right)
        {
            break;
        }
    }
    line << getBasePoint(ptx);
    return line;
}

void IPlot::drawData(QPainter& p)
{
    int penIdx = 0;
    QList<CPlotData::line_t> lines                = data->lines;
    QList<CPlotData::line_t>::const_iterator line = lines.begin();

    while(line != lines.end())
    {
        QPolygonF poly;
        getVisiblePolygon(line->points, poly);

        p.setPen(Qt::NoPen);
        p.setBrush(colors[penIdx]);
        p.drawPolygon(poly);

        p.setPen(thinLine ? pensThin[penIdx++] : pens[penIdx++]);
        p.setBrush(Qt::NoBrush);
        poly.pop_front();
        poly.pop_back();
        p.drawPolyline(poly);

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
        return;
    }

    p.setPen(Qt::darkBlue);
    recText.setHeight( fontHeight );
    recText.setWidth( scaleWidthX1 );

    int ix_ = -1;

    const int iy = bottom + deadAreaY;
    const CPlotAxis::tic_t * t = data->x().ticmark();
    while ( t )
    {
        int ix = left + data->x().val2pt( t->val ) - ( scaleWidthX1 + 1 ) / 2;
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

    if(!isZoomed())
    {
        return;
    }

    qreal scale = (right - left) / (limMax - limMin);

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
    {
        return;
    }

    p.setPen(Qt::darkBlue);
    recText.setHeight( fontHeight );
    recText.setWidth( scaleWidthY1 );

    int ix = left - scaleWidthY1 - deadAreaX;
    int iy;

    qreal limMin, limMax, useMin, useMax;
    data->y().getLimits(limMin, limMax, useMin, useMax);

    // draw min/max labels 1st;
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

    if(!isZoomed())
    {
        return;
    }

    qreal scale = (top - bottom) / (limMax - limMin);

    int x = left - 5;
    int y = bottom + (useMin - limMin) * scale;
    int h = (useMax - useMin) * scale;

    p.setPen(QPen(Qt::red,3));
    p.drawLine(x,y, x, y + h);
}


void IPlot::drawGridX( QPainter &p )
{
    CPlotAxis::tictype_e oldtic = data->x().setTicType( CPlotAxis::eTicNorm );

    const int dy = rectGraphArea.height();
    const CPlotAxis::tic_t * t = data->x().ticmark();

    QPen oldpen = p.pen();
    p.setPen( QPen( QColor(0,150,0,128), 1, Qt::DotLine ) );

    const int iy = rectGraphArea.top();
    while ( t )
    {
        int ix = left + data->x().val2pt( t->val );
        p.drawLine( ix, iy, ix, iy + dy );
        t = data->x().ticmark( t );
    }
    p.setPen( oldpen );
    data->x().setTicType( oldtic );
}


void IPlot::drawGridY( QPainter &p )
{
    CPlotAxis::tictype_e oldtic = data->y().setTicType( CPlotAxis::eTicNorm );
    const int dx = rectGraphArea.width();
    const CPlotAxis::tic_t * t = data->y().ticmark();

    QPen oldpen = p.pen();
    p.setPen( QPen( QColor(0,150,0,128), 1, Qt::DotLine ) );

    const int ix = rectGraphArea.left();
    while(nullptr != t)
    {
        int iy = bottom - data->y().val2pt( t->val );
        p.drawLine( ix, iy, ix + dx, iy );
        t = data->y().ticmark( t );
    }

    // draw min/max lines
    qreal limMin, limMax, useMin, useMax;
    data->y().getLimits(limMin, limMax, useMin, useMax);

    if(data->ymin > useMin)
    {
        int iy = bottom - data->y().val2pt( data->ymin );
        p.drawLine( ix, iy, ix + dx, iy );
    }
    if(data->ymax < useMax)
    {
        int iy = bottom - data->y().val2pt( data->ymax );
        p.drawLine( ix, iy, ix + dx, iy );
    }

    p.setPen( oldpen );
    data->y().setTicType( oldtic );
}

void IPlot::drawXTic( QPainter & p )
{
    const CPlotAxis::tic_t * t = data->x().ticmark();

    p.setPen(QPen(Qt::black, 2));
    const int iyb = rectGraphArea.bottom();
    const int iyt = rectGraphArea.top();
    while(nullptr != t)
    {
        const int ix = left + data->x().val2pt( t->val );
        p.drawLine( ix, iyb, ix, iyb - 5 );
        p.drawLine( ix, iyt, ix, iyt + 5 );
        t = data->x().ticmark( t );
    }
}


void IPlot::drawYTic( QPainter &p )
{
    const CPlotAxis::tic_t * t = data->y().ticmark();

    p.setPen(QPen(Qt::black, 2));
    const int ixl = rectGraphArea.left();
    const int ixr = rectGraphArea.right();
    while ( t )
    {
        const int iy = bottom - data->y().val2pt( t->val );
        p.drawLine( ixl, iy, ixl + 5, iy );
        p.drawLine( ixr, iy, ixr - 5, iy );
        t = data->y().ticmark( t );
    }
}

void IPlot::drawLegend(QPainter& p)
{
    if((data->lines.size() < 2) || (mode == eModeIcon))
    {
        return;
    }

    int penIdx = 0;
    QFontMetrics fm(p.font());
    int h = fm.height();

    int x = rectGraphArea.left() + 10;
    int y = rectGraphArea.top()  + 2 + h;

    QList<CPlotData::line_t> lines                  = data->lines;
    QList<CPlotData::line_t>::const_iterator line   = lines.begin();

    while(line != lines.end())
    {
        p.setPen(Qt::black);
        p.drawText(x + 30,y,line->label);
        p.setPen(pens[penIdx++]);
        p.drawLine(x, y, x + 20, y);

        y += fm.height();
        ++line;
    }
}

void IPlot::drawDecoration( QPainter &p )
{
    if(posMouse != NOPOINT)
    {
        // draw the vertical `you are here` line
        int x = posMouse.x();
        p.setPen(QPen(Qt::red, 2));
        if(x >= left && x <= right)
        {
            p.drawLine(x, top, x, bottom);

            // check if the mouse is near a waypoint
            foreach(const CPlotData::point_t& tag, data->tags)
            {
                int ptx = left + data->x().val2pt( tag.point.x() );

                if(qAbs(x - ptx) < 10)
                {
                    QFont f = CMainWindow::self().getMapFont();
                    f.setBold(true);
                    QFontMetrics fm(f);
                    QRect r = fm.boundingRect(tag.label);
                    r.moveCenter(QPoint(ptx, top - fm.height()/2 - fm.descent()));
                    r.adjust(-3,-2,3,0);

                    p.setPen(Qt::NoPen);
                    p.setBrush(Qt::white);
                    p.drawRoundedRect(r,3,3);

                    p.setFont(f);
                    p.setPen(Qt::darkBlue);
                    p.drawText(r, Qt::AlignCenter, tag.label);

                    break;
                }
            }
        }
    }

    if((idxSel1 != NOIDX) && (idxSel2 != NOIDX) && !data->badData)
    {
        int penIdx = 3;

        const QPolygonF& polyline = data->lines.first().points.mid(idxSel1, idxSel2 - idxSel1 + 1);
        QPolygonF line;
        getVisiblePolygon(polyline, line);

        // avoid drawing if the whole interval is outside the visible range
        if(!(line.first().x() >= right || line.last().x() <= left))
        {
            // draw the background
            p.setPen(Qt::NoPen);
            p.setBrush(colors[penIdx]);
            p.drawPolygon(line);

            // draw the foreground
            p.setPen(thinLine ? pensThin[penIdx] : pens[penIdx]);
            p.setBrush(Qt::NoBrush);
            line.pop_front();
            line.pop_back();
            p.drawPolyline(line);

            p.setPen(QPen(Qt::darkBlue, 2));
            p.drawLine(line.first().x(), top, line.first().x(), bottom);
            p.drawLine(line.last().x(),  top, line.last().x(),  bottom);
        }
    }

    if(!scrOptRange.isNull())
    {
        scrOptRange->draw(p);
    }
}

void IPlot::drawTags(QPainter& p)
{
    if(data->tags.isEmpty())
    {
        return;
    }

    CPlotAxis& xaxis = data->x();
    CPlotAxis& yaxis = data->y();

    QVector<CPlotData::point_t>::const_iterator tag = data->tags.begin();
    while(tag != data->tags.end())
    {
        int ptx = left   + xaxis.val2pt( tag->point.x() );
        int pty = bottom - yaxis.val2pt( tag->point.y() );

        if (left < ptx &&  ptx < right)
        {
            QPixmap icon = tag->icon.scaled(10,10, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            p.drawPixmap(ptx - icon.width() / 2, 2, icon);

            p.setPen(QPen(Qt::white, 3));
            if( 9 < pty)
            {
                if (pty > bottom)
                {
                    pty = bottom;
                }

                p.drawLine(ptx, top, ptx, pty);
                p.setPen(QPen(Qt::black, 1));
                p.drawLine(ptx, top, ptx, pty);
            }
        }
        ++tag;
    }
}

void IPlot::drawActivities(QPainter& p)
{
    if((mode == eModeIcon) || (trk->getActivities().getAllFlags() == 0))
    {
        return;
    }

    const QList<CActivityTrk::activity_range_t>& ranges = trk->getActivities().getActivityRanges();


    QRect rectClipping = QRect(0,0,right - left,22);
    p.save();
    p.translate(left, bottom - 22);
    p.setClipRect(rectClipping);
    p.setBrush(QColor(0,170,0,100));
    p.setPen(Qt::NoPen);
    p.drawRect(rectClipping);

    QRect rectIconFrame(0,0,20,20);
    QRect rectIcon(2,2,16,16);
    foreach(const CActivityTrk::activity_range_t& range, ranges)
    {
        int x1, x2;
        if(data->axisType == CPlotData::eAxisTime)
        {
            x1 = data->x().val2pt(range.t1);
            x2 = data->x().val2pt(range.t2);
        }
        else
        {
            x1 = data->x().val2pt(range.d1);
            x2 = data->x().val2pt(range.d2);
        }

        p.setPen(QPen(Qt::darkGreen,2));
        p.drawLine(x1, 0, x1, 20);
        p.drawLine(x2, 0, x2, 20);

        int d = (x2 - x1);
        if(d < 20)
        {
            continue;
        }

        int c = x1 + d/2;

        rectIconFrame.moveCenter(QPoint(c,10));
        p.setBrush(QColor(255, 255, 255, 100));
        p.drawRoundedRect(rectIconFrame, 3, 3);

        rectIcon.moveCenter(QPoint(c, 10));
        p.drawPixmap(rectIcon, QPixmap(range.icon));
    }

    p.restore();
}

void IPlot::save(QImage& image)
{
    resize(image.size());
    setSizes();
    buffer = QImage(image.size(), QImage::Format_ARGB32);
    draw();
    image = buffer;
}

void IPlot::slotContextMenu(const QPoint & point)
{
    QPoint p = mapToGlobal(point);

    actionResetZoom->setEnabled(isZoomed());
    actionStopRange->setEnabled((mouseClickState != eMouseClickIdle) && !(idxSel1 == NOIDX || idxSel2 == NOIDX));
    actionPrint->setEnabled(mouseClickState != eMouseClick2nd);

    menu->exec(p);
}

void IPlot::slotSave()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGraphPath", QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName( this, tr("Select output file"), path, "PNG Image (*.png)");

    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    if(fi.suffix().toLower() != "png")
    {
        filename += ".png";
    }

    QImage img(size(), QImage::Format_ARGB32);
    QPainter p;
    p.begin(&img);
    p.fillRect(rect(), QBrush(Qt::white));
    draw(p);
    p.end();

    img.save(filename);

    path = fi.absolutePath();
    cfg.setValue("Paths/lastGraphPath", path);
}


void IPlot::slotHidePoints()
{
    trk->hideSelectedPoints();
    slotStopRange();
}

void IPlot::slotShowPoints()
{
    trk->showSelectedPoints();
    slotStopRange();
}

void IPlot::slotActivity()
{
    trk->setActivity();
    slotStopRange();
}

void IPlot::slotCopy()
{
    trk->copySelectedPoints();
    slotStopRange();
}

void IPlot::slotStopRange()
{
    scrOptRange->deleteLater();
    trk->setMode(CGisItemTrk::eModeNormal, objectName());
    idxSel1 = idxSel2 = NOIDX;
    mouseClickState = eMouseClickIdle;

    emit sigMouseClickState(mouseClickState);

    // update canvas if visible
    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->update();
    }
}

void IPlot::slotResetZoom()
{
    data->x().resetZoom();
    setSizes();

    needsRedraw = true;
    update();
}


void IPlot::setMouseRangeFocus(const CGisItemTrk::trkpt_t * ptRange1, const CGisItemTrk::trkpt_t *ptRange2)
{
    if(nullptr == ptRange1 || nullptr == ptRange2)
    {
        idxSel1 = NOIDX;
        idxSel2 = NOIDX;
    }
    else
    {
        if(ptRange1->idxTotal < ptRange2->idxTotal)
        {
            while(ptRange1->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange1++;
            }
            while(ptRange2->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange2--;
            }
            idxSel1 = ptRange1->idxVisible;
            idxSel2 = ptRange2->idxVisible;
        }
        else
        {
            while(ptRange1->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange1--;
            }
            while(ptRange2->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange2++;
            }
            idxSel1 = ptRange2->idxVisible;
            idxSel2 = ptRange1->idxVisible;
        }
    }
    update();
}

bool IPlot::isZoomed() const
{
    qreal limMin, limMax, useMin, useMax;
    data->x().getLimits(limMin, limMax, useMin, useMax);

    return !((limMax - limMin) <= (useMax - useMin));
}
