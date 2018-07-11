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

#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/router/brouter/CRouterBRouterInfo.h"
#include "gis/rte/router/brouter/CRouterBRouterSetup.h"
#include "gis/rte/router/brouter/CRouterBRouterSetupWizard.h"
#include "gis/rte/router/brouter/CRouterBRouterToolShell.h"
#include "gis/rte/router/CRouterBRouter.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"
#include <QtNetwork>
#include <QtWidgets>

CRouterBRouter* CRouterBRouter::pSelf;

CRouterBRouter::CRouterBRouter(QWidget *parent)
    : IRouter(false, parent)
{
    pSelf = this;
    setupUi(this);

    labelBRouterWarning->hide();

    setup = new CRouterBRouterSetup(this);
    setup->load();

    connect(toolSetup, &QToolButton::clicked, this, &CRouterBRouter::slotToolSetupClicked);
    connect(toolProfileInfo, &QToolButton::clicked, this, &CRouterBRouter::slotToolProfileInfoClicked);
    connect(setup, &CRouterBRouterSetup::sigDisplayOnlineProfileFinished, this, &CRouterBRouter::slotDisplayProfileInfo);
    connect(setup, &CRouterBRouterSetup::sigError, this, &CRouterBRouter::slotDisplayError);

    comboAlternative->addItem(tr("original"), "0");
    comboAlternative->addItem(tr("first alternative"), "1");
    comboAlternative->addItem(tr("second alternative"), "2");
    comboAlternative->addItem(tr("third alternative"), "3");

    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouter::slotRequestFinished);

    timerCloseStatusMsg = new QTimer(this);
    timerCloseStatusMsg->setSingleShot(true);
    timerCloseStatusMsg->setInterval(5000);
    connect(timerCloseStatusMsg, &QTimer::timeout, this, &CRouterBRouter::slotCloseStatusMsg);

    routerSetup = dynamic_cast<CRouterSetup*>(parent);

    connect(toolConsole,       &QToolButton::clicked, this, &CRouterBRouter::slotToggleConsole);
    connect(toolToggleBRouter, &QToolButton::clicked, this, &CRouterBRouter::slotToggleBRouter);
    connect(pushBRouterError,  &QPushButton::clicked, this, &CRouterBRouter::slotClearError);

    textBRouterOutput->setVisible(false);
    textBRouterError->setVisible(false);
    pushBRouterError->setVisible(false);

    //set textBRouterOutput as parent of ToolShell to ensure Toolshell is destroyed before text
    brouterShell = new CRouterBRouterToolShell(textBRouterOutput,textBRouterOutput);
    connect(brouterShell, &CRouterBRouterToolShell::sigProcessStateChanged, this, &CRouterBRouter::slotBRouterStateChanged);
    connect(brouterShell, &CRouterBRouterToolShell::sigProcessError,        this, &CRouterBRouter::slotBRouterError);

    updateDialog();

    SETTINGS;

    cfg.beginGroup("Route/brouter");
    comboProfile->setCurrentIndex(cfg.value("profile", 0).toInt());
    checkFastRecalc->setChecked(cfg.value("fastRecalc", false).toBool() && (setup->installMode == CRouterBRouterSetup::eModeLocal));
    comboAlternative->setCurrentIndex(cfg.value("alternative", 0).toInt());
    cfg.endGroup();
}

CRouterBRouter::~CRouterBRouter()
{
    isShutdown = true;
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
    slotClearError();
    setup->load();
    getBRouterVersion();
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
    textBRouterError->setText(error);
    if (!details.isEmpty())
    {
        textBRouterError->append(details);
    }
    QTextCursor cursor = textBRouterError->textCursor();
    cursor.movePosition(QTextCursor::Start);
    textBRouterError->setTextCursor(cursor);
    textBRouterError->setVisible(true);
    textBRouterOutput->setVisible(false);
    pushBRouterError->setVisible(true);
}

void CRouterBRouter::slotClearError()
{
    textBRouterError->clear();
    textBRouterError->setVisible(false);
    pushBRouterError->setVisible(false);
    brouterError = QProcess::UnknownError;
}

void CRouterBRouter::slotDisplayProfileInfo(const QString &profile, const QString &content)
{
    slotClearError();
    CRouterBRouterInfo info;
    info.setLabel(profile);
    info.setInfo(content);
    info.exec();
}

void CRouterBRouter::setupLocalDir(QString localDir)
{
    if (setup->isLocalBRouterDefaultDir())
    {
        setup->localDir = localDir;
        setup->save();
    }
}

