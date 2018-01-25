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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "helpers/CDraw.h"
#include "realtime/CRtDraw.h"
#include "realtime/opensky/CRtOpenSky.h"
#include "realtime/opensky/CRtOpenSkyInfo.h"

#include <QJsonDocument>
#include <QtNetwork>
#include <QtWidgets>

const QString CRtOpenSky::strIcon("://icons/48x48/OpenSky.png");

CRtOpenSky::CRtOpenSky(QTreeWidget *parent)
    : IRtSource(eTypeOpenSky, true, parent)
{
    setIcon(eColumnIcon, QIcon(strIcon));
    setText(eColumnName, "OpenSky");
    setCheckState(eColumnCheckBox, Qt::Checked);

    registerWithTreeWidget();

    timer = new QTimer(this);
    timer->setInterval(10100);
    timer->setSingleShot(false);
    timer->start();
    connect(timer, &QTimer::timeout, this, &CRtOpenSky::slotUpdate);

    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRtOpenSky::slotRequestFinished);

    slotUpdate();
}

void CRtOpenSky::registerWithTreeWidget()
{
    QTreeWidget * tree = treeWidget();
    if(tree != nullptr)
    {
        QTreeWidgetItem * itemInfo = new QTreeWidgetItem(this);
        itemInfo->setFlags(Qt::ItemIsEnabled|Qt::ItemNeverHasChildren);
        tree->setItemWidget(itemInfo, eColumnWidget, new CRtOpenSkyInfo(*this, tree));
        emit sigChanged();
    }
}

void CRtOpenSky::loadSettings(QSettings& cfg)
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::loadSettings(cfg);
    showNames = cfg.value("showNames", showNames).toBool();

    QTreeWidget * tree = treeWidget();
    QTreeWidgetItem * item = child(0);
    if((item != nullptr) && (tree != nullptr))
    {
        CRtOpenSkyInfo * info = dynamic_cast<CRtOpenSkyInfo*>(tree->itemWidget(item, eColumnWidget));
        if(info != nullptr)
        {
            info->loadSettings(cfg);
        }
    }

    emit sigChanged();
}

void CRtOpenSky::saveSettings(QSettings& cfg) const
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::saveSettings(cfg);
    cfg.setValue("showNames", showNames);

    QTreeWidget * tree = treeWidget();
    QTreeWidgetItem * item = child(0);
    if((item != nullptr) && (tree != nullptr))
    {
        CRtOpenSkyInfo * info = dynamic_cast<CRtOpenSkyInfo*>(tree->itemWidget(item, eColumnWidget));
        if(info != nullptr)
        {
            info->saveSettings(cfg);
        }
    }
}

QString CRtOpenSky::getDescription() const
{
    return tr("<b>OpenSky</b><br/>"
              "An online service that provides positional data of civil aircrafts"
              );
}

const QDateTime& CRtOpenSky::getTimestamp() const
{
    QMutexLocker lock(&IRtSource::mutex);
    return timestamp;
}

qint32 CRtOpenSky::getNumberOfAircrafts() const
{
    QMutexLocker lock(&IRtSource::mutex);
    return aircrafts.count();
}

bool CRtOpenSky::getShowNames() const
{
    QMutexLocker lock(&IRtSource::mutex);
    return showNames;
}

CRtOpenSky::aircraft_t CRtOpenSky::getAircraftByKey(const QString& key, bool& ok) const
{
    QMutexLocker lock(&IRtSource::mutex);
    ok = false;
    if(aircrafts.contains(key))
    {
        ok = true;
        return aircrafts[key];
    }

    return aircraft_t();
}

void CRtOpenSky::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
{
    if(checkState(eColumnCheckBox) != Qt::Checked)
    {
        return;
    }

    QPolygonF tmp2 = viewport;
    rt->convertRad2Px(tmp2);

    QFontMetrics fm(p.font());

    p.setPen(Qt::yellow);
    p.setBrush(Qt::yellow);
    QPixmap icon("://icons/16x16/Aircraft.png");
    QRect rectIcon = icon.rect();
    rectIcon.moveCenter(QPoint(0,0));

    for(const QString& key : aircrafts.keys())
    {
        aircraft_t& aircraft = aircrafts[key];

        aircraft.point = aircraft.pos * DEG_TO_RAD;
        rt->convertRad2Px(aircraft.point);

        if(!tmp2.boundingRect().contains(aircraft.point))
        {
            continue;
        }

        p.save();
        p.translate(aircraft.point);
        p.rotate(aircraft.heading);
        p.drawPixmap(rectIcon, icon);
        p.restore();

        if(showNames)
        {
            QString name = aircraft.callsign.isEmpty() ? tr("unkn.") : aircraft.callsign;
            QRect rectLabel = fm.boundingRect(name);
            rectLabel.moveCenter(aircraft.point.toPoint() + QPoint(0,-8));
            rectLabel.adjust(-1,-1,1,1);
            if(!CDraw::doesOverlap(blockedAreas, rectLabel))
            {
                CDraw::text(name, p, rectLabel.center(), Qt::darkBlue);
                blockedAreas << rectLabel;
            }
        }
    }
}

