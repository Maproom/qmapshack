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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWidget.h"
#include "gis/rte/CGisItemRte.h"
#include "CRouterBRouter.h"
#include "CRouterBRouterSetup.h"
#include "CRouterBRouterInfo.h"
#include "CRouterBRouterSetupException.h"
#include "CRouterBRouterSetupWizard.h"
#include "CRouterBRouterToolShell.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"
#include <QtNetwork>
#include <QtWidgets>

CRouterBRouter::CRouterBRouter(QWidget *parent)
    : IRouter(false, parent)
{
    setupUi(this);

    setup = new CRouterBRouterSetup(this);
    try
    {
        setup->load();
    }
    catch (CRouterBRouterSetupException &e)
    {
        setup->onInvalidSetup();
    }

    connect(toolSetup, &QToolButton::clicked, this, &CRouterBRouter::slotToolSetupClicked);
    connect(toolProfileInfo, &QToolButton::clicked, this, &CRouterBRouter::slotToolProfileInfoClicked);
    connect(setup, &CRouterBRouterSetup::sigDisplayOnlineProfileFinished, this, &CRouterBRouter::slotDisplayProfileInfo);
    connect(setup, &CRouterBRouterSetup::sigError, this, &CRouterBRouter::slotDisplayError);

    comboAlternative->addItem(tr("original"), "0");
    comboAlternative->addItem(tr("first alternative"), "1");
    comboAlternative->addItem(tr("second alternative"), "2");
    comboAlternative->addItem(tr("third alternative"), "3");

    SETTINGS;

    cfg.beginGroup("Route/brouter");
    comboProfile->setCurrentIndex(cfg.value("profile", 0).toInt());
    checkFastRecalc->setChecked(cfg.value("fastRecalc", false).toBool() && (setup->installMode == CRouterBRouterSetup::ModeLocal));
    comboAlternative->setCurrentIndex(cfg.value("alternative", 0).toInt());
    cfg.endGroup();

    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouter::slotRequestFinished);

    timerCloseStatusMsg = new QTimer(this);
    timerCloseStatusMsg->setSingleShot(true);
    timerCloseStatusMsg->setInterval(5000);
    connect(timerCloseStatusMsg, &QTimer::timeout, this, &CRouterBRouter::slotCloseStatusMsg);

    routerSetup = dynamic_cast<CRouterSetup*>(parent);

    connect(toolConsole, &QToolButton::clicked, this, &CRouterBRouter::slotToggleConsole);
    connect(toolToggleBRouter, &QToolButton::clicked, this, &CRouterBRouter::slotToggleBRouter);

    textBRouterOutput->setVisible(false);
    textBRouterError->setVisible(false);

    brouterShell = new CRouterBRouterToolShell(textBRouterOutput,this);
    connect(brouterShell, &CRouterBRouterToolShell::sigProcessStateChanged, this, &CRouterBRouter::slotBRouterStateChanged);
    connect(brouterShell, &CRouterBRouterToolShell::sigProcessError, this, &CRouterBRouter::slotBRouterError);

    updateDialog();
}

CRouterBRouter::~CRouterBRouter()
{
    if (brouterState != QProcess::NotRunning)
    {
        stopBRouter();
    }
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    cfg.setValue("profile", comboProfile->currentIndex());
    cfg.setValue("alternative", comboAlternative->currentIndex());
    cfg.setValue("fastRecalc", checkFastRecalc->isChecked());
    cfg.endGroup();
}

void CRouterBRouter::slotToolSetupClicked() const
{
    stopBRouter();
    CRouterBRouterSetupWizard setupWizard;
    setupWizard.exec();
    clearError();
    try
    {
        setup->load();
    }
    catch (CRouterBRouterSetupException &e)
    {
        setup->onInvalidSetup();
    }
    updateDialog();
}

void CRouterBRouter::slotToolProfileInfoClicked() const
{
    const int index = comboProfile->currentIndex();
    if (index > -1)
    {
        setup->displayProfileAsync(setup->getProfiles().at(index));
    }
}

