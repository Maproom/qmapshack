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

#ifndef CRTAISINFO_H
#define CRTAISINFO_H

#include "realtime/IRtInfo.h"
#include "ui_IRtAisInfo.h"

#include <QPointer>
#include <QTcpSocket>

class CRtAisRecord;
class CRtAis;
class CRtDraw;

class CRtAisInfo : public IRtInfo, private Ui::IRtAisInfo
{
    Q_OBJECT
public:
    CRtAisInfo(CRtAis& source, QWidget* parent);
    virtual ~CRtAisInfo() = default;

    void loadSettings(QSettings& cfg);
    void saveSettings(QSettings& cfg) const;
signals:
    void sigChanged();

public slots:
    void slotHelp() const;
    void slotConnect(bool yes);
    void slotConnected();
    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError socketError);
    void slotReadyRead();
    void slotUpdate();

private:
    void startRecord(const QString& filename) override;
    void fillTrackData(CTrackData& data) override;

    bool verifyLine(const QString& line);
    void disconnectFromHost();
    void autoConnect(int msec);

    using fNmeaHandler = std::function<void(const QStringList&)>;
    fNmeaHandler nmeaDefault = [&](const QStringList& t){qDebug() << t[0] << "unknown";};

    using fAisHandler = std::function<void(const QByteArray&)>;
    fAisHandler aisDefault = [&](const QByteArray& t){qDebug() << t[0] << "unknown";};

    struct ais_position_report_t
    {
        uint8_t type;
        uint8_t repeat;
        uint32_t mmsi;
        uint16_t speed;
        int8_t accuracy;
        int32_t lon;
        int32_t lat;
        uint16_t course;
        uint16_t heading;
        uint8_t second;
    };

    struct ais_static_and_voyage_t
    {
        uint8_t type;
        uint8_t repeat;
        uint32_t mmsi;
        uint8_t version;
        uint32_t imo;
        QString callsign;
        QString shipName;
        uint8_t shipType;
        uint16_t dimToBow;
        uint16_t dimToStern;
        uint8_t dimToPort;
        uint8_t dimToStarboard;
        uint8_t positionFix;
        uint8_t etaMonth;
        uint8_t etaDay;
        uint8_t etaHour;
        uint8_t etaMinute;
        uint8_t draught;
        QString destination;
        uint8_t dte;
    };

    struct ais_aid_to_navigation_t
    {
        uint8_t type;
        uint8_t repeat;
        uint32_t mmsi;
        uint8_t aidType;
        QString name;
        int8_t accuracy;
        int32_t lon;
        int32_t lat;
        uint16_t dimToBow;
        uint16_t dimToStern;
        uint8_t dimToPort;
        uint8_t dimToStarboard;
    };

    uint32_t getInt(const QByteArray& data, int start, int count);
    int64_t getSignedInt(const QByteArray& data, int start, int count);
    void getString(const QByteArray& data, QString& string, int start, int count);

    void nmeaVDM(const QStringList& tokens);
    void ais01(const QByteArray& data);
    void ais05(const QByteArray& data);
    void ais18(const QByteArray& data);
    void ais21(const QByteArray& data);
    void ais24(const QByteArray& data);

    QTcpSocket* socket;
    QTimer* timer;

    QHash<QString, fNmeaHandler> nmeaDict;
    QHash<uint8_t, fAisHandler> aisDict;

    QDateTime lastTimestamp;

    QByteArray assembler;
    uint8_t lastFragment = 0;
    uint8_t lastFragmentId = 0;
};

#endif //CRTAISINFO_H

