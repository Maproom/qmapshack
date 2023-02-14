/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/CGisWorkspace.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CSettings.h"
#include "realtime/ais/CRtAis.h"
#include "realtime/ais/CRtAisInfo.h"
#include "realtime/ais/CRtAisRecord.h"
#include "CMainWindow.h"

#include <QtWidgets>
#include <bitset>

CRtAisInfo::CRtAisInfo(CRtAis& source, QWidget* parent)
    : IRtInfo(&source, parent)
{
    setupUi(this);
    connect(toolHelp, &QToolButton::clicked, this, &CRtAisInfo::slotHelp);
    connect(toolConnect, &QToolButton::toggled, this, &CRtAisInfo::slotConnect);
    connect(toolPause, &QToolButton::toggled, toolReset, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, toolFile, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, toolToTrack, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, lineKey, &QLineEdit::setEnabled);
    connect(toolFile, &QToolButton::clicked, this, &CRtAisInfo::slotSetFilename);
    connect(toolReset, &QToolButton::clicked, this, &CRtAisInfo::slotResetRecord);
    connect(toolToTrack, &QToolButton::clicked, this, &CRtAisInfo::slotToTrack);
    connect(checkShowNames, &QCheckBox::toggled, &source, &CRtAis::slotSetShowNames);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &CRtAisInfo::slotConnected);
    connect(socket, &QTcpSocket::disconnected, this, &CRtAisInfo::slotDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &CRtAisInfo::slotError);
    connect(socket, &QTcpSocket::readyRead, this, &CRtAisInfo::slotReadyRead);

    timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &CRtAisInfo::slotUpdate);

    labelStatus->setText("-");

    nmeaDict["VDM"] = [&](const QStringList& t){nmeaVDM(t);};
    aisDict[0] = [&](const QByteArray& t){ais01(t);};
}

void CRtAisInfo::loadSettings(QSettings& cfg)
{
    lineHost->setText(cfg.value("host", "").toString());
    spinPort->setValue(cfg.value("port", 5631).toUInt());
    checkAutomaticConnect->setChecked(cfg.value("automatic connect", false).toBool());
    lineKey->setText(cfg.value("mmsi", "").toString());
    startRecord(cfg.value("filename", "").toString());
    if(toolRecord->isEnabled())
    {
        toolRecord->setChecked(cfg.value("record", false).toBool());
    }

    autoConnect(1000);
}

void CRtAisInfo::saveSettings(QSettings& cfg) const
{
    cfg.setValue("host", lineHost->text());
    cfg.setValue("port", spinPort->value());
    cfg.setValue("automatic connect", checkAutomaticConnect->isChecked());
    cfg.setValue("record", toolRecord->isChecked());
    cfg.setValue("mmsi", lineKey->text());
    cfg.setValue("filename", toolFile->toolTip());
}

void CRtAisInfo::slotHelp() const
{
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
                             tr("AIS\n"
                                "Use this to receive AIS NMEA data from a network source. This could "
                                "be a publicly streaming source, sending AIS data in clear text over "
                                "TCP port. It could also be shared from a Linux serial port using ncat. "
                                "\n\n"
                                "Example sharing from Linux:\n"
                                "ncat -v -k -l 5631 < /dev/ttyUSB0\n"
                                "Replace /dev/ttyUSB0 with the tty device of your AIS receiver"
                                )
                             );
}

void CRtAisInfo::slotConnect(bool yes)
{
    labelStatus->setText("-");

    if(yes)
    {
        lineHost->setEnabled(false);
        spinPort->setEnabled(false);
        socket->connectToHost(lineHost->text(), spinPort->value());
        toolConnect->setIcon(QIcon("://icons/32x32/Connecting.png"));
    }
    else
    {
        timer->stop();
        disconnectFromHost();
        lineHost->setEnabled(true);
        spinPort->setEnabled(true);
    }
}

void CRtAisInfo::slotConnected()
{
    timer->start();
    toolConnect->setChecked(true);
    toolConnect->setIcon(QIcon("://icons/32x32/Connected.png"));
}

void CRtAisInfo::slotDisconnected()
{
    lineHost->setEnabled(true);
    spinPort->setEnabled(true);
    toolConnect->setChecked(false);
    toolConnect->setIcon(QIcon("://icons/32x32/Disconnected.png"));

    slotUpdate();

    autoConnect(5000);
}

void CRtAisInfo::slotError(QAbstractSocket::SocketError /*socketError*/)
{
    slotDisconnected();
    labelStatus->setText("<b style='color: red;'>" + socket->errorString() + "</b>");
}

