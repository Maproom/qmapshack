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

#ifndef CGISLISTWKS_H
#define CGISLISTWKS_H

#include <QMap>
#include <QPointer>
#include <QSqlDatabase>
#include <QTreeWidget>
#include <initializer_list>

#include "gis/prj/IGisProject.h"
#include "gis/trk/CTrackData.h"

struct action_t;
class QAction;
class CGeoSearch;
class IGisProject;
class CDBProject;
class IDeviceWatcher;
class QActionGroup;

class CGisListWks : public QTreeWidget {
  Q_OBJECT
 public:
  CGisListWks(QWidget* parent);
  virtual ~CGisListWks();

  // enum column_e { eColumnName = 2 };

  void setExternalMenu(QMenu* project);
  bool hasProject(IGisProject* project);

  IGisProject* getProjectByKey(const QString& key);
  CDBProject* getProjectById(quint64 id, const QString& db);

  bool event(QEvent* e) override;

  void addProject(IGisProject* proj);

  void removeDevice(const QString& key);

  void setUserFocus(const QString& key, bool yes);

  bool hasDeviceSupport() const { return deviceWatcher != nullptr; }

  /// all context-menu actions of this widget, for keyboard shortcut configuration
  QList<QAction*> shortcutActions() const;

 public slots:
  void slotLoadWorkspace();
  void slotCopyProject();

 signals:
  void sigChanged();
  void sigItemDeleted();

