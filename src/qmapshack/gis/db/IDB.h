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

#ifndef IDB_H
#define IDB_H

#include <QCoreApplication>
#include <QMap>
#include <QSqlDatabase>

class IDB
{
    Q_DECLARE_TR_FUNCTIONS(IDB)

public:
    IDB();
    virtual ~IDB();

    QSqlDatabase& getDb() { return db; }

    static quint64 getLastInsertID(QSqlDatabase& db, const QString& table);

    bool isUsable() const
    {
        return db.isOpen();
    }

protected:
    static QMap<QString, int> references;
    QSqlDatabase db;
    void setup(const QString& connectionName);
    bool setupDB(QString &error);
    virtual bool initDB() = 0;
    virtual bool migrateDB(int version) = 0;
};

#endif //IDB_H

