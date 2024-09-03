/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "canvas/CCanvas.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvasSetup.h"
#include "dem/CDemDraw.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/GeoMath.h"
#include "gis/IGisLine.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/trk/CGisItemTrk.h"
#include "grid/CGrid.h"
#include "grid/CGridSetup.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconManager.h"
#include "map/CMapDraw.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/CMouseEditArea.h"
#include "mouse/CMouseEditRte.h"
#include "mouse/CMouseEditTrk.h"
#include "mouse/CMouseMoveWpt.h"
#include "mouse/CMouseNormal.h"
#include "mouse/CMousePrint.h"
#include "mouse/CMouseRadiusWpt.h"
#include "mouse/CMouseRangeTrk.h"
#include "mouse/CMouseRuler.h"
#include "mouse/CMouseSelect.h"
#include "mouse/CMouseWptBubble.h"
#include "plot/CPlotProfile.h"
#include "poi/CPoiDraw.h"
#include "poi/IPoiItem.h"
#include "realtime/CRtDraw.h"
#include "units/IUnit.h"
#include "widgets/CColorLegend.h"

qreal CCanvas::gisLayerOpacity = 1.0;

#define X_OFF_STATUS 20
#define Y_OFF_STATUS 50
#define WIDTH_PROFILE_LARGE 300
#define HEIGHT_PROFILE_LARGE 120
#define WIDTH_PROFILE_SMALL 200
#define HEIGHT_PROFILE_SMALL 80

inline QSize getTrackProfileSize(int height) {
  return height > 700 ? QSize(WIDTH_PROFILE_LARGE, HEIGHT_PROFILE_LARGE)
                      : QSize(WIDTH_PROFILE_SMALL, HEIGHT_PROFILE_SMALL);
}

CCanvas::CCanvas(QWidget* parent, const QString& name) : QWidget(parent) {
  setFocusPolicy(Qt::WheelFocus);

  if (name.isEmpty()) {
    for (int count = 1;; ++count) {
      QString name = tr("View %1").arg(count);
      if (nullptr == CMainWindow::self().findChild<CCanvas*>(name)) {
        setObjectName(name);
        break;
      }
    }
  } else {
    setObjectName(name);
  }

  setMouseTracking(true);

  grabGesture(Qt::PinchGesture);

  map = new CMapDraw(this);
  poi = new CPoiDraw(this);
  grid = new CGrid(map);
  dem = new CDemDraw(this);
  gis = new CGisDraw(this);
  rt = new CRtDraw(this);

  // map has to be first!
  allDrawContext << map << poi << dem << gis << rt;

  mouse = new CMouseAdapter(this);
  mouse->setDelegate(new CMouseNormal(gis, this, mouse));

  connect(map, &CMapDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);
  connect(poi, &CPoiDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);
  connect(dem, &CDemDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);
  connect(gis, &CGisDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);
  connect(rt, &CRtDraw::sigCanvasUpdate, this, &CCanvas::slotTriggerCompleteUpdate);

  timerToolTip = new QTimer(this);
  timerToolTip->setSingleShot(true);
  connect(timerToolTip, &QTimer::timeout, this, &CCanvas::slotToolTip);

  loadIndicator1 = new QMovie("://animation/loader.gif", QByteArray(), this);
  mapLoadIndicator = new QLabel(this);
  mapLoadIndicator->setMovie(loadIndicator1);
  loadIndicator1->start();
  mapLoadIndicator->show();

  loadIndicator2 = new QMovie("://animation/loader2.gif", QByteArray(), this);
  demLoadIndicator = new QLabel(this);
  demLoadIndicator->setMovie(loadIndicator2);
  loadIndicator2->start();
  demLoadIndicator->show();

  loadIndicator3 = new QMovie("://animation/loader3.gif", QByteArray(), this);
  poiLoadIndicator = new QLabel(this);
  poiLoadIndicator->setMovie(loadIndicator3);
  loadIndicator3->start();
  poiLoadIndicator->show();

  textStatusMessages = new QTextBrowser(this);
  textStatusMessages->setFrameStyle(QFrame::NoFrame);
  textStatusMessages->setMinimumWidth(300);
  textStatusMessages->hide();

  labelTrackStatistic = new QLabel(this);
  labelTrackStatistic->setAutoFillBackground(true);
  labelTrackStatistic->hide();

  labelTrackInfo = new QLabel(this);
  labelTrackInfo->setAutoFillBackground(true);
  labelTrackInfo->hide();

  connect(map, &CMapDraw::sigStartThread, mapLoadIndicator, &QLabel::show);
  connect(map, &CMapDraw::sigStopThread, mapLoadIndicator, &QLabel::hide);

  connect(poi, &CPoiDraw::sigStartThread, poiLoadIndicator, &QLabel::show);
  connect(poi, &CPoiDraw::sigStopThread, poiLoadIndicator, &QLabel::hide);

  connect(dem, &CDemDraw::sigStartThread, demLoadIndicator, &QLabel::show);
  connect(dem, &CDemDraw::sigStopThread, demLoadIndicator, &QLabel::hide);

  timerTrackOnFocus = new QTimer(this);
  timerTrackOnFocus->setSingleShot(false);
  timerTrackOnFocus->start(1000);

  connect(timerTrackOnFocus, &QTimer::timeout, this, &CCanvas::slotCheckTrackOnFocus);

  labelHelp = new QTextBrowser(this);
  labelHelp->setOpenLinks(false);
  connect(map, &CMapDraw::sigActiveMapsChanged, labelHelp, &QLabel::setVisible);
  connect(labelHelp, &QTextBrowser::anchorClicked, &CMainWindow::self(),
          static_cast<void (CMainWindow::*)(const QUrl&)>(&CMainWindow::slotLinkActivated));

  buildHelpText();
}

