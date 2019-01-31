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
#include "realtime/gpstether/CRtGpsTether.h"
#include "realtime/gpstether/CRtGpsTetherInfo.h"
#include "units/IUnit.h"

#include <QtCore>
#include <QtNetwork>
#include <QtWidgets>

class CRtGpsTether;

CRtGpsTetherInfo::CRtGpsTetherInfo(CRtGpsTether &source, QWidget *parent)
    : QWidget(parent)
    , source(source)
{
    setupUi(this);
    connect(toolHelp, &QToolButton::clicked, this, &CRtGpsTetherInfo::slotHelp);
    connect(toolConnect, &QToolButton::toggled, this, &CRtGpsTetherInfo::slotConnect);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &CRtGpsTetherInfo::slotConnected);
    connect(socket, &QTcpSocket::disconnected, this, &CRtGpsTetherInfo::slotDisconnected);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(socket, &QTcpSocket::readyRead, this, &CRtGpsTetherInfo::slotReadyRead);

    timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &CRtGpsTetherInfo::slotUpdate);

    labelStatus->setText("-");

    dict["GPGSV"] = [&](const QStringList& t){nmeaGPGSV(t);};
    dict["GLGSV"] = [&](const QStringList& t){nmeaGPGSV(t);};
    dict["GNGSV"] = [&](const QStringList& t){nmeaGPGSV(t);};
    dict["GAGSV"] = [&](const QStringList& t){nmeaGPGSV(t);};
    dict["PQGSV"] = [&](const QStringList& t){nmeaGPGSV(t);};

    dict["GPGSA"] = [&](const QStringList& t){nmeaGPGSA(t);};
    dict["GLGSA"] = [&](const QStringList& t){nmeaGPGSA(t);};
    dict["GNGSA"] = [&](const QStringList& t){nmeaGPGSA(t);};
    dict["GAGSA"] = [&](const QStringList& t){nmeaGPGSA(t);};
    dict["PQGSA"] = [&](const QStringList& t){nmeaGPGSA(t);};

    dict["GPRMC"] = [&](const QStringList& t){nmeaGPRMC(t);};
    dict["GLRMC"] = [&](const QStringList& t){nmeaGPRMC(t);};
    dict["GNRMC"] = [&](const QStringList& t){nmeaGPRMC(t);};
    dict["GARMC"] = [&](const QStringList& t){nmeaGPRMC(t);};
    dict["PQRMC"] = [&](const QStringList& t){nmeaGPRMC(t);};

    dict["GPGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};
    dict["GLGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};
    dict["GNGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};
    dict["GAGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};
    dict["PQGGA"] = [&](const QStringList& t){nmeaGPGGA(t);};

    dict["GPVTG"] = [&](const QStringList& t){nmeaGPVTG(t);};
    dict["GLVTG"] = [&](const QStringList& t){nmeaGPVTG(t);};
    dict["GNVTG"] = [&](const QStringList& t){nmeaGPVTG(t);};
    dict["GAVTG"] = [&](const QStringList& t){nmeaGPVTG(t);};
    dict["PQVTG"] = [&](const QStringList& t){nmeaGPVTG(t);};
}

CRtGpsTetherInfo::~CRtGpsTetherInfo()
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

void CRtGpsTetherInfo::slotHelp() const
{
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
                             tr("GPS Tether\n"
                                "The basic idea of this GPS source is to receive a NMEA stream "
                                "via Ethernet connection. You can use the Android app \"GPS Tether\" "
                                "to provide a host streaming NMEA data. Your Android device must be "
                                "in the same network or provide a network as a hot spot.\n"
                                "For configuration you need to know your Android device's IP address "
                                "or it's host name provided by a DNS. The app will tell you the address. "
                                "Additionally you need the port number as configured in the app."
                                )
                             );
}

void CRtGpsTetherInfo::loadSettings(QSettings& cfg)
{
    lineHost->setText(cfg.value("host", "").toString());
    spinPort->setValue(cfg.value("port", 10110).toUInt());
}

void CRtGpsTetherInfo::saveSettings(QSettings& cfg) const
{
    cfg.setValue("host", lineHost->text());
    cfg.setValue("port", spinPort->value());
}

QPointF CRtGpsTetherInfo::getPosition() const
{
    if(gga.isValid)
    {
        return QPointF(gga.lon, gga.lat);
    }
    if(rmc.isValid)
    {
        return QPointF(rmc.lon, rmc.lat);
    }

    return NOPOINTF;
}

void CRtGpsTetherInfo::slotConnect(bool yes)
{
    if(yes)
    {
        rmc.isValid = false;
        gga.isValid = false;
        labelStatus->setText("-");
        lineHost->setEnabled(false);
        spinPort->setEnabled(false);
        socket->connectToHost(lineHost->text(), spinPort->value());
    }
    else
    {
        timer->stop();
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

void CRtGpsTetherInfo::slotConnected()
{
    timer->start();
    toolConnect->setChecked(true);
}

void CRtGpsTetherInfo::slotDisconnected()
{
    lineHost->setEnabled(true);
    spinPort->setEnabled(true);
    toolConnect->setChecked(false);
}

void CRtGpsTetherInfo::slotError(QAbstractSocket::SocketError socketError)
{
    slotDisconnected();
    labelStatus->setText("<b style='color: red;'>" + socket->errorString() + "</b>");
}

void CRtGpsTetherInfo::slotReadyRead()
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

void CRtGpsTetherInfo::slotUpdate()
{
    QDateTime timestamp;

    if(rmc.isValid)
    {
        timestamp = rmc.datetime;
        QString val, unit;
        IUnit::degToStr(rmc.lon, rmc.lat, val);
        labelPosition->setText(val);

        IUnit::self().meter2speed(rmc.groundSpeed, val, unit);
        labelSpeed->setText(QString("%1%2").arg(val).arg(unit));
        labelTime->setText(IUnit::datetime2string(rmc.datetime,true));
    }

    if(gga.isValid)
    {
        timestamp = gga.datetime;
        QString val, unit;
        IUnit::degToStr(gga.lon, gga.lat, val);
        labelPosition->setText(val);
        IUnit::self().meter2elevation(gga.altAboveSeaLevel, val, unit);
        labelElevation->setText(QString("%1%2").arg(val).arg(unit));
        labelTime->setText(IUnit::datetime2string(gga.datetime,true));
        labelTime->setText(IUnit::datetime2string(gga.datetime,true));
    }

    if(lastTimestamp != timestamp)
    {
        emit sigChanged();
    }
    else
    {
        labelPosition->setText("-");
        labelSpeed->setText("-");
        labelElevation->setText("-");
        labelTime->setText("-");
    }
    lastTimestamp = timestamp;
}

bool CRtGpsTetherInfo::verifyLine(const QString& line)
{
    quint8 cs = 0;
    const QByteArray& data = line.toLatin1();


    for(int i = 1; i < data.size() - 3; i++)
    {
        cs ^= data[i];
    }

    return line.right(2).toInt(0,16) == cs;
}

void CRtGpsTetherInfo::nmeaGPGSV(const QStringList& tokens)
{
}

void CRtGpsTetherInfo::nmeaGPGSA(const QStringList& tokens)
{
}

void CRtGpsTetherInfo::nmeaGPRMC(const QStringList& tokens)
{
    const QString& id= tokens[0];
    if(tokens.count() < 12)
    {
        qDebug() << id << "too short";
        return;
    }

//    qDebug() << id << "ok";
    if(tokens[2] == "V")
    {
        rmc.isValid = false;
        return;
    }
    rmc.isValid = true;
    rmc.datetime = QDateTime::fromString(tokens[1]+tokens[9], "hhmmss.00ddMMyy").addYears(100);
    rmc.datetime.setTimeSpec(Qt::UTC);

    {
        qreal tmp = tokens[3].toDouble();
        qreal val = int(tmp/100);
        val += (tmp - val*100)/60;
        rmc.lat = tokens[4] == "N" ? val : -val;
    }

    {
        qreal tmp = tokens[5].toDouble();
        qreal val = int(tmp/100);
        val += (tmp - val*100)/60;
        rmc.lon = tokens[6] == "E" ? val : -val;
    }

    rmc.groundSpeed = tokens[7].toDouble() * 1.852 / 3.6;
    rmc.magneticVariation = tokens[11] == "E" ? tokens[10].toDouble() : -tokens[10].toDouble();
    rmc.trackMadeGood = tokens[8].toDouble();
}

void CRtGpsTetherInfo::nmeaGPGGA(const QStringList& tokens)
{
    const QString& id= tokens[0];
    if(tokens.count() < 15)
    {
        qDebug() << id << "too short";
        return;
    }

    //qDebug() << id << "ok" << tokens;
    if(tokens[6].toInt() == 0)
    {
        gga.isValid = false;
        return;
    }

    gga.isValid = true;
    gga.datetime.setTime(QTime::fromString(tokens[1],"hhmmss.00"));
    gga.datetime.setDate(QDate::currentDate());
    gga.datetime.setTimeSpec(Qt::UTC);

    {
        qreal tmp = tokens[2].toDouble();
        qreal val = int(tmp/100);
        val += (tmp - val*100)/60;
        gga.lat = tokens[3] == "N" ? val : -val;
    }

    {
        qreal tmp = tokens[4].toDouble();
        qreal val = int(tmp/100);
        val += (tmp - val*100)/60;
        gga.lon = tokens[5] == "E" ? val : -val;
    }
    gga.quality = tokens[6].toInt();
    gga.numSatelites = tokens[7].toInt();
    gga.horizDilution = tokens[8].toDouble();
    gga.altAboveSeaLevel = tokens[9].toDouble();
    gga.geodialSeparation = tokens[11].toDouble();
    gga.age = tokens[13].toInt();
    gga.diffRefStation = tokens[14].toInt();


//    qDebug() << gga.datetime << gga.lat << gga.lon << gga.quality << gga.numSatelites
//             << gga.altAboveSeaLevel << gga.geodialSeparation << gga.horizDilution;
}

void CRtGpsTetherInfo::nmeaGPVTG(const QStringList& tokens)
{
//    qDebug() << tokens;
}
