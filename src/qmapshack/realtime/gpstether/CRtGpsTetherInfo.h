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

#ifndef CRTGPSINFO_H
#define CRTGPSINFO_H

#include "ui_IRtGpsInfo.h"

#include <functional>
#include <QTcpSocket>
#include <QWidget>

using fNemaLine = std::function<void(const QStringList&)>;

class CRtGpsTether;
class QSettings;

class CRtGpsTetherInfo : public QWidget, private Ui::IRtGpsTetherInfo
{
    Q_OBJECT
public:
    CRtGpsTetherInfo(CRtGpsTether& source, QWidget *parent);
    virtual ~CRtGpsTetherInfo();

    void loadSettings(QSettings& cfg);
    void saveSettings(QSettings& cfg) const;

private slots:
    void slotHelp() const;
    void slotConnect(bool yes);
    void slotConnected();
    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError socketError);
    void slotReadyRead();

private:
    bool verifyLine(const QString& line);

    fNemaLine nmeaDefault = [&](const QStringList& t){qDebug() << t[0] << "unknown";};

    void nmeaGPGSV(const QStringList& tokens);
    void nmeaGPRMC(const QStringList& tokens);
    void nmeaGPGGA(const QStringList& tokens);
    void nmeaGPVTG(const QStringList& tokens);

private:
    CRtGpsTether& source;

    QTcpSocket * socket;
    QHash<QString,fNemaLine> dict;

    struct rmc_t
    {
        QDateTime datetime;
        bool valid {false};
        qreal lat {0.0};
        qreal lon {0.0};
        qreal groundSpeed {0.0};
        qreal trackMadeGood {0.0};
        qreal magneticVariation {0.0};
    };

    QHash<QString, rmc_t> rmc;

    struct gga_t
    {
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

    QHash<QString, gga_t> gga;
};

#endif //CRTGPSINFO_H
