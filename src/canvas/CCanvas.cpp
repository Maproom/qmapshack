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
#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "canvas/CCanvasSetup.h"
#include "dem/CDemDraw.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/IGisLine.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/trk/CGisItemTrk.h"
#include "grid/CGrid.h"
#include "grid/CGridSetup.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "map/CMapDraw.h"
#include "mouse/CMouseEditArea.h"
#include "mouse/CMouseEditRte.h"
#include "mouse/CMouseEditTrk.h"
#include "mouse/CMouseMoveWpt.h"
#include "mouse/CMouseNormal.h"
#include "mouse/CMousePrint.h"
#include "mouse/CMouseRangeTrk.h"
#include "mouse/CMouseWptBubble.h"
#include "plot/CPlotProfile.h"
#include "units/IUnit.h"
#include "widgets/CColorLegend.h"

#include <QtWidgets>


CCanvas::CCanvas(QWidget *parent, const QString &name)
    : QWidget(parent)
{
    setFocusPolicy(Qt::WheelFocus);

    if(name.isEmpty())
    {
        int count = 1;
        while(1)
        {
            QString name = tr("View %1").arg(count);
            if(nullptr == CMainWindow::self().findChild<CCanvas*>(name))
            {
                setObjectName(name);
                break;
            }
            count++;
        }
    }
    else
    {
        setObjectName(name);
    }

    setMouseTracking(true);

    map     = new CMapDraw(this);
    grid    = new CGrid(map);
    dem     = new CDemDraw(this);
    gis     = new CGisDraw(this);
    mouse   = new CMouseNormal(gis, this);

    connect(map, &CMapDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);
    connect(dem, &CDemDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);
    connect(gis, &CGisDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);

    timerToolTip = new QTimer(this);
    timerToolTip->setSingleShot(true);
    connect(timerToolTip, &QTimer::timeout, this, &CCanvas::slotToolTip);

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
    labelStatusMessages->setWordWrap(true);
    labelStatusMessages->setMinimumWidth(300);
    labelStatusMessages->setAlignment(Qt::AlignJustify);
    labelStatusMessages->hide();

    connect(map, &CMapDraw::sigStartThread, mapLoadIndicator, &QLabel::show);
    connect(map, &CMapDraw::sigStopThread,  mapLoadIndicator, &QLabel::hide);

    connect(dem, &CDemDraw::sigStartThread, demLoadIndicator, &QLabel::show);
    connect(dem, &CDemDraw::sigStopThread,  demLoadIndicator, &QLabel::hide);

    timerTrackOnFocus = new QTimer(this);
    timerTrackOnFocus->setSingleShot(false);
    timerTrackOnFocus->start(1000);

    connect(timerTrackOnFocus, &QTimer::timeout, this, &CCanvas::slotCheckTrackOnFocus);
}

CCanvas::~CCanvas()
{
    /*
        Some mouse objects call methods from their canvas on destruction.
        So they are better deleted now explicitly before any other object
        in CCanvas is destroyed.
     */
    delete mouse;
    saveSizeTrackProfile();
}

void CCanvas::setOverrideCursor(const QCursor& cursor, const QString& src)
{
//    qDebug() << "setOverrideCursor" << src;
    QApplication::setOverrideCursor(cursor);
}

void CCanvas::restoreOverrideCursor(const QString& src)
{
//    qDebug() << "restoreOverrideCursor" << src;
    QApplication::restoreOverrideCursor();
}

void CCanvas::changeOverrideCursor(const QCursor& cursor, const QString &src)
{
//    qDebug() << "changeOverrideCursor" << src;
    QApplication::changeOverrideCursor(cursor);
}

void CCanvas::saveConfig(QSettings& cfg)
{
    map->saveConfig(cfg);
    dem->saveConfig(cfg);
    grid->saveConfig(cfg);
    cfg.setValue("posFocus",  posFocus);
    cfg.setValue("proj",      map->getProjection());
    cfg.setValue("scales",    map->getScalesType());
    cfg.setValue("backColor", backColor.name());
}

void CCanvas::loadConfig(QSettings& cfg)
{
    posFocus = cfg.value("posFocus", posFocus).toPointF();
    setProjection(cfg.value("proj", map->getProjection()).toString());
    setScales((CCanvas::scales_type_e)cfg.value("scales",  map->getScalesType()).toInt());

    const QString &backColorStr = cfg.value("backColor", "#FFFFBF").toString();
    backColor = QColor(backColorStr);

    map->loadConfig(cfg);
    dem->loadConfig(cfg);
    grid->loadConfig(cfg);

    dem->zoom(map->zoom());
    gis->zoom(map->zoom());
}