CCanvas::~CCanvas() {
  saveSizeTrackProfile();

  /* stop running drawing-threads and don't destroy unless they have finished*/
  for (IDrawContext* context : std::as_const(allDrawContext)) {
    context->quit();
  }
  for (IDrawContext* context : std::as_const(allDrawContext)) {
    context->wait();
  }

  /*
      Some mouse objects call methods from their canvas on destruction.
      So they are better deleted now explicitly before any other object
      in CCanvas is destroyed.
   */
  delete mouse;
}

void addHtmlRow(QString& text, const QString& col1, const QString& col2, const QString& td1, const QString& td2) {
  text += QString("<tr><td %3>%1</td><td %4>%2<div/></td></tr>").arg(col1, col2, td1, td2);
}

void addHtmlRow(QString& text, const QString& col1, const QString& td1) {
  text += QString("<tr><td %2>%1<div/></td></tr>").arg(col1, td1);
}

void CCanvas::buildHelpText() {
  QString home = CMainWindow::self().getHomePath();
  if (home.isEmpty()) {
    home = tr("None selected");
  }

  QString msg = "<table>";

  const QString& msgHelp = tr("<p align='right'>This help will be closed the moment you activate a map.</p>");
  addHtmlRow(msg, "", msgHelp, "", "");

  const QString& msgWelcome =
      tr("<h1>Welcome</h1>"
         "<p>Thank you for using QMapShack. QMapShack is a non-profit Open Source project. It's maintained by "
         "enthusiast and volunteers. It's very mature and useful but lacks certain amenities you might be used "
         "from 'free' commercial software. As a matter of fact we lack the resources to provide you with a centralized "
         "and "
         "convenient download of maps, elevation data and routing data. You have to download these resources "
         "from various sites on your own. But this page might help you a bit</p>");
  addHtmlRow(msg, "<img src='://pics/DockWidgets.png'/>", msgWelcome, "rowspan='3' style='padding-right: 20px;'", "");

  const QString& msgLetsStart = tr(
      "<h2>Let's Start</h2>"
      "<p>As a first step make yourself comfortable by arranging the dock "
      "widgets around this area to your personal liking. You can do this by grabbing the dock widget's title bars with "
      "your mouse (keep left button pressed on title bar) and move them where you want them. You can attach the "
      "dock widgets at the left, right, top or bottom. And you can stack them by placing them right above another "
      "dock widget. If you do not want to use the functionality of a dock widget you can close it and "
      "make QMapShack more simple for you. Also note that the tool bar is a dock widget, too. You can place it "
      "anywhere you like.</p>");

  addHtmlRow(msg, msgLetsStart, "");

  const QString& msgWiki =
      tr("<h2>The Wiki</h2>"
         "<p>QMapShack has an extensive <a href='ShowWiki'>Wiki</a> with a lot of information about how to use it. "
         "There is a <a href='ShowQuickStart'>Quick Start Guide</a> translated into several languages. Read it and you "
         "will "
         "discover the full variety of features QMapShack offers you.</p>");

  addHtmlRow(msg, msgWiki, "");

  const QString& msgCurHome = tr("<h2>&nbsp;</h2>"
                                 "<p>Current home:<br/>"
                                 "%1</p>")
                                  .arg(home);

  const QString& msgGiveHome = tr(
      "<h2>Give it a home</h2>"
      "<p>QMapShack needs a folder on your hard drive where all data is kept. Naturally this will grow very large. We "
      "recommend a folder with some subfolders:</p>"
      "<ul>"
      "<li>Maps - where all maps are stored</li>"
      "<li>DEM - where all elevation data is stored</li>"
      "<li>POI - where all POI collections are stored</li>"
      "<li>Routino - where routing data for the Routino offline router is stored</li>"
      "<li>BRouter - where routing data for the BRouter offline router is stored</li>"
      "<li>Databases - where you create databases to organize your GIS data</li>"
      "<li>GPX - where you keep GPX files</li>"
      "</ul>"
      "<p>Do you want to create these folders right now? Give me a <a  href='SetupHome'>path</a> and QMapShack will "
      "create all "
      "sub-folder and update all settings.</p>");

  addHtmlRow(msg, msgCurHome, msgGiveHome, "", "");

  const QString& msgMapLinks =
      tr("<h2>&nbsp;</h2>"
         "<p>Impatient Users:<p>"
         "<ul>"
         "<li><a href='GetMaps'>I want some online maps.</a></li>"
         "</ul>"
         "<p>Patient Users:<p>"
         "<ul>"
         "<li><a href='ShowQuickStart'>Quick Start Guide.</a></li>"
         "<li><a href='https://github.com/Maproom/qmapshack/wiki/DocGettingStarted#add-maps'>Getting Started.</a></li>"
         "<li><a href='https://github.com/Maproom/qmapshack/wiki/DocInstallMapDem'>Install Maps & DEM</a></li>"
         "<li><a href='https://github.com/Maproom/qmapshack/wiki/DocBasicsMapDem'>Basics Maps & DEM.</a></li>"
         "<li><a href='https://github.com/Maproom/qmapshack/wiki/DocMapDemSources'>Sources for Maps</a></li>"
         "</ul>");

  const QString& msgMaps = tr(
      "<h2>Maps!</h2>"
      "<p>A map must always be defined by a single file that is recognized by QMapShack. This file has to be placed "
      "into one "
      "of the registered map folders (<a href='MapFolders'>File->Setup Map Paths</a>). QMapShack will find it and list "
      "it. You can reload the list of maps by doing a right click on the map list and select 'Reload Maps' from the "
      "menu.</p> "
      "<p>A map must be activated to be visible. Right click on the map entry and select 'Activate' from the menu. The "
      "moment a map "
      "is activated this help will disappear. To see it again you simply add another map view "
      "(<a href='NewView'>View->Add Map View</a>).</p> "
      "<p>To start with some online maps click on the link for <a href='GetMaps'>impatient users</a>. If you have a "
      "Garmin GPS device "
      "with non-commercial maps you can copy the *.img and *.jnx files from your device into the map folder.</p> "
      "</p>If you own raster maps in a format that is supported by GDAL you have to create a virtual map definition "
      "file (*.vrt), first. "
      "A *.vrt file can reference one or several other files. You can use QMapShack's built-in VRT Builder (<a "
      "href='VrtBuilder'>Tool->VRT Builder</a>) "
      "to create such a file.</p>");

  addHtmlRow(msg, msgMapLinks, msgMaps, "", "");

  const QString msgDataLinks =
      tr("<h2>&nbsp;</h2>"
         "<ul>"
         "<li><a href='https://github.com/Maproom/qmapshack/wiki/DocGisDatabase'>Databases</a></li>"
         "<li><a href='https://github.com/Maproom/qmapshack/wiki/AdvProjects'>Databases & Projects</a></li>"
         "</ul>");

  const QString msgData =
      tr("<h2>Your Data</h2>"
         "<p>To load tracks and waypoints use <a href='LoadData'>File->Load GIS Data</a> from the menu. Each file will "
         "be an independent project "
         "in the workspace. You can manage your data in single files or you use a database. You can <a "
         "href='CreateDB'>create a database</a> in "
         "the database dockwidget.</p>");

  addHtmlRow(msg, msgDataLinks, msgData, "", "");

  const QString& msgDEMLinks =
      tr("<h2>&nbsp;</h2>"
         "<p>Impatient Users:<p>"
         "<ul>"
         "<li><a href='GetDems'>I want some online DEMs.</a></li>"
         "</ul>"
         "<p>Patient Users:<p>"
         "<ul>"
         "<li><a href='https://github.com/Maproom/qmapshack/wiki/DocMapDemSources'>Sources for DEM</a></li>"
         "</ul>");

  const QString& msgDem =
      tr("<h2>Elevation Data</h2>"
         "<p>Elevation data (Digital Elevation Model - DEM) is used in various functions of QMapShack and is "
         "completely independent from "
         "the maps. However the same principles as for maps apply. DEM data must always be defined by a single file "
         "that is recognized by "
         "QMapShack. This file has to be placed into one of the registered DEM folders (<a "
         "href='DemFolders'>File->Setup DEM Paths</a>). "
         "QMapShack will find it and list it. You can reload the list of DEM files by doing a right click on the DEM "
         "list and select "
         "'Reload DEM' from the menu.</p>"
         "<p>Similar to raster maps DEM files have to be wrapped by a *.vrt file to be recognized. A *.vrt file can "
         "reference one "
         "or several other files. Again, you can use QMapShack's built-in VRT Builder (<a href='VrtBuilder'>Tool->VRT "
         "Builder</a>) "
         "to create such a file.</p>"
         "<p>To start with some online DEMs you can click on the link for <a href='GetDems'>impatient users</a>. But "
         "keep in mind "
         "that using online DEMs slows down QMapShack significantly.</p>");

  addHtmlRow(msg, msgDEMLinks, msgDem, "", "");

  const QString& msgRouting =
      tr("<h2>Routing</h2>"
         "<p>QMapShack supports online routing services. But if you really want to make use of the routing "
         "possibilities you need "
         "the routing database stored locally. There are two offline routing engines available.</p>"
         "<h3>Routino</h3>"
         "<p>For Routino you have to create routing data from the Open Street Map database files. QMapShack has a "
         "built-in Routino "
         "Database Creator (<a href='CreateRoutino'>Tool->Create Routino Database</a>).</p>"
         "<h3>BRouter</h3>"
         "<p>A second option is to use a local BRouter installation with a local routing database. Use the <a "
         "href='BRouterSetup'>"
         "BRouter setup</a> to download and install all necessary data.</p>");

  addHtmlRow(msg, "", msgRouting, "", "");

  labelHelp->clear();
  labelHelp->setHtml(msg);
  labelHelp->moveCursor(QTextCursor::Start);
  labelHelp->ensureCursorVisible();
}

