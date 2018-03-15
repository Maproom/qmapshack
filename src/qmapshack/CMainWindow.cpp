/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#include "CAbout.h"
#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "config.h"
#include "dem/CDemDraw.h"
#include "dem/CDemList.h"
#include "gis/CGisDatabase.h"
#include "gis/CGisWorkspace.h"
#include "gis/IGisLine.h"
#include "gis/WptIcons.h"
#include "gis/db/CSetupWorkspace.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/router/CRouterRoutino.h"
#include "gis/trk/CActivityTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"
#include "helpers/CToolBarConfig.h"
#include "helpers/CToolBarSetupDialog.h"
#include "helpers/CWptIconDialog.h"
#include "map/CMapDraw.h"
#include "map/CMapItem.h"
#include "map/CMapList.h"
#include "realtime/CRtWorkspace.h"
#include "setup/IAppSetup.h"
#include "tool/CImportDatabase.h"
#include "tool/CMapVrtBuilder.h"
#include "tool/CRoutinoDatabaseBuilder.h"
#include "units/CCoordFormatSetup.h"
#include "units/CTimeZoneSetup.h"
#include "units/CUnitsSetup.h"
#include "units/IUnit.h"
#include "version.h"

#include <QtGui>
#include <QtSql>
#include <QtWidgets>

#ifdef WIN32
#include "device/CDeviceWatcherWindows.h"
#include <dbt.h>
#include <guiddef.h>
#include <initguid.h>
#include <usbiodef.h>
#include <windows.h>
#endif // WIN32

CMainWindow * CMainWindow::pSelf = nullptr;

QDir CMainWindow::homeDir;
const QString CMainWindow::mapsPath = "Maps";
const QString CMainWindow::demPath = "Dem";
const QString CMainWindow::routinoPath = "Routino";
const QString CMainWindow::brouterPath = "BRouter";
const QString CMainWindow::databasePath = "Database";
const QString CMainWindow::gpxPath = "GPX";
const QSet<QString> CMainWindow::paths = {mapsPath, demPath, routinoPath, brouterPath, databasePath, gpxPath};

QMutex CMainWindow::mutex(QMutex::NonRecursive);

