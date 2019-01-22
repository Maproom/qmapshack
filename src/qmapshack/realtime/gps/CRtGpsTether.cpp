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

#include <QtCore>
#include <QtNetwork>
#include <QtWidgets>

CRtGpsTether::CRtGpsTether(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(toolHelp, &QToolButton::clicked, this, &CRtGpsTether::slotHelp);
    connect(toolConnect, &QToolButton::toggled, this, &CRtGpsTether::slotConnect);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &CRtGpsTether::slotConnected);
    connect(socket, &QTcpSocket::disconnected, this, &CRtGpsTether::slotDisconnected);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    labelStatus->setText("-");
}


void CRtGpsTether::slotHelp() const
{
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
                             tr("GPS Tether\n"
                                "The basic idea of this GPS source is to receive a NMEA stream "
                                "via Ethernet connection. You can use the Android app \"GPS Tether\" "
                                "to provide a host streaming NMEA data. You Android device must be "
                                "in the same network or provide a network as a hot spot.\n"
                                "For configuration you need to know your Android device's IP address "
                                "or it's host name provided by a DNS. Additionally you need the port number "
                                "as configured in the app."
                                )
                             );
}

void CRtGpsTether::loadSettings(QSettings& cfg)
{
    cfg.beginGroup("GpsTether");
    lineHost->setText(cfg.value("host", "").toString());
    spinPort->setValue(cfg.value("port", 10110).toUInt());
    cfg.endGroup();
}

void CRtGpsTether::saveSettings(QSettings& cfg) const
{
    cfg.beginGroup("GpsTether");
    cfg.setValue("host", lineHost->text());
    cfg.setValue("port", spinPort->value());
    cfg.endGroup();
}

void CRtGpsTether::slotConnect(bool yes)
{
    if(yes)
    {
        labelStatus->setText("-");
        lineHost->setEnabled(false);
        spinPort->setEnabled(false);
        socket->connectToHost(lineHost->text(), spinPort->value());
    }
    else
    {
        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            socket->disconnectFromHost();
        }
        else
        {
            socket->abort();
            lineHost->setEnabled(true);
            spinPort->setEnabled(true);
        }
    }
}

void CRtGpsTether::slotConnected()
{
    toolConnect->setChecked(true);
}

void CRtGpsTether::slotDisconnected()
{
    lineHost->setEnabled(true);
    spinPort->setEnabled(true);
    toolConnect->setChecked(false);
}

void CRtGpsTether::slotError(QAbstractSocket::SocketError socketError)
{
    slotDisconnected();
    labelStatus->setText("<b style='color: red;'>" + socket->errorString() + "</b>");
}