void CCanvas::setOverrideCursor(const QCursor& cursor, const QString& /*src*/) {
  //    qDebug() << "setOverrideCursor" << src;
  QApplication::setOverrideCursor(cursor);
}

void CCanvas::restoreOverrideCursor(const QString& /*src*/) {
  //    qDebug() << "restoreOverrideCursor" << src;
  QApplication::restoreOverrideCursor();
}

void CCanvas::changeOverrideCursor(const QCursor& cursor, const QString& /*src*/) {
  //    qDebug() << "changeOverrideCursor" << src;
  QApplication::changeOverrideCursor(cursor);
}

void CCanvas::triggerCompleteUpdate(CCanvas::redraw_e flags) {
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->slotTriggerCompleteUpdate(flags);
  }
}

void CCanvas::saveConfig(QSettings& cfg) {
  map->saveConfig(cfg);
  poi->saveConfig(cfg);
  dem->saveConfig(cfg);
  grid->saveConfig(cfg);
  cfg.setValue("posFocus", posFocus);
  cfg.setValue("proj", map->getProjection());
  cfg.setValue("scales", map->getScalesType());
  cfg.setValue("backColor", backColor.name());
}

void CCanvas::loadConfig(QSettings& cfg) {
  posFocus = cfg.value("posFocus", posFocus).toPointF();
  setProjection(cfg.value("proj", map->getProjection()).toString());
  setScales((CCanvas::scales_type_e)cfg.value("scales", map->getScalesType()).toInt());

  const QString& backColorStr = cfg.value("backColor", "#FFFFBF").toString();
  backColor = QColor(backColorStr);

  map->loadConfig(cfg);
  poi->loadConfig(cfg);
  dem->loadConfig(cfg);
  grid->loadConfig(cfg);

  const QList<IDrawContext*>& allContext = allDrawContext.mid(1);
  for (IDrawContext* context : allContext) {
    context->zoom(map->zoom());
  }
}