CMainWindow::CMainWindow()
    : id(qrand())
{
    qDebug() << "Application ID:" << id;
    SETTINGS;
    homeDir = cfg.value("Paths/homePath", "").toString();

    pSelf = this;
    setupUi(this);
    setWindowTitle(WHAT_STR);
    dockRealtime->toggleViewAction()->setChecked(false);

    initWptIcons();

    IUnit::setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt(), this);
    IUnit::setSlopeMode((IUnit::slope_mode_e)cfg.value("Units/slopeMode", IUnit::eSlopeDegrees).toInt());

    QByteArray tz;
    IUnit::tz_mode_e tzmode;
    bool useShortFormat;
    tz = cfg.value("Units/timezone", "UTC").toByteArray();
    tzmode = (IUnit::tz_mode_e)cfg.value("Units/timezone/mode", IUnit::eTZUtc).toInt();
    useShortFormat = cfg.value("Units/time/useShortFormat", false).toBool();
    IUnit::setTimeZoneSetup(tzmode, tz, useShortFormat);

    IUnit::coord_format_e coordFormat;
    coordFormat = (IUnit::coord_format_e)cfg.value("Units/coordFormat", IUnit::eCoordFormat1).toInt();
    IUnit::setCoordFormat(coordFormat);

    CKnownExtension::init(IUnit::self());
    CActivityTrk::init();

    widgetGisWorkspace = new CGisWorkspace(menuProject, this);
    dockWorkspace->setWidget(widgetGisWorkspace);

    widgetGisDatabase = new CGisDatabase(this);
    dockDatabase->setWidget(widgetGisDatabase);

    widgetRtWorkspace = new CRtWorkspace(this);
    dockRealtime->setWidget(widgetRtWorkspace);

    // start ---- restore window geometry -----
    cfg.beginGroup("MainWindow");
    if ( cfg.contains("geometry"))
    {
        restoreGeometry(cfg.value("geometry").toByteArray());
    }
    else
    {
        QTimer::singleShot(500, this, SLOT(showMaximized()));
    }

    if ( cfg.contains("state"))
    {
        restoreState(cfg.value("state").toByteArray());
    }

    if (cfg.contains("displaymode"))
    {
        displayMode = static_cast<Qt::WindowStates>(cfg.value("displaymode").toInt());
        if (displayMode == Qt::WindowFullScreen)
        {
            displayMode = Qt::WindowMaximized;
        }
    }

    if (cfg.contains("dockstate"))
    {
        dockStates = cfg.value("dockstate").toByteArray();
    }

    menuVisible = cfg.value("menuvisible",false).toBool();

    if(windowState() == Qt::WindowFullScreen)
    {
        displayRegular();
    }
    cfg.endGroup();

    // end ---- restore window geometry -----

    connect(actionAbout,                 &QAction::triggered,            this,      &CMainWindow::slotAbout);
    connect(actionHelp,                  &QAction::triggered,            this,      &CMainWindow::slotHelp);
    connect(actionQuickstart,            &QAction::triggered,            this,      &CMainWindow::slotQuickstart);
    connect(actionAddMapView,            &QAction::triggered,            this,      &CMainWindow::slotAddCanvas);
    connect(actionCloneMapView,          &QAction::triggered,            this,      &CMainWindow::slotCloneCanvas);
    connect(actionShowScale,             &QAction::changed,              this,      &CMainWindow::slotUpdateCurrentWidget);
    connect(actionShowGrid,              &QAction::changed,              this,      static_cast<void (CMainWindow::*)()>(&CMainWindow::update));
    connect(actionPOIText,               &QAction::changed,              this,      &CMainWindow::slotUpdateCurrentWidget);
    connect(actionMapToolTip,            &QAction::changed,              this,      &CMainWindow::slotUpdateCurrentWidget);
    connect(actionNightDay,              &QAction::changed,              this,      &CMainWindow::slotUpdateCurrentWidget);
    connect(actionMinMaxTrackValues,     &QAction::changed,              this,      &CMainWindow::slotUpdateCurrentWidget);
    connect(actionProfileIsWindow,       &QAction::toggled,              this,      &CMainWindow::slotSetProfileMode);
    connect(actionSetupMapFont,          &QAction::triggered,            this,      &CMainWindow::slotSetupMapFont);
    connect(actionSetupMapBackground,    &QAction::triggered,            this,      &CMainWindow::slotSetupMapBackground);
    connect(actionSetupGrid,             &QAction::triggered,            this,      &CMainWindow::slotSetupGrid);
    connect(actionSetupMapPaths,         &QAction::triggered,            this,      &CMainWindow::slotSetupMapPath);
    connect(actionSetupDEMPaths,         &QAction::triggered,            this,      &CMainWindow::slotSetupDemPath);
    connect(actionSetupMapView,          &QAction::triggered,            this,      &CMainWindow::slotSetupMapView);
    connect(actionSetupTimeZone,         &QAction::triggered,            this,      &CMainWindow::slotSetupTimeZone);
    connect(actionSetupUnits,            &QAction::triggered,            this,      &CMainWindow::slotSetupUnits);
    connect(actionSetupWorkspace,        &QAction::triggered,            this,      &CMainWindow::slotSetupWorkspace);
    connect(actionSetupCoordFormat,      &QAction::triggered,            this,      &CMainWindow::slotSetupCoordFormat);
    connect(actionSetupToolbar,          &QAction::triggered,            this,      &CMainWindow::slotSetupToolbar);
    connect(actionImportDatabase,        &QAction::triggered,            this,      &CMainWindow::slotImportDatabase);
    connect(actionSaveGISData,           &QAction::triggered,            widgetGisWorkspace, &CGisWorkspace::slotSaveAll);
    connect(actionLoadGISData,           &QAction::triggered,            this,      &CMainWindow::slotLoadGISData);
    connect(actionVrtBuilder,            &QAction::triggered,            this,      &CMainWindow::slotBuildVrt);
    connect(actionStoreView,             &QAction::triggered,            this,      &CMainWindow::slotStoreView);
    connect(actionLoadView,              &QAction::triggered,            this,      &CMainWindow::slotLoadView);
    connect(actionClose,                 &QAction::triggered,            this,      &CMainWindow::close);
    connect(actionCreateRoutinoDatabase, &QAction::triggered,            this,      &CMainWindow::slotCreateRoutinoDatabase);
    connect(actionPrintMap,              &QAction::triggered,            this,      &CMainWindow::slotPrintMap);
    connect(actionSetupWaypointIcons,    &QAction::triggered,            this,      &CMainWindow::slotSetupWptIcons);
    connect(actionCloseTab,              &QAction::triggered,            this,      &CMainWindow::slotCloseTab);
    connect(actionToggleDocks,           &QAction::triggered,            this,      &CMainWindow::slotToggleDocks);
    connect(actionFullScreen,            &QAction::triggered,            this,      &CMainWindow::slotFullScreen);
    connect(actionStartQMapTool,         &QAction::triggered,            this,      &CMainWindow::slotStartQMapTool);
    connect(tabWidget,                   &QTabWidget::tabCloseRequested, this,      &CMainWindow::slotTabCloseRequest);
    connect(tabWidget,                   &QTabWidget::currentChanged,    this,      &CMainWindow::slotCurrentTabCanvas);
    connect(tabMaps,                     &QTabWidget::currentChanged,    this,      &CMainWindow::slotCurrentTabMaps);
    connect(tabDem,                      &QTabWidget::currentChanged,    this,      &CMainWindow::slotCurrentTabDem);

    if(IAppSetup::getPlatformInstance()->findExecutable("qmaptool").isEmpty())
    {
        actionStartQMapTool->setVisible(false);
    }

    cfg.beginGroup("Canvas");
    CMapDraw::loadMapPath(cfg);
    CDemDraw::loadDemPath(cfg);

    cfg.beginGroup("Views");
    QStringList names = cfg.childGroups();

    for(const QString &name : names)
    {
        CCanvas * view = addView(name);

        cfg.beginGroup(name);
        view->loadConfig(cfg);
        cfg.endGroup(); // name
    }
    if(names.isEmpty())
    {
        CCanvas * view = addView(QString());
        // call just to setup default values
        view->loadConfig(cfg);
    }
    cfg.endGroup(); // Views
    testForNoView();

    CCanvas::gisLayerOpacity = cfg.value("gisLayerOpacity",1.0).toFloat();
    widgetGisWorkspace->setOpacity(CCanvas::gisLayerOpacity);

    actionShowScale->setChecked(cfg.value("isScaleVisible", true).toBool());
    actionShowGrid->setChecked(cfg.value("isGridVisible", false).toBool());
    actionPOIText->setChecked(cfg.value("POIText", true).toBool());
    actionMapToolTip->setChecked(cfg.value("MapToolTip", true).toBool());
    actionNightDay->setChecked(cfg.value("isNight", false).toBool());
    actionMinMaxTrackValues->setChecked(cfg.value("MinMaxTrackValues", false).toBool());
    actionFlipMouseWheel->setChecked(cfg.value("flipMouseWheel", false).toBool());
    actionProfileIsWindow->setChecked(cfg.value("profileIsWindow", false).toBool());
    mapFont = cfg.value("mapFont", font()).value<QFont>();
    tabWidget->setCurrentIndex(cfg.value("visibleCanvas",0).toInt());
    cfg.endGroup(); // Canvas



    QStatusBar * status = statusBar();
    lblPosWGS84 = new QLabel(status);
    status->addPermanentWidget(lblPosWGS84);

    lblElevation = new QLabel(status);
    status->addPermanentWidget(lblElevation);

    lblSlope = new QLabel(status);
    status->addPermanentWidget(lblSlope);

    lblPosGrid = new QLabel(status);
    status->addPermanentWidget(lblPosGrid);


    docks << dockMaps
          << dockDem
          << dockWorkspace
          << dockDatabase
          << dockRte
          << dockRealtime;


    if (cfg.contains("MainWindow/activedocks"))
    {
        const QStringList & dockNames = cfg.value("MainWindow/activedocks").toStringList();
        for(QDockWidget * const & dock : docks)
        {
            if(dockNames.contains(dock->objectName()))
            {
                activeDocks << dock;
            }
        }
    }

    for (QDockWidget * const & dock : docks)
    {
        connect(dock, &QDockWidget::visibilityChanged, this, &CMainWindow::slotDockVisibilityChanged);
    }


    QAction * actionToggleToolBar = toolBar->toggleViewAction();
    actionToggleToolBar->setObjectName("actionToggleToolBar");
    actionToggleToolBar->setIcon(QIcon(":/icons/32x32/ToolBar.png"));
    menuWindow->insertAction(actionSetupToolbar,actionToggleToolBar);

    QAction * actionToggleMaps = dockMaps->toggleViewAction();
    actionToggleMaps->setObjectName("actionToggleMaps");
    actionToggleMaps->setIcon(QIcon(":/icons/32x32/ToggleMaps.png"));
    menuWindow->insertAction(actionSetupToolbar,actionToggleMaps);

    QAction * actionToggleDem = dockDem->toggleViewAction();
    actionToggleDem->setObjectName("actionToggleDem");
    actionToggleDem->setIcon(QIcon(":/icons/32x32/ToggleDem.png"));
    menuWindow->insertAction(actionSetupToolbar,actionToggleDem);

    QAction * actionToggleWorkspace = dockWorkspace->toggleViewAction();
    actionToggleWorkspace->setObjectName("actionToggleWorkspace");
    actionToggleWorkspace->setIcon(QIcon(":/icons/32x32/ToggleGis.png"));
    menuWindow->insertAction(actionSetupToolbar,actionToggleWorkspace);

    QAction * actionToggleRealtime = dockRealtime->toggleViewAction();
    actionToggleRealtime->setObjectName("actionToggleRealtime");
    actionToggleRealtime->setIcon(QIcon(":/icons/32x32/ToggleRealTime.png"));
    menuWindow->insertAction(actionSetupToolbar,actionToggleRealtime);

    QAction * actionToggleDatabase = dockDatabase->toggleViewAction();
    actionToggleDatabase->setObjectName("actionToggleDatabase");
    actionToggleDatabase->setIcon(QIcon(":/icons/32x32/ToggleDatabase.png"));
    menuWindow->insertAction(actionSetupToolbar,actionToggleDatabase);

    QAction * actionToggleRte = dockRte->toggleViewAction();
    actionToggleRte->setObjectName("actionToggleRte");
    actionToggleRte->setIcon(QIcon(":/icons/32x32/ToggleRouter.png"));
    menuWindow->insertAction(actionSetupToolbar,actionToggleRte);


    menuWindow->insertSeparator(actionSetupToolbar);

    QAction * separator = new QAction("---------------",this);
    separator->setSeparator(true);
    separator->setObjectName("separator");

    QList<QAction *> availableActions;
    availableActions << separator
                     << actionAddMapView
                     << actionShowScale
                     << actionSetupMapFont
                     << actionShowGrid
                     << actionSetupGrid
                     << actionFlipMouseWheel
                     << actionSetupMapPaths
                     << actionPOIText
                     << actionNightDay
                     << actionMapToolTip
                     << actionMinMaxTrackValues
                     << actionSetupDEMPaths
                     << actionAbout
                     << actionHelp
                     << actionSetupMapView
                     << actionLoadGISData
                     << actionSaveGISData
                     << actionSetupTimeZone
                     << actionAddEmptyProject
                     << actionSearchGoogle
                     << actionCloseAllProjects
                     << actionSetupUnits
                     << actionSetupWorkspace
                     << actionImportDatabase
                     << actionVrtBuilder
                     << actionStoreView
                     << actionLoadView
                     << actionProfileIsWindow
                     << actionClose
                     << actionCloneMapView
                     << actionCreateRoutinoDatabase
                     << actionPrintMap
                     << actionSetupCoordFormat
                     << actionSetupMapBackground
                     << actionSetupWaypointIcons
                     << actionCloseTab
                     << actionQuickstart
                     << actionSetupToolbar
                     << actionToggleMaps
                     << actionToggleDem
                     << actionToggleWorkspace
                     << actionToggleRealtime
                     << actionToggleDatabase
                     << actionToggleRte
                     << actionToggleDocks
                     << actionToggleToolBar
                     << actionFullScreen
                     << actionStartQMapTool;

    QAction * separator1 = new QAction("---------------",this);
    separator1->setSeparator(true);
    separator1->setObjectName("separator");

    QList<QAction *> defaultActions;
    defaultActions << actionSearchGoogle
                   << actionAddEmptyProject
                   << actionLoadGISData
                   << actionSaveGISData
                   << separator
                   << actionShowScale
                   << actionShowGrid
                   << actionPOIText
                   << actionNightDay
                   << actionMapToolTip
                   << actionMinMaxTrackValues
                   << actionProfileIsWindow
                   << separator1
                   << actionSetupToolbar
                   << actionToggleMaps
                   << actionToggleDem
                   << actionToggleWorkspace
                   << actionToggleRealtime
                   << actionToggleDatabase
                   << actionToggleRte
                   << actionToggleDocks
                   << actionFullScreen;

    toolBarConfig = new CToolBarConfig(this, toolBar, availableActions, defaultActions);
    toolBarConfig->loadSettings();

    prepareMenuForMac();

    // make sure all actions that have a shortcut are available even when menu and toolbar are not visible
    for (QAction * action : availableActions)
    {
        if (!action->shortcuts().isEmpty())
        {
            addAction(action);
        }
    }


    loadGISData(qlOpts->arguments);

    QTimer::singleShot(100, this, SLOT(slotSanityTest()));
}

