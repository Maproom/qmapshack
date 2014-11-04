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
#include "helpers/CSettings.h"
#include "canvas/CCanvas.h"
#include "GeoMath.h"
#include "map/CMapDraw.h"
#include "map/CMapList.h"
#include "dem/CDemList.h"
#include "dem/CDemDraw.h"
#include "units/IUnit.h"
#include "units/CTimeZoneSetup.h"
#include "units/CUnitsSetup.h"
#include "version.h"
#include "config.h"
#include "CAbout.h"
#include "gis/CGisWidget.h"
#include "gis/WptIcons.h"

#include <QtGui>
#include <QtWidgets>
#include <QtSql>

CMainWindow * CMainWindow::pSelf = 0;

CMainWindow::CMainWindow()
{
    SETTINGS;

    pSelf = this;
    setupUi(this);
    setWindowTitle(WHAT_STR);

    initWptIcons();

    IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt(), this);

    QString path = cfg.value("Paths/database", QDir::home().filePath(CONFIGDIR).append("/qms.db")).toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    gisWidget = new CGisWidget(menuProject, this);
    dockGis->setWidget(gisWidget);

    // start ---- restore window geometry -----
    if ( cfg.contains("MainWindow/geometry"))
    {
        restoreGeometry(cfg.value("MainWindow/geometry").toByteArray());
    }
    else
    {
        setGeometry(0,0,800,600);
    }

    if ( cfg.contains("MainWindow/state"))
    {
        restoreState(cfg.value("MainWindow/state").toByteArray());
    }
    // end ---- restore window geometry -----


    connect(actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
    connect(actionHelp, SIGNAL(triggered()), this, SLOT(slotHelp()));
    connect(actionAddMapWorkspace, SIGNAL(triggered()), this, SLOT(slotAddCanvas()));
    connect(actionShowScale, SIGNAL(changed()), this, SLOT(slotUpdateCurrentWidget()));
    connect(actionShowGrid, SIGNAL(changed()), this, SLOT(update()));
    connect(actionPOIText, SIGNAL(changed()), this, SLOT(slotUpdateCurrentWidget()));
    connect(actionMapToolTip, SIGNAL(changed()), this, SLOT(slotUpdateCurrentWidget()));
    connect(actionNightDay, SIGNAL(changed()), this, SLOT(slotUpdateCurrentWidget()));
    connect(actionSetupMapFont, SIGNAL(triggered()), this, SLOT(slotSetupMapFont()));
    connect(actionSetupGrid, SIGNAL(triggered()), this, SLOT(slotSetupGrid()));
    connect(actionSetupMapPaths, SIGNAL(triggered()), this, SLOT(slotSetupMapPath()));
    connect(actionSetupDEMPaths, SIGNAL(triggered()), this, SLOT(slotSetupDemPath()));
    connect(actionSetupMapWks, SIGNAL(triggered()), this, SLOT(slotSetupMapWks()));
    connect(actionSetupTimeZone, SIGNAL(triggered()), this, SLOT(slotSetupTimeZone()));
    connect(actionSetupUnits, SIGNAL(triggered()), this, SLOT(slotSetupUnits()));
    connect(actionSaveGISData, SIGNAL(triggered()), gisWidget, SLOT(slotSaveAll()));
    connect(actionLoadGISData, SIGNAL(triggered()), this, SLOT(slotLoadGISData()));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(slotTabCloseRequest(int)));


    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabCanvas(int)));
    connect(tabMaps, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabMaps(int)));
    connect(tabDem, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabDem(int)));

    cfg.beginGroup("Canvas");
    CMapDraw::loadMapPath(cfg);
    CDemDraw::loadDemPath(cfg);
    int N = cfg.value("numberOfCanvas").toInt();
    for(int i = 0; i < N; i++)
    {
        CCanvas * canvas = new CCanvas(tabWidget);
        tabWidget->addTab(canvas, canvas->objectName());
        connect(canvas, SIGNAL(sigMousePosition(QPointF,qreal)), this, SLOT(slotMousePosition(QPointF, qreal)));

        cfg.beginGroup(QString("Canvas%1").arg(i));
        canvas->loadConfig(cfg);
        cfg.endGroup();
    }
    if(N == 0)
    {
        CCanvas * canvas = new CCanvas(tabWidget);
        tabWidget->addTab(canvas, canvas->objectName());
        connect(canvas, SIGNAL(sigMousePosition(QPointF, qreal)), this, SLOT(slotMousePosition(QPointF, qreal)));
    }

    actionShowScale->setChecked(cfg.value("isScaleVisible", true).toBool());
    actionShowGrid->setChecked(cfg.value("isGridVisible", true).toBool());
    actionPOIText->setChecked(cfg.value("POIText", true).toBool());
    actionMapToolTip->setChecked(cfg.value("MapToolTip", true).toBool());
    actionNightDay->setChecked(cfg.value("isNight", false).toBool());
    actionFlipMouseWheel->setChecked(cfg.value("flipMouseWheel", false).toBool());
    mapFont = cfg.value("mapFont", font()).value<QFont>();
    tabWidget->setCurrentIndex(cfg.value("visibleCanvas",0).toInt());
    cfg.endGroup(); // Canvas

    QByteArray tz;
    IUnit::tz_mode_e tzmode;
    tz = cfg.value("Units/timezone", "UTC").toByteArray();
    tzmode = (IUnit::tz_mode_e)cfg.value("Units/timezone/mode", IUnit::eTZUtc).toInt();
    IUnit::setTimeZoneSetup(tzmode, tz);


    QStatusBar * status = statusBar();
    lblPosWGS84 = new QLabel(status);
    status->addPermanentWidget(lblPosWGS84);

    lblElevation = new QLabel(status);
    status->addPermanentWidget(lblElevation);

    lblPosGrid = new QLabel(status);
    status->addPermanentWidget(lblPosGrid);

    menuWindow->addAction(dockMaps->toggleViewAction());
    menuWindow->addAction(dockDem->toggleViewAction());
    menuWindow->addAction(dockGis->toggleViewAction());

    loadGISData(qlOpts->arguments);
}