void CCanvas::setMap(const QString& filename) { map->buildMapList(filename); }

void CCanvas::abortMouse() { mouse->unfocus(); }

void CCanvas::resetMouse() {
  mouse->setDelegate(new CMouseNormal(gis, this, mouse));
  if (underMouse()) {
    while (QApplication::overrideCursor()) {
      CCanvas::restoreOverrideCursor("resetMouse");
    }
    CCanvas::setOverrideCursor(*mouse, "resetMouse");
  }
}

void CCanvas::mouseTrackingLost() { mouseLost = true; }

void CCanvas::setMouseMoveWpt(CGisItemWpt& wpt) { mouse->setDelegate(new CMouseMoveWpt(wpt, gis, this, mouse)); }

void CCanvas::setMouseRadiusWpt(CGisItemWpt& wpt) { mouse->setDelegate(new CMouseRadiusWpt(wpt, gis, this, mouse)); }

void CCanvas::setMouseEditTrk(const QPointF& pt) { mouse->setDelegate(new CMouseEditTrk(pt, gis, this, mouse)); }

void CCanvas::setMouseEditRte(const QPointF& pt) { mouse->setDelegate(new CMouseEditRte(pt, gis, this, mouse)); }

void CCanvas::setMouseEditTrk(CGisItemTrk& trk) { mouse->setDelegate(new CMouseEditTrk(trk, gis, this, mouse)); }

void CCanvas::setMouseRangeTrk(CGisItemTrk& trk) { mouse->setDelegate(new CMouseRangeTrk(trk, gis, this, mouse)); }

void CCanvas::setMouseEditArea(const QPointF& pt) { mouse->setDelegate(new CMouseEditArea(pt, gis, this, mouse)); }

void CCanvas::setMouseEditArea(CGisItemOvlArea& area) {
  mouse->setDelegate(new CMouseEditArea(area, gis, this, mouse));
}

void CCanvas::setMouseEditRte(CGisItemRte& rte) { mouse->setDelegate(new CMouseEditRte(rte, gis, this, mouse)); }

void CCanvas::setMouseWptBubble(const IGisItem::key_t& key) {
  mouse->setDelegate(new CMouseWptBubble(key, gis, this, mouse));
}

void CCanvas::setMouseRuler() { mouse->setDelegate(new CMouseRuler(gis, this, mouse)); }

void CCanvas::setMousePrint() { mouse->setDelegate(new CMousePrint(gis, this, mouse)); }

void CCanvas::setMouseSelect() { mouse->setDelegate(new CMouseSelect(gis, this, mouse)); }

void CCanvas::reportStatus(const QString& key, const QString& msg) {
  if (msg.isEmpty()) {
    statusMessages.remove(key);
  } else {
    statusMessages[key] = msg;
  }

  QString report;
  QStringList keys = statusMessages.keys();
  keys.sort();
  for (const QString& key : std::as_const(keys)) {
    report += statusMessages[key] + "\n";
  }

  if (report.isEmpty()) {
    textStatusMessages->hide();
  } else {
    textStatusMessages->setMinimumWidth(statusMessages.contains("CMouseRuler") ? 400 : 300);
    textStatusMessages->show();
    textStatusMessages->setText(report);

    qreal h;
    h = height() - Y_OFF_STATUS - getTrackProfileSize(height()).height() - 40;
    h = qMin(h, textStatusMessages->document()->size().height() + 10);
    textStatusMessages->setMinimumHeight(h);
    textStatusMessages->setMaximumHeight(h);
  }
  update();
}

void CCanvas::resizeEvent(QResizeEvent* e) {
  if (!setDrawContextSize(e->size())) {
    // reschedule resize event because one of the draw context threads is still running
    // and blocking the access to internal data.
    QApplication::postEvent(this, new QResizeEvent(e->size(), e->oldSize()));
    return;
  }

  needsRedraw = eRedrawAll;
  QWidget::resizeEvent(e);

  const QRect& r = rect();

  // move map loading indicator to new center of canvas
  QPoint p1(mapLoadIndicator->width() >> 1, mapLoadIndicator->height() >> 1);
  mapLoadIndicator->move(r.center() - p1);

  QPoint p2(demLoadIndicator->width() >> 1, demLoadIndicator->height() >> 1);
  demLoadIndicator->move(r.center() - p2);

  QPoint p3(poiLoadIndicator->width() >> 1, poiLoadIndicator->height() >> 1);
  poiLoadIndicator->move(r.center() - p3);

  textStatusMessages->move(X_OFF_STATUS, Y_OFF_STATUS);

  slotUpdateTrackInfo(false);
  setSizeTrackProfile();

  QSize s = e->size() - QSize(50, 50);
  labelHelp->move(25, 25);
  labelHelp->resize(s);
  buildHelpText();

  emit sigResize(e->size());
}

void CCanvas::paintEvent(QPaintEvent*) {
  if (!isVisible()) {
    return;
  }

  QPainter p;
  p.begin(this);
  USE_ANTI_ALIASING(p, true);

  // fill the background with default pattern
  p.fillRect(rect(), backColor);

  // ----- start to draw thread based content -----
  // move coordinate system to center of the screen
  p.translate(width() >> 1, height() >> 1);

  map->draw(p, needsRedraw, posFocus);
  poi->draw(p, needsRedraw, posFocus);
  dem->draw(p, needsRedraw, posFocus);
  p.setOpacity(gisLayerOpacity);
  gis->draw(p, needsRedraw, posFocus);
  rt->draw(p, needsRedraw, posFocus);
  p.setOpacity(1.0);

  // restore coordinate system to default
  p.resetTransform();
  // ----- start to draw fast content -----

  grid->draw(p, rect());
  if (map->isFinished() && dem->isFinished()) {
    if (gis->isFinished()) {
      gis->draw(p, rect());
    }
    if (rt->isFinished()) {
      rt->draw(p, rect());
    }
  }

  drawScale(p);
  mouse->draw(p, needsRedraw, rect());

  drawStatusMessages(p);
  drawTrackStatistic(p);

  p.end();
  needsRedraw = eRedrawNone;
}