void CRouterBRouter::slotDisplayError(const QString &error, const QString &details) const
{
    textBRouterError->setText(error + ": " + details);
    textBRouterError->setVisible(true);
}

void CRouterBRouter::clearError() const
{
    textBRouterError->clear();
    textBRouterError->setVisible(false);
}

void CRouterBRouter::slotDisplayProfileInfo(const QString &profile, const QString &content) const
{
    clearError();
    CRouterBRouterInfo info;
    info.setLabel(profile);
    info.setInfo(content);
    info.exec();
}

void CRouterBRouter::updateDialog() const
{
    if (setup->installMode == CRouterBRouterSetup::ModeLocal)
    {
        routerSetup->setRouterTitle(CRouterSetup::RouterBRouter,tr("BRouter (offline)"));
    }
    else
    {
        Q_ASSERT(setup->installMode == CRouterBRouterSetup::ModeOnline);
        routerSetup->setRouterTitle(CRouterSetup::RouterBRouter,tr("BRouter (online)"));
    }
    comboProfile->clear();
    for(const QString& profile : setup->getProfiles())
    {
        comboProfile->addItem(profile,profile);
    }
    updateLocalBRouterStatus();
}

void CRouterBRouter::slotCloseStatusMsg() const
{
    timerCloseStatusMsg->stop();
    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
        canvas->reportStatus("BRouter", "");
    }
}

QString CRouterBRouter::getOptions()
{
    return QString(tr("profile: %1, alternative: %2")
                   .arg(comboProfile->currentData().toString())
                   .arg(comboAlternative->currentData().toInt()+1));
}

bool CRouterBRouter::hasFastRouting()
{
    return setup->installMode == CRouterBRouterSetup::ModeLocal && checkFastRecalc->isChecked();
}

QNetworkRequest CRouterBRouter::getRequest(const QVector<wpt_t>& route_points) const
{
    QString lonlats;
    bool isNext = false;

    for(const wpt_t &pt : route_points)
    {
        if (isNext)
        {
            lonlats.append(QString("|%1,%2").arg(pt.lon).arg(pt.lat));
        } else {
            lonlats = QString("%1,%2").arg(pt.lon).arg(pt.lat);
            isNext = true;
        }
    }

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("lonlats",lonlats.toLatin1());
    urlQuery.addQueryItem("nogos", "");
    urlQuery.addQueryItem("profile", comboProfile->currentData().toString());
    urlQuery.addQueryItem("alternativeidx", comboAlternative->currentData().toString());
    urlQuery.addQueryItem("format", "gpx");

    QUrl url = getServiceUrl();
    url.setPath("/brouter");
    url.setQuery(urlQuery);

    return QNetworkRequest(url);
}

int CRouterBRouter::calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords)
{
    if(!hasFastRouting() || !mutex.tryLock())
    {
        return -1;
    }
    if (setup->installMode == CRouterBRouterSetup::ModeLocal && brouterState == QProcess::NotRunning)
    {
        startBRouter();
    }

    QVector<wpt_t> points;
    points << wpt_t(p1.y()*RAD_TO_DEG,p1.x()*RAD_TO_DEG);
    points << wpt_t(p2.y()*RAD_TO_DEG,p2.x()*RAD_TO_DEG);

    synchronous = true;

    QNetworkReply * reply = networkAccessManager->get(getRequest(points));

    try
    {
        reply->setProperty("options", getOptions());
        reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

        progress = new CProgressDialog(tr("Calculate route with %1").arg(getOptions()), 0, NOINT, this);

        QEventLoop eventLoop;
        connect(progress, &CProgressDialog::rejected, reply, &QNetworkReply::abort);
        connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        delete progress;

        if (reply->error() != QNetworkReply::NoError)
        {
            throw reply->errorString();
        }
        else
        {
            clearError();

            const QByteArray &res = reply->readAll();

            if(res.isEmpty())
            {
                throw tr("response is empty");
            }
            else
            {
                QDomDocument xml;
                xml.setContent(res);

                const QDomElement &xmlGpx = xml.documentElement();
                if(xmlGpx.isNull() || xmlGpx.tagName() != "gpx")
                {
                    throw QString(res);
                }
                else
                {
                    // read the shape
                    const QDomNodeList &xmlLatLng = xmlGpx.firstChildElement("trk")
                            .firstChildElement("trkseg")
                            .elementsByTagName("trkpt");
                    for(int n = 0; n < xmlLatLng.size(); n++)
                    {
                        const QDomElement &elem   = xmlLatLng.item(n).toElement();
                        coords << QPointF();
                        QPointF &point = coords.last();
                        point.setX(elem.attribute("lon").toFloat()*DEG_TO_RAD);
                        point.setY(elem.attribute("lat").toFloat()*DEG_TO_RAD);
                    }
                }
            }
        }
    }
    catch(const QString& msg)
    {
        coords.clear();
        if(!msg.isEmpty())
        {
            reply->deleteLater();
            mutex.unlock();
            throw tr("Bad response from server: %1").arg(msg);
        }
    }

    reply->deleteLater();
    slotCloseStatusMsg();
    mutex.unlock();
    return coords.size();
}

