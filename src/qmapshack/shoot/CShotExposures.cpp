/**********************************************************************************************
   Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

/**
   The exposure catalog: how to build each widget class a shot photographs without opening it.

   Not exposed: CExportDatabase and CSearchDatabase need a database with content, CRangeToolSetup exists
   only in the range mouse mode, CTemplateWidget is not user-facing, and the four CDetails* dialogs are
   opened by a recorded scenario. The dialogs that need a fixture item come with the fixture.

   A value a dialog keeps a reference to is a static of its own factory and set again for every build,
   so no build sees what another one left behind.
 */

#include <QImage>
#include <QLineEdit>
#include <QPainter>
#include <QUrl>

#include "CAbout.h"
#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "canvas/CCanvasSelect.h"
#include "canvas/CCanvasSetup.h"
#include "dem/CDemPathSetup.h"
#include "gis/CGisDatabase.h"
#include "gis/CGisItemRate.h"
#include "gis/CGisListDB.h"
#include "gis/CGisWorkspace.h"
#include "gis/CSetupWorkspace.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/CSetupDatabase.h"
#include "gis/db/CSetupFolder.h"
#include "gis/rte/router/brouter/CRouterBRouterInfo.h"
#include "gis/rte/router/routino/CRouterRoutinoPathSetup.h"
#include "gis/search/CGeoSearchConfig.h"
#include "gis/search/CGeoSearchConfigDialog.h"
#include "gis/search/CGeoSearchWebConfigDialog.h"
#include "gis/search/CSearchExplanationDialog.h"
#include "gis/summary/CGisSummary.h"
#include "gis/summary/CGisSummarySetup.h"
#include "gis/trk/CCutTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "grid/CGrid.h"
#include "grid/CGridSetup.h"
#include "grid/CProjWizard.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CMapIconSizesSetup.h"
#include "helpers/COverviewAdvisory.h"
#include "helpers/CPhotoViewer.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSelectProjectDialog.h"
#include "helpers/CShortcutConfig.h"
#include "helpers/CShortcutSetupDialog.h"
#include "helpers/CToolBarConfig.h"
#include "helpers/CToolBarSetupDialog.h"
#include "helpers/CVrtAdvisoryDialog.h"
#include "helpers/CWptIconDialog.h"
#include "map/CMapDraw.h"
#include "map/CMapPathSetup.h"
#include "poi/CPoiPathSetup.h"
#include "print/CScreenshotDialog.h"
#include "realtime/CRtSelectSource.h"
#include "realtime/CRtWorkspace.h"
#include "shoot/CShotRegistry.h"
#include "units/CCoordFormatSetup.h"
#include "units/CTimeZoneSetup.h"
#include "units/CUnitsSetup.h"
#include "widgets/CTextEditWidget.h"

namespace {
/** @return the main window's child of class T, nullptr if there is none */
template <typename T>
T* live() {
  return CMainWindow::self().findChild<T*>();
}

/** @brief One link, so the link editor has a row */
QList<IGisItem::link_t>& demoLinks() {
  static QList<IGisItem::link_t> links;
  links = {IGisItem::link_t{QUrl("https://www.qmapshack.org"), QObject::tr("The QMapShack project page"), ""}};
  return links;
}

/** @brief A VRT whose overviews need rebuilding */
COverviewAdvisory::advice_t demoAdvice() {
  COverviewAdvisory::advice_t advice;
  advice.suggestedLevels << 2 << 4 << 8 << 16 << 32;

  COverviewAdvisory::file_info_t weak;
  weak.path = "tiles/n47e011.tif";
  COverviewAdvisory::file_info_t coarse;
  coarse.path = "tiles/n47e012.tif";
  coarse.overviewSizes << 4096 << 2048;
  advice.perFileInfo << weak << coarse;

  advice.diskUsageBytes = 3LL * 1024 * 1024 * 1024;
  advice.diskUsageIsEstimate = true;
  return advice;
}

/** @brief The raster demoAdvice() is about */
COverviewAdvisory::geometry_t demoGeometry() {
  COverviewAdvisory::geometry_t geometry;
  geometry.xsizePx = 40000;
  geometry.ysizePx = 30000;
  geometry.pixelSizeX = 10.0;
  geometry.pixelSizeY = 10.0;
  return geometry;
}

/** @brief A drawn photo, so the viewer has something to show */
QList<CGisItemWpt::image_t>& demoImages() {
  QImage pixmap(640, 480, QImage::Format_RGB32);
  QPainter p(&pixmap);
  QLinearGradient gradient(0, 0, 0, 480);
  gradient.setColorAt(0, QColor(80, 130, 180));
  gradient.setColorAt(1, QColor(230, 220, 190));
  p.fillRect(pixmap.rect(), gradient);
  p.setPen(Qt::white);
  p.drawText(pixmap.rect(), Qt::AlignCenter, QObject::tr("Demo photo"));
  p.end();

  CGisItemWpt::image_t image;
  image.pixmap = pixmap;
  image.info = QObject::tr("A synthetic photo for the documentation shots.");
  image.fileName = "demo.jpg";

  static QList<CGisItemWpt::image_t> images;
  images = {image};
  return images;
}
}  // namespace

