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

#ifndef CGISLISTDB_H
#define CGISLISTDB_H

#include <QTreeWidget>
#include <QSqlDatabase>

#include <gis/db/IDBFolder.h>

class QMenu;
class CDBFolderDatabase;

class CGisListDB : public QTreeWidget
{
    Q_OBJECT
    public:
        CGisListDB(QWidget * parent);
        virtual ~CGisListDB();

    private slots:
        void slotContextMenu(const QPoint& point);
        void slotAddFolder();

    private:
        void initDB();
        void migrateDB(int version);

        void addFolder(IDBFolder::type_e type, quint64 key, IDBFolder *parent);


        QSqlDatabase db;

        QMenu * menuDatabase;
        QAction * actionAddFolder;

        QMenu * menuProject;
        QMenu * menuItem;

        QTreeWidgetItem * itemLostFound;
        CDBFolderDatabase * itemDatabase;
};

#endif //CGISLISTDB_H

