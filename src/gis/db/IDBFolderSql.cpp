/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "CMainWindow.h"
#include "gis/CGisListDB.h"
#include "gis/db/CDBFolderLostFound.h"
#include "gis/db/IDBFolderSql.h"
#include "gis/db/macros.h"
#include "helpers/CSettings.h"

#include <QtNetwork>
#include <QtSql>

IDBFolderSql::IDBFolderSql(QSqlDatabase &db, QTreeWidget *parent)
    : IDBFolder(false, db, eTypeDatabase, 1, parent)
{
    socket = new QUdpSocket(this);
}

void IDBFolderSql::expanding()
{
    IDBFolder::expanding();

    folderLostFound  = new CDBFolderLostFound(db, nullptr);
    insertChild(0, folderLostFound);
}

void IDBFolderSql::updateLostFound()
{
    if(folderLostFound)
    {
        folderLostFound->update();
    }
}

bool IDBFolderSql::update()
{
    QSqlQuery query(db);
    QList<IDBFolder*> dbFoldersDel;

    /* Database folders are a bit special as there are no items. But a lost & found folder.
     *
     * As there can be folders removed from the database and new folders this is a bit tricky.
     *
     * dbFoldersAdd is filled with all folders IDs attached with the database folder. Now we
     * iterate over all existing items and remove their ID from dbFoldersAdd. Additionally
     * the folder item is updated. If the update returns false, the folders was removed from
     * the database or an error occured. In both cases the item is registered for removal in
     * dbFoldersDel.
     *
     * When done with the iteration all folders registered for removal are deleted and the
     * new ones are created. Finally lost & found is updated.
     */

    // get all folder IDs attached to this folder
    QList<quint64> dbFoldersAdd;
    query.prepare("SELECT child FROM folder2folder WHERE parent=:parent");
    query.bindValue(":parent", id);
    QUERY_EXEC(return false);
    while(query.next())
    {
        dbFoldersAdd << query.value(0).toULongLong();
    }

    const int N = childCount();
    for(int i = 1; i < N; i++)
    {
        IDBFolder * folder = dynamic_cast<IDBFolder*>(child(i));
        if(folder)
        {
            dbFoldersAdd.removeAll(folder->getId());
            if(!folder->update())
            {
                dbFoldersDel << folder;
            }
        }
    }

    qDeleteAll(dbFoldersDel);

    // add folders
    query.prepare("SELECT t1.child, t2.type FROM folder2folder AS t1, folders AS t2 WHERE t1.parent = :id AND t2.id = t1.child ORDER BY t2.id");
    query.bindValue(":id", id);
    QUERY_EXEC(return false);
    while(query.next())
    {
        quint64 idChild     = query.value(0).toULongLong();
        quint32 typeChild   = query.value(1).toInt();
        if(dbFoldersAdd.contains(idChild))
        {
            createFolderByType(db, typeChild, idChild, this);
        }
    }

    takeChild(0);
    sortChildren(CGisListDB::eColumnName, Qt::AscendingOrder);
    insertChild(0, folderLostFound);

    updateLostFound();
    return true;
}

void IDBFolderSql::announceChange() const
{
    SETTINGS;
    bool enabled = cfg.value("Database/listenUpdate", false).toBool();
    if(!enabled)
    {
        return;
    }
    quint16 port = cfg.value("Database/port", UDP_PORT).toUInt();


    QByteArray msg;
    QDataStream stream(&msg, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_5_2);

    quint32 tan = qrand();

    stream << quint32(0);
    stream << tan;
    stream << CMainWindow::self().id;
    stream << db.driverName();
    stream << getDBName();
    stream << getDBHost();

    QList<QNetworkInterface> netdevices = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface &netdevice : netdevices)
    {
        QList<QNetworkAddressEntry> networks = netdevice.addressEntries();
        for(const QNetworkAddressEntry &network : networks)
        {
            socket->writeDatagram(msg, network.broadcast(), port);
            socket->writeDatagram(msg, network.broadcast(), port);
            socket->writeDatagram(msg, network.broadcast(), port);
        }
    }
}
