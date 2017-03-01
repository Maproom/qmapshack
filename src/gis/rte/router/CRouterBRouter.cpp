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
#include "CRouterBRouterInfo.h"
#include "helpers/CSettings.h"
#include <QtNetwork>
#include <QtWidgets>

CRouterBRouter::CRouterBRouter(QWidget *parent)
    : IRouter(false, parent)
{
    setupUi(this);

    setup.load();

    connect(toolSetup, &QToolButton::clicked, this, &CRouterBRouter::slotToolSetupClicked);
    connect(toolProfileInfo, &QToolButton::clicked, this, &CRouterBRouter::slotToolProfileInfoClicked);
    connect(&setup, &CRouterBRouterSetup::displayOnlineProfileFinished, this, &CRouterBRouter::slotDisplayProfileInfo);

    comboAlternative->addItem(tr("original"), "0");
    comboAlternative->addItem(tr("first alternative"), "1");
    comboAlternative->addItem(tr("second alternative"), "2");
    comboAlternative->addItem(tr("third alternative"), "3");

    SETTINGS;

    cfg.beginGroup("Route/brouter");
    comboProfile->setCurrentIndex(cfg.value("profile", 0).toInt());
    checkFastRecalc->setChecked(cfg.value("fastRecalc", false).toBool() and (setup.installMode == CRouterBRouterSetup::ModeLocal));
    comboAlternative->setCurrentIndex(cfg.value("alternative", 0).toInt());
    cfg.endGroup();

    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouter::slotRequestFinished);

    timerCloseStatusMsg = new QTimer(this);
    timerCloseStatusMsg->setSingleShot(true);
    timerCloseStatusMsg->setInterval(5000);
    connect(timerCloseStatusMsg, &QTimer::timeout, this, &CRouterBRouter::slotCloseStatusMsg);

    routerSetup = dynamic_cast<CRouterSetup*>(parent);

    brouterShell = nullptr;
    brouterState = QProcess::NotRunning;

    connect(toolConsole, &QToolButton::clicked, this, &CRouterBRouter::slotToggleConsole);
    connect(toolToggleBRouter, &QToolButton::clicked, this, &CRouterBRouter::slotToggleBRouter);

    updateDialog();
    textBRouterOutput->setVisible(false);
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

void CRouterBRouter::slotToolSetupClicked()
{
    stopBRouter();
    CRouterBRouterSetupWizard setupWizard;
    setupWizard.exec();
    setup.load();
    updateDialog();
}

void CRouterBRouter::slotToolProfileInfoClicked()
{
    const int index = comboProfile->currentIndex();
    if (index > -1)
    {
        setup.displayOnlineProfileAsync(setup.getProfiles().at(index));
    }
}

void CRouterBRouter::slotDisplayProfileInfo(QString profile, QString content)
{
    CRouterBRouterInfo info;
    info.setLabel(profile);
    info.setInfo(content);
    info.exec();
}

void CRouterBRouter::updateDialog()
{
    if (setup.installMode == CRouterBRouterSetup::ModeLocal)
    {
        routerSetup->setRouterTitle(CRouterSetup::RouterBRouter,tr("BRouter (offline)"));
    }
    else
    {
        routerSetup->setRouterTitle(CRouterSetup::RouterBRouter,tr("BRouter (online)"));
    }
    comboProfile->clear();
    for(const QString& profile : setup.getProfiles())
    {
        comboProfile->addItem(profile,profile);
    }
    updateLocalBRouterStatus();
}

void CRouterBRouter::slotCloseStatusMsg()
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
    const QString str;
    return str;
}

bool CRouterBRouter::hasFastRouting()
{
    return setup.installMode == CRouterBRouterSetup::ModeLocal and checkFastRecalc->isChecked();
}

QNetworkRequest CRouterBRouter::getRequest(const QVector<wpt_t>& route_points)
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

    QNetworkRequest request;

    request.setUrl(url);

    return request;
}

void CRouterBRouter::calcRoute(const IGisItem::key_t& key)
{
    if (setup.installMode == CRouterBRouterSetup::ModeLocal and brouterState == QProcess::NotRunning)
    {
        startBRouter();
    }
    CGisItemRte *rte = dynamic_cast<CGisItemRte*>(CGisWidget::self().getItemByKey(key));
    if(nullptr == rte)
    {
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

    QNetworkReply* reply = networkAccessManager->get(getRequest(points));

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
}

int CRouterBRouter::calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords)
{
    if (setup.installMode == CRouterBRouterSetup::ModeLocal and brouterState == QProcess::NotRunning)
    {
        startBRouter();
    }
    if(!hasFastRouting() or !mutex.tryLock())
    {
        return -1;
    }

    QVector<wpt_t> points;
    points << wpt_t(p1.y()*RAD_TO_DEG,p1.x()*RAD_TO_DEG);
    points << wpt_t(p2.y()*RAD_TO_DEG,p2.x()*RAD_TO_DEG);

    synchronous = true;

    QNetworkReply* reply = networkAccessManager->get(getRequest(points));

    reply->setProperty("options", getOptions());
    reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Routing request sent to server. Please wait..."));
    }

    QEventLoop eventLoop;
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    const QByteArray res = reply->readAll();
    reply->deleteLater();

    if(res.isEmpty())
    {
        coords.clear();
    }
    else
    {
        QDomDocument xml;
        xml.setContent(res);

        const QDomElement xmlGpx = xml.documentElement();
        if(xmlGpx.isNull() || xmlGpx.tagName() != "gpx")
        {
            coords.clear();
            QMessageBox::warning(0,tr("Failed..."), tr("Bad response from server:\n%1").arg(QString(res)), QMessageBox::Abort);
        }
        else
        {
            // read the shape
            const QDomNodeList xmlLatLng = xmlGpx.firstChildElement("trk")
                    .firstChildElement("trkseg")
                    .elementsByTagName("trkpt");
            for(int n = 0; n < xmlLatLng.size(); n++)
            {
                const QDomElement elem   = xmlLatLng.item(n).toElement();
                coords << QPointF();
                QPointF& point = coords.last();
                point.setX(elem.attribute("lon").toFloat()*DEG_TO_RAD);
                point.setY(elem.attribute("lat").toFloat()*DEG_TO_RAD);
            }
        }
    }
    slotCloseStatusMsg();
    mutex.unlock();
    return coords.size();
}

