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

#ifndef CQLGTDB_H
#define CQLGTDB_H

#include <QDir>
#include <QObject>
#include <QPointer>
#include <QSqlDatabase>
#include <QTreeWidgetItem>
#include <QCoreApplication>

class CImportDatabase;
class CQmsDb;

class CQlgtDb : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(CQlgtDb)
public:
    enum EntryType_e
    {
        eWpt        = QTreeWidgetItem::UserType + 3,
        eTrk        = QTreeWidgetItem::UserType + 4,
        eRte        = QTreeWidgetItem::UserType + 5,
        eOvl        = QTreeWidgetItem::UserType + 6,
        eMap        = QTreeWidgetItem::UserType + 7,
        eDry        = QTreeWidgetItem::UserType + 8,

        eFolder0    = QTreeWidgetItem::UserType + 100,
        eFolderT    = QTreeWidgetItem::UserType + 101,
        eFolder1    = QTreeWidgetItem::UserType + 102,
        eFolder2    = QTreeWidgetItem::UserType + 103,
        eFolderN    = QTreeWidgetItem::UserType + 104
    };

    CQlgtDb(const QString& filename, CImportDatabase * parent);
    virtual ~CQlgtDb();

    void start(const QString& filename);

private:
    void initDB();
    void migrateDB(int version);
    void printStatistic();
    void xferFolders();
    void xferItems();
    void xferItem(quint64 id);
    QSqlDatabase db;
    QDir path;
    QString name;

    CImportDatabase * gui;

    QPointer<CQmsDb> dbQms;

    quint32 nItems;
    quint32 nFolders;
    quint32 nWpt;
    quint32 nTrk;
    quint32 nRte;
    quint32 nOvl;
    quint32 nDiary;
};

#endif //CQLGTDB_H