void CRouterBRouter::calcRoute(const IGisItem::key_t& key)
{
    mutex.lock();
    if (setup->installMode == CRouterBRouterSetup::ModeLocal && brouterState == QProcess::NotRunning)
    {
        startBRouter();
    }
    CGisItemRte *rte = dynamic_cast<CGisItemRte*>(CGisWidget::self().getItemByKey(key));
    if(nullptr == rte)
    {
        mutex.unlock();
        return;
    }

    rte->reset();

    slotCloseStatusMsg();

    QVector<wpt_t> points;
    for(const CGisItemRte::rtept_t &pt : rte->getRoute().pts)
    {
        points << wpt_t(pt.lat,pt.lon);
    }

    synchronous = false;

    QNetworkReply * reply = networkAccessManager->get(getRequest(points));

    reply->setProperty("key.item", key.item);
    reply->setProperty("key.project", key.project);
    reply->setProperty("key.device", key.device);
    reply->setProperty("options", getOptions());
    reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
        canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Routing request sent to server. Please wait..."));
    }

    progress = new CProgressDialog(tr("Calculate route with %1").arg(getOptions()), 0, NOINT, this);

    connect(progress, &CProgressDialog::rejected, reply, &QNetworkReply::abort);
}

void CRouterBRouter::slotRequestFinished(QNetworkReply* reply)
{
    if (synchronous)
    {
        return;
    }

    delete progress;

    try
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            throw reply->errorString();
        }

        const QByteArray &res = reply->readAll();
        reply->deleteLater();

        if(res.isEmpty())
        {
            throw tr("response is empty");
        }

        clearError();

        QDomDocument xml;
        xml.setContent(res);

        const QDomElement &xmlGpx = xml.documentElement();
        if(xmlGpx.isNull() || xmlGpx.tagName() != "gpx")
        {
            throw QString(res);
        }

        IGisItem::key_t key;
        key.item    = reply->property("key.item").toString();
        key.project = reply->property("key.project").toString();
        key.device  = reply->property("key.device").toString();
        qint64 time = reply->property("time").toLongLong();
        time = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - time;

        CGisItemRte * rte = dynamic_cast<CGisItemRte*>(CGisWidget::self().getItemByKey(key));
        if(rte != nullptr)
        {
            rte->setResultFromBRouter(xml, reply->property("options").toString() + tr("<br/>Calculation time: %1s").arg(time/1000.0, 0,'f',2));
        }
    }
    catch(const QString& msg)
    {
        if(!msg.isEmpty())
        {
            CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
            if(canvas)
            {
                canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Bad response from server:<br/>%1").arg(msg));
            }
            timerCloseStatusMsg->start();
            reply->deleteLater();
            mutex.unlock();
            return;
        }
    }

    slotCloseStatusMsg();
    mutex.unlock();
}

