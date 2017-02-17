/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler oliver.eichler@gmx.de

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
#include "CSingleInstanceProxy.h"
#include <QtNetwork>

CSingleInstanceProxy::CSingleInstanceProxy(const QStringList filenames)
{
    QLocalSocket socket;
    socket.connectToServer("QMapShack");
    if(socket.waitForConnected(1000))
    {
        QDataStream stream(&socket);
        stream << filenames;
        socket.waitForBytesWritten(1000);
        exit(0);
    }


    server = new QLocalServer(this);
    connect(server, &QLocalServer::newConnection, this, &CSingleInstanceProxy::slotNewConnection);

    server->listen("QMapShack");
}

void CSingleInstanceProxy::slotNewConnection()
{
    QLocalSocket * socket = server->nextPendingConnection();
    if(socket == nullptr)
    {
        return;
    }

    if(socket->waitForReadyRead(2000))
    {
        QStringList filenames;
        QDataStream stream(socket);
        stream >> filenames;

        CMainWindow::self().loadGISData(filenames);

        CMainWindow::self().raise();
        QApplication::setActiveWindow(&CMainWindow::self());
    }

    socket->close();
    delete socket;
}
