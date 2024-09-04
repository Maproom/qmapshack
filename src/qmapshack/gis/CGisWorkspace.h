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

#ifndef CGISWORKSPACE_H
#define CGISWORKSPACE_H

#include <QEvent>
#include <QSqlDatabase>
#include <QWidget>

#include "db/IDBFolder.h"
#include "gis/IGisItem.h"
#include "helpers/Tristate.h"
#include "ui_IGisWorkspace.h"

class CGisDraw;
class IGisProject;
class CSearchExplanationDialog;
struct IPoiItem;

enum event_types_e {
  eEvtD2WReqInfo = QEvent::User + 1,
  eEvtD2WShowFolder = QEvent::User + 2,
  eEvtD2WHideFolder = QEvent::User + 3,
  eEvtD2WShowItems = QEvent::User + 4,
  eEvtD2WHideItems = QEvent::User + 5,
  eEvtD2WUpdateLnF = QEvent::User + 6,
  eEvtD2WUpdateItems = QEvent::User + 7,
  eEvtD2WReload = QEvent::User + 8

  ,
  eEvtW2DAckInfo = QEvent::User + 100,
  eEvtW2DCreate = QEvent::User + 101

  ,
  eEvtA2WCutTrk = QEvent::User + 200,
  eEvtA2WSave = QEvent::User + 201,
  eEvtA2WSync = QEvent::User + 202
};

struct evt_item_t {
  evt_item_t(quint64 id, quint32 type) : id(id), type(type) {}
  quint64 id;
  quint32 type;
};

class CEvtD2WReqInfo : public QEvent {
 public:
  CEvtD2WReqInfo(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WReqInfo)), id(id), db(db) {}

  quint64 id;
  QString db;
};

class CEvtD2WShowFolder : public QEvent {
 public:
  CEvtD2WShowFolder(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WShowFolder)), id(id), db(db) {}

  quint64 id;
  QString db;
};

class CEvtD2WHideFolder : public QEvent {
 public:
  CEvtD2WHideFolder(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WHideFolder)), id(id), db(db) {}

  quint64 id;
  QString db;
};

class CEvtD2WShowItems : public QEvent {
 public:
  CEvtD2WShowItems(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WShowItems)), id(id), db(db) {}

  /// if true only the items in the list are loaded. Any other item loaded but not part of the list will be removed.
  bool addItemsExclusively = false;
  quint64 id;
  QString db;
  QList<evt_item_t> items;
};

class CEvtD2WHideItems : public QEvent {
 public:
  CEvtD2WHideItems(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WHideItems)), id(id), db(db) {}

  quint64 id;
  QString db;
  QSet<QString> keys;
};

class CEvtW2DAckInfo : public QEvent {
 public:
  CEvtW2DAckInfo(Qt::CheckState checkState, quint64 id, const QString& db, const QString& host)
      : QEvent(QEvent::Type(eEvtW2DAckInfo)), checkState(checkState), id(id), db(db), host(host) {}

  CEvtW2DAckInfo(quint64 id, const QString& db, const QString& host)
      : QEvent(QEvent::Type(eEvtW2DAckInfo)), id(id), db(db), host(host) {}

  Qt::CheckState checkState = Qt::Unchecked;
  bool updateLostFound = false;
  quint64 id;
  QString db;
  QString host;
  QSet<QString> keysChildren;
};

class CEvtD2WUpdateLnF : public QEvent {
 public:
  CEvtD2WUpdateLnF(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WUpdateLnF)), id(id), db(db) {}

  quint64 id;
  QString db;
};

class CEvtW2DCreate : public QEvent {
 public:
  CEvtW2DCreate(const QString& name, IDBFolder::type_e type, quint64 id, const QString& db, const QString& host)
      : QEvent(QEvent::Type(eEvtW2DCreate)), name(name), type(type), idParent(id), db(db), host(host) {}

  QString name;
  IDBFolder::type_e type;
  quint64 idParent;
  quint64 idChild = 0;
  QString db;
  QString host;
};

class CEvtD2WUpdateItems : public QEvent {
 public:
  CEvtD2WUpdateItems(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WUpdateItems)), id(id), db(db) {}

  quint64 id;
  QString db;
};

class CEvtD2WReload : public QEvent {
 public:
  CEvtD2WReload(const QString& db) : QEvent(QEvent::Type(eEvtD2WReload)), db(db) {}

