/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CSingleInstanceProxy.h"

#include <QtNetwork>

#include "CMainWindow.h"

CSingleInstanceProxy::CSingleInstanceProxy(const QStringList filenames) {
  serverName = CMainWindow::self().getUser();
  if (serverName != "QMapShack") {
    serverName = "QMapShack-" + serverName;
  }

  QLocalSocket socket;
  socket.connectToServer(serverName);
  if (socket.waitForConnected(1000)) {
    // if the connection is successful another instance
    // is already running. In that case the list of files to
    // open is sent to the primary instance. And this instance
    // will be closed imediately.
    QDataStream stream(&socket);
    stream << filenames;
    socket.waitForBytesWritten(3000);

    // wait for confirmation
    socket.waitForReadyRead(3000);
    bool ok;
    stream >> ok;
    qDebug() << "Sent parameters to primary instance. Result" << ok;
    qDebug() << "There can only be one. Exit.";
    exit(0);
  }

  // Looks like we are the first instance.
  // Create a server socket and wait for other instances to connect.
  server = new QLocalServer(this);
  connect(server, &QLocalServer::newConnection, this, &CSingleInstanceProxy::slotNewConnection);
  server->removeServer(serverName);
  if (!server->listen(serverName)) {
    qDebug() << "CSingleInstanceProxy: Failed to start single instance server socket.";
  } else {
    qDebug() << "CSingleInstanceProxy: Single instance server socket listening to" << server->fullServerName();
  }
}

CSingleInstanceProxy::~CSingleInstanceProxy() { qDebug() << "CSingleInstanceProxy::~CSingleInstanceProxy()"; }

void CSingleInstanceProxy::slotNewConnection() {
  QLocalSocket* socket = server->nextPendingConnection();
  if (socket == nullptr) {
    return;
  }

  // Each secondary instance will send a QStringList with files to open
  // The list can be empty.
  if (socket->waitForReadyRead(3000)) {
    QStringList filenames;
    QDataStream stream(socket);
    stream >> filenames;

    CMainWindow& w = CMainWindow::self();
    w.loadGISData(filenames);

    // confirm that files are loaded
    stream << true;
    socket->waitForBytesWritten(3000);

    // raise the application window to top of desktop
    w.raise();
    w.activateWindow();
  }

  socket->close();
  delete socket;
}