void CRouterBRouter::slotRequestFinished(QNetworkReply* reply)
{

    if (synchronous)
    {
        return;
    }

    if(reply->error() != QNetworkReply::NoError)
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas)
        {
            canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Bad response from server:<br/>%1").arg(reply->errorString()));
            timerCloseStatusMsg->start();
        }
        reply->deleteLater();
        return;
    }

    const QByteArray res = reply->readAll();
    reply->deleteLater();

    if(res.isEmpty())
    {
        return;
    }

    QDomDocument xml;
    xml.setContent(res);

    const QDomElement xmlGpx = xml.documentElement();
    if(xmlGpx.isNull() || xmlGpx.tagName() != "gpx")
    {
        QMessageBox::warning(0,tr("Failed..."), tr("Bad response from server:\n%1").arg(QString(res)), QMessageBox::Abort);
        return;
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

    slotCloseStatusMsg();
}

QUrl CRouterBRouter::getServiceUrl()
{
    switch (setup.installMode)
    {
    case CRouterBRouterSetup::ModeLocal:
    {
        QUrl url(QString("http://"));
        url.setHost(setup.localHost);
        url.setPort(setup.localPort.toInt());
        return url;
    }
    case CRouterBRouterSetup::ModeOnline:
    {
        return QUrl(setup.onlineServiceUrl);
    }
    default:
    {
        return QUrl();
    }
    }
}

void CRouterBRouter::slotToggleConsole()
{
    textBRouterOutput->setVisible(!textBRouterOutput->isVisible());
}

void CRouterBRouter::slotToggleBRouter()
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

void CRouterBRouter::startBRouter()
{
    if (setup.isLocalBRouterInstalled())
    {
        if (brouterShell == nullptr)
        {
            textBRouterOutput->clear();
            brouterShell = new CRouterBRouterToolShell(textBRouterOutput,this);
            connect(brouterShell, &CRouterBRouterToolShell::processStateChanged, this, &CRouterBRouter::slotBRouterStateChanged);
        }

        //# BRouter standalone server
        //# java -cp brouter.jar btools.brouter.RouteServer <segmentdir> <profile-map> <customprofiledir> <port> <maxthreads>
        //# maxRunningTime is the request timeout in seconds, set to 0 to disable timeout//    JAVA_OPTS=
        //    CLASSPATH=../brouter.jar
        //    java $JAVA_OPTS -cp $CLASSPATH btools.server.RouteServer ../segments4 ../profiles2 ../customprofiles 17777 1

        if (brouterState == QProcess::NotRunning)
        {
            QStringList args;
            args << setup.localJavaOpts.split(QRegExp("\\s+"));
            args << QString("-DmaxRunningTime=%1").arg(setup.localMaxRunningTime);
            args << "-cp";
            args << "brouter.jar";
            args << "btools.server.RouteServer";
            args << setup.localSegmentsDir;
            args << setup.localProfileDir;
            args << setup.localCustomProfileDir;
            args << setup.localPort;
            args << setup.localNumberThreads;
            brouterShell->start(setup.localDir, "java", args);
        }
    }
}

void CRouterBRouter::stopBRouter()
{
    if (brouterState != QProcess::NotRunning and brouterShell != nullptr)
    {
        brouterShell->stop();
    }
    textBRouterOutput->setVisible(false);
}

void CRouterBRouter::slotBRouterStateChanged(const QProcess::ProcessState newState)
{
    if (newState == QProcess::NotRunning and brouterShell != nullptr)
    {
        delete brouterShell;
        brouterShell = nullptr;
    }

    brouterState = newState;

    updateLocalBRouterStatus();
}

void CRouterBRouter::updateLocalBRouterStatus()
{
    if (setup.installMode == CRouterBRouterSetup::ModeLocal)
    {
        if (setup.isLocalBRouterInstalled())
        {
            switch(brouterState)
            {
            case QProcess::Running:
            {
                labelStatus->setText(tr("running"));
                toolConsole->setVisible(true);
                break;
            }
            case QProcess::Starting:
            {
                labelStatus->setText(tr("starting"));
                toolConsole->setVisible(true);
                break;
            }
            case QProcess::NotRunning:
            {
                labelStatus->setText(tr("stopped"));
                toolConsole->setVisible(false);
                break;
            }
            default:
            {
                labelStatus->setText(tr("invalid"));
                toolConsole->setVisible(false);
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
        labelStatus->setText(tr("online"));
        toolConsole->setVisible(false);
        toolToggleBRouter->setVisible(false);
        checkFastRecalc->setVisible(false);
        textBRouterOutput->clear();
        textBRouterOutput->setVisible(false);
    }
}
