/**********************************************************************************************
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CROUTERBROUTERLOCAL_H
#define CROUTERBROUTERLOCAL_H

#include <QAbstractSocket>
#include <QProcess>

class CRouterBRouter;
class CRouterBRouterSetup;
class CRouterBRouterToolShell;
class QEventLoop;

class CRouterBRouterLocal : public QObject
{
    Q_OBJECT
public:
    CRouterBRouterLocal(CRouterBRouter& parent);
    virtual ~CRouterBRouterLocal() {}

    void startBRouter();
    void stopBRouter() const;
    bool isBRouterNotRunning() const;
    bool isBRouterError() const;
    void clearBRouterError();
    void getBRouterVersion() const;
    void updateLocalBRouterStatus() const;

private slots:
    void slotBRouterStateChanged(const QProcess::ProcessState newState);
    void slotBRouterError(const QProcess::ProcessError error, const QString& errorString);
    void slotBRouterSocketError(const QAbstractSocket::SocketError error);
    void slotBRouterSocketConnected();

private:
    bool usesLocalBindaddress() const;

    CRouterBRouter& brouter;

    CRouterBRouterToolShell* brouterShell {nullptr};
    QProcess::ProcessState brouterState { QProcess::NotRunning };
    QProcess::ProcessError brouterError { QProcess::UnknownError };

    enum connect_state_e { eNone=0, eConnected=1, eError=2 };
    QEventLoop* eventLoop { nullptr };
    QAbstractSocket::SocketError socketError { QAbstractSocket::ConnectionRefusedError };
};

#endif //CROUTERBROUTERLOCAL_H