void CCanvas::mousePressEvent(QMouseEvent* e) {
  if (labelHelp->isVisible()) {
    return;
  }

  if (!mousePressMutex.tryLock()) {
    return;
  }

  mouse->mousePressEvent(e);
  QWidget::mousePressEvent(e);
  e->accept();

  mousePressMutex.unlock();
}

void CCanvas::mouseMoveEvent(QMouseEvent* e) {
  QPointF pos = e->pos();
  map->convertPx2Rad(pos);
  qreal ele = dem->getElevationAt(pos, true);
  qreal slope = dem->getSlopeAt(pos, true);
  emit sigMousePosition(pos * RAD_TO_DEG, ele, slope);

  mouse->mouseMoveEvent(e);
  QWidget::mouseMoveEvent(e);
  e->accept();
}

void CCanvas::mouseReleaseEvent(QMouseEvent* e) {
  mouse->mouseReleaseEvent(e);
  QWidget::mouseReleaseEvent(e);
  e->accept();
}

void CCanvas::mouseDoubleClickEvent(QMouseEvent* e) {
  mouse->mouseDoubleClickEvent(e);
  QWidget::mouseDoubleClickEvent(e);
}

void CCanvas::wheelEvent(QWheelEvent* e) {
  if (labelHelp->isVisible()) {
    return;
  }

  mouse->wheelEvent(e);

  // angleDelta() returns the eighths of a degree
  // of the mousewheel
  // -> zoom in/out every 15 degrees = every 120 eights
  const int EIGHTS_ZOOM = 15 * 8;
  zoomAngleDelta += e->angleDelta().y();
  if (abs(zoomAngleDelta) < EIGHTS_ZOOM) {
    return;
  }

  zoomAngleDelta = 0;

  QPointF pos = e->position();
  QPointF pt1 = pos;

  map->convertPx2Rad(pt1);
  setZoom(CMainWindow::self().flipMouseWheel() ? (e->angleDelta().y() < 0) : (e->angleDelta().y() > 0), needsRedraw);
  map->convertRad2Px(pt1);

  moveMap(pos - pt1);

  update();
}

void CCanvas::enterEvent(QEvent* e) {
  if (labelHelp->isVisible()) {
    return;
  }

  Q_UNUSED(e);
  CCanvas::setOverrideCursor(*mouse, "enterEvent");
  setMouseTracking(true);
}

void CCanvas::leaveEvent(QEvent*) {
  // bad hack to stop bad number of override cursors.
  while (QApplication::overrideCursor()) {
    CCanvas::restoreOverrideCursor("leaveEvent");
  }

  setMouseTracking(false);
}

void CCanvas::keyPressEvent(QKeyEvent* e) {
  qDebug() << Qt::hex << e->key();
  bool doUpdate = true;

  switch (e->key()) {
    case Qt::Key_Plus:
      setZoom(true, needsRedraw);
      break;

    case Qt::Key_Minus:
      setZoom(false, needsRedraw);
      break;

    /* move the map with keys up, down, left and right */
    case Qt::Key_Up:
      moveMap(QPointF(0, height() / 4));
      break;

    case Qt::Key_Down:
      moveMap(QPointF(0, -height() / 4));
      break;

    case Qt::Key_Left:
      moveMap(QPointF(width() / 4, 0));
      break;

    case Qt::Key_Right:
      moveMap(QPointF(-width() / 4, 0));
      break;

    case Qt::Key_Escape: {
      break;
    }

    default:
      doUpdate = false;
  }

  if (doUpdate) {
    mouse->keyPressEvent(e);
    e->accept();
    update();
  } else {
    QWidget::keyPressEvent(e);
  }
}

void CCanvas::drawStatusMessages(QPainter& p) {
  if (textStatusMessages->isVisible()) {
    QRect r = textStatusMessages->frameGeometry();
    r.adjust(-5, -5, 5, 5);
    p.setPen(CDraw::penBorderGray);
    p.setBrush(textStatusMessages->palette().color(textStatusMessages->backgroundRole()));
    p.drawRoundedRect(r, RECT_RADIUS, RECT_RADIUS);
  }
}

void CCanvas::drawTrackStatistic(QPainter& p) {
  p.save();
  p.setPen(CDraw::penBorderGray);
  p.setBrush(labelTrackStatistic->palette().color(labelTrackStatistic->backgroundRole()));
  if (labelTrackStatistic->isVisible()) {
    QRect r = labelTrackStatistic->frameGeometry();
    r.adjust(-5, -5, 5, 5);
    p.drawRoundedRect(r, RECT_RADIUS, RECT_RADIUS);
  }

  if (labelTrackInfo->isVisible()) {
    QRect r = labelTrackInfo->frameGeometry();
    r.adjust(-5, -5, 5, 5);
    p.drawRoundedRect(r, RECT_RADIUS, RECT_RADIUS);
  }
  p.restore();
}

