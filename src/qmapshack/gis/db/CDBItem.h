/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CDBITEM_H
#define CDBITEM_H

#include <QCoreApplication>
#include <QTreeWidgetItem>

class IDBFolder;
class QSqlDatabase;

class CDBItem : public QTreeWidgetItem
{
    Q_DECLARE_TR_FUNCTIONS(CDBItem)
public:
    CDBItem(QSqlDatabase& db, quint64 id, IDBFolder * parent);
    virtual ~CDBItem() = default;

    /**
       @brief Get the database id
       @return The ID value used by the database
     */
    quint64 getId() const
    {
        return id;
    }

    /**
       @brief Get the item key

       This is not the full blown QMapShack item key with project and device key. It's just the item key.

       @return The string with the item key
     */
    const QString& getKey() const
    {
        return key;
    }


    QString getName() const;

    /**
       @brief Send show/hide events to the workspace
     */
    void toggle();

    /**
       @brief Delete the folder/item relation in the database
     */
    void remove();

    /**
       @brief Update the time the item is in the lost & found folder
     */
    void updateAge();

private:
    friend bool sortByTime(CDBItem * item1, CDBItem * item2);
    QSqlDatabase& db;
    quint64 id;

    int type = 0;
    QString key;
    QDateTime date;
};

#endif //CDBITEM_H

