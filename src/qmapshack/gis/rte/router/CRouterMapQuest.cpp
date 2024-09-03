/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/rte/router/CRouterMapQuest.h"

#include <QtNetwork>
#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWorkspace.h"
#include "gis/proj_x.h"
#include "gis/rte/CGisItemRte.h"
#include "helpers/CSettings.h"
const QByteArray CRouterMapQuest::keyMapQuest = "Fmjtd%7Cluu2n16t2h%2Crw%3Do5-haya0";

CRouterMapQuest::CRouterMapQuest(QWidget* parent) : IRouter(false, parent) {
  setupUi(this);

  comboMQPreference->addItem(tr("Fastest"), "fastest");
  comboMQPreference->addItem(tr("Shortest"), "shortest");
  comboMQPreference->addItem(tr("Bicycle"), "bicycle");
  comboMQPreference->addItem(tr("Pedestrian"), "pedestrian");

  comboMQLanguage->addItem(tr("US English"), "en_US");
  comboMQLanguage->addItem(tr("British English"), "en_GB");
  comboMQLanguage->addItem(tr("Danish"), "da_DK");
  comboMQLanguage->addItem(tr("Dutch"), "nl_NL");
  comboMQLanguage->addItem(tr("French"), "fr_FR");
  comboMQLanguage->addItem(tr("German"), "de_DE");
  comboMQLanguage->addItem(tr("Italian"), "it_IT");
  comboMQLanguage->addItem(tr("Norwegian"), "no_NO");
  comboMQLanguage->addItem(tr("Spanish"), "es_ES");
  comboMQLanguage->addItem(tr("Swedish"), "sv_SE");

  SETTINGS;
  int langIdx;
  QString locale = QLocale::system().name();

  cfg.beginGroup("Route/mapquest");
  langIdx = comboMQLanguage->findData(locale);
  comboMQPreference->setCurrentIndex(cfg.value("preference", 0).toInt());
  checkMQAvoidLimAccess->setChecked(cfg.value("avoidLimAccess", false).toBool());
  checkMQAvoidTollRoads->setChecked(cfg.value("avoidTollRoads", false).toBool());
  checkMQAvoidSeasonal->setChecked(cfg.value("avoidSeasonal", false).toBool());
  checkMQAvoidUnpaved->setChecked(cfg.value("avoidUnpaved", false).toBool());
  checkMQAvoidFerry->setChecked(cfg.value("avoidFerry", false).toBool());
  checkMQAvoidCountryBorder->setChecked(cfg.value("avoidCountryBorder", false).toBool());
  comboMQLanguage->setCurrentIndex(cfg.value("language", langIdx).toInt());
  cfg.endGroup();

  networkAccessManager = new QNetworkAccessManager(this);
  connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterMapQuest::slotRequestFinished);

  timerCloseStatusMsg = new QTimer(this);
  timerCloseStatusMsg->setSingleShot(true);
  timerCloseStatusMsg->setInterval(5000);
  connect(timerCloseStatusMsg, &QTimer::timeout, this, &CRouterMapQuest::slotCloseStatusMsg);
}

CRouterMapQuest::~CRouterMapQuest() {
  SETTINGS;
  cfg.beginGroup("Route/mapquest");
  cfg.setValue("preference", comboMQPreference->currentIndex());
  cfg.setValue("avoidLimAccess", checkMQAvoidLimAccess->isChecked());
  cfg.setValue("avoidTollRoads", checkMQAvoidTollRoads->isChecked());
  cfg.setValue("avoidSeasonal", checkMQAvoidSeasonal->isChecked());
  cfg.setValue("avoidUnpaved", checkMQAvoidUnpaved->isChecked());
  cfg.setValue("avoidFerry", checkMQAvoidFerry->isChecked());
  cfg.setValue("avoidCountryBorder", checkMQAvoidCountryBorder->isChecked());
  cfg.setValue("language", comboMQLanguage->currentIndex());
  cfg.endGroup();
}