void CRtAisInfo::slotReadyRead()
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

        int start = line.indexOf("!");
        const QStringList& tokens = line.mid(start+1).split(',');
        nmeaDict.value(tokens[0].mid(2), nmeaDefault)(tokens);
    }
}

void CRtAisInfo::slotUpdate()
{
    CRtAis* _source = dynamic_cast<CRtAis*>(source.data());
    CRtAisRecord* _record = dynamic_cast<CRtAisRecord*>(record.data());

    checkShowNames->setChecked(_source->getShowNames());
    labelNumberOfShips->setText(QString::number(_source->getNumberOfShips()));
    labelTimestamp->setText(lastTimestamp.toString());

    if(!record.isNull() && toolRecord->isChecked())
    {
        bool ok = false;
        const CRtAis::ship_t* ship = _source->getShipByKey(lineKey->text(), ok);
        if(ok)
        {
            if(!_record->writeEntry(*ship))
            {
                QMessageBox::critical(this, tr("Error..."), record->getError(), QMessageBox::Ok);
                toolPause->setChecked(true);
            }
        }
    }
}

void CRtAisInfo::disconnectFromHost()
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

void CRtAisInfo::autoConnect(int msec)
{
    if(checkAutomaticConnect->isChecked())
    {
        QTimer::singleShot(msec, this, [&](){toolConnect->setChecked(true);});
    }
}

bool CRtAisInfo::verifyLine(const QString& line)
{
    quint8 cs = 0;
    const QByteArray& data = line.toLatin1();

    for(int i = 1; i < data.size() - 3; i++)
    {
        cs ^= data[i];
    }

    return line.rightRef(2).toInt(0, 16) == cs;
}

void CRtAisInfo::nmeaVDM(const QStringList& tokens)
{
    const int fragments = tokens[1].toInt();
    const int fragmentNumber = tokens[2].toInt();
    const int fragmentId = tokens[3].toInt();
    const QString& channel = tokens[4];
    const QByteArray& payload = tokens[5].toLatin1();

    QByteArray data(payload.size(), 0);
    for(int i = 0; i< payload.size(); i++)
    {
        uint8_t c = payload[i] - 48;
        if(c > 40) c -= 8;
        data[i] = c;
    }

    if(fragments > fragmentNumber)
    {
        // qWarning() << "Received fragment";
        if(fragmentNumber == 1)
        {
            assembler.clear();
        }
        else if(fragmentNumber != lastFragment+1 || fragmentId != lastFragmentId)
        {
            qWarning() << "Fragment number " << fragmentNumber << " is not after fragment " << lastFragment << " or fragment id " << fragmentId << " not same as " << lastFragmentId;
            assembler.clear();
            lastFragment = 0;
            return;
        }
        assembler.append(data);
        lastFragment = fragmentNumber;
        lastFragmentId = fragmentId;
        return;
    }
    else if(fragments == 1)
    {
        assembler.clear();
        lastFragment = 0;
    }

    assembler.append(data);

    const uint8_t type = assembler[0];
    switch(type)
    {
    case 1:
    case 2:
    case 3:
        ais01(assembler);
        break;
    case 5:
        ais05(assembler);
        break;
    case 18:
    case 19:
        ais18(assembler);
        break;
    case 21:
        ais21(assembler);
        break;
    case 24:
        ais24(assembler);
        break;
    //default:
        //qWarning() << "Unknown type " << type;
    }

    // This doesnt work?
    //aisDict.value(type, aisDefault)(data);
}

void CRtAisInfo::ais01(const QByteArray& data)
{
    CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

    ais_position_report_t ais;
    ais.type = data[0];
    ais.repeat = getInt(data, 6, 2);
    ais.mmsi = getInt(data, 8, 30);
    ais.speed = getInt(data, 50, 10);
    ais.accuracy = getSignedInt(data, 60, 1);
    ais.lon = getSignedInt(data, 61, 28);
    ais.lat = getSignedInt(data, 89, 27);
    ais.course = getInt(data, 116, 12);
    ais.heading = getInt(data, 128, 9);
    ais.second = getInt(data, 137, 6);

    bool ok = true;
    CRtAis::ship_t* ship = _source->getShipByKey(QString::number(ais.mmsi) , ok);
    ship->longitude = ais.lon / 600000.0;
    ship->latitude = ais.lat / 600000.0;
    ship->heading = ais.heading > 360 ? ais.course > 360 ? -1 : ais.course : ais.heading;
    ship->velocity = ais.speed / 10.0;
    ship->pos = QPointF(ship->longitude, ship->latitude);
    ship->timePosition = QDateTime::currentSecsSinceEpoch();
    lastTimestamp = QDateTime::currentDateTime();

    emit sigChanged();

    //qWarning() << "A, MMSI:" << ship->mmsi << ", Lat: " << ship->latitude << ", Lon: " << ship->longitude;
}

