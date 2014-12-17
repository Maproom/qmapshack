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

#ifndef IDBFOLDER_H
#define IDBFOLDER_H

#include <QTreeWidgetItem>

class QSqlDatabase;
class CEvtW2DAckInfo;

class IDBFolder : public QTreeWidgetItem
{
    public:
        enum type_e
        {
             eTypeLostFound = 1
            ,eTypeDatabase = 2
            ,eTypeGroup = 3
            ,eTypeProject = 4
            ,eTypeOther = 5
        };

        enum column_e
        {
             eColumnCheckbox = 0
            ,eColumnName = 1
        };

        IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidgetItem * parent);
        IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidget * parent);
        virtual ~IDBFolder();

        quint64 getId(){return id;}
        QString getDBName();

        virtual void expanding();
        virtual void update(CEvtW2DAckInfo * info);
        virtual void toggle();
        virtual void remove();

        static IDBFolder * createFolderByType(QSqlDatabase &db, int type, quint64 id, QTreeWidgetItem *parent);

        bool operator<(const QTreeWidgetItem &other) const;

    protected:        
        void setupFromDB();
        virtual void addChildren(const QSet<QString> &activeChildren);
        virtual void remove(quint64 idParent, quint64 idFolder);

        QSqlDatabase& db;

        quint64 id;
        QString key;
        bool isLoadable;        


};

#endif //IDBFOLDER_H

