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
#include "gis/trk/CKnownExtension.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"
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

CMainWindow * CMainWindow::pSelf = 0;

CMainWindow::CMainWindow()
{
    SETTINGS;

    pSelf = this;
    setupUi(this);
    setWindowTitle(WHAT_STR);

    initWptIcons();

    IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt(), this);
    CKnownExtension::init(IUnit::self());

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


    connect(actionAbout,                 SIGNAL(triggered()),            this,      SLOT(slotAbout()));
    connect(actionHelp,                  SIGNAL(triggered()),            this,      SLOT(slotHelp()));
    connect(actionAddMapView,            SIGNAL(triggered()),            this,      SLOT(slotAddCanvas()));
    connect(actionCloneMapView,          SIGNAL(triggered()),            this,      SLOT(slotCloneCanvas()));
    connect(actionShowScale,             SIGNAL(changed()),              this,      SLOT(slotUpdateCurrentWidget()));
    connect(actionShowGrid,              SIGNAL(changed()),              this,      SLOT(update()));
    connect(actionPOIText,               SIGNAL(changed()),              this,      SLOT(slotUpdateCurrentWidget()));
    connect(actionMapToolTip,            SIGNAL(changed()),              this,      SLOT(slotUpdateCurrentWidget()));
    connect(actionNightDay,              SIGNAL(changed()),              this,      SLOT(slotUpdateCurrentWidget()));
    connect(actionProfileIsWindow,       SIGNAL(toggled(bool)),          this,      SLOT(slotSetProfileMode(bool)));
    connect(actionSetupMapFont,          SIGNAL(triggered()),            this,      SLOT(slotSetupMapFont()));
    connect(actionSetupGrid,             SIGNAL(triggered()),            this,      SLOT(slotSetupGrid()));
    connect(actionSetupMapPaths,         SIGNAL(triggered()),            this,      SLOT(slotSetupMapPath()));
    connect(actionSetupDEMPaths,         SIGNAL(triggered()),            this,      SLOT(slotSetupDemPath()));
    connect(actionSetupMapView,          SIGNAL(triggered()),            this,      SLOT(slotSetupMapView()));
    connect(actionSetupTimeZone,         SIGNAL(triggered()),            this,      SLOT(slotSetupTimeZone()));
    connect(actionSetupUnits,            SIGNAL(triggered()),            this,      SLOT(slotSetupUnits()));
    connect(actionSetupWorkspace,        SIGNAL(triggered()),            this,      SLOT(slotSetupWorkspace()));
    connect(actionSetupCoordFormat,      SIGNAL(triggered(bool)),        this,      SLOT(slotSetupCoordFormat()));
    connect(actionImportDatabase,        SIGNAL(triggered()),            this,      SLOT(slotImportDatabase()));
    connect(actionSaveGISData,           SIGNAL(triggered()),            gisWidget, SLOT(slotSaveAll()));
    connect(actionLoadGISData,           SIGNAL(triggered()),            this,      SLOT(slotLoadGISData()));
    connect(actionVrtBuilder,            SIGNAL(triggered()),            this,      SLOT(slotBuildVrt()));
    connect(actionStoreView,             SIGNAL(triggered()),            this,      SLOT(slotStoreView()));
    connect(actionLoadView,              SIGNAL(triggered()),            this,      SLOT(slotLoadView()));
    connect(actionClose,                 SIGNAL(triggered()),            this,      SLOT(close()));
    connect(actionCreateRoutinoDatabase, SIGNAL(triggered()),            this,      SLOT(slotCreateRoutinoDatabase()));
    connect(actionPrintMap,              SIGNAL(triggered()),            this,      SLOT(slotPrintMap()));
    connect(tabWidget,                   SIGNAL(tabCloseRequested(int)), this,      SLOT(slotTabCloseRequest(int)));

    connect(tabWidget,                   SIGNAL(currentChanged(int)),    this,      SLOT(slotCurrentTabCanvas(int)));
    connect(tabMaps,                     SIGNAL(currentChanged(int)),    this,      SLOT(slotCurrentTabMaps(int)));
    connect(tabDem,                      SIGNAL(currentChanged(int)),    this,      SLOT(slotCurrentTabDem(int)));

    cfg.beginGroup("Canvas");
    CMapDraw::loadMapPath(cfg);
    CDemDraw::loadDemPath(cfg);

    cfg.beginGroup("Views");
    QStringList names = cfg.childGroups();

    foreach(const QString &name, names)
    {
        CCanvas * view = new CCanvas(tabWidget, name);
        tabWidget->addTab(view, view->objectName());
        connect(view, SIGNAL(sigMousePosition(QPointF,qreal)), this, SLOT(slotMousePosition(QPointF, qreal)));

        cfg.beginGroup(name);
        view->loadConfig(cfg);
        cfg.endGroup(); // name
    }
    if(names.isEmpty())
    {
        CCanvas * view = new CCanvas(tabWidget,"");
        tabWidget->addTab(view, view->objectName());
        connect(view, SIGNAL(sigMousePosition(QPointF, qreal)), this, SLOT(slotMousePosition(QPointF, qreal)));
    }
    cfg.endGroup(); // Views

    actionShowScale->setChecked(cfg.value("isScaleVisible", true).toBool());
    actionShowGrid->setChecked(cfg.value("isGridVisible", true).toBool());
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

    lblPosGrid = new QLabel(status);
    status->addPermanentWidget(lblPosGrid);

    menuWindow->addAction(dockMaps->toggleViewAction());
    menuWindow->addAction(dockDem->toggleViewAction());
    menuWindow->addAction(dockGis->toggleViewAction());
    menuWindow->addAction(dockRte->toggleViewAction());

    loadGISData(qlOpts->arguments);
}