void CRouterMapQuest::slotCloseStatusMsg() {
  timerCloseStatusMsg->stop();
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    canvas->reportStatus("MapQuest", "");
  }
}

QString CRouterMapQuest::getOptions() {
  QString str;
  int times = 1;

  str += ", " + tr("mode \"%1\"").arg(comboMQPreference->currentText());

  if (checkMQAvoidLimAccess->isChecked()) {
    str += ", " + tr("no highways");
  }

  if (str.size() > 40 * times) {
    str += "<br/>";
    times++;
  }

  if (checkMQAvoidTollRoads->isChecked()) {
    str += ", " + tr("no toll roads");
  }

  if (str.size() > 40 * times) {
    str += "<br/>";
    times++;
  }

  if (checkMQAvoidSeasonal->isChecked()) {
    str += ", " + tr("no seasonal");
  }

  if (str.size() > 40 * times) {
    str += "<br/>";
    times++;
  }

  if (checkMQAvoidUnpaved->isChecked()) {
    str += ", " + tr("no unpaved");
  }

  if (str.size() > 40 * times) {
    str += "<br/>";
    times++;
  }

  if (checkMQAvoidFerry->isChecked()) {
    str += ", " + tr("no ferry");
  }

  if (str.size() > 40 * times) {
    str += "<br/>";
    times++;
  }

  if (checkMQAvoidCountryBorder->isChecked()) {
    str += ", " + tr("no crossing of country borders");
  }

  return str;
}

void CRouterMapQuest::addMapQuestLocations(QDomDocument& xml, QDomElement& locations, CGisItemRte& rte) {
  SGisLine line;
  rte.getPolylineFromData(line);

  for (const IGisLine::point_t& pt : std::as_const(line)) {
    QDomElement location = xml.createElement("location");
    location.appendChild(
        xml.createTextNode(QString("%1,%2").arg(pt.coord.y() * RAD_TO_DEG).arg(pt.coord.x() * RAD_TO_DEG)));
    locations.appendChild(location);
  }
}

