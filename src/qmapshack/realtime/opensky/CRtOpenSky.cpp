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
        tree->setItemWidget(itemInfo, eColumnWidget, new CRtOpenSkyInfo(*this, tree));
        emit sigChanged();
    }
}

void CRtOpenSky::loadSettings(QSettings& cfg)
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::loadSettings(cfg);
    showNames = cfg.value("showNames", showNames).toBool();
    emit sigChanged();
}

void CRtOpenSky::saveSettings(QSettings& cfg) const
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::saveSettings(cfg);
    cfg.setValue("showNames", showNames);
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

void CRtOpenSky::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) const
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

    for(const aircraft_t& aircraft : aircrafts)
    {
        QPointF pos = aircraft.pos * DEG_TO_RAD;
        rt->convertRad2Px(pos);

        if(!tmp2.boundingRect().contains(pos))
        {
            continue;
        }

        p.save();
        p.translate(pos);
        p.rotate(aircraft.heading);
        p.drawPixmap(rectIcon, icon);
        p.restore();

        if(showNames)
        {
            QString name = aircraft.callsign.isEmpty() ? tr("unkn.") : aircraft.callsign;
            QRect rectLabel = fm.boundingRect(name);
            rectLabel.moveCenter(pos.toPoint() + QPoint(0,-8));
            rectLabel.adjust(-1,-1,1,1);
            if(!CDraw::doesOverlap(blockedAreas, rectLabel))
            {
                CDraw::text(name, p, rectLabel.center(), Qt::darkBlue);
                blockedAreas << rectLabel;
            }
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
            aircraft.callsign   = jsonStateArray[1].toString();
            qreal lon           = jsonStateArray[5].toDouble();
            qreal lat           = jsonStateArray[6].toDouble();
            aircraft.pos        = QPointF(lon,lat);
            aircraft.heading    = jsonStateArray[10].toDouble();

            aircrafts[key] = aircraft;
        }
    }

    emit sigChanged();
}

