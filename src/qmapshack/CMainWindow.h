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

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QPointer>

#include "ui_IMainWindow.h"

class CMapList;
class CDemList;
class CPoiList;
class QLabel;
class CGisWorkspace;
class CGisDatabase;
class CRtWorkspace;
class CCanvas;
class CGeoSearchConfig;
class CToolBarConfig;
class CGeoSearchWeb;
struct SGisLine;
class CWptIconManager;
class CHelp;

class CMainWindow : public QMainWindow, private Ui::IMainWindow {
  Q_OBJECT
 public:
  static CMainWindow& self() { return *pSelf; }

  static QWidget* getBestWidgetForParent();

  QString getHomePath() { return homeDir.exists() ? homeDir.absolutePath() : ""; }

  QString getMapsPath() { return homeDir.exists(mapsPath) ? homeDir.absoluteFilePath(mapsPath) : ""; }
  QString getDemPath() { return homeDir.exists(demPath) ? homeDir.absoluteFilePath(demPath) : ""; }
  QString getRoutinoPath() { return homeDir.exists(routinoPath) ? homeDir.absoluteFilePath(routinoPath) : ""; }
  QString getBRouterPath() { return homeDir.exists(brouterPath) ? homeDir.absoluteFilePath(brouterPath) : ""; }
  QString getDatabasePath() { return homeDir.exists(databasePath) ? homeDir.absoluteFilePath(databasePath) : ""; }
  QString getGpxPath() { return homeDir.exists(gpxPath) ? homeDir.absoluteFilePath(gpxPath) : ""; }

  static QString getUser();

  virtual ~CMainWindow();

  void addMapList(CMapList* list, const QString& name);
  void addDemList(CDemList* list, const QString& name);
  void addPoiList(CPoiList* list, const QString& name);
  void addWidgetToTab(QWidget* w);

  bool isScaleVisible() const;
  bool isGridVisible() const;
  bool isNight() const;
  bool isPoiText() const;
  bool isMapToolTip() const;
  bool isShowMinMaxTrackLabels() const;

  bool isShowMinMaxSummary() const;
  bool isShowTrackSummary() const;
  bool isShowTrackInfoTable() const;
  bool isShowTrackInfoPoints() const;
  bool isShowTrackProfile() const;
  bool isShowTrackHighlight() const;

  bool flipMouseWheel() const;
  bool profileIsWindow() const;
  const QFont& getMapFont() const { return mapFont; }

  void zoomCanvasTo(const QRectF rect);
  void resetMouse();
  /**
     @brief Read the elevation from DEM data attached to the currently visible canvas for a given location
     @param pos   a position in units of [rad]
     @return If no elevation value can be found for the position NOFLOAT is returned.
   */
  qreal getElevationAt(const QPointF& pos) const;
  void getElevationAt(const QPolygonF& pos, QPolygonF& ele) const;
  void getElevationAt(SGisLine& line) const;

  qreal getSlopeAt(const QPointF& pos) const;
  void getSlopeAt(const QPolygonF& pos, QPolygonF& slope) const;
  /**
     @brief Get pointer to the currently visible canvas object.
     @return If the currently visible tab does not contain a CCanvas object 0 is returned.
   */
  CCanvas* getVisibleCanvas() const;
  QList<CCanvas*> getCanvas() const;

  QAction* getMapSetupAction() { return actionSetupMapPaths; }

  QAction* getDemSetupAction() { return actionSetupDEMPaths; }

  QAction* getPoiSetupAction() { return actionSetupPOIPaths; }

  void loadGISData(const QStringList& filenames);

  const qint32 id;

 signals:
  void sigCanvasChange();

 public slots:
  void slotLinkActivated(const QString& link);
  void slotLinkActivated(const QUrl& url);
  void slotSetupMapView();
  void slotSetupGrid();

 protected:
  bool eventFilter(QObject* obj, QEvent* event) override;
#ifdef Q_OS_WIN64
  bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result);
#endif  // Q_OS_WIN64
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private slots:
  void slotAbout();
  void slotWiki();
  void slotQuickstart();
  void slotAddCanvas(const QString& name);
  void slotCloneCanvas();
  void slotTabCloseRequest(int i);
  void slotCurrentTabCanvas(int i);
  void slotCurrentTabMaps(int i);
  void slotCurrentTabDem(int i);
  void slotMousePosition(const QPointF& pos, qreal ele, qreal slope);
  void slotUpdateTabWidgets();
  void slotSetupMapFont();
  void slotSetupMapBackground();
  void slotSetupMapPath();
  void slotSetupPoiPath();
  void slotSetupDemPath();
  void slotSetupTimeZone();
  void slotSetupUnits();
  void slotSetupWorkspace();
  void slotSetupCoordFormat();
  void slotSetupToolbar();
  void slotImportDatabase();
  void slotLoadGISData();
  void slotBuildVrt();
  void slotStoreView();
  void slotLoadView();
  void slotSetProfileMode(bool on);
  void slotCreateRoutinoDatabase();
  void slotPrintMap();
  void slotTakeScreenshot();
  void slotSetupWptIcons();
  void slotSanityTest();
  void slotCloseTab();
  void slotToggleDocks();
  void slotDockVisibilityChanged(bool visible);
  void slotFullScreen();
  void slotStartQMapTool();
  void slotGeoSearchConfigChanged();
  void slotDockFloating(bool floating);
  void slotRenameView();
  void slotHelp();
  void slotMapMoveAndZoom(int idx, const QPointF& focus);
  void slotLinkMapViews(bool on);

 private:
  friend int main(int argc, char** argv);
  CMainWindow();
  void prepareMenuForMac();
  void testForNoView();
  bool docksVisible() const;
  void showDocks() const;
  void hideDocks();
  void displayRegular();
  void displayFullscreen();
  CCanvas* addView(const QString& name);
  void setupHomePath();

  static CMainWindow* pSelf;
  static QDir homeDir;
  static const QString mapsPath;
  static const QString poisPath;
  static const QString demPath;
  static const QString routinoPath;
  static const QString brouterPath;
  static const QString databasePath;
  static const QString gpxPath;
  static const QSet<QString> paths;

  /// status bar label
  QLabel* lblPosWGS84;
  QLabel* lblElevation;
  QLabel* lblSlope;
  QLabel* lblPosGrid;

  QFont mapFont;

  CGisWorkspace* widgetGisWorkspace;
  CGisDatabase* widgetGisDatabase;
  CRtWorkspace* widgetRtWorkspace;

  CToolBarConfig* toolBarConfig;
  CGeoSearchConfig* geoSearchConfig;

  CGeoSearchWeb* geoSearchWeb;
  CWptIconManager* wptIconManager;

  QList<QDockWidget*> docks;
  QList<QDockWidget*> activeDocks;
  QByteArray dockStates;
  bool menuVisible = false;

  static QMutex mutex;

  QPointer<CHelp> help;
};

#endif  // CMAINWINDOW_H