void CRouterMapQuest::calcRoute(const IGisItem::key_t& key) {
  CGisItemRte* rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
  if (nullptr == rte) {
    return;
  }

  rte->reset();

  slotCloseStatusMsg();

  QDomDocument xml;

  QDomElement route = xml.createElement("route");
  xml.appendChild(route);

  QDomElement locations = xml.createElement("locations");
  route.appendChild(locations);
  addMapQuestLocations(xml, locations, *rte);

  QDomElement options = xml.createElement("options");
  route.appendChild(options);

  QDomElement shapeFormat = xml.createElement("shapeFormat");
  shapeFormat.appendChild(xml.createTextNode("raw"));
  options.appendChild(shapeFormat);

  QDomElement generalize = xml.createElement("generalize");
  generalize.appendChild(xml.createTextNode("0"));
  options.appendChild(generalize);

  QDomElement unit = xml.createElement("unit");
  unit.appendChild(xml.createTextNode("k"));
  options.appendChild(unit);

  QDomElement routeType = xml.createElement("routeType");
  routeType.appendChild(xml.createTextNode(comboMQPreference->itemData(comboMQPreference->currentIndex()).toString()));
  options.appendChild(routeType);

  QDomElement locale = xml.createElement("locale");
  locale.appendChild(xml.createTextNode(comboMQLanguage->itemData(comboMQLanguage->currentIndex()).toString()));
  options.appendChild(locale);

  QDomElement avoids = xml.createElement("avoids");
  if (checkMQAvoidLimAccess->isChecked()) {
    QDomElement avoid = xml.createElement("avoid");
    avoid.appendChild(xml.createTextNode("Limited Access"));
    avoids.appendChild(avoid);
  }
  if (checkMQAvoidTollRoads->isChecked()) {
    QDomElement avoid = xml.createElement("avoid");
    avoid.appendChild(xml.createTextNode("Toll road"));
    avoids.appendChild(avoid);
  }
  if (checkMQAvoidSeasonal->isChecked()) {
    QDomElement avoid = xml.createElement("avoid");
    avoid.appendChild(xml.createTextNode("Approximate Seasonal Closure"));
    avoids.appendChild(avoid);
  }
  if (checkMQAvoidUnpaved->isChecked()) {
    QDomElement avoid = xml.createElement("avoid");
    avoid.appendChild(xml.createTextNode("Unpaved"));
    avoids.appendChild(avoid);
  }
  if (checkMQAvoidFerry->isChecked()) {
    QDomElement avoid = xml.createElement("avoid");
    avoid.appendChild(xml.createTextNode("Ferry"));
    avoids.appendChild(avoid);
  }
  if (checkMQAvoidCountryBorder->isChecked()) {
    QDomElement avoid = xml.createElement("avoid");
    avoid.appendChild(xml.createTextNode("Country border crossing"));
    avoids.appendChild(avoid);
  }

  options.appendChild(avoids);

  QString xmlstr = xml.toString(0);
  qDebug() << xmlstr;
  xmlstr = xmlstr.replace("\n", "");

  QUrl url("http://open.mapquestapi.com");
  url.setPath("/directions/v2/route");

  QUrlQuery urlQuery;
  urlQuery.addQueryItem("key", keyMapQuest);
  urlQuery.addQueryItem("ambiguities", "ignore");
  urlQuery.addQueryItem("inFormat", "xml");
  urlQuery.addQueryItem("outFormat", "xml");
  urlQuery.addQueryItem("xml", QUrl::toPercentEncoding(xmlstr));
  url.setQuery(urlQuery);

  QNetworkRequest request;
  request.setUrl(url);
  QNetworkReply* reply = networkAccessManager->get(request);

  reply->setProperty("key.item", key.item);
  reply->setProperty("key.project", key.project);
  reply->setProperty("key.device", key.device);
  reply->setProperty("options", getOptions());
  reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    canvas->reportStatus("MapQuest", tr("<b>MapQuest</b><br/>Routing request sent to server. Please wait..."));
  }
}

void CRouterMapQuest::slotRequestFinished(QNetworkReply* reply) {
  if (reply->error() != QNetworkReply::NoError) {
    CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
    if (canvas) {
      canvas->reportStatus("MapQuest",
                           tr("<b>MapQuest</b><br/>Bad response from server:<br/>%1").arg(reply->errorString()));
      timerCloseStatusMsg->start();
    }
    reply->deleteLater();
    return;
  }

  QByteArray res = reply->readAll();
  reply->deleteLater();

  if (res.isEmpty()) {
    return;
  }

  QDomDocument xml;
  xml.setContent(res);

  //    QFile f("test.xml");
  //    f.open(QIODevice::WriteOnly);
  //    f.write(xml.toString().toUtf8());
  //    f.close();

  QDomElement response = xml.firstChildElement("response");
  QDomElement info = response.firstChildElement("info");
  QDomElement statusCode = info.firstChildElement("statusCode");

  if (statusCode.isNull() || statusCode.text().toInt() != 0) {
    CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
    if (canvas) {
      QDomElement messages = info.firstChildElement("messages");
      QDomElement message = messages.firstChildElement("message");
      canvas->reportStatus("MapQuest", tr("<b>MapQuest</b><br/>Bad response from server:<br/>%1").arg(message.text()));
      timerCloseStatusMsg->start();
    }
    return;
  }

  IGisItem::key_t key;
  key.item = reply->property("key.item").toString();
  key.project = reply->property("key.project").toString();
  key.device = reply->property("key.device").toString();
  qint64 time = reply->property("time").toLongLong();

  time = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - time;

  CGisItemRte* rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
  if (rte != nullptr) {
    rte->setResult(
        xml, reply->property("options").toString() + tr("<br/>Calculation time: %1s").arg(time / 1000.0, 0, 'f', 2));
  }

  slotCloseStatusMsg();
}