void CCanvas::drawScale(QPainter& p, QRectF drawRect) {
  if (!CMainWindow::self().isScaleVisible()) {
    return;
  }

  // step I: get the approximate distance for 200px in the bottom right corner
  QPointF brc(drawRect.bottomRight() - QPoint(50, 30));
  QPointF pt1 = brc;
  QPointF pt2 = brc - QPoint(-200, 0);

  map->convertPx2Rad(pt1);
  map->convertPx2Rad(pt2);

  qreal d = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y());

  // step II: derive the actual scale length in [m]
  qreal a = (int)log10(d);
  qreal b = log10(d) - a;

  if (0 <= b && b < log10(3.0f)) {
    d = 1 * qPow(10, a);
  } else if (log10(3.0f) < b && b < log10(5.0f)) {
    d = 3 * qPow(10, a);
  } else {
    d = 5 * qPow(10, a);
  }

  // step III: convert the scale length from [m] into [px]
  pt1 = brc;
  map->convertPx2Rad(pt1);
  pt2 = GPS_Math_Wpt_Projection(pt1, d, -90 * DEG_TO_RAD);

  map->convertRad2Px(pt1);
  map->convertRad2Px(pt2);

  p.setPen(QPen(Qt::white, 9));
  p.drawLine(pt1, pt2 + QPoint(9, 0));
  p.setPen(QPen(Qt::black, 7));
  p.drawLine(pt1, pt2 + QPoint(9, 0));
  p.setPen(QPen(Qt::white, 5));
  p.drawLine(pt1, pt2 + QPoint(9, 0));

  QVector<qreal> pattern;
  pattern << 2 << 4;
  QPen pen(Qt::black, 5, Qt::CustomDashLine);
  pen.setDashPattern(pattern);
  p.setPen(pen);
  p.drawLine(pt1, pt2 + QPoint(9, 0));

  QPoint pt3(pt2.x() + (pt1.x() - pt2.x()) / 2, pt2.y());

  QString val, unit;
  IUnit::self().meter2distance(d, val, unit);
  CDraw::text(QString("%1 %2").arg(val, unit), p, pt3, Qt::black);
}

void CCanvas::slotTriggerCompleteUpdate(CCanvas::redraw_e flags) {
  needsRedraw = (redraw_e)(needsRedraw | flags);
  update();
}

void CCanvas::slotToolTip() {
  QString str;
  if (!poi->getToolTip(posToolTip, str)) {
    map->getToolTip(posToolTip, str);
    if (str.isEmpty()) {
      return;
    }
  }
  QPoint p = mapToGlobal(posToolTip + QPoint(32, 0));
  QToolTip::showText(p, str);
}

void CCanvas::slotCheckTrackOnFocus() {
  const IGisItem::key_t& key = CGisItemTrk::getKeyUserFocus();

  // any changes?
  if (key != keyTrackOnFocus) {
    // get access to current track object
    delete plotTrackProfile;
    delete colorLegend;
    keyTrackOnFocus.clear();
    labelTrackStatistic->clear();
    labelTrackStatistic->hide();
    labelTrackInfo->clear();
    labelTrackInfo->hide();

    // get access to next track object
    CGisItemTrk* trk2 = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    if (nullptr == trk2) {
      update();
      return;
    }

    // create new profile plot, the plot will register itself at the track
    plotTrackProfile = new CPlotProfile(
        trk2, trk2->limitsGraph1, CMainWindow::self().profileIsWindow() ? IPlot::eModeWindow : IPlot::eModeIcon, this);
    setSizeTrackProfile();
    // track profile visibility is set in slotUpdateTrackInfo()

    colorLegend = new CColorLegend(this, trk2);
    colorLegend->setGeometry(20, 20, 40, 300);

    // finally store the new key as track on focus
    keyTrackOnFocus = key;

    slotUpdateTrackInfo(false);
    update();
  }
}

void CCanvas::slotUpdateTrackInfo(bool updateVisuals) {
  CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(keyTrackOnFocus));

  if (trk == nullptr) {
    labelTrackInfo->clear();
    labelTrackInfo->hide();
    labelTrackStatistic->clear();
    labelTrackStatistic->hide();
    return;
  }

  bool trackStatisticIsVisible = false;
  if (isShowTrackSummary() || isShowMinMaxSummary()) {
    trackStatisticIsVisible = true;

    QString text;
    if (isShowTrackSummary()) {
      text += trk->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowActivity);
    }

    if (isShowMinMaxSummary()) {
      text += trk->getInfoLimits();
    }

    labelTrackStatistic->setText(text);
    labelTrackStatistic->adjustSize();

    labelTrackStatistic->move(rect().width() - labelTrackStatistic->width() - 20,
                              rect().height() - labelTrackStatistic->height() - 60);
    labelTrackStatistic->show();
  } else {
    labelTrackStatistic->clear();
    labelTrackStatistic->hide();
  }

  if (isShowTrackInfoTable()) {
    int cnt = 1;
    QString text;

    text += "<table>";
    for (const CTrackData::trkpt_t& trkpt : trk->getTrackData()) {
      if (trkpt.isHidden() || trkpt.desc.isEmpty()) {
        continue;
      }
      text += "<tr><td><img src=" + CWptIconManager::self().getNumberedBullet(cnt++) + "/></td><td>" + trkpt.desc +
              "</td></tr>";
    }
    text += "</table>";

    labelTrackInfo->setText(text);
    labelTrackInfo->adjustSize();

    const int x = rect().width() - labelTrackInfo->width() - 20;
    const int y = rect().height() - (trackStatisticIsVisible ? labelTrackStatistic->height() + 20 : 0) -
                  labelTrackInfo->height() - 60;

    labelTrackInfo->move(x, y);
    labelTrackInfo->setVisible(cnt > 1);
  } else {
    labelTrackInfo->clear();
    labelTrackInfo->hide();
  }

  if (isVisible() && (plotTrackProfile != nullptr)) {
    plotTrackProfile->setVisible(isShowTrackProfile());
  }

  if (updateVisuals) {
    trk->updateVisuals(CGisItemTrk::eVisualPlot, "CCanvas::slotUpdateTrackInfo()");
  }

  update();
}