void CCanvas::resetMouse()
{
    mouse->deleteLater();
    mouse = new CMouseNormal(gis, this);
    if(underMouse())
    {
        while(QApplication::overrideCursor())
        {
            CCanvas::restoreOverrideCursor("resetMouse");
        }
        CCanvas::setOverrideCursor(*mouse, "resetMouse");
    }
}

void CCanvas::setMouseMoveWpt(CGisItemWpt& wpt)
{
    mouse->deleteLater();
    mouse = new CMouseMoveWpt(wpt, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseMoveWpt");
        CCanvas::setOverrideCursor(*mouse, "setMouseMoveWpt");
    }
}

void CCanvas::setMouseEditTrk(const QPointF &pt)
{
    mouse->deleteLater();
    mouse = new CMouseEditTrk(pt, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseEditTrk");
        CCanvas::setOverrideCursor(*mouse, "setMouseEditTrk");
    }
}

void CCanvas::setMouseEditRte(const QPointF &pt)
{
    mouse->deleteLater();
    mouse = new CMouseEditRte(pt, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseEditRte");
        CCanvas::setOverrideCursor(*mouse, "setMouseEditRte");
    }
}

void CCanvas::setMouseEditTrk(CGisItemTrk& trk)
{
    mouse->deleteLater();
    mouse = new CMouseEditTrk(trk, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseEditTrk");
        CCanvas::setOverrideCursor(*mouse, "setMouseEditTrk");
    }
}

void CCanvas::setMouseRangeTrk(CGisItemTrk& trk)
{
    mouse->deleteLater();
    mouse = new CMouseRangeTrk(trk, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseRangeTrk");
        CCanvas::setOverrideCursor(*mouse, "setMouseRangeTrk");
    }
}

void CCanvas::setMouseEditArea(const QPointF& pt)
{
    mouse->deleteLater();
    mouse = new CMouseEditArea(pt, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseEditArea");
        CCanvas::setOverrideCursor(*mouse, "setMouseEditArea");
    }
}

void CCanvas::setMouseEditArea(CGisItemOvlArea& area)
{
    mouse->deleteLater();
    mouse = new CMouseEditArea(area, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseEditArea");
        CCanvas::setOverrideCursor(*mouse, "setMouseEditArea");
    }
}

void CCanvas::setMouseEditRte(CGisItemRte& rte)
{
    mouse->deleteLater();
    mouse = new CMouseEditRte(rte, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseEditRte");
        CCanvas::setOverrideCursor(*mouse, "setMouseEditRte");
    }
}

void CCanvas::setMouseWptBubble(const IGisItem::key_t& key)
{
    mouse->deleteLater();
    mouse = new CMouseWptBubble(key, gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMouseWptBubble");
        CCanvas::setOverrideCursor(*mouse, "setMouseWptBubble");
    }
}

void CCanvas::setMousePrint()
{
    mouse->deleteLater();
    mouse = new CMousePrint(gis, this);
    if(underMouse())
    {
        CCanvas::restoreOverrideCursor("setMousePrint");
        CCanvas::setOverrideCursor(*mouse, "setMousePrint");
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
    foreach(const QString &key, keys)
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
    }
    update();
}

void CCanvas::resizeEvent(QResizeEvent * e)
{
    needsRedraw = eRedrawAll;

    setDrawContextSize(e->size());
    QWidget::resizeEvent(e);

    // move map loading indicator to new center of canvas
    QPoint p1(mapLoadIndicator->width()>>1, mapLoadIndicator->height()>>1);
    mapLoadIndicator->move(rect().center() - p1);

    QPoint p2(demLoadIndicator->width()>>1, demLoadIndicator->height()>>1);
    demLoadIndicator->move(rect().center() - p2);

    labelStatusMessages->move(20,50);

    setSizeTrackProfile();
}

void CCanvas::paintEvent(QPaintEvent * e)
{
    Q_UNUSED(e);

    if(!isVisible())
    {
        return;
    }

    QPainter p;
    p.begin(this);
    USE_ANTI_ALIASING(p,true);

    // fill the background with default pattern
    p.fillRect(rect(), backColor);

    // ----- start to draw thread based content -----
    // move coordinate system to center of the screen
    p.translate(width() >> 1, height() >> 1);

    map->draw(p, needsRedraw, posFocus);
    dem->draw(p, needsRedraw, posFocus);
    gis->draw(p, needsRedraw, posFocus);

    // restore coordinate system to default
    p.resetTransform();
    // ----- start to draw fast content -----

    grid->draw(p, rect());
    if(map->isFinished() && dem->isFinished() && gis->isFinished())
    {
        gis->draw(p, rect());
    }
    mouse->draw(p, needsRedraw, rect());


    drawStatusMessages(p);
    drawScale(p);

    p.end();
    needsRedraw = eRedrawNone;
}

