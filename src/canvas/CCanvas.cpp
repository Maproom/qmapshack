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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "canvas/CCanvasSetup.h"
#include "GeoMath.h"
#include "map/CMapDraw.h"
#include "dem/CDemDraw.h"
#include "grid/CGrid.h"
#include "grid/CGridSetup.h"
#include "units/IUnit.h"
#include "mouse/CMouseNormal.h"
#include "mouse/CMouseMoveWpt.h"
#include "mouse/CMouseEditLine.h"
#include "gis/CGisWidget.h"
#include "gis/CGisDraw.h"
#include "gis/trk/CGisItemTrk.h"
#include "plot/CPlotProfile.h"


#include <QtWidgets>

QPen CCanvas::penBorderBlue(QColor(10,10,150,220),2);
QPen CCanvas::penBorderGray(Qt::lightGray,2);
QPen CCanvas::penBorderBlack(QColor(0,0,0,200),2);
QBrush CCanvas::brushBackWhite(QColor(255,255,255,255));
QBrush CCanvas::brushBackYellow(QColor(0xff, 0xff, 0xcc, 0xE0));


CCanvas::CCanvas(QWidget *parent)
    : QWidget(parent)
    , posFocus(12.00 * DEG_TO_RAD, 49.00 * DEG_TO_RAD)
    , plotTrackProfile(0)
{
    setFocusPolicy(Qt::WheelFocus);

    int count = 1;
    while(1)
    {
        QString name = tr("Workspace %1").arg(count);
        if(CMainWindow::self().findChild<CCanvas*>(name) == 0)
        {
            setObjectName(name);
            break;
        }
        count++;
    }

    setMouseTracking(true);

    map     = new CMapDraw(this);
    grid    = new CGrid(map);
    dem     = new CDemDraw(this);
    gis     = new CGisDraw(this);
    mouse   = new CMouseNormal(gis, this);

    connect(map, SIGNAL(sigCanvasUpdate(CCanvas::redraw_e)), this, SLOT(slotTriggerCompleteUpdate(CCanvas::redraw_e)));
    connect(dem, SIGNAL(sigCanvasUpdate(CCanvas::redraw_e)), this, SLOT(slotTriggerCompleteUpdate(CCanvas::redraw_e)));
    connect(gis, SIGNAL(sigCanvasUpdate(CCanvas::redraw_e)), this, SLOT(slotTriggerCompleteUpdate(CCanvas::redraw_e)));

    timerToolTip = new QTimer(this);
    timerToolTip->setSingleShot(true);
    connect(timerToolTip, SIGNAL(timeout()), this, SLOT(slotToolTip()));

    loadIndicator1 = new QMovie(this);
    loadIndicator1->setFileName("://animation/loader.gif");
    mapLoadIndicator = new QLabel(this);
    mapLoadIndicator->setMovie(loadIndicator1);
    loadIndicator1->start();
    mapLoadIndicator->show();

    loadIndicator2 = new QMovie(this);
    loadIndicator2->setFileName("://animation/loader2.gif");
    demLoadIndicator = new QLabel(this);
    demLoadIndicator->setMovie(loadIndicator2);
    loadIndicator2->start();
    demLoadIndicator->show();

    labelStatusMessages = new QLabel(this);


    connect(map, SIGNAL(sigStartThread()), mapLoadIndicator, SLOT(show()));
    connect(map, SIGNAL(sigStopThread()), mapLoadIndicator, SLOT(hide()));

    connect(dem, SIGNAL(sigStartThread()), demLoadIndicator, SLOT(show()));
    connect(dem, SIGNAL(sigStopThread()), demLoadIndicator, SLOT(hide()));

    timerTrackOnFocus = new QTimer(this);
    timerTrackOnFocus->setSingleShot(false);
    timerTrackOnFocus->start(1000);

    connect(timerTrackOnFocus, SIGNAL(timeout()), this, SLOT(slotCheckTrackOnFocus()));
}

CCanvas::~CCanvas()
{

}

void CCanvas::saveConfig(QSettings& cfg)
{
    map->saveConfig(cfg);
    dem->saveConfig(cfg);
    grid->saveConfig(cfg);
    cfg.setValue("posFocus", posFocus);
    cfg.setValue("proj", map->getProjection());
}

void CCanvas::loadConfig(QSettings& cfg)
{
    map->loadConfig(cfg);    
    dem->loadConfig(cfg);
    grid->loadConfig(cfg);
    posFocus = cfg.value("posFocus", posFocus).toPointF();
    setProjection(cfg.value("proj", map->getProjection()).toString());

    dem->zoom(map->zoom());
    gis->zoom(map->zoom());
}