// --- takes nothing but a parent ------------------------------------------------------------

SHOT_EXPOSE("About", "The about box", CAbout, [](CShotContext&, QWidget* p) -> QWidget* { return new CAbout(p); });
SHOT_EXPOSE("UnitsSetup", "Unit system and slope display", CUnitsSetup,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CUnitsSetup(p); });
SHOT_EXPOSE("CoordFormatSetup", "Coordinate format", CCoordFormatSetup,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CCoordFormatSetup(p); });
SHOT_EXPOSE("TimeZoneSetup", "Time zone and date format", CTimeZoneSetup,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CTimeZoneSetup(p); });
SHOT_EXPOSE("MapIconSizesSetup", "Icon sizes on the map", CMapIconSizesSetup,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CMapIconSizesSetup(p); });
SHOT_EXPOSE("SearchExplanation", "Workspace search syntax help", CSearchExplanationDialog,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CSearchExplanationDialog(p); });
SHOT_EXPOSE("CutTrk", "Cut track options", CCutTrk,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CCutTrk(p); });
SHOT_EXPOSE("GisItemRate", "Rating and keywords", CGisItemRate,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CGisItemRate(p, {"demo", "documentation"}, 3.5); });
SHOT_EXPOSE("RouterBRouterInfo", "BRouter status and version", CRouterBRouterInfo,
            [](CShotContext&, QWidget*) -> QWidget* {
              CRouterBRouterInfo* dlg = new CRouterBRouterInfo();
              dlg->setLabel(QObject::tr("BRouter"));
              dlg->setInfo(QObject::tr("The information BRouter reports about itself appears here."));
              return dlg;
            });
SHOT_EXPOSE("ProgressDialog", "Progress of a long operation", CProgressDialog,
            [](CShotContext&, QWidget* p) -> QWidget* {
              return new CProgressDialog(QObject::tr("Reading files..."), 0, 100, p);
            });
SHOT_EXPOSE("TextEditWidget", "Rich text editor for descriptions and comments", CTextEditWidget,
            [](CShotContext&, QWidget* p) -> QWidget* {
              return new CTextEditWidget(QObject::tr("<b>A description</b><p>Text, links and images.</p>"), p);
            });

// --- takes data no fixture holds --------------------------------------------------------------

SHOT_EXPOSE("LinksDialog", "Links attached to an item", CLinksDialog,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CLinksDialog(demoLinks(), p); });
SHOT_EXPOSE("PhotoViewer", "A photo attached to a waypoint", CPhotoViewer,
            [](CShotContext&, QWidget* p) -> QWidget* { return new CPhotoViewer(demoImages(), 0, p); });
SHOT_EXPOSE("ProjWizard", "Build a projection string", CProjWizard, [](CShotContext&, QWidget*) -> QWidget* {
  // The wizard writes into a line edit it does not own; parent it to the dialog.
  QLineEdit* line = new QLineEdit();
  line->setText("+proj=merc +ellps=WGS84 +datum=WGS84 +units=m +no_defs");
  CProjWizard* dlg = new CProjWizard(*line);
  line->setParent(dlg);
  line->hide();
  return dlg;
});
SHOT_EXPOSE("VrtAdvisoryDialog", "A VRT whose overviews make rendering slow", CVrtAdvisoryDialog,
            [](CShotContext&, QWidget* p) -> QWidget* {
              return new CVrtAdvisoryDialog("alps.vrt", demoAdvice(), demoGeometry(), false, p);
            });

// --- takes an object of the running application -------------------------------------------------

SHOT_EXPOSE("WptIconDialog", "Waypoint symbol chooser", CWptIconDialog,
            [](CShotContext&, QWidget*) -> QWidget* { return new CWptIconDialog(&CMainWindow::self()); });
SHOT_EXPOSE("SelectProjectDialog", "Which project an item goes into", CSelectProjectDialog,
            [](CShotContext&, QWidget*) -> QWidget* {
              static QString key;
              static QString name;
              static IGisProject::type_e type;
              key.clear();
              name.clear();
              type = IGisProject::eTypeQms;
              // No workspace tree: the new-project form.
              return new CSelectProjectDialog(key, name, type, nullptr);
            });
SHOT_EXPOSE("CanvasSetup", "Canvas projection and scales", CCanvasSetup, [](CShotContext&, QWidget*) -> QWidget* {
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  return (nullptr == canvas) ? nullptr : new CCanvasSetup(canvas);
});
SHOT_EXPOSE("CanvasSelect", "Which canvas an action applies to", CCanvasSelect,
            [](CShotContext&, QWidget* p) -> QWidget* {
              static CCanvas* canvas = nullptr;
              canvas = CMainWindow::self().getVisibleCanvas();
              return (nullptr == canvas) ? nullptr : new CCanvasSelect(canvas, p);
            });