void CMainWindow::prepareMenuForMac()
{
    toolBar->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockMaps->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockDem->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockWorkspace->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockRealtime->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockDatabase->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockRte->toggleViewAction()->setMenuRole(QAction::NoRole);
}

CMainWindow::~CMainWindow()
{
    CActivityTrk::release();

    SETTINGS;
    cfg.beginGroup("MainWindow");
    cfg.setValue("state", saveState());
    cfg.setValue("geometry", saveGeometry());
    cfg.setValue("units", IUnit::self().type);
    QStringList activeDockNames;
    for (QDockWidget * const & dock : activeDocks)
    {
        activeDockNames << dock->objectName();
    }
    cfg.setValue("activedocks",activeDockNames);

    cfg.setValue("displaymode",static_cast<int>(displayMode));
    cfg.setValue("dockstate",dockStates);
    cfg.setValue("menuvisible",menuVisible);
    cfg.endGroup();

    /*
       The "Canvas" section will hold all settings global to all views
       and "Views" section containing a subsection for each view.
     */
    cfg.beginGroup("Canvas");
    QList<CCanvas*> allViews;
    QList<QWidget*> allOtherTabs;

    // save setup of all views
    cfg.beginGroup("Views");
    // remove all previous setups in this section first
    cfg.remove(QString());

    for(int i = 0; i < tabWidget->count(); i++)
    {
        CCanvas * view = dynamic_cast<CCanvas*>(tabWidget->widget(i));
        if(nullptr == view)
        {
            allOtherTabs << tabWidget->widget(i);
            continue;
        }

        // save views
        cfg.beginGroup(view->objectName());
        view->saveConfig(cfg);
        cfg.endGroup();

        allViews << view;
    }
    cfg.endGroup(); // Views

    cfg.setValue("gisLayerOpacity", CCanvas::gisLayerOpacity);
    cfg.setValue("visibleCanvas", tabWidget->currentIndex());
    cfg.setValue("isScaleVisible", actionShowScale->isChecked());
    cfg.setValue("isGridVisible", actionShowGrid->isChecked());
    cfg.setValue("POIText", actionPOIText->isChecked());
    cfg.setValue("MapToolTip", actionMapToolTip->isChecked());
    cfg.setValue("isNight", actionNightDay->isChecked());
    cfg.setValue("MinMaxTrackValues", actionMinMaxTrackValues->isChecked());
    cfg.setValue("flipMouseWheel", actionFlipMouseWheel->isChecked());
    cfg.setValue("profileIsWindow",actionProfileIsWindow->isChecked());
    cfg.setValue("mapFont", mapFont);
    CMapDraw::saveMapPath(cfg);
    CDemDraw::saveDemPath(cfg);
    cfg.endGroup(); // Canvas


    /*
        Delete all widgets in the tab widget other than views. The IPlot objects
        in a track detail dialog send update events to the view on destruction.
        So it is important that these are destroyed first.
     */
    qDeleteAll(allOtherTabs);
    /*
        Delete all canvas objects now to make sure they are destroyed before all
        other objects. This allows children of the canvas to access central objects
        like CGisWorkspace safely upon their destruction. (e.g. CMouseRangeTrk to reset
        it's track's draw mode by key)
     */
    qDeleteAll(allViews);

    QByteArray tz;
    IUnit::tz_mode_e tzmode;
    bool useShortFormat;
    IUnit::getTimeZoneSetup(tzmode, tz, useShortFormat);

    cfg.setValue("Units/timezone", tz);
    cfg.setValue("Units/timezone/mode", tzmode);
    cfg.setValue("Units/time/useShortFormat", useShortFormat);
    cfg.setValue("Units/coordFormat", IUnit::getCoordFormat());
    cfg.setValue("Units/slopeMode", IUnit::getSlopeMode());

    toolBarConfig->saveSettings();
}