void CCanvas::moveTo(const QPointF& newFocus) {
  if (posFocus == newFocus) {
    return;
  }

  posFocus = newFocus;
  slotTriggerCompleteUpdate(eRedrawAll);
}

void CCanvas::moveMap(const QPointF& delta) {
  map->convertRad2Px(posFocus);
  posFocus -= delta;
  map->convertPx2Rad(posFocus);

  emit sigMove();
  emit sigMoveAndZoom(map->zoom(), posFocus);

  slotTriggerCompleteUpdate(eRedrawAll);
}

void CCanvas::zoomTo(const QRectF& rect) {
  posFocus = rect.center();
  map->zoom(rect);
  const QList<IDrawContext*>& allContext = allDrawContext.mid(1);
  for (IDrawContext* context : allContext) {
    context->zoom(map->zoom());
  }
  emit sigZoom();
  emit sigMoveAndZoom(map->zoom(), posFocus);
  slotTriggerCompleteUpdate(eRedrawAll);
}

void CCanvas::setupGrid() {
  CGridSetup dlg(grid, map);
  dlg.exec();
  update();
}

void CCanvas::setupBackgroundColor() {
  QColorDialog::setCustomColor(0, 0x00FFFFBF);
  const QColor& selected = QColorDialog::getColor(backColor, this, tr("Setup Map Background"));

  if (selected.isValid()) {
    backColor = selected;
    update();
  }
}

void CCanvas::convertGridPos2Str(const QPointF& pos, QString& str, bool simple) {
  grid->convertPos2Str(pos, str, simple);
}

void CCanvas::convertRad2Px(QPointF& pos) const { map->convertRad2Px(pos); }

void CCanvas::convertPx2Rad(QPointF& pos) const { map->convertPx2Rad(pos); }

void CCanvas::displayInfo(const QPoint& px) {
  if (CMainWindow::self().isMapToolTip()) {
    posToolTip = px;

    timerToolTip->stop();
    timerToolTip->start(500);
  }
  QToolTip::hideText();
}

void CCanvas::findPoiCloseBy(const QPoint& px, QSet<IPoiItem>& poiItems, QList<QPointF>& posPoiHighlight) const {
  poi->findPoiCloseBy(px, poiItems, posPoiHighlight);

  IPoiItem mapPoi = map->findPOICloseBy(px);
  if (mapPoi.pos != NOPOINTF) {
    poiItems.insert(mapPoi);
    posPoiHighlight.append(mapPoi.pos);
  }
}

void CCanvas::findPoisIn(const QRectF& degRect, QSet<IPoiItem>& poiItems, QList<QPointF>& posPoiHighlight) const {
  return poi->findPoisIn(degRect, poiItems, posPoiHighlight);
}
void CCanvas::setup() {
  CCanvasSetup dlg(this);
  dlg.exec();
}

QString CCanvas::getProjection() { return map->getProjection(); }

void CCanvas::setProjection(const QString& proj) {
  for (IDrawContext* context : std::as_const(allDrawContext)) {
    if (!context->setProjection(proj)) {
      QMessageBox::warning(this, tr("Map Projection..."),
                           tr("Failed to setup map projection. Please configure a valid projection."), QMessageBox::Ok);

      QTimer::singleShot(1000, &CMainWindow::self(), &CMainWindow::slotSetupMapView);
      return;
    }
  }
}

void CCanvas::setScales(const scales_type_e type) {
  for (IDrawContext* context : std::as_const(allDrawContext)) {
    context->setScales(type);
  }
}

CCanvas::scales_type_e CCanvas::getScalesType() { return map->getScalesType(); }

qreal CCanvas::getElevationAt(const QPointF& pos) const { return dem->getElevationAt(pos); }

void CCanvas::getElevationAt(const QPolygonF& pos, QPolygonF& ele) const { return dem->getElevationAt(pos, ele); }

qreal CCanvas::getSlopeAt(const QPointF& pos) const { return dem->getSlopeAt(pos); }

void CCanvas::getSlopeAt(const QPolygonF& pos, QPolygonF& slope) const { return dem->getSlopeAt(pos, slope); }

void CCanvas::getElevationAt(SGisLine& line) const { return dem->getElevationAt(line); }

void CCanvas::setZoom(bool in, redraw_e& needsRedraw) {
  map->zoom(in, needsRedraw);
  const QList<IDrawContext*>& allContext = allDrawContext.mid(1);
  for (IDrawContext* context : allContext) {
    context->zoom(map->zoom());
  }

  emit sigZoom();
  emit sigMoveAndZoom(map->zoom(), posFocus);
}

void CCanvas::zoom(int index) {
  if (map->zoom() == index) {
    return;
  }

  map->zoom(index);
  const QList<IDrawContext*>& allContext = allDrawContext.mid(1);
  for (IDrawContext* context : allContext) {
    context->zoom(map->zoom());
  }
  slotTriggerCompleteUpdate(eRedrawAll);
}

bool CCanvas::findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline) {
  return map->findPolylineCloseBy(pt1, pt2, threshold, polyline);
}

void CCanvas::saveSizeTrackProfile() {
  if (plotTrackProfile.isNull()) {
    return;
  }

  if (plotTrackProfile->windowFlags() & Qt::Window) {
    SETTINGS;
    cfg.beginGroup("Canvas");
    cfg.beginGroup("Profile");
    cfg.beginGroup(objectName());

    cfg.setValue("geometry", plotTrackProfile->saveGeometry());

    cfg.endGroup();  // objectName()
    cfg.endGroup();  // Profile
    cfg.endGroup();  // Canvas
  }
}