void CCanvas::resetMouse()
{
    mouse->deleteLater();
    mouse = new CMouseNormal(gis, this);
    if(underMouse())
    {
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(*mouse);
    }
}

void CCanvas::setMouseMoveWpt(CGisItemWpt& wpt)
{
    mouse->deleteLater();
    mouse = new CMouseMoveWpt(wpt, gis, this);
    if(underMouse())
    {
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(*mouse);
    }
}

void CCanvas::setMouseEditLine(CGisItemTrk& trk)
{
    mouse->deleteLater();
    mouse = new CMouseEditLine(trk, gis, this);
    if(underMouse())
    {
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(*mouse);
    }
}


void CCanvas::reportStatus(const QString& key, const QString& msg)
{
    if(msg.isEmpty())
    {
        statusMessages.remove(key);
    }
    else
    {
        statusMessages[key] = msg;
    }

    QString report;
    QStringList keys = statusMessages.keys();
    keys.sort();
    foreach(const QString& key, keys)
    {
        report += statusMessages[key] + "\n";
    }

    if(report.isEmpty())
    {
        labelStatusMessages->hide();
    }
    else
    {
        labelStatusMessages->show();
        labelStatusMessages->setText(report);
        labelStatusMessages->adjustSize();
        update();
    }
}

void CCanvas::resizeEvent(QResizeEvent * e)
{
    needsRedraw = eRedrawAll;

    QSize s = e->size();
    if(map) map->resize(s);
    if(dem) dem->resize(s);
    if(gis) gis->resize(s);

    QWidget::resizeEvent(e);

    // move map loading indicator to new center of canvas
    QPoint p1(mapLoadIndicator->width()>>1, mapLoadIndicator->height()>>1);
    mapLoadIndicator->move(rect().center() - p1);

    QPoint p2(demLoadIndicator->width()>>1, demLoadIndicator->height()>>1);
    demLoadIndicator->move(rect().center() - p2);

    labelStatusMessages->move(20,20);

    if(plotTrackProfile)
    {
        if(s.height() < 700)
        {
            plotTrackProfile->resize(200,80);
        }
        else
        {
            plotTrackProfile->resize(300,120);
        }

        plotTrackProfile->move(20, height() - plotTrackProfile->height() - 20);
    }
}

void CCanvas::paintEvent(QPaintEvent * e)
{
    Q_UNUSED(e);

    if(!isVisible())
    {
        return;
    }

    const QRectF& r = e->rect();

    QPainter p;
    p.begin(this);
    USE_ANTI_ALIASING(p,true);

    // fill the backbround with default pattern
    p.fillRect(rect(), "#FFFFBF");

    // ----- start to draw thread based content -----
    // move coordinate system to center of the screen
    p.translate(width() >> 1, height() >> 1);

    map->draw(p, needsRedraw, posFocus, r);
    dem->draw(p, needsRedraw, posFocus, r);
    gis->draw(p, needsRedraw, posFocus, r);   

    // restore coordinate system to default
    p.resetTransform();
    // ----- start to draw fast content -----

    grid->draw(p, rect());
    mouse->draw(p, needsRedraw, rect());
    gis->draw(p, rect());

    drawStatusMessages(p);
    drawScale(p);

    p.end();
    needsRedraw = eRedrawNone;

}

void CCanvas::mousePressEvent(QMouseEvent * e)
{
    mouse->mousePressEvent(e);
    QWidget::mousePressEvent(e);
}

void CCanvas::mouseMoveEvent(QMouseEvent * e)
{
    qreal ele = NOFLOAT;
    QPointF pos = e->pos();
    map->convertPx2Rad(pos);
    ele = dem->getElevationAt(pos);
    emit sigMousePosition(pos * RAD_TO_DEG, ele);

    mouse->mouseMoveEvent(e);
    QWidget::mouseMoveEvent(e);
}

void CCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    mouse->mouseReleaseEvent(e);
    QWidget::mouseReleaseEvent(e);
}

void CCanvas::wheelEvent(QWheelEvent * e)
{
    mouse->wheelEvent(e);

    QPointF pos = e->posF();
    QPointF pt1 = pos;

    map->convertPx2Rad(pt1);
    setZoom(CMainWindow::self().flipMouseWheel() ? (e->delta() < 0) : (e->delta() > 0), needsRedraw);
    map->convertRad2Px(pt1);

    map->convertRad2Px(posFocus);
    posFocus -= (pos - pt1);
    map->convertPx2Rad(posFocus);

    update();
}


