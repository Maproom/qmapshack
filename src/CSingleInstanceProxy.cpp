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
        // if the connection is successful another instance
        // is already running. In that case the list of files to
        // open is sent to the primary instance. And this instance
        // will be closed imediately.
        QDataStream stream(&socket);
        stream << filenames;
        socket.waitForBytesWritten(3000);

        qDebug() << "There is alread an instance of QMapShack runnig. Exit this one.";
        exit(0);
    }

    // Looks like we are the first instance.
    // Create a server socket and wait for other instances to connect.
    server = new QLocalServer(this);
    connect(server, &QLocalServer::newConnection, this, &CSingleInstanceProxy::slotNewConnection);
    server->listen("QMapShack");
}

CSingleInstanceProxy::~CSingleInstanceProxy()
{
}

void CSingleInstanceProxy::slotNewConnection()
{
    QLocalSocket * socket = server->nextPendingConnection();
    if(socket == nullptr)
    {
        return;
    }

    // Each secondoray instance will send a QStringList with files to open
    // The list can be empty.
    if(socket->waitForReadyRead(3000))
    {
        QStringList filenames;
        QDataStream stream(socket);
        stream >> filenames;

        CMainWindow& w = CMainWindow::self();
        w.loadGISData(filenames);

        // raise the application window to top of desktop
        w.raise();
        QApplication::setActiveWindow(&w);
    }

    socket->close();
    delete socket;
}