CMainWindow::~CMainWindow()
{
    int cnt = 0;

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
    cfg.remove("");

    for(int i = 0; i < tabWidget->count(); i++)
    {
        CCanvas * view = dynamic_cast<CCanvas*>(tabWidget->widget(i));
        if(view == 0)
        {
            allOtherTabs << tabWidget->widget(i);
            continue;
        }
        cnt++;
        // save views
        cfg.beginGroup(view->objectName());
        view->saveConfig(cfg);
        cfg.endGroup();

        allViews << view;
    }
    cfg.endGroup(); // Views

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

    IUnit::coord_format_e coordFormat;
    IUnit::getCoordFormat(coordFormat);
    cfg.setValue("Units/coordFormat", coordFormat);
}

QWidget * CMainWindow::getBestWidgetForParent()
{
    QWidget * w = CProgressDialog::self();
    if(w)
    {
        return w;
    }

    w = self().getVisibleCanvas();
    if(w)
    {
        return w;
    }

    return &self();
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

bool CMainWindow::profileIsWindow()
{
    return actionProfileIsWindow->isChecked();
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
    if(tabWidget->indexOf(w) == NOIDX)
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


void CMainWindow::getEelevationAt(SGisLine &line)
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

void CMainWindow::getEelevationAt(const QPolygonF &pos, QPolygonF& ele)
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

    CCanvas * canvas = new CCanvas(tabWidget,"");
    tabWidget->addTab(canvas, canvas->objectName());
    connect(canvas, SIGNAL(sigMousePosition(QPointF, qreal)), this, SLOT(slotMousePosition(QPointF, qreal)));

    tabWidget->setCurrentWidget(canvas);
}

void CMainWindow::slotCloneCanvas()
{
    CCanvas * source = getVisibleCanvas();
    if(source == 0)
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
    if(target == 0)
    {
        return;
    }

    target->loadConfig(view);

    SETTINGS;
    cfg.beginGroup("Canvas");
    cfg.beginGroup("Views");
    cfg.beginGroup(target->objectName());
    target->saveConfig(cfg);
    cfg.endGroup();
    cfg.endGroup();
    cfg.endGroup();
}

void CMainWindow::slotTabCloseRequest(int i)
{
    QMutexLocker lock(&CMapItem::mutexActiveMaps);

    QWidget * w = tabWidget->widget(i);

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
}

void CMainWindow::slotMousePosition(const QPointF& pos, qreal ele)
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

void CMainWindow::slotSetupMapView()
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
    foreach(const QString &filename, filenames)
    {
        gisWidget->loadGisProject(filename);
    }
}


void CMainWindow::slotStoreView()
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas == 0)
    {
        return;
    }

    SETTINGS;
    QString path = cfg.value("Paths/lastViewPath", QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName( this, tr("Select output file"), path,"QMapShack View (*.view)");

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
    QString filename = QFileDialog::getOpenFileName(this, tr("Select file to load"), path, "QMapShack View (*.view)");

    if(filename.isEmpty())
    {
        return;
    }

    slotAddCanvas();

    CCanvas * canvas = getVisibleCanvas();
    if(canvas == 0)
    {
        return;
    }

    QSettings view(filename, QSettings::IniFormat);
    canvas->loadConfig(view);

    cfg.beginGroup("Canvas");
    cfg.beginGroup("Views");
    cfg.beginGroup(canvas->objectName());
    canvas->saveConfig(cfg);
    cfg.endGroup();
    cfg.endGroup();
    cfg.endGroup();

    QFileInfo fi(filename);
    path = fi.absolutePath();
    cfg.setValue("Paths/lastViewPath", path);
}

void CMainWindow::slotSetProfileMode(bool on)
{
    for(int i = 0; i < tabWidget->count(); i++)
    {
        CCanvas * view = dynamic_cast<CCanvas*>(tabWidget->widget(i));
        if(view == 0)
        {
            continue;
        }

        view->showProfileAsWindow(on);
    }
}

void CMainWindow::slotPrintMap()
{
    CCanvas * canvas = getVisibleCanvas();
    if(canvas == 0)
    {
        return;
    }

    canvas->setMousePrint();
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

        if ( (ext == "QMS") || (ext == "GPX"))
        {
            event->acceptProposedAction();
        }
    }
}


void CMainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    QUrl url;

    QStringList filenames;
    foreach(url, urls)
    {
        filenames << url.toLocalFile();
    }

    loadGISData(filenames);

    event->acceptProposedAction();
}
