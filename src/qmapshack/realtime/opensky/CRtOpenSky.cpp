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
#include "realtime/CRtDraw.h"
#include "realtime/opensky/CRtOpenSky.h"

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

    timer = new QTimer(this);
    timer->setInterval(5000);
    timer->setSingleShot(false);
    timer->start();
    connect(timer, &QTimer::timeout, this, &CRtOpenSky::slotUpdate);

    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRtOpenSky::slotRequestFinished);
}

QString CRtOpenSky::getDescription() const
{
    return tr("<b>OpenSky</b><br/>"
              "An online service that provides positional data of civil aircrafts"
              );
}

void CRtOpenSky::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) const
{
    if(checkState(eColumnCheckBox) != Qt::Checked)
    {
        return;
    }

    QPolygonF tmp2 = viewport;
    rt->convertRad2Px(tmp2);

    p.setPen(Qt::yellow);
    p.setBrush(Qt::yellow);
    QPixmap icon("://icons/16x16/Aircraft.png");
    QRect rect = icon.rect();
    rect.moveCenter(QPoint(0,0));

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
        p.drawPixmap(rect, icon);
        p.restore();
    }
}

void CRtOpenSky::loadSettings(QSettings& cfg)
{
    IRtSource::loadSettings(cfg);
}

void CRtOpenSky::saveSettings(QSettings& cfg) const
{
    IRtSource::saveSettings(cfg);
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

    QMutexLocker lock(&IRtSource::mutex);
    aircrafts.clear();
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

    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas != nullptr)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawRt);
    }
}