QUrl CRouterBRouter::getServiceUrl() const
{
    if (setup->installMode == CRouterBRouterSetup::ModeLocal)
    {
        QUrl url(QString("http://"));
        url.setHost(setup->localHost);
        url.setPort(setup->localPort.toInt());
        return url;
    }
    else
    {
        Q_ASSERT(setup->installMode == CRouterBRouterSetup::ModeOnline);
        return QUrl(setup->onlineServiceUrl);
    }
}

void CRouterBRouter::slotToggleConsole() const
{
    textBRouterOutput->setVisible(!textBRouterOutput->isVisible());
    textBRouterError->setVisible(brouterError != QProcess::UnknownError && !textBRouterOutput->isVisible());
}

void CRouterBRouter::slotToggleBRouter() const
{
    if (brouterState == QProcess::NotRunning)
    {
        startBRouter();
    }
    else
    {
        stopBRouter();
    }
}

void CRouterBRouter::startBRouter() const
{
    if (setup->isLocalBRouterInstalled())
    {
        textBRouterOutput->clear();
        //# BRouter standalone server
        //# java -cp brouter.jar btools.brouter.RouteServer <segmentdir> <profile-map> <customprofiledir> <port> <maxthreads>
        //# maxRunningTime is the request timeout in seconds, set to 0 to disable timeout//    JAVA_OPTS=
        //    CLASSPATH=../brouter.jar
        //    java $JAVA_OPTS -cp $CLASSPATH btools.server.RouteServer ../segments4 ../profiles2 ../customprofiles 17777 1

        if (brouterState == QProcess::NotRunning)
        {
            QStringList args;
            args << setup->localJavaOpts.split(QRegExp("\\s+"));
            args << QString("-DmaxRunningTime=%1").arg(setup->localMaxRunningTime);
            args << "-cp";
            args << "brouter.jar";
            args << "btools.server.RouteServer";
            args << setup->localSegmentsDir;
            args << setup->localProfileDir;
            args << setup->localCustomProfileDir;
            args << setup->localPort;
            args << setup->localNumberThreads;
            brouterShell->start(setup->localDir, "java", args);
        }
    }
}

void CRouterBRouter::stopBRouter() const
{
    if (brouterState != QProcess::NotRunning)
    {
        brouterShell->stop();
    }
    textBRouterOutput->setVisible(false);
}

void CRouterBRouter::slotBRouterStateChanged(const QProcess::ProcessState newState)
{
    brouterState = newState;
    updateLocalBRouterStatus();
}

void CRouterBRouter::slotBRouterError(const QProcess::ProcessError error, const QString &errorString)
{
    brouterError = error;
    slotDisplayError(tr("Error"),errorString);
    updateLocalBRouterStatus();
}

void CRouterBRouter::updateLocalBRouterStatus() const
{
    if (setup->installMode == CRouterBRouterSetup::ModeLocal)
    {
        if (setup->isLocalBRouterInstalled())
        {
            switch(brouterState)
            {
            case QProcess::Starting:
            {
                labelStatus->setText(tr("starting"));
                toolConsole->setVisible(true);
                break;
            }
            case QProcess::Running:
            {
                labelStatus->setText(tr("running"));
                toolConsole->setVisible(true);
                break;
            }
            case QProcess::NotRunning:
            {
                labelStatus->setText(tr("stopped"));
                toolConsole->setVisible(brouterError != QProcess::UnknownError);
                break;
            }
            }
            checkFastRecalc->setEnabled(true);
            toolToggleBRouter->setEnabled(true);
        }
        else
        {
            labelStatus->setText(tr("not installed"));
            toolConsole->setVisible(false);
            toolToggleBRouter->setEnabled(false);
            checkFastRecalc->setEnabled(false);
        }
        toolToggleBRouter->setVisible(true);
        checkFastRecalc->setVisible(true);
    }
    else
    {
        Q_ASSERT(setup->installMode == CRouterBRouterSetup::ModeOnline);
        labelStatus->setText(tr("online"));
        toolConsole->setVisible(false);
        toolToggleBRouter->setVisible(false);
        checkFastRecalc->setVisible(false);
        textBRouterOutput->clear();
        textBRouterOutput->setVisible(false);
    }
}