void CMainWindow::setupHomePath()
{
    SETTINGS;
    homeDir = cfg.value("Paths/homePath", QDir::homePath()).toString();
    const QString& homePath = QFileDialog::getExistingDirectory(this, tr("Select folder..."), homeDir.absolutePath());
    if(homePath.isEmpty())
    {
        return;
    }

    homeDir = homePath;
    for(const QString& path : paths)
    {
        if(!homeDir.exists(path))
        {
            homeDir.mkpath(path);
        }
    }

    CMapDraw::setupMapPath(homeDir.absoluteFilePath(mapsPath));
    CDemDraw::setupDemPath(homeDir.absoluteFilePath(demPath));
    CRouterRoutino::self().setupPath(homeDir.absoluteFilePath(routinoPath));
    //todo: BRouter
    cfg.setValue("Database/lastDatabasePath", homeDir.absoluteFilePath(databasePath));
    cfg.setValue("Paths/lastGisPath", homeDir.absoluteFilePath(gpxPath));

    cfg.setValue("Paths/homePath", homeDir.absolutePath());
}

CCanvas *CMainWindow::addView(const QString& name)
{
    CCanvas * view = new CCanvas(tabWidget, name);
    tabWidget->addTab(view, view->objectName());
    connect(view, &CCanvas::sigMousePosition, this, &CMainWindow::slotMousePosition);
    connect(actionMinMaxTrackValues, &QAction::triggered, view, &CCanvas::slotUpdateTrackStatistic);

    return view;
}