CMainWindow::~CMainWindow()
{
    int cnt = 0;

    SETTINGS;
    cfg.setValue("MainWindow/state", saveState());
    cfg.setValue("MainWindow/geometry", saveGeometry());
    cfg.setValue("MainWindow/units", IUnit::self().type);


    cfg.beginGroup("Canvas");
    QList<CCanvas*> allCanvas;
    for(int i = 0; i < tabWidget->count(); i++)
    {
        CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
        if(canvas == 0)
        {
            continue;
        }
        cnt++;
        cfg.beginGroup(QString("Canvas%1").arg(i));
        canvas->saveConfig(cfg);
        cfg.endGroup();

        allCanvas << canvas;
    }

    cfg.setValue("visibleCanvas", tabWidget->currentIndex());
    cfg.setValue("numberOfCanvas", cnt);
    cfg.setValue("isScaleVisible", actionShowScale->isChecked());
    cfg.setValue("isGridVisible", actionShowGrid->isChecked());
    cfg.setValue("POIText", actionPOIText->isChecked());
    cfg.setValue("MapToolTip", actionMapToolTip->isChecked());
    cfg.setValue("isNight", actionNightDay->isChecked());
    cfg.setValue("flipMouseWheel", actionFlipMouseWheel->isChecked());
    cfg.setValue("mapFont", mapFont);
    CMapDraw::saveMapPath(cfg);
    CDemDraw::saveDemPath(cfg);
    cfg.endGroup(); // Canvas

    /*
        Delete all canvas objects now to make sure they are destroyed before all
        other objects. This allows children of the canvas to access central objects
        like CGisWidget safely uppon their destruction. (e.g. CMouseRangeTrk to reset
        it's track's draw mode by key)
    */
    qDeleteAll(allCanvas);

    QByteArray tz;
    IUnit::tz_mode_e tzmode;
    IUnit::getTimeZoneSetup(tzmode, tz);

    cfg.setValue("Units/timezone", tz);
    cfg.setValue("Units/timezone/mode", tzmode);

}