void CRtAisInfo::ais05(const QByteArray& data)
{
    CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

    ais_static_and_voyage_t ais;
    ais.type = data[0];
    ais.repeat = getInt(data, 6, 2);
    ais.mmsi = getInt(data, 8, 30);
    ais.version = getInt(data, 38, 2);
    ais.imo = getInt(data, 40, 30);
    getString(data, ais.callsign, 70, 42);
    getString(data, ais.shipName, 112, 120);
    ais.shipType = getInt(data, 232, 8);
    ais.dimToBow = getInt(data, 240, 9);
    ais.dimToStern = getInt(data, 249, 9);
    ais.dimToPort = getInt(data, 258, 6);
    ais.dimToStarboard = getInt(data, 264, 6);
    ais.positionFix = getInt(data, 270, 4);
    ais.etaMonth = getInt(data, 274, 4);
    ais.etaDay = getInt(data, 278, 5);
    ais.etaHour = getInt(data, 283, 5);
    ais.etaMinute = getInt(data, 288, 6);
    ais.draught = getInt(data, 294, 8);
    getString(data, ais.destination, 302, 120);
    ais.dte = getInt(data, 422, 1);

    bool ok = false;
    CRtAis::ship_t* ship = _source->getShipByKey(QString::number(ais.mmsi) , ok);
    if(ok)
    {
        ship->imo = QString::number(ais.imo);
        ship->callsign = ais.callsign;
        ship->name = ais.shipName;

        lastTimestamp = QDateTime::currentDateTime();

        emit sigChanged();

        //qWarning() << "A, MMSI: " << ais.mmsi << ", IMO: " << ais.imo << ", name: " << ais.shipName;
    }
}

void CRtAisInfo::ais18(const QByteArray& data)
{
    CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

    ais_position_report_t ais;
    ais.type = data[0];
    ais.repeat = getInt(data, 6, 2);
    ais.mmsi = getInt(data, 8, 30);
    ais.speed = getInt(data, 46, 10);
    ais.accuracy = getSignedInt(data, 56, 1);
    ais.lon = getSignedInt(data, 57, 28);
    ais.lat = getSignedInt(data, 85, 27);
    ais.course = getInt(data, 112, 12);
    ais.heading = getInt(data, 124, 9);
    ais.second = getInt(data, 133, 6);

    bool ok = true;
    CRtAis::ship_t* ship = _source->getShipByKey(QString::number(ais.mmsi) , ok);
    ship->longitude = ais.lon / 600000.0;
    ship->latitude = ais.lat / 600000.0;
    ship->heading = ais.heading > 360 ? ais.course > 360 ? -1 : ais.course : ais.heading;
    ship->velocity = ais.speed / 10.0;
    ship->pos = QPointF(ship->longitude, ship->latitude);
    ship->timePosition = QDateTime::currentSecsSinceEpoch();

    lastTimestamp = QDateTime::currentDateTime();

    emit sigChanged();

    //qWarning() << "B, MMSI:" << ship->mmsi << ", Lat: " << ship->latitude << ", Lon: " << ship->longitude;
}

void CRtAisInfo::ais21(const QByteArray& data)
{
    CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

    ais_aid_to_navigation_t ais;
    ais.type = data[0];
    ais.repeat = getInt(data, 6, 2);
    ais.mmsi = getInt(data, 8, 30);
    ais.aidType = getInt(data, 38, 5);
    getString(data, ais.name, 43, 120);
    ais.accuracy = getSignedInt(data, 163, 1);
    ais.lon = getSignedInt(data, 164, 28);
    ais.lat = getSignedInt(data, 192, 27);
    ais.dimToBow = getInt(data, 219, 9);
    ais.dimToStern = getInt(data, 228, 9);
    ais.dimToPort = getInt(data, 237, 6);
    ais.dimToStarboard = getInt(data, 243, 6);

    bool ok = true;
    CRtAis::ship_t* ship = _source->getShipByKey(QString::number(ais.mmsi) , ok);
    ship->longitude = ais.lon / 600000.0;
    ship->latitude = ais.lat / 600000.0;
    ship->pos = QPointF(ship->longitude, ship->latitude);
    ship->timePosition = QDateTime::currentSecsSinceEpoch();
    ship->name = ais.name;
    ship->aid = true;
    ship->heading = -1;
    ship->velocity = -1;

    lastTimestamp = QDateTime::currentDateTime();

    emit sigChanged();
}