QWidget * CMainWindow::getBestWidgetForParent()
{
    QWidget * w = CProgressDialog::self();
    if(w)
    {
        return w;
    }

    // this is a workaround for unittesting
    if(nullptr == pSelf)
    {
        return nullptr;
    }

    w = self().getVisibleCanvas();
    if(w)
    {
        return w;
    }

    return &self();
}

QString CMainWindow::getUser()
{
    QString user = getenv("USER");
    if(user.isEmpty())
    {
        user = getenv("USERNAME"); //for windows

        if(user.isEmpty())
        {
            user = "QMapShack";
        }
    }

    return user;
}

bool CMainWindow::isScaleVisible() const
{
    return actionShowScale->isChecked();
}

bool CMainWindow::isGridVisible() const
{
    return actionShowGrid->isChecked();
}

bool CMainWindow::isNight() const
{
    return actionNightDay->isChecked();
}

bool CMainWindow::isPOIText() const
{
    return actionPOIText->isChecked();
}

bool CMainWindow::isMapToolTip() const
{
    return actionMapToolTip->isChecked();
}

bool CMainWindow::isMinMaxTrackValues() const
{
    return actionMinMaxTrackValues->isChecked();
}

bool CMainWindow::flipMouseWheel() const
{
    return actionFlipMouseWheel->isChecked();
}

bool CMainWindow::profileIsWindow() const
{
    return actionProfileIsWindow->isChecked();
}

void CMainWindow::addMapList(CMapList * list, const QString &name)
{
    tabMaps->addTab(list,name);
    connect(list, &CMapList::sigSetupMapPath, this, &CMainWindow::slotSetupMapPath);
}

void CMainWindow::addDemList(CDemList * list, const QString &name)
{
    tabDem->addTab(list,name);
    connect(list, &CDemList::sigSetupDemPath, this, &CMainWindow::slotSetupDemPath);
}

void CMainWindow::addWidgetToTab(QWidget * w)
{
    if(tabWidget->indexOf(w) == NOIDX)
    {
        tabWidget->addTab(w, w->objectName().replace("&", "&&"));
    }
    tabWidget->setCurrentWidget(w);
}

CCanvas* CMainWindow::getVisibleCanvas() const
{
    return dynamic_cast<CCanvas*>(tabWidget->currentWidget());
}

QList<CCanvas*> CMainWindow::getCanvas() const
{
    QList<CCanvas*> result;
    const int N = tabWidget->count();
    for(int n = 0; n < N; n++)
    {
        CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->widget(n));
        if(canvas != nullptr)
        {
            result << canvas;
        }
    }

    return result;
}

void CMainWindow::zoomCanvasTo(const QRectF rect)
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->zoomTo(rect);
    }
}

qreal CMainWindow::getElevationAt(const QPointF& pos) const
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        return canvas->getElevationAt(pos);
    }
    else
    {
        for(int i = 0; i < tabWidget->count(); i++)
        {
            canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
            if(canvas)
            {
                return canvas->getElevationAt(pos);
            }
        }
    }
    return NOFLOAT;
}


void CMainWindow::getElevationAt(SGisLine &line) const
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->getElevationAt(line);
    }
    else
    {
        for(int i = 0; i < tabWidget->count(); i++)
        {
            canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
            if(canvas)
            {
                canvas->getElevationAt(line);
                return;
            }
        }

        for(int i = 0; i < line.size(); i++)
        {
            line[i].resetElevation();
        }
    }
}

void CMainWindow::getElevationAt(const QPolygonF &pos, QPolygonF& ele) const
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

qreal CMainWindow::getSlopeAt(const QPointF& pos) const
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        return canvas->getSlopeAt(pos);
    }
    else
    {
        for(int i = 0; i < tabWidget->count(); i++)
        {
            canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
            if(canvas)
            {
                return canvas->getSlopeAt(pos);
            }
        }
    }
    return NOFLOAT;
}

