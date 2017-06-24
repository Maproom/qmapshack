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

#include "CAbout.h"
#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "config.h"
#include "dem/CDemDraw.h"
#include "dem/CDemList.h"
#include "gis/CGisWidget.h"
#include "gis/IGisLine.h"
#include "gis/WptIcons.h"
#include "gis/db/CSetupWorkspace.h"
#include "gis/prj/IGisProject.h"
#include "gis/trk/CActivityTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconDialog.h"
#include "map/CMapDraw.h"
#include "map/CMapItem.h"
#include "map/CMapList.h"
#include "tool/CImportDatabase.h"
#include "tool/CMapVrtBuilder.h"
#include "tool/CRoutinoDatabaseBuilder.h"
#include "units/CCoordFormatSetup.h"
#include "units/CTimeZoneSetup.h"
#include "units/CUnitsSetup.h"
#include "units/IUnit.h"
#include "version.h"
#include "toolbar/CToolBarSetup.h"

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

CMainWindow::CMainWindow()
    : id(qrand())
{
    qDebug() << "Application ID:" << id;
    SETTINGS;

    pSelf = this;
    setupUi(this);
    setWindowTitle(WHAT_STR);

    initWptIcons();

    IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt(), this);
    CKnownExtension::init(IUnit::self());
    CActivityTrk::init();

    gisWidget = new CGisWidget(menuProject, this);
    dockGis->setWidget(gisWidget);

    // start ---- restore window geometry -----
    if ( cfg.contains("MainWindow/geometry"))
    {
        restoreGeometry(cfg.value("MainWindow/geometry").toByteArray());
    }
    else
    {
        QTimer::singleShot(500, this, SLOT(showMaximized()));
    }

    if ( cfg.contains("MainWindow/state"))
    {
        restoreState(cfg.value("MainWindow/state").toByteArray());
    }
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
    connect(actionSaveGISData,           &QAction::triggered,            gisWidget, &CGisWidget::slotSaveAll);
    connect(actionLoadGISData,           &QAction::triggered,            this,      &CMainWindow::slotLoadGISData);
    connect(actionVrtBuilder,            &QAction::triggered,            this,      &CMainWindow::slotBuildVrt);
    connect(actionStoreView,             &QAction::triggered,            this,      &CMainWindow::slotStoreView);
    connect(actionLoadView,              &QAction::triggered,            this,      &CMainWindow::slotLoadView);
    connect(actionClose,                 &QAction::triggered,            this,      &CMainWindow::close);
    connect(actionCreateRoutinoDatabase, &QAction::triggered,            this,      &CMainWindow::slotCreateRoutinoDatabase);
    connect(actionPrintMap,              &QAction::triggered,            this,      &CMainWindow::slotPrintMap);
    connect(actionSetupWaypointIcons,    &QAction::triggered,            this,      &CMainWindow::slotSetupWptIcons);
    connect(actionCloseTab,              &QAction::triggered,            this,      &CMainWindow::slotCloseTab);
    connect(tabWidget,                   &QTabWidget::tabCloseRequested, this,      &CMainWindow::slotTabCloseRequest);

    connect(tabWidget,                   &QTabWidget::currentChanged,    this,      &CMainWindow::slotCurrentTabCanvas);
    connect(tabMaps,                     &QTabWidget::currentChanged,    this,      &CMainWindow::slotCurrentTabMaps);
    connect(tabDem,                      &QTabWidget::currentChanged,    this,      &CMainWindow::slotCurrentTabDem);

    cfg.beginGroup("Canvas");
    CMapDraw::loadMapPath(cfg);
    CDemDraw::loadDemPath(cfg);

    cfg.beginGroup("Views");
    QStringList names = cfg.childGroups();

    for(const QString &name : names)
    {
        CCanvas * view = new CCanvas(tabWidget, name);
        tabWidget->addTab(view, view->objectName());
        connect(view, &CCanvas::sigMousePosition, this, &CMainWindow::slotMousePosition);

        cfg.beginGroup(name);
        view->loadConfig(cfg);
        cfg.endGroup(); // name
    }
    if(names.isEmpty())
    {
        CCanvas * view = new CCanvas(tabWidget, QString());
        view->loadConfig(cfg);
        tabWidget->addTab(view, view->objectName());
        connect(view, &CCanvas::sigMousePosition, this, &CMainWindow::slotMousePosition);
    }
    cfg.endGroup(); // Views
    testForNoView();

    CCanvas::gisLayerOpacity = cfg.value("gisLayerOpacity",1.0).toFloat();
    gisWidget->setOpacity(CCanvas::gisLayerOpacity);

    actionShowScale->setChecked(cfg.value("isScaleVisible", true).toBool());
    actionShowGrid->setChecked(cfg.value("isGridVisible", false).toBool());
    actionPOIText->setChecked(cfg.value("POIText", true).toBool());
    actionMapToolTip->setChecked(cfg.value("MapToolTip", true).toBool());
    actionNightDay->setChecked(cfg.value("isNight", false).toBool());
    actionFlipMouseWheel->setChecked(cfg.value("flipMouseWheel", false).toBool());
    actionProfileIsWindow->setChecked(cfg.value("profileIsWindow", false).toBool());
    mapFont = cfg.value("mapFont", font()).value<QFont>();
    tabWidget->setCurrentIndex(cfg.value("visibleCanvas",0).toInt());
    cfg.endGroup(); // Canvas

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


    QStatusBar * status = statusBar();
    lblPosWGS84 = new QLabel(status);
    status->addPermanentWidget(lblPosWGS84);

    lblElevation = new QLabel(status);
    status->addPermanentWidget(lblElevation);

    lblSlope = new QLabel(status);
    status->addPermanentWidget(lblSlope);

    lblPosGrid = new QLabel(status);
    status->addPermanentWidget(lblPosGrid);

    menuWindow->addAction(toolBar->toggleViewAction());
    menuWindow->addAction(dockMaps->toggleViewAction());
    menuWindow->addAction(dockDem->toggleViewAction());
    menuWindow->addAction(dockGis->toggleViewAction());
    menuWindow->addAction(dockRte->toggleViewAction());

    prepareMenuForMac();

    loadGISData(qlOpts->arguments);

    QTimer::singleShot(100, this, SLOT(slotSanityTest()));
}