void CCanvas::enterEvent(QEvent * e)
{
    Q_UNUSED(e);
    QApplication::setOverrideCursor(*mouse);

    mouse->setMouseTracking(true);
}


void CCanvas::leaveEvent(QEvent * e)
{
    Q_UNUSED(e);
    QApplication::restoreOverrideCursor();

    mouse->setMouseTracking(false);
}

void CCanvas::keyPressEvent(QKeyEvent * e)
{
    qDebug() << hex << e->key();
    bool doUpdate = false;

    if(e->key() == Qt::Key_Plus)
    {
        setZoom(true, needsRedraw);
        doUpdate = true;
    }
    else if(e->key() == Qt::Key_Minus)
    {
        setZoom(false, needsRedraw);
        doUpdate = true;
    }
    else if(e->key() == Qt::Key_Up)
    {
        moveMap(QPointF(0, height()/4));
    }
    else if(e->key() == Qt::Key_Down)
    {
        moveMap(QPointF(0, -height()/4));
    }
    else if(e->key() == Qt::Key_Left)
    {
        moveMap(QPointF(width()/4, 0));
    }
    else if(e->key() == Qt::Key_Right)
    {
        moveMap(QPointF(-width()/4, 0));
    }


    if(doUpdate)
    {
        e->accept();
        update();
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}


void CCanvas::drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color)
{
    CCanvas::drawText(str,p,center, color, CMainWindow::self().getMapFont());
}


void CCanvas::drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color, const QFont& font)
{

    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(str);

    r.moveCenter(center);

    p.setPen(Qt::white);
    p.setFont(font);

    p.drawText(r.topLeft() - QPoint(-1,-1), str);
    p.drawText(r.topLeft() - QPoint( 0,-1), str);
    p.drawText(r.topLeft() - QPoint(+1,-1), str);

    p.drawText(r.topLeft() - QPoint(-1, 0), str);
    p.drawText(r.topLeft() - QPoint(+1, 0), str);

    p.drawText(r.topLeft() - QPoint(-1,+1), str);
    p.drawText(r.topLeft() - QPoint( 0,+1), str);
    p.drawText(r.topLeft() - QPoint(+1,+1), str);

    p.setPen(color);
    p.drawText(r.topLeft(),str);

}


void CCanvas::drawText(const QString& str, QPainter& p, const QRect& r, const QColor& color)
{

    p.setPen(Qt::white);
    p.setFont(CMainWindow::self().getMapFont());

    p.drawText(r.adjusted(-1,-1,-1,-1),Qt::AlignCenter,str);
    p.drawText(r.adjusted( 0,-1, 0,-1),Qt::AlignCenter,str);
    p.drawText(r.adjusted(+1,-1,+1,-1),Qt::AlignCenter,str);

    p.drawText(r.adjusted(-1, 0,-1, 0),Qt::AlignCenter,str);
    p.drawText(r.adjusted(+1, 0,+1, 0),Qt::AlignCenter,str);

    p.drawText(r.adjusted(-1,+1,-1,+1),Qt::AlignCenter,str);
    p.drawText(r.adjusted( 0,+1, 0,+1),Qt::AlignCenter,str);
    p.drawText(r.adjusted(+1,+1,+1,+1),Qt::AlignCenter,str);

    p.setPen(color);
    p.drawText(r,Qt::AlignCenter,str);

}

void CCanvas::drawStatusMessages(QPainter& p)
{
    if(labelStatusMessages->isVisible())
    {
        QRect r = labelStatusMessages->rect();
        r.adjust(-5, -5, 5, 5);
        r.moveTopLeft(QPoint(15,15));
        p.setPen(CCanvas::penBorderGray);
        p.setBrush(CCanvas::brushBackWhite);
        p.drawRoundedRect(r, 5, 5);
    }
}

