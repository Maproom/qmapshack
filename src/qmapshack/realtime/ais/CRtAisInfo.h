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
    fAisHandler aisDefault = [&](const QByteArray& t){qDebug() << QString::number(t[0]) << "unknown";};

    struct ais_position_report_t
    {
        quint8 type;
        quint8 repeat;
        quint32 mmsi;
        quint16 speed;
        qint8 accuracy;
        qint32 lon;
        qint32 lat;
        quint16 course;
        quint16 heading;
        quint8 second;
    };

    struct ais_static_and_voyage_t
    {
        quint8 type;
        quint8 repeat;
        quint32 mmsi;
        quint8 version;
        quint32 imo;
        QString callsign;
        QString shipName;
        quint8 shipType;
        quint16 dimToBow;
        quint16 dimToStern;
        quint8 dimToPort;
        quint8 dimToStarboard;
        quint8 positionFix;
        quint8 etaMonth;
        quint8 etaDay;
        quint8 etaHour;
        quint8 etaMinute;
        quint8 draught;
        QString destination;
        quint8 dte;
    };

    struct ais_aid_to_navigation_t
    {
        quint8 type;
        quint8 repeat;
        quint32 mmsi;
        quint8 aidType;
        QString name;
        qint8 accuracy;
        qint32 lon;
        qint32 lat;
        quint16 dimToBow;
        quint16 dimToStern;
        quint8 dimToPort;
        quint8 dimToStarboard;
    };

    quint32 get6bitInt(const QByteArray& data, int start, int count);
    qint64 get6bitSignedInt(const QByteArray& data, int start, int count);
    void getString(const QByteArray& data, QString& string, int start, int count);

    enum aisType
    {
        positionReportClassA = 1,
        positionReportClassAassignedScheduled = 2,
        positionReportClassAresponseToInterrogation = 3,
        staticAndVoyageRelatedData = 5,
        standardClassBpositionReport = 18,
        extendedClassBequipmentPositionReport = 19,
        aidToNavigationReport = 21,
        staticDataReport = 24,
    };

    static constexpr quint8 asciiTo6bitLower = 48;
    static constexpr quint8 asciiTo6BitGapMarker = 40;
    static constexpr quint8 asciiTo6bitUpper = 8;

    void nmeaVDM(const QStringList& tokens);

    void aisClassAcommon(const QByteArray& data);
    void aisStaticAndVoyage(const QByteArray& data);
    void aisClassBcommon(const QByteArray& data);
    void aisAidToNavigation(const QByteArray& data);
    void aisStatic(const QByteArray& data);

    QTcpSocket* socket;
    QTimer* timer;

    QHash<QString, fNmeaHandler> nmeaDict;
    QHash<quint8, fAisHandler> aisDict;

    QDateTime lastTimestamp;

    QByteArray assembler;
    quint8 lastFragment = 0;
    quint8 lastFragmentId = 0;
};

#endif //CRTAISINFO_H

