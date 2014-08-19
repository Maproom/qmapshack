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

class QAction;

class CGisListWks : public QTreeWidget
{
    Q_OBJECT
    public:
        CGisListWks(QWidget * parent);
        virtual ~CGisListWks();

        bool hasProject(const QString& key);

    signals:
        void sigChanged();

    private slots:
        void slotContextMenu(const QPoint& point);
        void slotSaveProject();
        void slotSaveAsProject();
        void slotCloseProject();
        void slotItemDoubleClicked(QTreeWidgetItem * item, int);
        void slotEditItem();
        void slotDeleteItem();
        void slotProjWpt();
        void slotMoveWpt();
        void slotFocusTrk(bool on);

    private:
        QMenu * menuProject;
        QAction  * actionSave;
        QAction  * actionSaveAs;
        QAction  * actionClose;

        QMenu * menuItem;
        QAction * actionEditDetails;
        QAction * actionDelete;
        QAction * actionProjWpt;
        QAction * actionMoveWpt;
        QAction * actionFocusTrk;

};

#endif //CGISLISTWKS_H

