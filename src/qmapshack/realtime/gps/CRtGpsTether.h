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

#ifndef CRTGPSTETHER_H
#define CRTGPSTETHER_H

#include "realtime/gps/IRtGpsDevice.h"

#include <QNmeaPositionInfoSource>
#include <QTcpSocket>

class CRtGpsTether : public QNmeaPositionInfoSource, public IRtGpsDevice
{
    Q_OBJECT
public:
    CRtGpsTether(QObject * parent);
    virtual ~CRtGpsTether() = default;

    bool hasConfig() const override
    {
        return true;
    }
    QString getConfig() const override;
    QString getHelp() const override;
    void loadSettings(QSettings& cfg) override;
    void saveSettings(QSettings& cfg) const override;
    void configure() override;

private slots:
    void slotConnected();
    void slotError(QAbstractSocket::SocketError error);
    void slotState(QAbstractSocket::SocketState state);

private:
    void connectToHost();

private:
    friend class CRtGpsTetherSetup;
    QString host;
    quint16 port = 0;
    QTcpSocket * socket;
};

#endif //CRTGPSTETHER_H