void CMainWindow::prepareMenuForMac()
{
    dockMaps->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockMaps->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockDem->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockGis->toggleViewAction()->setMenuRole(QAction::NoRole);
    dockRte->toggleViewAction()->setMenuRole(QAction::NoRole);
}

CMainWindow::~CMainWindow()
{
    CActivityTrk::release();

    SETTINGS;
    cfg.setValue("MainWindow/state", saveState());
    cfg.setValue("MainWindow/geometry", saveGeometry());
    cfg.setValue("MainWindow/units", IUnit::self().type);


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
        like CGisWidget safely upon their destruction. (e.g. CMouseRangeTrk to reset
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

    CCanvas * canvas = new CCanvas(tabWidget, QString());
    tabWidget->addTab(canvas, canvas->objectName());
    connect(canvas, &CCanvas::sigMousePosition, this, &CMainWindow::slotMousePosition);

    tabWidget->setCurrentWidget(canvas);

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
        label->setText(tr("Use <b>Menu->View->Add Map View</b> to open a new view. Or <b>Menu->File->Load Map View</b> to restore a saved one. Or click <a href='newview'>here</a>."));
        label->setObjectName("NoViewInfo");
        connect(label, &QLabel::linkActivated, this, &CMainWindow::slotLinkActivated);
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
        lblElevation->setText(tr("Ele: %1%2").arg(val).arg(unit));
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
        lblSlope->setText(tr("Slope: %1%2").arg(val).arg(QChar(0260)));
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
    CToolBarSetup dlg(this);
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
        gisWidget->loadGisProject(filename);
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
    if(link == "newview")
    {
        actionAddMapView->trigger();
    }
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

        if ((ext == "QMS") || (ext == "GPX") || (ext == "SLF") || (ext == "FIT") || (ext == "TCX"))
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

