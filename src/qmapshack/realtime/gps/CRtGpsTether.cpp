/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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
#include "realtime/gps/CRtGpsTether.h"
#include "realtime/gps/CRtGpsTetherSetup.h"

#include <QtCore>
#include <QtNetwork>

CRtGpsTether::CRtGpsTether(QObject *parent)
    : QNmeaPositionInfoSource(QNmeaPositionInfoSource::RealTimeMode, parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &CRtGpsTether::slotConnected);
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &CRtGpsTether::slotError);
    connect(socket, &QTcpSocket::stateChanged, this, &CRtGpsTether::slotState);
}

QString CRtGpsTether::getConfig() const
{
    return tr("Host: %1, Port: %2").arg(host).arg(port);
}

QString CRtGpsTether::getHelp() const
{
    return tr("GPS Tether\n"
              "The basic idea of this GPS source is to receive a NMEA stream "
              "via Ethernet connection. You can use the Android app \"GPS Tether\" "
              "to provide a host streaming NMEA data. You Android device must be "
              "in the same network or provide a network as a hot spot.\n"
              "For configuration you need to know your Android device's IP address "
              "or it's host name provided by a DNS. Additionally you need the port number "
              "as configured in the app."
              );
}

void CRtGpsTether::loadSettings(QSettings& cfg)
{
    cfg.beginGroup("device/GpsTether");
    host = cfg.value("host", "").toString();
    port = cfg.value("port", 0).toUInt();
    cfg.endGroup();

    connectToHost();
}

void CRtGpsTether::saveSettings(QSettings& cfg) const
{
    cfg.beginGroup("device/GpsTether");
    cfg.setValue("host", host);
    cfg.setValue("port", port);
    cfg.endGroup();
}

void CRtGpsTether::configure()
{
    CRtGpsTetherSetup dlg(*this, CMainWindow::self().getBestWidgetForParent());
    if(dlg.exec() == QDialog::Accepted)
    {
        connectToHost();
    }
}

void CRtGpsTether::connectToHost()
{
    if(socket->state() != QTcpSocket::UnconnectedState)
    {
        socket->disconnectFromHost();
        socket->waitForDisconnected();
    }

    qDebug() << "connect to" << host << port;
    socket->connectToHost(host, port, QIODevice::ReadOnly);
}

void CRtGpsTether::slotConnected()
{
    if(device() == nullptr)
    {
        setDevice(socket);
        setUpdateInterval(1000);
        startUpdates();
    }
}

void CRtGpsTether::slotError(QAbstractSocket::SocketError error)
{
    qDebug() << "socket error" << error << socket->errorString();
}

void CRtGpsTether::slotState(QAbstractSocket::SocketState state)
{
    qDebug() << "socket state" << state;
}