  QString db;
};

class CEvtA2WCutTrk : public QEvent {
 public:
  CEvtA2WCutTrk(const IGisItem::key_t& key) : QEvent(QEvent::Type(eEvtA2WCutTrk)), key(key) {}

  const IGisItem::key_t key;
};

class CEvtA2WSave : public QEvent {
 public:
  CEvtA2WSave(const QString& key) : QEvent(QEvent::Type(eEvtA2WSave)), key(key) {}

  const QString key;
};

class CEvtA2WSync : public QEvent {
 public:
  CEvtA2WSync(const QString& key) : QEvent(QEvent::Type(eEvtA2WSync)), key(key) {}

  const QString key;
};

class CGisWorkspace : public QWidget, private Ui::IGisWorkspace {
  Q_OBJECT
 public:
  static CGisWorkspace& self() { return *pSelf; }
  virtual ~CGisWorkspace();

  void loadGisProject(const QString& filename);
  /**
     @brief Draw all loaded data in the workspace that is visible

     This method is called from The CGisDraw thread. The thread has to make sure
     that everything is thread safe.

     @param p         the painter to be used
     @param viewport  the viewport in units of rad
     @param gis       the draw context to be used
   */
  void draw(QPainter& p, const QPolygonF& viewport, CGisDraw* gis);

  /**
     @brief Receive the current mouse position

     Iterate over all projects and pass the position

     @param pos   the mouse position on the screen in pixel
   */
  void mouseMove(const QPointF& pos);
  /**
     @brief Draw all data that is time variant and can't wait for a full update

     This method is called directly from the main thread's paintEvent() method.

     @param p         the painter to be used
     @param viewport  the viewport in units of rad
     @param gis       the draw context to be used
   */
  void fastDraw(QPainter& p, const QRectF& viewport, CGisDraw* gis);

  /**
     @brief Get items close to the given point

     Note: Do not store the pointers of items permanently as they can become invalid
     once you reach the main event loop again. Store the key instead.

     @param pos       the position in pixel
     @param items     an empty item list that will get filled with temporary pointers
   */
  void getItemsByPos(const QPointF& pos, QList<IGisItem*>& items);

  /**
     @brief Get items matching the given area

     @param area      a rectangle in screen pixel coordinates
     @param flags     flag field with IGisItem::selection_e flags set
     @param items     a list to receive the temporary pointers to the found items
   */
  void getItemsByArea(const QRectF& area, IGisItem::selflags_t flags, QList<IGisItem*>& items);

  /**
     @brief Find first item with matching key
     @param key       the item's key as it is returned from IGisItem::getKey()
     @return If no item is found 0 is returned.
   */
  IGisItem* getItemByKey(const IGisItem::key_t& key);

  void getItemsByKeys(const QList<IGisItem::key_t>& keys, QList<IGisItem*>& items);

  void getNogoAreas(QList<IGisItem*>& nogos);

  /**
     @brief Delete all items with matching key from workspace

     @param key       the item's key as it is returned from IGisItem::getKey()
   */
  void delItemByKey(const IGisItem::key_t& key);

  void delItemsByKey(const QList<IGisItem::key_t>& keys);

  /**
     @brief Edit / view item details
     @param key       the item's key as it is returned from IGisItem::getKey()
   */
  void editItemByKey(const IGisItem::key_t& key);

  /**
     @brief Select a project and add a copy of the item to the project
     @param key       the item's key as it is returned from IGisItem::getKey()
   */
  void copyItemByKey(const IGisItem::key_t& key);

  /**
     @brief Select a project and add a copy of all items in the list
     @param keys      a list of item keys to copy
   */
  IGisProject* copyItemsByKey(const QList<IGisItem::key_t>& keys);

  /**
     @brief Clone waypoint and move clone
     @param key       the item's key as it is returned from IGisItem::getKey()
   */
  void projWptByKey(const IGisItem::key_t& key);

  /**
     @brief Move waypoint via mouse
     @param key       the item's key as it is returned from IGisItem::getKey()
   */
  void moveWptByKey(const IGisItem::key_t& key);

  /**
     @brief Add a new waypoint by Position
     @param pt    the position in degrees
   */
  void addWptByPos(const QPointF& pt, const QString& name = QString(), const QString& desc = QString()) const;