void CCanvas::mousePressEvent(QMouseEvent * e)
{
    if(!mousePressMutex.tryLock())
    {
        return;
    }

    mouse->mousePressEvent(e);
    QWidget::mousePressEvent(e);
    e->accept();

    mousePressMutex.unlock();
}

void CCanvas::mouseMoveEvent(QMouseEvent * e)
{
    QPointF pos = e->pos();
    map->convertPx2Rad(pos);
    qreal ele = dem->getElevationAt(pos);
    emit sigMousePosition(pos * RAD_TO_DEG, ele);

    mouse->mouseMoveEvent(e);
    QWidget::mouseMoveEvent(e);
    e->accept();
}

void CCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    mouse->mouseReleaseEvent(e);
    QWidget::mouseReleaseEvent(e);
    e->accept();
}

void CCanvas::mouseDoubleClickEvent(QMouseEvent * e)
{
    mouse->mouseDoubleClickEvent(e);
    QWidget::mouseDoubleClickEvent(e);
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
    CCanvas::setOverrideCursor(*mouse, "enterEvent");

    mouse->setMouseTracking(true);
}


void CCanvas::leaveEvent(QEvent * e)
{
    Q_UNUSED(e);
    // bad hack to stop bad number of override cursors.
    while(QApplication::overrideCursor())
    {
        CCanvas::restoreOverrideCursor("leaveEvent");
    }

    mouse->setMouseTracking(false);
}

