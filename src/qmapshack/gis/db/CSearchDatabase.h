/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSEARCHDATABASE_H
#define CSEARCHDATABASE_H

#include "ui_ISearchDatabase.h"
#include <QDialog>

class CGisListDB;
class IDBFolder;
class QSqlDatabase;

class CSearchDatabase : public QDialog, private Ui::ISearchDatabase
{
    Q_OBJECT
public:
    CSearchDatabase(IDBFolder& db, CGisListDB * parent);
    virtual ~CSearchDatabase() = default;

    bool event(QEvent * e) override;

signals:
    void sigItemChanged(QTreeWidgetItem * item, int column);

private slots:
    void slotSearch();
    void slotItemChanged(QTreeWidgetItem * item, int column);

private:
    void addWithParentFolders(QTreeWidget * result, IDBFolder * folder, QMap<quint64, IDBFolder *> &folders, QSqlDatabase &sqlDB);
    void updateFolder(IDBFolder * folder, CEvtW2DAckInfo * evt);
    IDBFolder& dbFolder;

    bool internalEdit = false;
};

#endif //CSEARCHDATABASE_H