void CRtAisInfo::ais24(const QByteArray& data)
{
    CRtAis* _source = dynamic_cast<CRtAis*>(source.data());


    ais_static_and_voyage_t ais;
    ais.type = data[0];
    ais.repeat = getInt(data, 6, 2);
    ais.mmsi = getInt(data, 8, 30);

    uint8_t part = getInt(data, 38, 2);
    if(part == 0)
    {
        getString(data, ais.shipName, 40, 120);

        bool ok = false;
        CRtAis::ship_t* ship = _source->getShipByKey(QString::number(ais.mmsi) , ok);
        if(ok)
        {
            ship->name = ais.shipName;

            lastTimestamp = QDateTime::currentDateTime();

            emit sigChanged();

            //qWarning() << "BA, MMSI: " << ais.mmsi << ", name: " << ais.shipName;
        }
    }
    else if(part == 1)
    {
        ais.shipType = getInt(data, 40, 8);
        getString(data, ais.callsign, 90, 42);
        ais.dimToBow = getInt(data, 132, 9);
        ais.dimToStern = getInt(data, 141, 9);
        ais.dimToPort = getInt(data, 150, 6);
        ais.dimToStarboard = getInt(data, 156, 6);

        bool ok = false;
        CRtAis::ship_t* ship = _source->getShipByKey(QString::number(ais.mmsi) , ok);
        if(ok)
        {
            ship->callsign = ais.callsign;

            lastTimestamp = QDateTime::currentDateTime();

            emit sigChanged();

            //qWarning() << "BB, MMSI: " << ais.mmsi << ", callsign: " << ais.shipName;
        }
    }
}

uint32_t CRtAisInfo::getInt(const QByteArray& data, int start, int count)
{
    int mask = 0x3f;

    int end = start + count;

    int from = start / 6;
    int to = end / 6;

    int from_mask_ignore_bits = start % 6;
    int from_mask = (1 << (6 - from_mask_ignore_bits)) - 1;

    int to_mask_include_bits = end % 6;
    int to_mask = mask & ~((1 << (6 - to_mask_include_bits)) - 1);

    if (from == to)
    {
        return (data[from] & (from_mask & to_mask)) >> (6 - to_mask_include_bits);
    }
    else
    {
        int ret = from_mask & data[from++];
        for (int i = from; i < to; i++)
        {
            ret <<= 6;
            ret |= data[i] & mask;
        }
        if (to_mask_include_bits > 0)
        {
            ret <<= to_mask_include_bits;
            ret |= (data[to] & to_mask) >> (6 - to_mask_include_bits);
        }

        return ret;
    }
}

int64_t CRtAisInfo::getSignedInt(const QByteArray& data, int start, int count)
{
    int ret = getInt(data, start, count);
    int check_mask = 1 << (count - 1);
    if ((check_mask & ret) == check_mask)
    {
        ret = ~ret;
        int mask = (1 << (count)) - 1;
        ret &= mask;
        ret *= -1;
    }
    return ret;
}

void CRtAisInfo::getString(const QByteArray& data, QString& string, int start, int count)
{
    string.clear();
    int end = start + count;
    int ci = 0;
    for (int i = start; i < end && end - i >= 6; i += 6)
    {
        uint8_t c = getInt(data, i, 6) & 0x3F;
        if (c < 32)
        {
            string.insert(ci++, c+64);
        }
        else
        {
            string.insert(ci++, c);
        }
    }
    string.replace(QRegExp("^[@\\s]+"), "");
    string.replace(QRegExp("[@\\s]+$"), "");
}

void CRtAisInfo::startRecord(const QString& filename)
{
    delete record;

    toolFile->setToolTip(filename);
    toolRecord->setEnabled(false);

    if(filename.trimmed().isEmpty())
    {
        return;
    }

    record = new CRtAisRecord(this);

    if(!record->setFile(filename))
    {
        delete record;
        QMessageBox::critical(this, tr("Failed..."), record->getError(), QMessageBox::Ok);
        return;
    }

    toolRecord->setEnabled(true);
}

void CRtAisInfo::fillTrackData(CTrackData& data)
{
    CTrackData::trkseg_t seg;
    seg.pts = record->getTrack();
    data.segs << seg;
    data.name = lineKey->text();
}
