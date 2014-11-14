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

class IDBFolder : public QTreeWidgetItem
{
    public:
        enum type_e
        {
             eTypeLostFound = QTreeWidgetItem::UserType + 1
            ,eTypeDatabase  = QTreeWidgetItem::UserType + 2
            ,eTypeGroup     = QTreeWidgetItem::UserType + 3
            ,eTypeProject   = QTreeWidgetItem::UserType + 4
            ,eTypeOther     = QTreeWidgetItem::UserType + 5
        };

        enum column_e
        {
              eColumnSym
            , eColumnName
        };


        IDBFolder(QSqlDatabase& db, type_e type, quint64 id, QTreeWidgetItem * parent);
        IDBFolder(QSqlDatabase& db, type_e type, quint64 id, QTreeWidget * parent);
        virtual ~IDBFolder();

        quint64 getId(){return id;}

        virtual void expanding();

        virtual void checked();

        static IDBFolder * createFolderByType(QSqlDatabase &db, int type, quint64 id, QTreeWidgetItem *parent);

    protected:
        void setupFromDB();


    private:
        QSqlDatabase& db;
        quint64 id;

};

#endif //IDBFOLDER_H