SHOT_EXPOSE("SetupWorkspace", "Workspace behaviour", CSetupWorkspace, [](CShotContext&, QWidget* p) -> QWidget* {
  return new CSetupWorkspace(&CGisWorkspace::self(), &CGisDatabase::self(), p);
});
SHOT_EXPOSE("GridSetup", "Grid projection and colour", CGridSetup, [](CShotContext&, QWidget*) -> QWidget* {
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  CGrid* grid = (nullptr == canvas) ? nullptr : canvas->findChild<CGrid*>();
  CMapDraw* map = (nullptr == canvas) ? nullptr : canvas->findChild<CMapDraw*>();
  return (nullptr == grid || nullptr == map) ? nullptr : new CGridSetup(grid, map);
});
SHOT_EXPOSE("ScreenshotDialog", "Save or print a screenshot of the map", CScreenshotDialog,
            [](CShotContext&, QWidget* p) -> QWidget* {
              CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
              return (nullptr == canvas) ? nullptr : new CScreenshotDialog(*canvas, p);
            });
SHOT_EXPOSE("ToolBarSetupDialog", "Which buttons the tool bar carries", CToolBarSetupDialog,
            [](CShotContext&, QWidget* p) -> QWidget* {
              CToolBarConfig* config = live<CToolBarConfig>();
              return (nullptr == config) ? nullptr : new CToolBarSetupDialog(p, config);
            });
SHOT_EXPOSE("ShortcutSetupDialog", "Keyboard shortcuts", CShortcutSetupDialog,
            [](CShotContext&, QWidget* p) -> QWidget* {
              CShortcutConfig* config = live<CShortcutConfig>();
              return (nullptr == config) ? nullptr : new CShortcutSetupDialog(p, config);
            });
SHOT_EXPOSE("GeoSearchConfigDialog", "Which geo search service to use", CGeoSearchConfigDialog,
            [](CShotContext&, QWidget* p) -> QWidget* {
              CGeoSearchConfig* config = live<CGeoSearchConfig>();
              return (nullptr == config) ? nullptr : new CGeoSearchConfigDialog(p, config);
            });
SHOT_EXPOSE("GisSummarySetup", "Which folders the summary watches", CGisSummarySetup,
            [](CShotContext&, QWidget*) -> QWidget* {
              CGisSummary* summary = live<CGisSummary>();
              return (nullptr == summary) ? nullptr : new CGisSummarySetup(*summary);
            });
SHOT_EXPOSE("RtSelectSource", "Add a realtime source", CRtSelectSource, [](CShotContext&, QWidget*) -> QWidget* {
  CRtWorkspace* wks = live<CRtWorkspace>();
  return (nullptr == wks) ? nullptr : new CRtSelectSource(*wks);
});
SHOT_EXPOSE("SetupDatabase", "Add or create a database", CSetupDatabase, [](CShotContext&, QWidget*) -> QWidget* {
  CGisListDB* list = live<CGisListDB>();
  return (nullptr == list) ? nullptr : new CSetupDatabase(*list);
});

// --- takes values the dialog writes back ---------------------------------------------------------

SHOT_EXPOSE("GeoSearchWebConfigDialog", "Web services offered on a position", CGeoSearchWebConfigDialog,
            [](CShotContext&, QWidget* p) -> QWidget* {
              static QList<CGeoSearchWeb::service_t> services;
              services.clear();
              return new CGeoSearchWebConfigDialog(services, p);
            });
SHOT_EXPOSE("SetupFolder", "Name and type of a database folder", CSetupFolder,
            [](CShotContext&, QWidget* p) -> QWidget* {
              static IDBFolder::type_e type;
              static QString name;
              type = IDBItem::eTypeGroup;
              name = QObject::tr("Demo Folder");
              return new CSetupFolder(type, name, true, p);
            });
SHOT_EXPOSE("SelectDBFolder", "Which database folder to use", CSelectDBFolder,
            [](CShotContext&, QWidget* p) -> QWidget* {
              static QList<quint64> ids;
              static QString db;
              static QString host;
              ids.clear();
              db.clear();
              host.clear();
              return new CSelectDBFolder(ids, db, host, p);
            });
SHOT_EXPOSE("MapPathSetup", "Where map files are looked for", CMapPathSetup, [](CShotContext&, QWidget*) -> QWidget* {
  static QStringList paths;
  static QString cachePath;
  paths.clear();
  cachePath.clear();
  return new CMapPathSetup(paths, cachePath);
});
SHOT_EXPOSE("DemPathSetup", "Where elevation files are looked for", CDemPathSetup,
            [](CShotContext&, QWidget*) -> QWidget* {
              static QStringList paths;
              paths.clear();
              return new CDemPathSetup(paths);
            });
SHOT_EXPOSE("PoiPathSetup", "Where POI files are looked for", CPoiPathSetup, [](CShotContext&, QWidget*) -> QWidget* {
  static QStringList paths;
  paths.clear();
  return new CPoiPathSetup(paths);
});
SHOT_EXPOSE("RouterRoutinoPathSetup", "Where Routino databases are looked for", CRouterRoutinoPathSetup,
            [](CShotContext&, QWidget*) -> QWidget* {
              static QStringList paths;
              paths.clear();
              return new CRouterRoutinoPathSetup(paths);
            });