void CMainWindow::getSlopeAt(const QPolygonF &pos, QPolygonF& slope) const
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas)
    {
        canvas->getSlopeAt(pos, slope);
    }
    else
    {
        for(int i = 0; i < tabWidget->count(); i++)
        {
            canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
            if(canvas)
            {
                canvas->getSlopeAt(pos, slope);
                return;
            }
        }
        slope.clear();
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

void CMainWindow::slotQuickstart()
{
    // show menu action for German help if system language is German.
    QString locale = QLocale::system().name();
    if(locale.size() >= 2)
    {
        locale = locale.left(2).toLower();
        if(locale == "de")
        {
            QDesktopServices::openUrl(QUrl("https://bitbucket.org/maproom/qmapshack/wiki/DocQuickStartGerman"));
        }
        else if(locale == "ru")
        {
            QDesktopServices::openUrl(QUrl("https://bitbucket.org/maproom/qmapshack/wiki/DocQuickStartRussian"));
        }
        else if(locale == "es")
        {
            QDesktopServices::openUrl(QUrl("https://bitbucket.org/maproom/qmapshack/wiki/DocQuickStartSpanish"));
        }
        else
        {
            QDesktopServices::openUrl(QUrl("https://bitbucket.org/maproom/qmapshack/wiki/DocQuickStartEnglish"));
        }
    }
    else
    {
        QDesktopServices::openUrl(QUrl("https://bitbucket.org/maproom/qmapshack/wiki/DocQuickStartEnglish"));
    }
}


void CMainWindow::slotAddCanvas()
{
    int i, cnt = 0;
    for(i = 0; i < tabWidget->count(); i++)
    {
        CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
        if(nullptr != canvas)
        {
            cnt++;
        }
    }

    CCanvas * view = addView(QString());
    tabWidget->setCurrentWidget(view);

    testForNoView();
}

void CMainWindow::slotCloneCanvas()
{
    CCanvas * source = getVisibleCanvas();
    if(nullptr == source)
    {
        return;
    }

    QTemporaryFile temp;
    temp.open();
    temp.close();

    QSettings view(temp.fileName(), QSettings::IniFormat);
    view.clear();

    source->saveConfig(view);

    slotAddCanvas();

    CCanvas * target = getVisibleCanvas();
    if(nullptr == target)
    {
        return;
    }

    target->loadConfig(view);
    target->slotTriggerCompleteUpdate(CCanvas::redraw_e::eRedrawGis);

    SETTINGS;
    cfg.beginGroup("Canvas");
    cfg.beginGroup("Views");
    cfg.beginGroup(target->objectName());
    target->saveConfig(cfg);
    cfg.endGroup();
    cfg.endGroup();
    cfg.endGroup();

    testForNoView();
}

void CMainWindow::testForNoView()
{
    if(tabWidget->count() == 0)
    {
        QLabel * label = new QLabel(tabWidget);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);
        label->setText(tr("Use <b>Menu->View->Add Map View</b> to open a new view. Or <b>Menu->File->Load Map View</b> to restore a saved one. Or click <a href='NewView'>here</a>."));
        label->setObjectName("NoViewInfo");
        connect(label, &QLabel::linkActivated, this, static_cast<void (CMainWindow::*)(const QString&)>(&CMainWindow::slotLinkActivated));
        tabWidget->addTab(label, "*");
        return;
    }

    QLabel * label = tabWidget->findChild<QLabel*>("NoViewInfo");

    if(label && tabWidget->count() > 1)
    {
        delete label;
    }
}

void CMainWindow::slotTabCloseRequest(int i)
{
    QMutexLocker lock(&CMapItem::mutexActiveMaps);

    delete tabWidget->widget(i);

    testForNoView();
}

static inline bool compareNames(QString s1, QString s2)
{
    return s1.replace("&", "") == s2.replace("&", "");
}

void CMainWindow::slotCurrentTabCanvas(int i)
{
    QString name = tabWidget->tabText(i);
    for(int n = 0; n < tabMaps->count(); n++)
    {
        bool isMapView = compareNames(name, tabMaps->tabText(n));

        actionSetupGrid->setEnabled(isMapView);
        actionSetupMapBackground->setEnabled(isMapView);
        actionSetupMapView->setEnabled(isMapView);

        if(isMapView)
        {
            tabMaps->setCurrentIndex(n);
            break;
        }
    }

    for(int n = 0; n < tabDem->count(); n++)
    {
        if(compareNames(name, tabDem->tabText(n)))
        {
            tabDem->setCurrentIndex(n);
            break;
        }
    }

    for(int n = 0; n < tabWidget->count(); n++)
    {
        CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->widget(n));
        if(canvas)
        {
            if(n == i)
            {
                canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
                canvas->showProfile(true);
            }
            else
            {
                canvas->showProfile(false);
            }
        }
    }
}

void CMainWindow::slotCurrentTabMaps(int i)
{
    QString name = tabMaps->tabText(i);
    for(int n = 0; n < tabWidget->count(); n++)
    {
        if(compareNames(name, tabWidget->tabText(n)))
        {
            tabWidget->setCurrentIndex(n);
            break;
        }
    }

    for(int n = 0; n < tabDem->count(); n++)
    {
        if(compareNames(name, tabDem->tabText(n)))
        {
            tabDem->setCurrentIndex(n);
            break;
        }
    }
}

void CMainWindow::slotCurrentTabDem(int i)
{
    QString name = tabMaps->tabText(i);
    for(int n = 0; n < tabWidget->count(); n++)
    {
        if(compareNames(name, tabWidget->tabText(n)))
        {
            tabWidget->setCurrentIndex(n);
            break;
        }
    }

    for(int n = 0; n < tabMaps->count(); n++)
    {
        if(compareNames(name, tabMaps->tabText(n)))
        {
            tabMaps->setCurrentIndex(n);
            break;
        }
    }
}

