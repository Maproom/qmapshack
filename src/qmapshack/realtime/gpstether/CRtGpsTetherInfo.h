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

#ifndef CRTGPSINFO_H
#define CRTGPSINFO_H

#include "realtime/gpstether/CRtGpsTetherRecord.h"
#include "realtime/IRtInfo.h"
#include "ui_IRtGpsTetherInfo.h"

#include <functional>
#include <QTcpSocket>
#include <QWidget>


class CRtGpsTether;
class QSettings;

class CRtGpsTetherInfo : public IRtInfo, private Ui::IRtGpsTetherInfo
{
    Q_OBJECT
public:
    CRtGpsTetherInfo(CRtGpsTether& source, QWidget* parent);
    virtual ~CRtGpsTetherInfo();

    void loadSettings(QSettings& cfg);
    void saveSettings(QSettings& cfg) const;

    QPointF getPosition() const;
    qreal getHeading() const;
signals:
    void sigChanged();

private slots:
    void slotHelp() const;
    void slotConnect(bool yes);
    void slotConnected();
    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError socketError);
    void slotReadyRead();
    void slotUpdate();

private:
    bool verifyLine(const QString& line);
    void disconnectFromHost();
    void autoConnect(int msec);

    using fNemaLine = std::function<void(const QStringList&)>;
    fNemaLine nmeaDefault = [&](const QStringList& t){qDebug() << t[0] << "unknown";};

    void nmeaGSV(const QStringList& tokens);
    void nmeaRMC(const QStringList& tokens);
    void nmeaGGA(const QStringList& tokens);
    void nmeaVTG(const QStringList& tokens);
    void nmeaGSA(const QStringList& tokens);


private:
    void startRecord(const QString& filename) override;
    void fillTrackData(CTrackData& data) override;

    QTcpSocket* socket;
    QTimer* timer;

    QHash<QString, fNemaLine> dict;

    QDateTime lastTimestamp;

    struct rmc_t
    {
        bool isValid {false};
        QDateTime datetime;
        qreal lat {0.0};
        qreal lon {0.0};
        qreal groundSpeed {0.0};
        qreal trackMadeGood {0.0};
        qreal magneticVariation {0.0};
    };

    rmc_t rmc;

    struct gga_t
    {
        bool isValid {false};
        QDateTime datetime;
        qreal lat  {0.0};
        qreal lon  {0.0};
        qint32 quality {-1};
        qint32 numSatelites {0};
        qreal horizDilution {0.0};
        qreal altAboveSeaLevel {0.0};
        qreal geodialSeparation {0.0};
        qreal age {0};
        qint32 diffRefStation {0};
    };

    gga_t gga;

    struct vtg_t
    {
        bool isValid {false};
        qreal trackDegreesTrue {0.0};
        qreal trackDegreesMagnetic {0.0};
        qreal speedKnots {0.0};
        qreal speedMeters {0.0};
    };

    vtg_t vtg;

    struct gsa_t
    {
        bool isValid {false};
        int fix {0};
        qreal hdop {0.0};
        qreal vdop {0.0};
    };

    gsa_t gsa;
};

#endif //CRTGPSINFO_H
