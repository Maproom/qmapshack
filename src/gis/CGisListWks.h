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

#ifndef CGISLISTWKS_H
#define CGISLISTWKS_H

#include <QTreeWidget>
#include <QPointer>
#include <QSqlDatabase>

struct action_t;
class QAction;
class CSearchGoogle;
class IGisProject;
class CDBProject;

class CGisListWks : public QTreeWidget
{
    Q_OBJECT
    public:
        CGisListWks(QWidget * parent);
        virtual ~CGisListWks();

        void setExternalMenu(QMenu * project);
        bool hasProject(IGisProject *project);

        IGisProject * getProjectByKey(const QString& key);
        CDBProject * getProjectById(quint64 id, const QString& db);

        bool event(QEvent * e);

    signals:
        void sigChanged();

    protected:
        void dragMoveEvent (QDragMoveEvent  * e );
        void dropEvent ( QDropEvent  * e );

    private slots:
        void slotSaveWorkspace();
        void slotLoadWorkspace();
        void slotContextMenu(const QPoint& point);
        void slotSaveProject();
        void slotSaveAsProject();
        void slotEditPrj();
        void slotCloseProject();
        void slotItemDoubleClicked(QTreeWidgetItem * item, int);
        void slotEditItem();
        void slotDeleteItem();
        void slotProjWpt();
        void slotMoveWpt();        
        void slotFocusTrk(bool on);
        void slotEditTrk();
        void slotReverseTrk();
        void slotCombineTrk();
        void slotRangeTrk();
        void slotEditArea();
        void slotAddEmptyProject();
        void slotCloseAllProjects();
        void slotSearchGoogle(bool on);
        void slotCopyItem();


    private:
        void configDB();
        void initDB();
        void migrateDB(int version);

        QSqlDatabase db;

        QMenu * menuProject;
        QAction  * actionSave;
        QAction  * actionSaveAs;
        QAction  * actionEditPrj;
        QAction  * actionClose;

        QMenu * menuItem;
        QAction * actionEditDetails;
        QAction * actionCopyItem;
        QAction * actionDelete;
        QAction * actionProjWpt;
        QAction * actionMoveWpt;
        QAction * actionFocusTrk;
        QAction * actionEditTrk;
        QAction * actionReverseTrk;
        QAction * actionCombineTrk;
        QAction * actionRangeTrk;
        QAction * actionEditArea;

        QMenu * menuNone;

        QPointer<CSearchGoogle> searchGoogle;

        bool saveOnExit;
        qint32 saveEvery;

};

#endif //CGISLISTWKS_H