  void addPoisAsWpt(const QSet<IPoiItem>& pois, IGisProject* project = nullptr) const;
  void addPoiAsWpt(const IPoiItem& poi, IGisProject* project = nullptr) const;
  void addPoiAsWpt(const IPoiItem& poi, tristate_e& openEditWindow, IGisProject* project = nullptr) const;

  void toggleWptBubble(const IGisItem::key_t& key);

  void deleteWptRadius(const IGisItem::key_t& key);

  void toggleNogoItem(const IGisItem::key_t& key);

  void editWptRadius(const IGisItem::key_t& key);

  void copyWptCoordByKey(const IGisItem::key_t& key);

  /**
     @brief Set user focus to track
     @param yes       true if focus is set
     @param key       the item's key as it is returned from IGisItem::getKey()
   */
  void focusTrkByKey(bool yes, const IGisItem::key_t& key);

  void focusRteByKey(bool yes, const IGisItem::key_t& key);

  void convertRouteToTrack(const IGisItem::key_t& key);

  void convertTrackToRoute(const IGisItem::key_t& key);

  void cutTrkByKey(const IGisItem::key_t& key);

  void addTrkInfoByKey(const IGisItem::key_t& key);

  void editTrkByKey(const IGisItem::key_t& key);

  void reverseTrkByKey(const IGisItem::key_t& key);

  /**
     @brief Combine all tracks in a given track's project

     This will collect all tracks in a project and pass them
     to the track combine dialog.

     @param keyTrk    the key of the first track
   */
  void combineTrkByKey(const IGisItem::key_t& keyTrk);

  /**
     @brief Combine al tracks in the given list of keys.

     @param keys  a list of GIS item keys
   */
  void combineTrkByKey(const QList<IGisItem::key_t>& keys, const QList<IGisItem::key_t>& keysPreSel);

  void colorTrkByKey(const QList<IGisItem::key_t>& keys);

  void rangeTrkByKey(const IGisItem::key_t& key);

  void copyTrkWithWptByKey(const IGisItem::key_t& key);

  void editRteByKey(const IGisItem::key_t& key);

  void reverseRteByKey(const IGisItem::key_t& key);

  void calcRteByKey(const IGisItem::key_t& key);

  void resetRteByKey(const IGisItem::key_t& key);

  void editAreaByKey(const IGisItem::key_t& key);

  void makeRteFromWpt(const QList<IGisItem::key_t>& keys);

  void editPrxWpt(const QList<IGisItem::key_t>& keys);

  void changeWptSymByKey(const QList<IGisItem::key_t>& keys, const QString& sym);

  void addEleToWptTrkByKey(const QList<IGisItem::key_t>& keys);

  void searchWebByKey(const IGisItem::key_t& key);

  /**
     @brief Select a project via dialog

     If a new project name is entered a new project is created. Else the pointer
     to an existing project is passed back.

     @return 0 if no project was selected.
   */
  IGisProject* selectProject(bool forceSelect);

  void postEventForWks(QEvent* event);

  void setOpacity(qreal val);

  bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline);

  bool areTagsHidden() const;
  void setTagsHidden(bool hidden);

  void tagItemsByKey(const QList<IGisItem::key_t>& keys);

  const CSearch& getCurrentSearch() const { return currentSearch; }

 signals:
  void sigChanged();

 public slots:
  void slotLateInit();
  void slotSaveAll();
  void slotWksItemSelectionReset();
  void slotActivityTrkByKey(const QList<IGisItem::key_t>& keys, trkact_t act);

 private slots:
  void slotSetGisLayerOpacity(int val);
  void slotSearch(const CSearch& currentSearch);

  void slotWksItemSelectionChanged();
  void slotWksItemPressed(QTreeWidgetItem* item);

 private:
  friend class CMainWindow;
  CGisWorkspace(QMenu* menuProject, QWidget* parent);

  static CGisWorkspace* pSelf;

  /**
      The item key of last item pressed in the workspace list.
      The key will be reset by getItemsByPos() which is used by
      the mouse object to find items close by for highlight.
   */
  IGisItem::key_t keyWksSelection;
  CSearch currentSearch;

  enum tags_hidden_e { eTagsHiddenTrue, eTagsHiddenFalse, eTagsHiddenUnknown };
};

#endif  // CGISWORKSPACE_H