bool CMainWindow::isScaleVisible()
{
    return actionShowScale->isChecked();
}

bool CMainWindow::isGridVisible()
{
    return actionShowGrid->isChecked();
}

bool CMainWindow::isNight()
{
    return actionNightDay->isChecked();
}

bool CMainWindow::isPOIText()
{
    return actionPOIText->isChecked();
}

bool CMainWindow::isMapToolTip()
{
    return actionMapToolTip->isChecked();
}

bool CMainWindow::flipMouseWheel()
{
    return actionFlipMouseWheel->isChecked();
}

void CMainWindow::addMapList(CMapList * list, const QString &name)
{
    tabMaps->addTab(list,name);
}

void CMainWindow::addDemList(CDemList * list, const QString &name)
{
    tabDem->addTab(list,name);
}

void CMainWindow::addWidgetToTab(QWidget * w)
{
    if(tabWidget->indexOf(w) == -1)
    {
        tabWidget->addTab(w, w->objectName());
    }
    tabWidget->setCurrentWidget(w);
}

CCanvas * CMainWindow::getVisibleCanvas()
{
    return dynamic_cast<CCanvas*>(tabWidget->currentWidget());
}

void CMainWindow::zoomCanvasTo(const QRectF rect)
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->zoomTo(rect);
    }
}

qreal CMainWindow::getEelevationAt(const QPointF& pos)
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        return canvas->getElevationAt(pos);
    }
    return NOFLOAT;
}

void  CMainWindow::getEelevationAt(const QPolygonF &pos, QPolygonF& ele)
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->getElevationAt(pos, ele);
    }
    else
    {
        for(int i = 0; i < tabWidget->count(); i++)
        {
            canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
            if(canvas)
            {
                canvas->getElevationAt(pos, ele);
                return;
            }
        }
        ele.clear();
    }
}

void CMainWindow::slotAbout()
{
    CAbout dlg(this);
    dlg.exec();
}

void CMainWindow::slotHelp()
{
    QDesktopServices::openUrl(QUrl("https://bitbucket.org/maproom/qmapshack/wiki/DocMain"));
}


void CMainWindow::slotAddCanvas()
{
    int i, cnt = 0;
    for(i = 0; i < tabWidget->count(); i++)
    {
         CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
         if(canvas == 0)
         {
             continue;
         }
         cnt++;
    }

    CCanvas * canvas = new CCanvas(tabWidget);
    tabWidget->addTab(canvas, canvas->objectName());
    connect(canvas, SIGNAL(sigMousePosition(QPointF, qreal)), this, SLOT(slotMousePosition(QPointF, qreal)));

    SETTINGS;
    cfg.beginGroup(QString("Canvas%1").arg(cnt));
    canvas->loadConfig(cfg);
    cfg.endGroup();

    tabWidget->setCurrentWidget(canvas);
}

void CMainWindow::slotTabCloseRequest(int i)
{
    QWidget * w = tabWidget->widget(i);
    tabWidget->removeTab(i);
    delete w;
}

void CMainWindow::slotCurrentTabCanvas(int i)
{
    QString name = tabWidget->tabText(i);
    for(int n = 0; n < tabMaps->count(); n++)
    {
        if(tabMaps->tabText(n) == name)
        {
            tabMaps->setCurrentIndex(n);
            break;
        }
    }
    for(int n = 0; n < tabDem->count(); n++)
    {
        if(tabDem->tabText(n) == name)
        {
            tabDem->setCurrentIndex(n);
            break;
        }
    }    
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
}