void CMainWindow::slotMousePosition(const QPointF& pos, qreal ele, qreal slope)
{
    QString str;
    IUnit::degToStr(pos.x(), pos.y(), str);
    lblPosWGS84->setText(str);

    if(ele != NOFLOAT)
    {
        QString val, unit;
        IUnit::self().meter2elevation(ele, val, unit);
        lblElevation->setText(tr("Ele.: %1%2").arg(val).arg(unit));
        lblElevation->show();
    }
    else
    {
        lblElevation->hide();
    }

    if(slope != NOFLOAT)
    {
        QString val;
        val.sprintf("%.1f", slope);
        lblSlope->setText(tr("Slope: %1%2", "terrain").arg(val).arg("°"));
        lblSlope->show();
    }
    else
    {
        lblSlope->hide();
    }

    if(actionShowGrid->isChecked())
    {
        CCanvas * canvas = getVisibleCanvas();
        if(canvas)
        {
            QString str;
            lblPosGrid->show();
            canvas->convertGridPos2Str(pos, str, false);
            lblPosGrid->setText(tr("[Grid: %1]").arg(str));
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

void CMainWindow::slotSetupMapBackground()
{
    CCanvas * canvas = getVisibleCanvas();
    if(nullptr == canvas)
    {
        return;
    }
    canvas->setupBackgroundColor();
}

void CMainWindow::slotSetupGrid()
{
    CCanvas * canvas = getVisibleCanvas();
    if(nullptr == canvas)
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

void CMainWindow::slotSetupMapView()
{
    CCanvas * canvas = getVisibleCanvas();
    if(nullptr == canvas)
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

    if(QDialog::Accepted == dlg.result())
    {
        CKnownExtension::init(IUnit::self());
    }
}

void CMainWindow::slotSetupWorkspace()
{
    CSetupWorkspace dlg(this);
    dlg.exec();
}

void CMainWindow::slotSetupCoordFormat()
{
    CCoordFormatSetup dlg(this);
    dlg.exec();
}

void CMainWindow::slotSetupToolbar()
{
    CToolBarSetupDialog dlg(this,toolBarConfig);
    dlg.exec();
}

void CMainWindow::slotImportDatabase()
{
    CImportDatabase * widget = new CImportDatabase(this);
    addWidgetToTab(widget);
}

void CMainWindow::slotBuildVrt()
{
    CMapVrtBuilder * widget = new CMapVrtBuilder(this);
    addWidgetToTab(widget);
}

void CMainWindow::slotCreateRoutinoDatabase()
{
    CRoutinoDatabaseBuilder * widget = new CRoutinoDatabaseBuilder(this);
    addWidgetToTab(widget);
}

void CMainWindow::slotLoadGISData()
{
    SETTINGS;
    QString path   = cfg.value("Paths/lastGisPath",   QDir::homePath()).toString();
    QString filter = cfg.value("Paths/lastGisFilter", IGisProject::filedialogAllSupported).toString();

    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Load GIS Data..."), path, IGisProject::filedialogLoadFilters, &filter);

    if(filenames.isEmpty())
    {
        return;
    }

    loadGISData(filenames);

    path = QFileInfo(filenames.first()).absolutePath();
    cfg.setValue("Paths/lastGisPath",   path);
    cfg.setValue("Paths/lastGisFilter", filter);
}


void CMainWindow::loadGISData(const QStringList& filenames)
{
    for(const QString &filename : filenames)
    {
        widgetGisWorkspace->loadGisProject(filename);
    }
}


void CMainWindow::slotStoreView()
{
    CCanvas * canvas = getVisibleCanvas();
    if(nullptr == canvas)
    {
        return;
    }

    SETTINGS;
    QString path = cfg.value("Paths/lastViewPath", QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName( this, tr("Select output file"), path, tr("QMapShack View (*.view)"));

    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    if(fi.suffix().toLower() != "view")
    {
        filename += ".view";
    }

    QSettings view(filename, QSettings::IniFormat);
    view.clear();

    canvas->saveConfig(view);

    path = fi.absolutePath();
    cfg.setValue("Paths/lastViewPath", path);
}

void CMainWindow::slotLoadView()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastViewPath", QDir::homePath()).toString();
    QString filename = QFileDialog::getOpenFileName(this, tr("Select file to load"), path, tr("QMapShack View (*.view)"));

    if(filename.isEmpty())
    {
        return;
    }

    slotAddCanvas();

    CCanvas * canvas = getVisibleCanvas();
    if(nullptr == canvas)
    {
        return;
    }

    QSettings view(filename, QSettings::IniFormat);
    canvas->loadConfig(view);

    cfg.beginGroup("Canvas");
    cfg.beginGroup("Views");
    cfg.beginGroup(canvas->objectName());
    canvas->saveConfig(cfg);
    cfg.endGroup(); // objectName
    cfg.endGroup(); // "Views"
    cfg.endGroup(); // "Canvas"

    QFileInfo fi(filename);
    path = fi.absolutePath();
    cfg.setValue("Paths/lastViewPath", path);
}

void CMainWindow::slotSetProfileMode(bool on)
{
    for(int i = 0; i < tabWidget->count(); i++)
    {
        CCanvas * view = dynamic_cast<CCanvas*>(tabWidget->widget(i));
        if(nullptr != view)
        {
            view->showProfileAsWindow(on);
        }
    }
}

void CMainWindow::slotPrintMap()
{
    CCanvas * canvas = getVisibleCanvas();
    if(nullptr != canvas)
    {
        canvas->setMousePrint();
    }
}

void CMainWindow::slotLinkActivated(const QString& link)
{
    if(link == "NewView")
    {
        actionAddMapView->trigger();
    }
    else if(link == "ShowWiki")
    {
        slotHelp();
    }
    else if(link == "ShowQuickStart")
    {
        slotQuickstart();
    }
    else if(link == "SetupHome")
    {
        setupHomePath();
        const int N = tabMaps->count();
        for(int n = 0; n < N; n++)
        {
            CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->widget(n));
            if(canvas != nullptr)
            {
                canvas->buildHelpText();
            }
        }
    }
    else if(link == "GetMaps")
    {
        CMapList * list = dynamic_cast<CMapList*>(tabMaps->currentWidget());
        if(list == nullptr)
        {
            return;
        }
        list->slotMapHonk();
    }
    else if(link == "MapFolders")
    {
        slotSetupMapPath();
    }
    else if(link == "VrtBuilder")
    {
        slotBuildVrt();
    }
    else if(link == "DemFolders")
    {
        slotSetupDemPath();
    }
}

void CMainWindow::slotLinkActivated(const QUrl& url)
{
    const QString& link = url.toString();
    if(link.startsWith("http"))
    {
        QDesktopServices::openUrl(url);
        return;
    }

    slotLinkActivated(link);
}

void CMainWindow::slotSetupWptIcons()
{
    CWptIconDialog dlg(this);
    dlg.exec();
}


void CMainWindow::slotCloseTab()
{
    CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->currentWidget());
    if(canvas == nullptr)
    {
        QWidget * widget = tabWidget->currentWidget();
        if(widget != nullptr)
        {
            widget->deleteLater();
        }
    }
}

void CMainWindow::slotToggleDocks()
{
    if (docksVisible())
    {
        dockStates = saveState();
        hideDocks();
    }
    else
    {
        showDocks();
        if (!dockStates.isEmpty())
        {
            restoreState(dockStates);
        }
    }
}

bool CMainWindow::docksVisible() const
{
    for (QDockWidget * const & dock : docks)
    {
        if (!dock->isHidden())
        {
            return true;
        }
    }
    return false;
}

void CMainWindow::showDocks() const
{
    if (activeDocks.isEmpty())
    {
        for (QDockWidget * const & dock : docks)
        {
            dock->show();
        }
    }
    else
    {
        const QList<QDockWidget *> docksToShow(activeDocks);
        for (QDockWidget * const & dock : docksToShow)
        {
            dock->show();
        }
    }
}

void CMainWindow::hideDocks()
{
    activeDocks.clear();
    for (QDockWidget * const & dock : docks)
    {
        if (!dock->isHidden())
        {
            dock->hide();
            activeDocks << dock;
        }
    }
}

void CMainWindow::slotDockVisibilityChanged(bool visible)
{
    if (visible)
    {
        activeDocks.clear();
    }
    else
    {
        for (QDockWidget * const & dock : docks)
        {
            if (!dock->isHidden())
            {
                visible = true;
                break;
            }
        }
    }
    actionToggleDocks->setChecked(visible);
}

void CMainWindow::slotFullScreen()
{
    QMutexLocker lock(&CMainWindow::mutex);

    Qt::WindowStates state = windowState();
    if(state == Qt::WindowFullScreen)
    {
        displayRegular();
    }
    else
    {
        displayMode = state;
        displayFullscreen();
    }
}

void CMainWindow::slotStartQMapTool()
{
    QProcess::startDetached("qmaptool");
}

void CMainWindow::displayRegular()
{
    if (!dockStates.isEmpty())
    {
        restoreState(dockStates);
    }
    tabWidget->tabBar()->setVisible(true);
    statusBar()->setVisible(true);
    if (menuVisible)
    {
        menuBar()->setVisible(true);
    }
    actionFullScreen->setIcon(QIcon(":/icons/32x32/FullScreen.png"));
    setWindowState(displayMode);
}

void CMainWindow::displayFullscreen()
{
    dockStates = saveState();
    setWindowState(Qt::WindowFullScreen);
    statusBar()->setVisible(false);
    menuVisible = menuBar()->isVisible();
    // menu is handled dynamically as on some platforms (e.g. ubuntu with unity)
    // the menu is not visible but it's actions are active nevertheless
    if (menuVisible)
    {
        menuBar()->setVisible(false);
    }
    if (docksVisible())
    {
        hideDocks();
    }
    if (!toolBarConfig->visibleInFullscreen())
    {
        toolBar->setVisible(false);
    }
    tabWidget->tabBar()->setVisible(false);
    actionFullScreen->setIcon(QIcon(":/icons/32x32/RegularScreen.png"));
}

#ifdef WIN32

static void sendDeviceEvent(DWORD unitmask, bool add)
{
    for (char i = 0; i < 26; ++i)
    {
        if (unitmask & 0x1)
        {
            QString path = QString(i + 'A') + ":/";
            qDebug() << "sendDeviceEvent" << path << add;
            CEventDevice * event = new CEventDevice(path, add);
            QCoreApplication::postEvent(CDeviceWatcherWindows::self(), event);
            //qDebug() << "postEvent";
        }
        unitmask = unitmask >> 1;
    }
}


bool CMainWindow::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
    MSG* msg = (MSG*)message;
    //qDebug() << "nativeEvent" << eventType << msg->message << msg->lParam << msg->wParam;

    if (WM_DEVICECHANGE == msg->message)
    {
        //qDebug() << "WM_DEVICECHANGE";
        PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)msg->lParam;
        switch (msg->wParam)
        {
        case DBT_DEVICEARRIVAL:
        {
            qDebug() << "DBT_DEVICEARRIVAL"<< pHdr->dbch_devicetype;
            if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                PDEV_BROADCAST_VOLUME pHdrv = (PDEV_BROADCAST_VOLUME)pHdr;
                sendDeviceEvent(pHdrv->dbcv_unitmask, true);
            }
            break;
        }

        case  DBT_DEVICEREMOVECOMPLETE:
        {
            qDebug() << "DBT_DEVICEREMOVECOMPLETE" << pHdr->dbch_devicetype;
            if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                PDEV_BROADCAST_VOLUME pHdrv = (PDEV_BROADCAST_VOLUME)pHdr;
                sendDeviceEvent(pHdrv->dbcv_unitmask, false);
            }
            break;
        }

        default:
        {
            break;
        }
        }
    }

    return QWidget::nativeEvent(eventType, message, result);
}
#endif // WIN32

void CMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        QFileInfo fi(urls[0].path());
        QString ext = fi.suffix().toUpper();

        if ((ext == "QMS") || (ext == "GPX") || (ext == "SLF") || (ext == "FIT") || (ext == "TCX") || (ext == "SML") || (ext == "LOG"))
        {
            event->acceptProposedAction();
        }
    }
}


void CMainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    QStringList filenames;
    for(const QUrl &url : urls)
    {
        filenames << url.toLocalFile();
    }

    loadGISData(filenames);

    event->acceptProposedAction();
}

void CMainWindow::slotSanityTest()
{
    projPJ pjsrc = pj_init_plus("+init=epsg:32661");
    if(pjsrc == nullptr)
    {
        QMessageBox::critical(this, tr("Fatal...")
                              ,tr("QMapShack detected a badly installed Proj4 library. The translation tables for EPSG projections usually stored in /usr/share/proj are missing. Please contact the package maintainer of your distribution to fix it.")
                              ,QMessageBox::Close);

        deleteLater();
        return;
    }

    pj_free(pjsrc);


    qDebug() << "Sanity test passed.";
}

