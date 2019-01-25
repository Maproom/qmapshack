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
    connect(socket, &QTcpSocket::readyRead, this, &CRtGpsTether::slotReadyRead);

    labelStatus->setText("-");

    dict["GPGSV"] = [&](const QStringList& t){nmeaGPGSV(t);};
    dict["GLGSV"] = [&](const QStringList& t){nmeaGLGSV(t);};
    dict["GPRMC"] = [&](const QStringList& t){nmeaGPRMC(t);};
    dict["GLRMC"] = [&](const QStringList& t){nmeaGPRMC(t);};
    dict["GNRMC"] = [&](const QStringList& t){nmeaGPRMC(t);};
    dict["GPGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};
    dict["GLGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};
    dict["GNGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};
}

CRtGpsTether::~CRtGpsTether()
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->disconnectFromHost();
        if(socket->state() != QAbstractSocket::UnconnectedState)
        {
            socket->waitForDisconnected();
        }
    }
    else if(socket->state() != QAbstractSocket::UnconnectedState)
    {
        socket->abort();
    }
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

void CRtGpsTether::slotReadyRead()
{
    while(socket->canReadLine())
    {
        QString line = socket->readLine();
        line = line.simplified();
        if(!verifyLine(line))
        {
            continue;
        }
        line.chop(3);

        const QStringList& tokens = line.mid(1).split(',');
        dict.value(tokens[0], nmeaDefault)(tokens);
    }
}

bool CRtGpsTether::verifyLine(const QString& line)
{
    quint8 cs = 0;
    const QByteArray& data = line.toLatin1();


    for(int i = 1; i < data.size() - 3; i++)
    {
        cs ^= data[i];
    }

    return line.right(2).toInt(0,16) == cs;
}

void CRtGpsTether::nmeaGPGSV(const QStringList& tokens)
{
}

void CRtGpsTether::nmeaGLGSV(const QStringList& tokens)
{
}

void CRtGpsTether::nmeaGPRMC(const QStringList& tokens)
{
    const QString& id= tokens[0];

    qDebug() << id << "known";

    if(!rmc.contains(id))
    {
        rmc[id] = rmc_t();
    }

    rmc_t& rmc_ = rmc[id];
    rmc_.valid = tokens[2] != "V";
    rmc_.time = QDateTime::fromString(tokens[1]+tokens[9], "hhmmss.00ddMMyy").addYears(100);
    rmc_.time.setTimeSpec(Qt::UTC);
    rmc_.lat = (tokens[4] == "N" ? tokens[3].toDouble() : -tokens[3].toDouble())/100;
    rmc_.lon = (tokens[6] == "E" ? tokens[5].toDouble() : -tokens[5].toDouble())/100;
    rmc_.groundSpeed = tokens[7].toDouble() * 1.852 / 3.6;
    rmc_.magneticVariation = tokens[11] == "E" ? tokens[10].toDouble() : -tokens[10].toDouble();
}

void CRtGpsTether::nmeaGPGGA(const QStringList& tokens)
{
    qDebug() << tokens;
}