void CRtOpenSky::fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt)
{
    if(!keyFocus.isEmpty())
    {
        p.save();
        p.setFont(QFont("Courier",10));

        const aircraft_t& aircraft = aircrafts[keyFocus];
        p.setPen(Qt::red);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(aircraft.point, 10, 10);

        QString text;
        text += tr("key:             %1").arg(aircraft.key) + "\n";
        text += tr("callsign:        %1").arg(aircraft.callsign) + "\n";
        text += tr("origin country:  %1").arg(aircraft.originCountry) + "\n";
        text += tr("time position:   %1").arg(QDateTime::fromTime_t(aircraft.timePosition).toString()) + "\n";
        text += tr("last contact:    %1").arg(QDateTime::fromTime_t(aircraft.lastContact).toString()) + "\n";
        text += tr("longitude:       %1°").arg(aircraft.longitude) + "\n";
        text += tr("latitude:        %1°").arg(aircraft.latitude) + "\n";
        text += tr("geo. alt.:       %1m").arg(aircraft.geoAltitude) + "\n";
        text += tr("on ground:       %1").arg(aircraft.onGround) + "\n";
        text += tr("velocity:        %1km/h").arg(aircraft.velocity*3.6) + "\n";
        text += tr("heading:         %1°").arg(aircraft.heading) + "\n";
        text += tr("vert. rate:      %1m/s").arg(aircraft.vertical_rate) + "\n";
        text += tr("baro. alt.:      %1m").arg(aircraft.baroAltitude) + "\n";
        text += tr("squak:           %1").arg(aircraft.squawk) + "\n";
        text += tr("spi:             %1").arg(aircraft.spi) + "\n";
        text += tr("position source: %1").arg(aircraft.positionSource);

        QFontMetricsF fm(p.font());
        QRectF rectText = fm.boundingRect(QRect(0, 0, 500, 0), Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, text);
        rectText.moveTopLeft(aircraft.point + QPointF(32,0));
        QRectF rectFrame = rectText.adjusted(-5,-5,5,5);

        p.setPen(CDraw::penBorderGray);
        p.setBrush(CDraw::brushBackWhite);
        PAINT_ROUNDED_RECT(p, rectFrame);

        p.setPen(Qt::black);
        p.drawText(rectText, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, text);

        p.restore();
    }
}

void CRtOpenSky::mouseMove(const QPointF& pos)
{
    if(checkState(eColumnCheckBox) != Qt::Checked)
    {
        return;
    }

    QMutexLocker lock(&IRtSource::mutex);

    keyFocus.clear();
    for(const QString& key : aircrafts.keys())
    {
        const aircraft_t& aircraft = aircrafts[key];
        if((aircraft.point - pos).manhattanLength() < 20)
        {
            keyFocus = key;
            break;
        }
    }
}

void CRtOpenSky::slotSetShowNames(bool yes)
{
    QMutexLocker lock(&IRtSource::mutex);
    showNames = yes;
    emit sigChanged();
}

void CRtOpenSky::slotUpdate()
{
    if(checkState(eColumnCheckBox) != Qt::Checked)
    {
        return;
    }

    QUrl url("https://opensky-network.org/");
    url.setPath("/api/states/all");

    QNetworkRequest request;
    request.setUrl(url);
    networkAccessManager->get(request);
}

void CRtOpenSky::slotRequestFinished(QNetworkReply* reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    if(data.isEmpty())
    {
        return;
    }

    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError)
    {
        qDebug() << error.errorString();
        return;
    }

    {
        QMutexLocker lock(&IRtSource::mutex);
        aircrafts.clear();

        timestamp = QDateTime::fromTime_t(json.object().value("time").toInt());
        const QJsonArray& jsonStates = json.object().value("states").toArray();
        for(const QJsonValue& jsonState : jsonStates)
        {
            aircraft_t aircraft;
            const QJsonArray& jsonStateArray = jsonState.toArray();
            QString key         = jsonStateArray[0].toString();

            aircraft.key            = key;
            aircraft.callsign       = jsonStateArray[1].toString();
            aircraft.originCountry  = jsonStateArray[2].toString();
            aircraft.timePosition   = jsonStateArray[3].toInt();
            aircraft.lastContact    = jsonStateArray[4].toInt();
            aircraft.longitude      = jsonStateArray[5].toDouble();
            aircraft.latitude       = jsonStateArray[6].toDouble();
            aircraft.geoAltitude    = jsonStateArray[7].toDouble();
            aircraft.onGround       = jsonStateArray[8].toBool();
            aircraft.velocity       = jsonStateArray[9].toDouble();
            aircraft.heading        = jsonStateArray[10].toDouble();
            aircraft.vertical_rate  = jsonStateArray[11].toDouble();

            aircraft.baroAltitude   = jsonStateArray[13].toDouble();
            aircraft.squawk         = jsonStateArray[14].toString();
            aircraft.spi            = jsonStateArray[15].toBool();
            aircraft.positionSource = jsonStateArray[16].toInt();

            aircraft.pos            = QPointF(aircraft.longitude,aircraft.latitude);
            aircrafts[key] = aircraft;
        }
    }

    emit sigChanged();
}