void CCanvas::keyPressEvent(QKeyEvent * e)
{
    qDebug() << hex << e->key();
    bool doUpdate = true;

    switch(e->key())
    {
    case Qt::Key_Plus:
        setZoom(true, needsRedraw);
        break;

    case Qt::Key_Minus:
        setZoom(false, needsRedraw);
        break;

    /* move the map with keys up, down, left and right */
    case Qt::Key_Up:
        moveMap(QPointF(         0,  height()/4));
        break;

    case Qt::Key_Down:
        moveMap(QPointF(         0, -height()/4));
        break;

    case Qt::Key_Left:
        moveMap(QPointF( width()/4,           0));
        break;

    case Qt::Key_Right:
        moveMap(QPointF(-width()/4,           0));
        break;

    case Qt::Key_Escape:
    {
        IMouseEditLine *lineMouse = dynamic_cast<IMouseEditLine*>(mouse);
        if(nullptr != lineMouse)
        {
            lineMouse->abortStep();
        }
        else
        {
            doUpdate = false;
        }
        break;
    }

    default:
        doUpdate = false;
    }

    if(doUpdate)
    {
        mouse->keyPressEvent(e);
        e->accept();
        update();
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}

void CCanvas::drawStatusMessages(QPainter& p)
{
    if(labelStatusMessages->isVisible())
    {
        QRect r = labelStatusMessages->rect();
        r.adjust(-5, -5, 5, 5);
        r.moveTopLeft(QPoint(15,45));
        p.setPen(CDraw::penBorderGray);
        p.setBrush(CDraw::brushBackWhite);
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
        d = 1 * qPow(10,a);
    }
    else if(log10(3.0f) < b && b < log10(5.0f))
    {
        d = 3 * qPow(10,a);
    }
    else
    {
        d = 5 * qPow(10,a);
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
    CDraw::text(QString("%1 %2").arg(val).arg(unit), p, pt3, Qt::black);
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
    const IGisItem::key_t& key = CGisItemTrk::getKeyUserFocus();

    // any changes?
    if(key != keyTrackOnFocus)
    {
        saveSizeTrackProfile();
        // get access to current track object
        delete plotTrackProfile;
        delete colorLegend;
        keyTrackOnFocus.clear();

        // get access to next track object
        CGisItemTrk * trk2 = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
        if(nullptr == trk2)
        {
            return;
        }

        // create new profile plot, the plot will register itself at the track
        plotTrackProfile = new CPlotProfile(trk2, trk2->limitsGraph1, CMainWindow::self().profileIsWindow() ? IPlot::eModeWindow : IPlot::eModeIcon, this);
        setSizeTrackProfile();
        if(isVisible())
        {
            plotTrackProfile->show();
        }

        colorLegend = new CColorLegend(this, trk2);
        colorLegend->setGeometry(20, 20, 40, 300);

        // finally store the new key as track on focus
        keyTrackOnFocus = key;
    }
}

void CCanvas::moveMap(const QPointF& delta)
{
    map->convertRad2Px(posFocus);
    posFocus -= delta;
    map->convertPx2Rad(posFocus);

    emit sigMove();

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

void CCanvas::setupBackgroundColor()
{
    QColorDialog::setCustomColor(0, "#FFFFBF");
    const QColor &selected = QColorDialog::getColor(backColor, this, "Test");

    if(selected.isValid())
    {
        backColor = selected;
        update();
    }
}

void CCanvas::convertGridPos2Str(const QPointF& pos, QString& str, bool simple)
{
    grid->convertPos2Str(pos, str, simple);
}

void CCanvas::convertRad2Px(QPointF& pos)
{
    map->convertRad2Px(pos);
}

void CCanvas::convertPx2Rad(QPointF& pos)
{
    map->convertPx2Rad(pos);
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
void CCanvas::setScales(const scales_type_e type)
{
    map->setScales(type);
    dem->setScales(type);
    gis->setScales(type);
}

CCanvas::scales_type_e CCanvas::getScalesType()
{
    return map->getScalesType();
}


qreal CCanvas::getElevationAt(const QPointF& pos)
{
    return dem->getElevationAt(pos);
}

void CCanvas::getElevationAt(const QPolygonF& pos, QPolygonF& ele)
{
    return dem->getElevationAt(pos, ele);
}

void CCanvas::getElevationAt(SGisLine& line)
{
    return dem->getElevationAt(line);
}

void CCanvas::setZoom(bool in, redraw_e& needsRedraw)
{
    map->zoom(in, needsRedraw);
    dem->zoom(map->zoom());
    gis->zoom(map->zoom());

    emit sigZoom();
}

bool CCanvas::findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline)
{
    return map->findPolylineCloseBy(pt1, pt2, threshold, polyline);
}

void CCanvas::saveSizeTrackProfile()
{
    if(plotTrackProfile.isNull())
    {
        return;
    }

    if(plotTrackProfile->windowFlags() & Qt::Window)
    {
        SETTINGS;
        cfg.beginGroup("Canvas");
        cfg.beginGroup("Profile");
        cfg.beginGroup(objectName());

        cfg.setValue("geometry", plotTrackProfile->saveGeometry());

        cfg.endGroup(); // objectName()
        cfg.endGroup(); // Profile
        cfg.endGroup(); // Canvas
    }
}

void CCanvas::setSizeTrackProfile()
{
    if(plotTrackProfile.isNull())
    {
        return;
    }

    if(plotTrackProfile->windowFlags() & Qt::Window)
    {
        SETTINGS;
        cfg.beginGroup("Canvas");
        cfg.beginGroup("Profile");
        cfg.beginGroup(objectName());

        if(cfg.contains("geometry"))
        {
            plotTrackProfile->restoreGeometry(cfg.value("geometry").toByteArray());
        }
        else
        {
            plotTrackProfile->resize(300,200);
            plotTrackProfile->move(100,100);
        }

        cfg.endGroup(); // objectName()
        cfg.endGroup(); // Profile
        cfg.endGroup(); // Canvas
    }
    else
    {
        if(size().height() < 700)
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

void CCanvas::showProfileAsWindow(bool yes)
{
    if(plotTrackProfile)
    {
        const IGisItem::key_t key = CGisItemTrk::getKeyUserFocus();

        delete plotTrackProfile;
        keyTrackOnFocus.clear();

        CGisWidget::self().focusTrkByKey(true, key);
    }
}

void CCanvas::showProfile(bool yes)
{
    if(plotTrackProfile)
    {
        if(yes)
        {
            plotTrackProfile->show();
        }
        else
        {
            plotTrackProfile->hide();
        }
    }
}

void CCanvas::setDrawContextSize(const QSize& s)
{
    if(map)
    {
        map->resize(s);
    }
    if(dem)
    {
        dem->resize(s);
    }
    if(gis)
    {
        gis->resize(s);
    }
}

void CCanvas::print(QPainter& p, const QRectF& area, const QPointF& focus)
{
    const QSize oldSize = size();
    const QSize newSize(area.size().toSize());

    setDrawContextSize(newSize);

    // ----- start to draw thread based content -----
    // move coordinate system to center of the screen
    p.translate(newSize.width() >> 1, newSize.height() >> 1);

    redraw_e redraw = eRedrawAll;

    map->draw(p, redraw, focus);
    dem->draw(p, redraw, focus);
    gis->draw(p, redraw, focus);

    map->wait();
    dem->wait();
    gis->wait();

    map->draw(p, redraw, focus);
    dem->draw(p, redraw, focus);
    gis->draw(p, redraw, focus);

    // restore coordinate system to default
    p.resetTransform();
    // ----- start to draw fast content -----

    QRect r(QPoint(0,0), area.size().toSize());

    grid->draw(p, r);
    gis->draw(p, r);

    setDrawContextSize(oldSize);
}