void CCanvas::setSizeTrackProfile() {
  if (plotTrackProfile.isNull()) {
    return;
  }

  if (plotTrackProfile->windowFlags() & Qt::Window) {
    SETTINGS;
    cfg.beginGroup("Canvas");
    cfg.beginGroup("Profile");
    cfg.beginGroup(objectName());

    if (cfg.contains("geometry")) {
      plotTrackProfile->restoreGeometry(cfg.value("geometry").toByteArray());
    } else {
      plotTrackProfile->resize(300, 200);
      plotTrackProfile->move(100, 100);
    }

    cfg.endGroup();  // objectName()
    cfg.endGroup();  // Profile
    cfg.endGroup();  // Canvas
  } else {
    plotTrackProfile->resize(getTrackProfileSize(height()));
    plotTrackProfile->move(20, height() - plotTrackProfile->height() - 20);
  }
}

void CCanvas::showProfileAsWindow() {
  if (plotTrackProfile) {
    const IGisItem::key_t key = CGisItemTrk::getKeyUserFocus();

    delete plotTrackProfile;
    keyTrackOnFocus.clear();

    CGisWorkspace::self().focusTrkByKey(true, key);
  }
}

void CCanvas::showProfile(bool yes) {
  if (nullptr != plotTrackProfile) {
    plotTrackProfile->setVisible(yes && isShowTrackProfile());
  }
}

bool CCanvas::setDrawContextSize(const QSize& s) {
  bool done = true;
  for (IDrawContext* context : std::as_const(allDrawContext)) {
    done &= context->resize(s);
  }
  return done;
}

void CCanvas::print(QPainter& p, const QRectF& area, const QPointF& focus, bool printScale) {
  const QSize oldSize = size();
  const QSize newSize(area.size().toSize());

  setDrawContextSize(newSize);

  // ----- start to draw thread based content -----
  // move coordinate system to center of the screen
  p.translate(newSize.width() >> 1, newSize.height() >> 1);

  redraw_e redraw = eRedrawAll;

  for (IDrawContext* context : std::as_const(allDrawContext)) {
    context->draw(p, redraw, focus);
  }

  for (IDrawContext* context : std::as_const(allDrawContext)) {
    context->wait();
  }

  for (IDrawContext* context : std::as_const(allDrawContext)) {
    context->draw(p, redraw, focus);
  }

  // restore coordinate system to default
  p.resetTransform();
  // ----- start to draw fast content -----

  QRect r(QPoint(0, 0), area.size().toSize());

  grid->draw(p, r);
  gis->draw(p, r);
  rt->draw(p, r);
  if (printScale) {
    drawScale(p, r);
  }

  setDrawContextSize(oldSize);
}

bool CCanvas::event(QEvent* event) {
  if (event->type() == QEvent::Gesture) {
    return gestureEvent(static_cast<QGestureEvent*>(event));
  } else if (mouseLost) {
    QMouseEvent* me = dynamic_cast<QMouseEvent*>(event);
    if (me != nullptr) {
      // notify IMouse that the upcomming QMouseEvent needs special treatment
      // as some mouse-events may have been lost
      mouse->afterMouseLostEvent(me);
      mouseLost = false;
    }
  }
  return QWidget::event(event);
}

bool CCanvas::gestureEvent(QGestureEvent* e) {
  if (QPinchGesture* pinch = dynamic_cast<QPinchGesture*>(e->gesture(Qt::PinchGesture))) {
    if (pinch->changeFlags() & QPinchGesture::CenterPointChanged) {
      const QPointF& move = pinch->centerPoint() - pinch->lastCenterPoint();
      if (!move.isNull()) {
        moveMap(move);
      }
    }
    if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged) {
      qreal pscale = pinch->totalScaleFactor();
      if (pscale < 0.8f || pscale > 1.25f) {
        const QPointF& center = pinch->centerPoint();
        const QPointF& pos = mapFromGlobal(QPoint(center.x(), center.y()));
        QPointF pt1 = pos;
        map->convertPx2Rad(pt1);
        setZoom(pscale > 1.0f, needsRedraw);
        map->convertRad2Px(pt1);
        const QPointF& move = pos - pt1;
        if (!move.isNull()) {
          moveMap(move);
        }
        pinch->setTotalScaleFactor(1.0f);
        emit sigZoom();
        emit sigMoveAndZoom(map->zoom(), posFocus);
        slotTriggerCompleteUpdate(needsRedraw);
      }
    }
    mouseLost = true;
    mouse->pinchGestureEvent(pinch);
  }
  return true;
}

void CCanvas::followPosition(const QPointF& pos) {
  QPointF pos_ = pos * DEG_TO_RAD;
  convertRad2Px(pos_);

  const QRect& r1 = rect();
  QRectF r2(0, 0, r1.width() * 0.3, r1.height() * 0.3);
  r2.moveCenter(r1.center());

  if (r2.contains(pos_)) {
    return;
  }

  moveMap(r1.center() - pos_);
}

bool CCanvas::isShowMinMaxSummary() const { return CMainWindow::self().isShowMinMaxSummary() && showTrackOverlays; }
bool CCanvas::isShowTrackSummary() const { return CMainWindow::self().isShowTrackSummary() && showTrackOverlays; }
bool CCanvas::isShowTrackInfoTable() const { return CMainWindow::self().isShowTrackInfoTable() && showTrackOverlays; }
bool CCanvas::isShowTrackInfoPoints() const { return CMainWindow::self().isShowTrackInfoPoints() && showTrackOverlays; }
bool CCanvas::isShowTrackProfile() const { return CMainWindow::self().isShowTrackProfile() && showTrackOverlays; }
bool CCanvas::isShowTrackHighlight() const { return CMainWindow::self().isShowTrackHighlight() && showTrackOverlays; }

void CCanvas::linkMapViewEnabled() { emit sigMoveAndZoom(map->zoom(), posFocus); }
