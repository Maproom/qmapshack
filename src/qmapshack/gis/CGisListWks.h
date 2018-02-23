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

#ifndef CGISLISTWKS_H
#define CGISLISTWKS_H

#include "gis/prj/IGisProject.h"

#include <QPointer>
#include <QSqlDatabase>
#include <QTreeWidget>

struct action_t;
class QAction;
class CSearchGoogle;
class IGisProject;
class CDBProject;
class IDeviceWatcher;
class QActionGroup;

class CGisListWks : public QTreeWidget
{
    Q_OBJECT
public:
    CGisListWks(QWidget * parent);
    virtual ~CGisListWks();

    enum column_e
    {
        eColumnIcon = 0
        ,eColumnCheckBox = eColumnIcon
        ,eColumnDecoration = eColumnIcon
        ,eColumnName = 1
    };


    void setExternalMenu(QMenu * project);
    bool hasProject(IGisProject *project);

    IGisProject * getProjectByKey(const QString& key);
    CDBProject * getProjectById(quint64 id, const QString& db);

    bool event(QEvent * e) override;

    void addProject(IGisProject *proj);

    void removeDevice(const QString& key);

public slots:
    void slotLoadWorkspace();

signals:
    void sigChanged();
    void sigItemDeleted();

protected:
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent     *e) override;

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
    void slotItemDoubleClicked(QTreeWidgetItem * item, int);
    void slotItemChanged(QTreeWidgetItem * item, int column);
    void slotEditItem();
    void slotDeleteItem();
    void slotBubbleWpt();
    void slotNogoItem();
    void slotDelRadiusWpt();
    void slotEditRadiusWpt();
    void slotProjWpt();
    void slotMoveWpt();
    void slotFocusTrk(bool on);
    void slotEditTrk();
    void slotReverseTrk();
    void slotCombineTrk();
    void slotRangeTrk();
    void slotActivityTrk();
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
    void slotSearchGoogle(bool on);
    void slotCopyItem();
    void slotSyncWksDev();
    void slotSyncDevWks();
    void slotRteFromWpt();
    void slotSyncDB();
    void slotSetSortMode(IGisProject::sorting_folder_e mode, bool checked);
    void slotCopyProject();
    void slotSymWpt();
    void slotAutoSaveProject(bool on);


private:
    void configDB();
    void initDB();
    void migrateDB(int version);
    void migrateDB1to2();
    void migrateDB2to3();
    void setVisibilityOnMap(bool visible);
    QAction * addSortAction(QMenu * menu, QActionGroup *actionGroup, const QString& icon, const QString& text, IGisProject::sorting_folder_e mode);

    QSqlDatabase db;

    QMenu * menuProjectWks;
    QAction  * actionSave;
    QAction  * actionSaveAs;
    QAction  * actionSaveAsStrict;
    QAction  * actionAutoSave;
    QAction  * actionCopyPrj;
    QAction  * actionEditPrj;
    QAction  * actionCloseProj;
    QAction  * actionShowOnMap;
    QAction  * actionHideFrMap;
    QAction  * actionSyncWksDev;
    QAction  * actionSyncDB;
    QAction  * actionSortByTime;
    QAction  * actionSortByName;



    QActionGroup * actionGroup;

    QMenu * menuProjectDev;
    QAction  * actionDelProj;
    QAction  * actionSyncDevWks;

    QMenu * menuProjectTrash;

    QMenu * menuItem;
    QMenu * menuItemTrk;
    QMenu * menuItemWpt;
    QMenu * menuItemRte;
    QMenu * menuItemOvl;
    QAction * actionEditDetails;
    QAction * actionCopyItem;
    QAction * actionDelete;
    QAction * actionBubbleWpt;
    QAction * actionDelRadiusWpt;
    QAction * actionNogoWpt;
    QAction * actionEditRadiusWpt;
    QAction * actionProjWpt;
    QAction * actionMoveWpt;
    QAction * actionFocusTrk;
    QAction * actionEditTrk;
    QAction * actionReverseTrk;
    QAction * actionCombineTrk;
    QAction * actionRangeTrk;
    QAction * actionActivityTrk;
    QAction * actionColorTrk;
    QAction * actionNogoTrk;
    QAction * actionCopyTrkWithWpt;
    QAction * actionFocusRte;
    QAction * actionCalcRte;
    QAction * actionResetRte;
    QAction * actionEditRte;
    QAction * actionNogoRte;
    QAction * actionReverseRte;
    QAction * actionRte2Trk;
    QAction * actionEditArea;
    QAction * actionNogoArea;
    QAction * actionRteFromWpt;
    QAction * actionSymWpt;

    QMenu * menuNone = nullptr;

    QPointer<CSearchGoogle> searchGoogle;

    bool saveOnExit = true;
    qint32 saveEvery = 5;

    IDeviceWatcher * deviceWatcher = nullptr;

    bool blockSorting = false;
};

#endif //CGISLISTWKS_H