void CMainWindow::slotCurrentTabMaps(int i)
{
    QString name = tabMaps->tabText(i);
    for(int n = 0; n < tabWidget->count(); n++)
    {
        if(tabWidget->tabText(n) == name)
        {
            tabWidget->setCurrentIndex(n);
            break;
        }
    }
    for(int n = 0; n < tabDem->count(); n++)
    {
        if(tabDem->tabText(n) == name)
        {
            tabDem->setCurrentIndex(n);
            break;
        }
    }
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
}

void CMainWindow::slotCurrentTabDem(int i)
{
    QString name = tabMaps->tabText(i);
    for(int n = 0; n < tabWidget->count(); n++)
    {
        if(tabWidget->tabText(n) == name)
        {
            tabWidget->setCurrentIndex(n);
            break;
        }
    }
    for(int n = 0; n < tabMaps->count(); n++)
    {
        if(tabMaps->tabText(n) == name)
        {
            tabMaps->setCurrentIndex(n);
            break;
        }
    }
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
}

void CMainWindow::slotMousePosition(const QPointF& pos, qreal ele)
{
    QString str;
    GPS_Math_Deg_To_Str(pos.x(), pos.y(), str);
    lblPosWGS84->setText(str);

    if(ele != NOFLOAT)
    {
        QString val, unit;
        IUnit::self().meter2elevation(ele, val, unit);
        lblElevation->setText(tr("Ele: %1%2").arg(val).arg(unit));
        lblElevation->show();
    }
    else
    {
        lblElevation->hide();
    }

    if(actionShowGrid->isChecked())
    {
        CCanvas * canvas = getVisibleCanvas();
        if(canvas)
        {
            QString str;
            lblPosGrid->show();
            canvas->convertGridPos2Str(pos, str);
            lblPosGrid->setText(str);
        }
    }
    else
    {
        lblPosGrid->hide();
    }
}

void CMainWindow::slotUpdateCurrentWidget()
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
        return;
    }

    QWidget * w = tabWidget->currentWidget();
    if(w)
    {
        w->update();
        return;
    }

}


void CMainWindow::slotSetupMapFont()
{
    bool ok = false;
    QFont f = QFontDialog::getFont(&ok, mapFont, this);
    if(ok)
    {
        mapFont = f;
        QWidget * w = tabWidget->currentWidget();
        if(w)
        {
            w->update();
        }
    }

}

void CMainWindow::slotSetupGrid()
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas == 0)
    {
        return;
    }
    canvas->setupGrid();
}

void CMainWindow::slotSetupMapPath()
{
    CMapDraw::setupMapPath();
}

void CMainWindow::slotSetupDemPath()
{
    CDemDraw::setupDemPath();
}

void CMainWindow::slotSetupMapWks()
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas == 0)
    {
        return;
    }
    canvas->setup();
}

void CMainWindow::slotSetupTimeZone()
{
    CTimeZoneSetup dlg(this);
    dlg.exec();
}

void CMainWindow::slotSetupUnits()
{
    CUnitsSetup dlg(this);
    dlg.exec();
}

void CMainWindow::slotLoadGISData()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();
    QString filter = cfg.value("Paths/lastGisFilter", "GPS Exchange Format (*.gpx)").toString();


    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Load GIS Data..."), path, "GPS Exchange Format (*.gpx);; QMapShack Binary (*.qms)", &filter);

    if(filenames.isEmpty())
    {
        return;
    }

    loadGISData(filenames);

    path = QFileInfo(filenames.first()).absolutePath();
    cfg.setValue("Paths/lastGisPath", path);
    cfg.setValue("Paths/lastGisFilter", filter);
}

void CMainWindow::loadGISData(const QStringList& filenames)
{
    foreach(const QString& filename, filenames)
    {
        gisWidget->loadGisProject(filename);
    }
}