 protected:
  void dragMoveEvent(QDragMoveEvent* e) override;
  void dropEvent(QDropEvent* e) override;
  void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) override;

 private slots:
  void slotSaveWorkspace();
  void slotContextMenu(const QPoint& point);
  void slotSaveProject();
  void slotSaveAsProject();
  void slotSaveAsStrictGpx11Project();
  void slotEditPrj();
  void slotCloseProject();
  void slotDeleteProject();
  void slotShowOnMap();
  void slotHideFrMap();
  void slotItemDoubleClicked(QTreeWidgetItem* item, int);
  void slotItemChanged(QTreeWidgetItem* item, int column);
  void slotEditItem();
  void slotEdit();
  void slotTagItem();
  void slotDeleteItem();
  void slotDelete();
  void slotBubbleWpt();
  void slotNogoItem();
  void slotDelRadiusWpt();
  void slotEditRadiusWpt();
  void slotProjWpt();
  void slotMoveWpt();
  void slotCopyCoordWpt();
  void slotFocusTrk(bool on);
  void slotEditTrk();
  void slotReverseTrk();
  void slotCombineTrk();
  void slotRangeTrk();
  void slotActivityTrk(trkact_t act);
  void slotColorTrk();
  void slotCopyTrkWithWpt();
  void slotFocusRte(bool on);
  void slotCalcRte();
  void slotResetRte();
  void slotEditRte();
  void slotReverseRte();
  void slotRte2Trk();
  void slotEditArea();
  void slotAddEmptyProject();
  void slotCloseAllProjects();
  void slotGeoSearch(bool on);
  void slotCopyItem();
  void slotCopy();
  void slotSyncWksDev();
  void slotSyncDevWks();
  void slotRteFromWpt();
  void slotEditPrxWpt();
  void slotSyncDB();
  void slotSetSortMode(IGisProject::sorting_folder_e mode, bool checked);
  void slotSymWpt();
  void slotEleWptTrk();
  void slotAutoSaveProject(bool on);
  void slotUserFocusPrj(bool yes);
  void slotAutoSyncProject(bool yes);
  void slotAddProjectFilter();
  void slotNewDevice();
  void slotSyncPrjToDevices();
  void slotToRoute();
  void slotToArea();

 private:
  /** @brief The context menu matching the current selection, as decided by updateActionState() */
  enum menu_e {
    eMenuNone,
    eMenuProjectWks,
    eMenuProjectDev,
    eMenuProjectTrash,
    eMenuItemTrk,
    eMenuItemWpt,
    eMenuItemRte,
    eMenuItemOvl,
    eMenuItemMulti
  };

  /**
     @brief Bring all actions in sync with the current selection

     @return the context menu for the selection
   */
  menu_e updateActionState();

  /** @brief Category of an action */
  enum category_e { eCategoryProject, eCategoryItem, eCategoryTrack, eCategoryWaypoint, eCategoryRoute, eCategoryArea };

  /** @brief The translated name for a category */
  QString categoryName(category_e category) const;

  /** @brief What the selection holds, which decides what an action can apply to */
  enum selection_e { eSelectionNone, eSelectionProjects, eSelectionItems, eSelectionMixed };

  selection_e selectionType() const;

  /** @brief The one selected item, or nullptr if the selection does not hold exactly one */
  QTreeWidgetItem* singleSelectedItem() const;

  /** @brief Disable every action, then enable the listed ones */
  void enableActionsOnly(const std::initializer_list<QAction*>& enabled);

  /**
     @brief Disable every action outside the given categories
   */
  void disableActionsOutside(const std::initializer_list<category_e>& categories);

  /** @brief The actions for a project */
  QList<QAction*> projectActions() const;

  /**
     @brief The projects an action applies to

     Either the selected projects, or the projects the selected items belong to
   */
  QList<IGisProject*> targetProjects() const;

  /** @brief The one project an action applies to, or nullptr if the selection does not resolve to exactly one */
  IGisProject* singleTargetProject() const;

  /**
     @brief Bring the project actions in sync with the projects they would act on

     @return the project menu those projects ask for
   */
  menu_e updateProjectActions(const QList<IGisProject*>& projects);

  void configDB();
  void initDB();
  void migrateDB(int version);
  void migrateDB1to2();
  void migrateDB2to3();
  void migrateDB3to4();
  void migrateDB4to5();
  void setVisibilityOnMap(bool visible);
  QAction* addSortAction(const QString& objName, QObject* parent, QActionGroup* actionGroup, const QString& icon,
                         const QString& text, IGisProject::sorting_folder_e mode, category_e category);

  template <typename Func>
  QAction* addAction(const QString& objName, const QIcon& icon, const QString& name, QObject* parent, Func slot,
                     category_e category) {
    QAction* action = new QAction(icon, name, parent);
    action->setObjectName(objName);
    // register with the widget so a user-assigned shortcut can actually trigger it,
    // independent of the transient context menu the action is also shown in
    QWidget::addAction(action);
    connect(action, &QAction::triggered, this, slot);
    tagCategory(action, category);
    return action;
  }

  /** @brief Record the action's category, for the shortcut setup dialog and for categoryActions */
  void tagCategory(QAction* action, category_e category);

  void showMenuProjectWks(const QPoint& p);
  void showMenuProjectDev(const QPoint& p);
  void showMenuProjectTrash(const QPoint& p);
  void showMenuItemTrk(const QPoint& p, const IGisItem::key_t& key);
  void showMenuItemWpt(const QPoint& p, CGisItemWpt* wpt);
  void showMenuItemRte(const QPoint& p);
  void showMenuItemOvl(const QPoint& p);
  void showMenuItem(const QPoint& p, const QList<IGisItem::key_t>& keysTrks, const QList<IGisItem::key_t>& keysWpts);

  void syncPrjToDevices(IGisProject* project, const QSet<QString>& keys);
  QSet<QString> getAllDeviceKeys() const;

  template <typename T>
  QList<IGisItem::key_t> selectedItems2Keys() const {
    QList<IGisItem::key_t> keys;
    const QList<QTreeWidgetItem*>& items = selectedItems();
    for (const QTreeWidgetItem* item : items) {
      const T* gisItem = dynamic_cast<const T*>(item);
      if (nullptr != gisItem) {
        keys << gisItem->getKey();
      }
    }
    return keys;
  }

  QSqlDatabase db;

  QMap<category_e, QList<QAction*>> categoryActions;

  QActionGroup* actionGroupSort;
  QAction* actionSave;
  QAction* actionSaveAs;
  QAction* actionSaveAsStrict;
  QAction* actionAutoSave;
  QAction* actionUserFocusPrj;
  QAction* actionAutoSyncToDev;
  QAction* actionCloseProj;
  QAction* actionShowOnMap;
  QAction* actionHideFrMap;
  QAction* actionSyncWksDev;
  QAction* actionSyncDB;
  QAction* actionSortByTime;
  QAction* actionSortByName;
  QAction* actionSortByRating;
  QAction* actionFilterProject;
  QAction* actionSyncDevWks;
  QAction* actionEditDetails;
  QAction* actionTagItem;
  QAction* actionCopyItem;
  QAction* actionDelete;
  QAction* actionBubbleWpt;
  QAction* actionDelRadiusWpt;
  QAction* actionEditRadiusWpt;
  QAction* actionProjWpt;
  QAction* actionMoveWpt;
  QAction* actionCopyCoordWpt;
  QAction* actionFocusTrk;
  QAction* actionEditTrk;
  QAction* actionReverseTrk;
  QAction* actionCombineTrk;
  QAction* actionRangeTrk;
  QAction* actionNogoTrk;
  QAction* actionCopyTrkWithWpt;
  QAction* actionFocusRte;
  QAction* actionCalcRte;
  QAction* actionResetRte;
  QAction* actionEditRte;
  QAction* actionReverseRte;
  QAction* actionRte2Trk;
  QAction* actionEditArea;
  QAction* actionNogoArea;
  QAction* actionRteFromWpt;
  QAction* actionEditPrxWpt;
  QAction* actionChangeIconWpt;
  QAction* actionToRoute;
  QAction* actionToArea;

  QAction* actionEleWptTrk;

  QMenu* menuNone = nullptr;

  QPointer<CGeoSearch> geoSearch;

  bool saveOnExit = true;
  qint32 saveEvery = 5;

  IDeviceWatcher* deviceWatcher = nullptr;

  bool blockSorting = false;
};

#endif  // CGISLISTWKS_H