void CCanvas::drawScale(QPainter& p)
{

    if(!CMainWindow::self().isScaleVisible())
    {
        return;
    }


    // step I: get the approximate distance for 200px in the bottom right corner
    QPointF brc(rect().bottomRight() - QPoint(50,30));
    QPointF pt1 = brc;
    QPointF pt2 = brc - QPoint(-200,0);

    map->convertPx2Rad(pt1);
    map->convertPx2Rad(pt2);

    qreal d = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y());

    // step II: derive the actual scale length in [m]
    qreal a = (int)log10(d);
    qreal b = log10(d) - a;

    if(0 <= b && b < log10(3.0f))
    {
        d = 1 * pow(10,a);
    }
    else if(log10(3.0f) < b && b < log10(5.0f))
    {
        d = 3 * pow(10,a);
    }
    else
    {
        d = 5 * pow(10,a);
    }

    // step III: convert the scale length from [m] into [px]
    pt1 = brc;
    map->convertPx2Rad(pt1);
    pt2 = GPS_Math_Wpt_Projection(pt1, d, -90 * DEG_TO_RAD);

    map->convertRad2Px(pt1);
    map->convertRad2Px(pt2);

    p.setPen(QPen(Qt::white, 9));
    p.drawLine(pt1, pt2 + QPoint(9,0));
    p.setPen(QPen(Qt::black, 7));
    p.drawLine(pt1, pt2 + QPoint(9,0));
    p.setPen(QPen(Qt::white, 5));
    p.drawLine(pt1, pt2 + QPoint(9,0));

    QVector<qreal> pattern;
    pattern << 2 << 4;
    QPen pen(Qt::black, 5, Qt::CustomDashLine);
    pen.setDashPattern(pattern);
    p.setPen(pen);
    p.drawLine(pt1, pt2 + QPoint(9,0));


    QPoint pt3(pt2.x() + (pt1.x() - pt2.x())/2, pt2.y());

    QString val, unit;
    IUnit::self().meter2distance(d,val,unit);
    drawText(QString("%1 %2").arg(val).arg(unit), p, pt3, Qt::black);
}

void CCanvas::slotTriggerCompleteUpdate(CCanvas::redraw_e flags)
{
    needsRedraw = (redraw_e)(needsRedraw | flags);
    update();
}


void CCanvas::slotToolTip()
{
    QString str;
    map->getToolTip(posToolTip, str);
    if(str.isEmpty())
    {
        return;
    }
    QPoint p = mapToGlobal(posToolTip + QPoint(32,0));
    QToolTip::showText(p,str);
}

void CCanvas::slotCheckTrackOnFocus()
{
    const QString& key = CGisItemTrk::getKeyUserFocus();

    // any changes?
    if(key != keyTrackOnFocus)
    {
        // get access to current track object
        delete plotTrackProfile;
        keyTrackOnFocus.clear();

        // get access to next track object
        CGisItemTrk * trk2 = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
        if(trk2 == 0)
        {
            return;
        }

        // create new profile plot, the plot will register itself at the track
        plotTrackProfile = new CPlotProfile(trk2, IPlot::eModeIcon, this);
        if(height() < 700)
        {
            plotTrackProfile->resize(200,80);
        }
        else
        {
            plotTrackProfile->resize(300,120);
        }
        plotTrackProfile->move(20, height() - plotTrackProfile->height() - 20);
        plotTrackProfile->show();

        // finally store the new key as track on focus
        keyTrackOnFocus = key;
    }
}

void CCanvas::moveMap(const QPointF& delta)
{
    map->convertRad2Px(posFocus);
    posFocus -= delta;
    map->convertPx2Rad(posFocus);

    slotTriggerCompleteUpdate(eRedrawAll);
}

void CCanvas::zoomTo(const QRectF& rect)
{
    posFocus = rect.center();
    map->zoom(rect);
    dem->zoom(map->zoom());
    gis->zoom(map->zoom());
    slotTriggerCompleteUpdate(eRedrawAll);
}

void CCanvas::setupGrid()
{
    CGridSetup dlg(grid, map);
    dlg.exec();
    update();
}

void CCanvas::convertGridPos2Str(const QPointF& pos, QString& str)
{
    grid->convertPos2Str(pos, str);
}

void CCanvas::displayInfo(const QPoint& px)
{
    if(CMainWindow::self().isMapToolTip())
    {
        posToolTip = px;

        timerToolTip->stop();
        timerToolTip->start(500);
    }
    QToolTip::hideText();
}

void CCanvas::setup()
{
    CCanvasSetup dlg(this);
    dlg.exec();
}

QString CCanvas::getProjection()
{
    return map->getProjection();
}

void CCanvas::setProjection(const QString& proj)
{
    map->setProjection(proj);
    dem->setProjection(proj);
    gis->setProjection(proj);
}

qreal CCanvas::getElevationAt(const QPointF& pos)
{
    return dem->getElevationAt(pos);
}

void CCanvas::getElevationAt(const QPolygonF& pos, QPolygonF& ele)
{
    return dem->getElevationAt(pos, ele);
}

void CCanvas::setZoom(bool in, redraw_e& needsRedraw)
{
    map->zoom(in, needsRedraw);
    dem->zoom(map->zoom());
    gis->zoom(map->zoom());
}