void CRouterBRouter::updateDialog() const
{
    if (setup->installMode == CRouterBRouterSetup::eModeLocal)
    {
        routerSetup->setRouterTitle(CRouterSetup::RouterBRouter,tr("BRouter (offline)"));
        labelCopyrightBRouter->setVisible(true);
        labelCopyrightBRouterWeb->setVisible(false);
    }
    else
    {
        Q_ASSERT(setup->installMode == CRouterBRouterSetup::eModeOnline);
        routerSetup->setRouterTitle(CRouterSetup::RouterBRouter,tr("BRouter (online)"));
        labelCopyrightBRouter->setVisible(false);
        labelCopyrightBRouterWeb->setVisible(true);
    }
    comboProfile->clear();
    bool hasItems = false;
    for(const QString& profile : setup->getProfiles())
    {
        comboProfile->addItem(profile,profile);
        hasItems = true;
    }
    comboProfile->setEnabled(hasItems);
    toolProfileInfo->setEnabled(hasItems);
    comboAlternative->setEnabled(hasItems);
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

void CRouterBRouter::routerSelected()
{
    getBRouterVersion();
}

bool CRouterBRouter::hasFastRouting()
{
    return setup->installMode == CRouterBRouterSetup::eModeLocal && checkFastRecalc->isChecked();
}

QNetworkRequest CRouterBRouter::getRequest(const QVector<QPointF> &routePoints, const QList<IGisItem*> &nogos) const
{
    QString lonLats;

    for(const QPointF &pt : routePoints)
    {
        if (!lonLats.isEmpty())
        {
            lonLats.append("|");
        }
        lonLats.append(QString("%1,%2").arg(pt.x(),0,'f',6).arg(pt.y(),0,'f',6));
    }

    QString nogoStr;
    QString nogoPolygons;
    QString nogoPolylines;

    for(IGisItem* const & item : nogos)
    {
        switch(item->type())
        {
        case IGisItem::eTypeWpt:
        {
            CGisItemWpt * wpt = static_cast<CGisItemWpt*>(item);
            const qreal& rad = wpt->getProximity();
            if (rad != NOFLOAT && rad > 0.)
            {
                const QPointF& pos = wpt->getPosition();
                if (!nogoStr.isEmpty())
                {
                    nogoStr.append("|");
                }
                nogoStr.append(QString("%1,%2,%3").arg(pos.x(),0,'f',6).arg(pos.y(),0,'f',6).arg(rad,0,'f',0));
            }
            break;
        }

        case IGisItem::eTypeOvl:
        case IGisItem::eTypeRte:
        case IGisItem::eTypeTrk:
        {
            IGisLine* line = dynamic_cast<IGisLine*>(item);
            Q_ASSERT(line!=nullptr);
            QPolygonF polygon;
            line->getPolylineDegFromData(polygon);
            QString nogoPoints;
            for (const QPointF point : polygon)
            {
                if (!nogoPoints.isEmpty())
                {
                    nogoPoints.append(",");
                }
                nogoPoints.append(QString("%1,%2").arg(point.x(),0,'f',6).arg(point.y(),0,'f',6));
            }
            if (item->type() == IGisItem::eTypeOvl)
            {
                if (!nogoPolygons.isEmpty())
                {
                    nogoPolygons.append("|");
                }
                nogoPolygons.append(QString("%1").arg(nogoPoints));
            }
            else
            {
                if (!nogoPolylines.isEmpty())
                {
                    nogoPolylines.append("|");
                }
                nogoPolylines.append(QString("%1").arg(nogoPoints));
            }
            break;
        }

        default:
        {
            Q_ASSERT(false);
        }
        }
    }

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("lonlats",lonLats.toLatin1());
    if (!nogoStr.isEmpty())
    {
        urlQuery.addQueryItem("nogos", nogoStr.toLatin1());
    }
    if (!nogoPolygons.isEmpty())
    {
        urlQuery.addQueryItem("polygons", nogoPolygons.toLatin1());
    }
    if (!nogoPolylines.isEmpty())
    {
        urlQuery.addQueryItem("polylines", nogoPolylines.toLatin1());
    }
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
    if(!hasFastRouting())
    {
        return -1;
    }

    const QVector<QPointF> points = {p1*RAD_TO_DEG, p2*RAD_TO_DEG};

    QList<IGisItem*> nogos;
    CGisWorkspace::self().getNogoAreas(nogos);

    return synchronousRequest(points,nogos,coords,false);
}

int CRouterBRouter::synchronousRequest(const QVector<QPointF> &points, const QList<IGisItem *> &nogos, QPolygonF &coords, bool isVersionRequest)
{
    if (isVersionRequest)
    {
        // wait for previous request to finish before issuing version-request
        mutex.lock();
    }
    else if (!mutex.tryLock())
    {
        // skip further on-the-fly-requests as long a previous request is still running
        return -1;
    }

    if (setup->installMode == CRouterBRouterSetup::eModeLocal && brouterState == QProcess::NotRunning)
    {
        startBRouter();
    }

    synchronous = true;

    QNetworkReply * reply = networkAccessManager->get(getRequest(points,nogos));

    try
    {
        reply->setProperty("options", getOptions());
        reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

        progress = new CProgressDialog(tr("Calculate route with %1").arg(getOptions()), 0, NOINT, this);

        QEventLoop eventLoop;
        connect(progress, &CProgressDialog::rejected, reply, &QNetworkReply::abort);
        connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        //Processing userinputevents in local eventloop would cause a SEGV when clicking 'abort' of calling LineOp
        eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

        delete progress;

        const QNetworkReply::NetworkError& netErr = reply->error();
        if (netErr == QNetworkReply::RemoteHostClosedError && nogos.size() > 1 && !isMinimumVersion(1,4,10))
        {
            throw tr("this version of BRouter does not support more then 1 nogo-area");
        }
        else if(netErr != QNetworkReply::NoError)
        {
            throw reply->errorString();
        }
        else
        {
            slotClearError();

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
                    if (isVersionRequest)
                    {
                        parseBRouterVersion(xmlGpx.attribute("creator"));
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
    }
    catch(const QString& msg)
    {
        coords.clear();
        if (isVersionRequest)
        {
            parseBRouterVersion("");
        }
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
    if (setup->installMode == CRouterBRouterSetup::eModeLocal && brouterState == QProcess::NotRunning)
    {
        startBRouter();
    }
    CGisItemRte *rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
    if(nullptr == rte)
    {
        mutex.unlock();
        return;
    }

    QList<IGisItem*> nogos;
    CGisWorkspace::self().getNogoAreas(nogos);

    rte->reset();

    slotCloseStatusMsg();

    QVector<QPointF> points;
    for(const CGisItemRte::rtept_t &pt : rte->getRoute().pts)
    {
        points << QPointF(pt.lon,pt.lat);
    }

    synchronous = false;

    QNetworkReply * reply = networkAccessManager->get(getRequest(points,nogos));

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
        const QNetworkReply::NetworkError& netErr = reply->error();
        if (netErr == QNetworkReply::RemoteHostClosedError && reply->property("nogos").toInt() > 1 && !isMinimumVersion(1,4,10))
        {
            throw tr("this version of BRouter does not support more then 1 nogo-area");
        }
        else if(netErr != QNetworkReply::NoError)
        {
            throw reply->errorString();
        }

        const QByteArray &res = reply->readAll();
        reply->deleteLater();

        if(res.isEmpty())
        {
            throw tr("response is empty");
        }

        slotClearError();

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

        CGisItemRte * rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
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
    if (setup->installMode == CRouterBRouterSetup::eModeLocal)
    {
        QUrl url(QString("http://"));
        url.setHost(setup->localHost);
        url.setPort(setup->localPort.toInt());
        return url;
    }
    else
    {
        Q_ASSERT(setup->installMode == CRouterBRouterSetup::eModeOnline);
        return QUrl(setup->onlineServiceUrl);
    }
}

void CRouterBRouter::slotToggleConsole() const
{
    textBRouterOutput->setVisible(!textBRouterOutput->isVisible());
    bool showError = brouterError != QProcess::UnknownError && !textBRouterOutput->isVisible();
    textBRouterError->setVisible(showError);
    pushBRouterError->setVisible(showError);
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
            if (usesLocalBindaddress())
            {
                args << setup->localHost;
            }
            brouterShell->start(setup->localDir, setup->localJavaExecutable, args);
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

void CRouterBRouter::getBRouterVersion()
{
    if (setup->installMode == CRouterBRouterSetup::eModeLocal)
    {
        if (setup->isLocalBRouterInstalled())
        {
            QProcess cmd;            

            QStringList args;
            args << "-cp";
            args << "brouter.jar";
            args << "btools.server.RouteServer";

            cmd.setWorkingDirectory(setup->localDir);
            cmd.start(setup->localJavaExecutable,args);

            cmd.waitForStarted();
            if (!cmd.waitForFinished(200))
            {
                cmd.kill();
            }

            parseBRouterVersion(QString(cmd.readAll()));

        }
        else
        {
            labelBRouter->setText(tr("BRouter: not found"));
        }
    }
    else
    {
        // use 2 points known to be routable:
        QVector<QPointF> points(2);
        points.replace(0,QPointF(0.1944047317331011,0.8495732565736815)*RAD_TO_DEG);
        points.replace(1,QPointF(0.1944047317331012,0.8495732565736816)*RAD_TO_DEG);
        QPolygonF rt;
        QList<IGisItem*> nogos;
        // parseBRouterVersion is called while parsing remote brouters xml-response:
        try
        {
            synchronousRequest(points,nogos,rt,true);
        }
        catch(const QString&)
        {
        }                        //fail silently
    }
}

void CRouterBRouter::parseBRouterVersion(const QString &text)
{
    // version string is either like "BRouter 1.4.9 / 24092017"
    // or (without the date) like "BRouter-1.4.9"
    QRegExp rx("\\bBRouter[- ](\\d+)\\.(\\d+)\\.(\\d+)\\b");
    if (rx.indexIn(text) > -1)
    {
        versionMajor = rx.cap(1).toInt();
        versionMinor = rx.cap(2).toInt();
        versionPatch = rx.cap(3).toInt();

        labelBRouter->setToolTip(tr("BRouter (Version %1.%2.%3)")
                                 .arg(versionMajor)
                                 .arg(versionMinor)
                                 .arg(versionPatch));
    }
    else
    {
        versionMajor = NOINT;
        versionMinor = NOINT;
        versionPatch = NOINT;

        labelBRouter->setToolTip("BRouter: (failed to read version)");
    }
}

bool CRouterBRouter::isMinimumVersion(int major, int minor, int patch) const
{
    if (versionMajor == NOINT || versionMinor == NOINT || versionPatch == NOINT)
    {
        return false;
    }
    if (versionMajor > major)
    {
        return true;
    }
    if (versionMajor < major)
    {
        return false;
    }
    if (versionMinor > minor)
    {
        return true;
    }
    if (versionMinor < minor)
    {
        return false;
    }
    if (versionPatch > patch)
    {
        return true;
    }
    if (versionPatch < patch)
    {
        return false;
    }
    return true;
}

bool CRouterBRouter::usesLocalBindaddress() const
{
    return setup->localBindLocalonly && isMinimumVersion(1,4,10);
}

void CRouterBRouter::slotBRouterStateChanged(const QProcess::ProcessState newState)
{
    brouterState = newState;
    updateLocalBRouterStatus();
}

void CRouterBRouter::slotBRouterError(const QProcess::ProcessError error, const QString &errorString)
{
    brouterError = error;
    slotDisplayError(tr("Error:"),errorString);
    updateLocalBRouterStatus();
}

void CRouterBRouter::updateLocalBRouterStatus() const
{
    static const QString msgBRouterWarning = tr(
        "QMapShack communicates with BRouter via a network connection. Usually this is done on a special "
        "address that can't be reached from outside your device. However BRouter listens for connections "
        "on all available interfaces. If you are in your own private network with an active firewall, this "
        "is not much of a problem. If you are in a public network every open port is a risk as it can be "
        "used by someone else to compromise your system. We do not recommend to use the local BRouter service "
        "in this case. If you see this warning and use BRouter version >= 1.4.10 you can enforce binding to "
        "local address by setting hostname to \"localhost\" and check \"Bind to hostname only\" in the setup "
        "using expert mode."
        );

    if (isShutdown)
    {
        return;
    }

    labelBRouterWarning->hide();
    if (setup->installMode == CRouterBRouterSetup::eModeLocal)
    {
        if (setup->isLocalBRouterInstalled())
        {
            switch(brouterState)
            {
            case QProcess::Starting:
            {
                SETTINGS;
                if(!usesLocalBindaddress() && cfg.value("Route/brouter/local/showWarning", true).toBool())
                {
                    QMessageBox mbox;
                    mbox.setWindowTitle(tr("Warning..."));
                    mbox.setIcon(QMessageBox::Warning);
                    mbox.setStandardButtons(QMessageBox::Ok);
                    mbox.setText(msgBRouterWarning);

                    QCheckBox * checkAgree = new QCheckBox(tr("I understand the risk. Don't tell me again."), &mbox);
                    mbox.setCheckBox(checkAgree);
                    mbox.exec();
                    cfg.setValue("Route/brouter/local/showWarning", !checkAgree->isChecked());
                }
                labelStatus->setText(tr("starting"));
                toolConsole->setVisible(true);
                break;
            }

            case QProcess::Running:
            {
                labelBRouterWarning->setVisible(!usesLocalBindaddress());
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
        Q_ASSERT(setup->installMode == CRouterBRouterSetup::eModeOnline);
        labelStatus->setText(tr("online"));
        toolConsole->setVisible(false);
        toolToggleBRouter->setVisible(false);
        checkFastRecalc->setVisible(false);
        textBRouterOutput->clear();
        textBRouterOutput->setVisible(false);
    }
}
