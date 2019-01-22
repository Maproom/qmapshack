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
#include "ui_IRtGpsTether.h"

#include <QWidget>
#include <QTcpSocket>

class CRtGpsTether : public QWidget, public IRtGpsDevice, private Ui::IRtGpsTether
{
    Q_OBJECT
public:
    CRtGpsTether(QWidget *parent);
    virtual ~CRtGpsTether() = default;

    void loadSettings(QSettings& cfg) override;
    void saveSettings(QSettings& cfg) const override;

private slots:
    void slotHelp() const;
    void slotConnect(bool yes);
    void slotConnected();
    void slotDisconnected();

    void slotError(QAbstractSocket::SocketError socketError);



private:
    QTcpSocket * socket;
};

#endif //CRTGPSTETHER_H

