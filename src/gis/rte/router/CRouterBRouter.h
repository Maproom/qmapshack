/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#ifndef CROUTERBROUTER_H
#define CROUTERBROUTER_H

#include "gis/rte/router/IRouter.h"
#include "ui_IRouterBRouter.h"

class CRouterBRouterSetup;
class CRouterBRouterSetupWizard;
class CRouterBRouterInfo;
class CRouterBRouterToolShell;
class CRouterSetup;
class CProgressDialog;
#include <QNetworkAccessManager>
#include <QProcess>
#include <QTimer>

class CRouterBRouter : public IRouter, private Ui::IRouterBRouter
{
    Q_OBJECT
public:
    CRouterBRouter(QWidget * parent);
    virtual ~CRouterBRouter();

    void calcRoute(const IGisItem::key_t& key) override;
    int calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords) override;
    bool hasFastRouting() override;
    QString getOptions() override;

private slots:
    void slotToolSetupClicked() const;
    void slotRequestFinished(QNetworkReply* reply);
    void slotCloseStatusMsg() const;
    void slotToolProfileInfoClicked() const;
    void slotDisplayError(const QString &error, const QString &details) const;
    void slotDisplayProfileInfo(const QString &profile, const QString &content) const;
    void slotBRouterStateChanged(const QProcess::ProcessState newState);
    void slotBRouterError(const QProcess::ProcessError error, const QString &errorString);
    void slotToggleBRouter() const;
    void slotToggleConsole() const;

private:

    struct wpt_t
    {
        wpt_t():
            lat(NOFLOAT),
            lon(NOFLOAT)
        {
        }

        wpt_t(qreal lat, qreal lon):
            lat(lat),
            lon(lon)
        {
        }
        qreal lat;
        qreal lon;
    };

    void updateDialog() const;
    void startBRouter() const;
    void stopBRouter() const;
    void updateLocalBRouterStatus() const;
    QNetworkRequest getRequest(const QVector<wpt_t>& route_points) const;
    QUrl getServiceUrl() const;
    void clearError() const;

    QNetworkAccessManager * networkAccessManager;
    QTimer * timerCloseStatusMsg;
    bool synchronous = false;
    QMutex mutex {QMutex::NonRecursive};
    CRouterBRouterSetup * setup;
    CRouterSetup * routerSetup;
    CRouterBRouterInfo * info;
    CRouterBRouterToolShell * brouterShell {nullptr};
    QProcess::ProcessState brouterState { QProcess::NotRunning };
    QProcess::ProcessError brouterError { QProcess::UnknownError };
    CProgressDialog * progress;
    bool isError { false };
    QString error;
    QString errorDetails;
};

#endif //CROUTERBROUTER_H

