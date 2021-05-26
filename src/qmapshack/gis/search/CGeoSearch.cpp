/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
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

#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/search/CGeoSearch.h"
#include "gis/search/CGeoSearchConfigDialog.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconManager.h"
#include "misc.h"
#include "version.h"

#include <QMutexLocker>
#include <QtNetwork>
#include <QtWidgets>
#include <QtXml>

CGeoSearch::CGeoSearch(CGisListWks* parent)
    : IGisProject(eTypeGeoSearch, "", parent)
    , searchConfig(&CGeoSearchConfig::self())
{
    networkAccessManager = new QNetworkAccessManager(this);

    QPointF focus;

    parent->takeTopLevelItem(parent->indexOfTopLevelItem(this));
    parent->insertTopLevelItem(0, this);

    edit = new QLineEdit(parent);

    actSymbol = edit->addAction(CWptIconManager::self().getWptIconByName(searchConfig->symbolName, focus), QLineEdit::TrailingPosition);
    actSymbol->setObjectName(searchConfig->symbolName);
    connect(actSymbol, &QAction::triggered, this, &CGeoSearch::slotChangeSymbol);

    QAction* actSetup = edit->addAction(QIcon("://icons/32x32/Apply.png"), QLineEdit::LeadingPosition);
    actSetup->setToolTip(tr("Setup Search"));
    connect(actSetup, &QAction::triggered, this, &CGeoSearch::slotSelectService);

    parent->setItemWidget(this, CGisListWks::eColumnName, edit);

    connect(edit, &QLineEdit::returnPressed, this, &CGeoSearch::slotStartSearch);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CGeoSearch::slotRequestFinished);
    connect(searchConfig, &CGeoSearchConfig::sigConfigChanged, this, &CGeoSearch::slotConfigChanged);

    setIcon();
}

CGeoSearch::~CGeoSearch()
{
}

void CGeoSearch::setIcon()
{
    if(searchConfig->accumulativeResults)
    {
        QPixmap displayIcon = QPixmap(48, 48);
        displayIcon.fill(Qt::transparent);
        QPainter painter(&displayIcon);
        painter.drawPixmap(0, 0, searchConfig->getCurrentIcon().pixmap(32, 32).scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        painter.drawPixmap(22, 22, QPixmap("://icons/48x48/AddGreen.png").scaled(26, 26, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        QTreeWidgetItem::setIcon(CGisListWks::eColumnDecoration, displayIcon);
    }
    else
    {
        QTreeWidgetItem::setIcon(CGisListWks::eColumnDecoration, searchConfig->getCurrentIcon());
    }
}

void CGeoSearch::slotChangeSymbol()
{
    QString iconName = CWptIconManager::self().selectWptIcon(treeWidget());
    if(!iconName.isEmpty())
    {
        QPointF focus;
        actSymbol->setObjectName(iconName);
        actSymbol->setIcon(CWptIconManager::self().getWptIconByName(iconName, focus));
    }

    searchConfig->symbolName = actSymbol->objectName();
    searchConfig->emitChanged();
}

void CGeoSearch::slotSelectService()
{
    QMenu* menu = new QMenu(edit);

    menu->addSection(tr("Select Service"));

    QActionGroup* actionGroup = new QActionGroup(menu);

    actionGroup->addAction(addService(CGeoSearchConfig::eServiceNominatim, tr("OSM Nominatim"), menu));
    actionGroup->addAction(addService(CGeoSearchConfig::eServiceGeonamesSearch, tr("Geonames Places"), menu));
    actionGroup->addAction(addService(CGeoSearchConfig::eServiceGeonamesAddress, tr("Geonames Address"), menu));
    actionGroup->addAction(addService(CGeoSearchConfig::eServiceGoogle, tr("Google"), menu));

    menu->addSeparator();
    QAction* actAccu = menu->addAction(QIcon("://icons/32x32/AddGreen.png"), tr("Accumulative Results"));
    actAccu->setCheckable(true);
    actAccu->setChecked(searchConfig->accumulativeResults);
    connect(actAccu, &QAction::triggered, this, &CGeoSearch::slotAccuResults);

    QAction* actReset = menu->addAction(QIcon("://icons/32x32/Reset.png"), tr("Reset Results"));
    actReset->setEnabled(childCount() != 0);
    connect(actReset, &QAction::triggered, this, &CGeoSearch::slotResetResults);


    menu->addSeparator();

    QAction* actSetup = menu->addAction(QIcon("://icons/32x32/Apply.png"), tr("Configure Services"));
    actSetup->setToolTip(tr("configure providers of geocoding search services"));
    connect(actSetup, &QAction::triggered, this, &CGeoSearch::slotSetupGeoSearch);

    menu->move(edit->parentWidget()->mapToGlobal(edit->geometry().topLeft()));
    menu->exec();
}

QAction* CGeoSearch::addService(CGeoSearchConfig::service_e service, const QString& name, QMenu* menu)
{
    QAction* action = menu->addAction(name);
    action->setCheckable(true);

    connect(action, &QAction::triggered, this, [this, service](bool checked){slotServiceSelected(service, checked);});
    action->setChecked(searchConfig->currentService == service);

    return action;
}

void CGeoSearch::slotServiceSelected(CGeoSearchConfig::service_e service, bool checked)
{
    if(!checked)
    {
        return;
    }
    searchConfig->currentService = service;
    searchConfig->emitChanged();
}

void CGeoSearch::slotSetupGeoSearch()
{
    CGeoSearchConfigDialog dlg(this->treeWidget(), searchConfig);
    dlg.exec();
}

void CGeoSearch::slotStartSearch()
{
    QMS_DELETE(itemStatus);

    if(!searchConfig->accumulativeResults)
    {
        qDeleteAll(takeChildren());
    }

    QString addr = edit->text();

    switch(searchConfig->currentService)
    {
    case CGeoSearchConfig::eServiceNone:
    {
        this->createErrorItem(tr("no service configured - please click setup-icon in search-field"));
        setExpanded(true);
        break;
    }

    case CGeoSearchConfig::eServiceGoogle:
    {
        requestGoogle(addr);
        edit->setEnabled(false);
        break;
    }

    case CGeoSearchConfig::eServiceGeonamesSearch:
    {
        requestGeonamesSearch(addr);
        edit->setEnabled(false);
        break;
    }

    case CGeoSearchConfig::eServiceGeonamesAddress:
    {
        requestGeonamesAddress(addr);
        edit->setEnabled(false);
        break;
    }

    case CGeoSearchConfig::eServiceNominatim:
    {
        requestNominatim(addr);
        edit->setEnabled(false);
        break;
    }

    default:
        searchConfig->currentService = CGeoSearchConfig::eServiceNone;
    }
}

void CGeoSearch::slotRequestFinished(QNetworkReply* reply)
{
    QMutexLocker lock2(&IGisItem::mutexItems);

    edit->setEnabled(true);

    if(reply->error() != QNetworkReply::NoError)
    {
        createErrorItem(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    CGeoSearchConfig::service_e service = CGeoSearchConfig::service_e(reply->property("service").toInt());

    reply->deleteLater();

    if(data.isEmpty())
    {
        createErrorItem("Response is empty");
        return;
    }

    switch(service)
    {
    case CGeoSearchConfig::eServiceGoogle:
    {
        parseGoogle(data);
        break;
    }

    case CGeoSearchConfig::eServiceGeonamesSearch:
    {
        parseGeonamesSearch(data);
        break;
    }

    case CGeoSearchConfig::eServiceGeonamesAddress:
    {
        parseGeonamesAddress(data);
        break;
    }

    case CGeoSearchConfig::eServiceNominatim:
    {
        parseNominatim(data);
        break;
    }

    default:
    {
        createErrorItem(tr("unexpected service-identifier %1").arg(service));
    }
    }

    setExpanded(true);

    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CGeoSearch::requestGoogle(QString& addr) const
{
    QUrl url("https://maps.googleapis.com");
    url.setPath("/maps/api/geocode/xml");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("address", addr.replace(" ", "+"));
    if (!searchConfig->googleApiKey.isEmpty())
    {
        urlQuery.addQueryItem("key", searchConfig->googleApiKey);
    }
    urlQuery.addQueryItem("sensor", "false");
    url.setQuery(urlQuery);

    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply* reply = networkAccessManager->get(request);
    reply->setProperty("service", QVariant(CGeoSearchConfig::eServiceGoogle));
}

void CGeoSearch::requestGeonamesSearch(QString& addr) const
{
    QUrl url("https://secure.geonames.org");
    url.setPath("/search");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("q", addr.replace(" ", "+"));
    if (!searchConfig->geonamesUsername.isEmpty())
    {
        urlQuery.addQueryItem("username", searchConfig->geonamesUsername);
    }
    url.setQuery(urlQuery);

    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply* reply = networkAccessManager->get(request);
    reply->setProperty("service", QVariant(CGeoSearchConfig::eServiceGeonamesSearch));
}

void CGeoSearch::requestGeonamesAddress(QString& addr) const
{
    QUrl url("http://api.geonames.org");
    url.setPath("/geoCodeAddress");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("q", addr.replace(" ", "+"));
    if (!searchConfig->geonamesUsername.isEmpty())
    {
        urlQuery.addQueryItem("username", searchConfig->geonamesUsername);
    }
    url.setQuery(urlQuery);

    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply* reply = networkAccessManager->get(request);
    reply->setProperty("service", QVariant(CGeoSearchConfig::eServiceGeonamesAddress));
}

void CGeoSearch::requestNominatim(QString& addr) const
{
    QString userAgent(WHAT_STR);
    userAgent.append(" (Nokia; Qt)");

    QUrl url("https://nominatim.openstreetmap.org");
    url.setPath("/search");

    QString limit;
    limit.setNum(searchConfig->nominatimLimit);

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("q", addr.replace(" ", "+"));
    urlQuery.addQueryItem("format", "xml");
    urlQuery.addQueryItem("addressdetails", "1");
    urlQuery.addQueryItem("limit", limit);
    if (!searchConfig->nominatimEmail.isEmpty())
    {
        urlQuery.addQueryItem("email", searchConfig->nominatimEmail);
    }
    url.setQuery(urlQuery);

    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    QNetworkReply* reply = networkAccessManager->get(request);
    reply->setProperty("service", QVariant(CGeoSearchConfig::eServiceNominatim));
}

void CGeoSearch::parseGoogle(const QByteArray& data)
{
    QString status;
    QDomDocument xml;

    xml.setContent(data);

    QDomElement root = xml.documentElement();

    if(root.tagName() != "GeocodeResponse")
    {
        status = tr("Unknown response");
        createErrorItem(status);
        return;
    }

    status = root.namedItem("status").toElement().text();
    if(status != "OK")
    {
        status = tr("Error: ");
        status += root.namedItem("error_message").toElement().text();
        createErrorItem(status);
        return;
    }

    {
        QDomNodeList xmlEntries = root.elementsByTagName("result");
        const qint32 N = xmlEntries.size();
        if(N)
        {
            for(int i = 0; i < N; i++)
            {
                QString address;
                QDomElement xmlEntry = xmlEntries.item(i).toElement();
                QDomElement xmlAddress = xmlEntry.namedItem("formatted_address").toElement();
                if(xmlAddress.isElement())
                {
                    address = xmlAddress.text();
                }

                QDomNode xmlGeometry = xmlEntry.namedItem("geometry");
                QDomNode xmlLocation = xmlGeometry.namedItem("location");
                qreal lon = xmlLocation.namedItem("lng").toElement().text().toDouble();
                qreal lat = xmlLocation.namedItem("lat").toElement().text().toDouble();


                new CGisItemWpt(QPointF(lon, lat), address, actSymbol->objectName(), this);
            }
        }
    }
}

void CGeoSearch::parseGeonamesSearch(const QByteArray& data)
{
    QString status;
    QDomDocument xml;

    xml.setContent(data);

    QDomElement root = xml.documentElement();

    if(root.tagName() != "geonames")
    {
        status = tr("Unknown response");
        createErrorItem(status);
        return;
    }

    QDomElement xmlStatus = root.namedItem("status").toElement();
    if (xmlStatus.isElement())
    {
        status = tr("Error: ");
        status.append(xmlStatus.attribute("message"));
        createErrorItem(status);
        return;
    }

    QDomNodeList xmlEntries = root.elementsByTagName("geoname");
    const qint32 N = xmlEntries.size();
    if(N)
    {
        for(int i = 0; i < N; i++)
        {
            QString address;
            bool isNotFirst = false;
            QDomElement xmlEntry = xmlEntries.item(i).toElement();

            if (xmlEntry.isElement())
            {
                QDomElement xmlLng = xmlEntry.namedItem("lng").toElement();
                QDomElement xmlLat = xmlEntry.namedItem("lat").toElement();

                if (xmlLng.isElement() && xmlLat.isElement())
                {
                    QDomElement xmlName = xmlEntry.namedItem("name").toElement();
                    QDomElement xmlCountryCode = xmlEntry.namedItem("countryCode").toElement();
                    QDomElement xmlCountryName = xmlEntry.namedItem("countryName").toElement();

                    if (xmlName.isElement() && !xmlName.text().isEmpty())
                    {
                        address.append(xmlName.text());
                        isNotFirst = true;
                    }
                    if (xmlCountryCode.isElement() && !xmlCountryCode.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(", ");
                        }
                        address.append(xmlCountryCode.text());
                        isNotFirst = true;
                    }
                    if (xmlCountryName.isElement() && !xmlCountryName.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(", ");
                        }
                        address.append(xmlCountryName.text());
//                        isNotFirst = true;
                    }

                    qreal lon = xmlLng.text().toDouble();
                    qreal lat = xmlLat.text().toDouble();

                    new CGisItemWpt(QPointF(lon, lat), address, actSymbol->objectName(), this);
                }
            }
        }
    }
}

void CGeoSearch::parseGeonamesAddress(const QByteArray& data)
{
    QString status;
    QDomDocument xml;

    xml.setContent(data);

    QDomElement root = xml.documentElement();

    if(root.tagName() != "geonames")
    {
        status = tr("Unknown response");
        createErrorItem(status);
        return;
    }

    QDomElement xmlStatus = root.namedItem("status").toElement();
    if (xmlStatus.isElement())
    {
        status = tr("Error: ");
        status.append(xmlStatus.attribute("message"));
        createErrorItem(status);
        return;
    }

    QDomNodeList xmlEntries = root.elementsByTagName("address");
    const qint32 N = xmlEntries.size();
    if(N)
    {
        for(int i = 0; i < N; i++)
        {
            QString address;
            bool isNotFirst = false;
            QDomElement xmlEntry = xmlEntries.item(i).toElement();

            if (xmlEntry.isElement())
            {
                QDomElement xmlLng = xmlEntry.namedItem("lng").toElement();
                QDomElement xmlLat = xmlEntry.namedItem("lat").toElement();

                if (xmlLng.isElement() && xmlLat.isElement())
                {
                    QDomElement xmlStreet = xmlEntry.namedItem("street").toElement();
                    QDomElement xmlHouseNumber = xmlEntry.namedItem("houseNumber").toElement();
                    QDomElement xmlLocality = xmlEntry.namedItem("locality").toElement();
                    QDomElement xmlPostalcode = xmlEntry.namedItem("postalcode").toElement();
                    QDomElement xmlAdminCode1 = xmlEntry.namedItem("adminCode1").toElement();
                    QDomElement xmlAdminName1 = xmlEntry.namedItem("adminName1").toElement();
                    QDomElement xmlAdminCode2 = xmlEntry.namedItem("adminCode2").toElement();
                    QDomElement xmlAdminName2 = xmlEntry.namedItem("adminName2").toElement();
                    QDomElement xmlAdminCode3 = xmlEntry.namedItem("adminCode3").toElement();
                    QDomElement xmlCountryCode = xmlEntry.namedItem("countryCode").toElement();

                    if (xmlStreet.isElement() && !xmlStreet.text().isEmpty())
                    {
                        address.append(xmlStreet.text());
                        isNotFirst = true;
                    }
                    if (xmlHouseNumber.isElement() && !xmlHouseNumber.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(" ");
                        }
                        address.append(xmlHouseNumber.text());
                        isNotFirst = true;
                    }
                    if (xmlLocality.isElement() && !xmlLocality.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(", ");
                        }
                        address.append(xmlLocality.text());
                        isNotFirst = true;
                    }
                    if (xmlPostalcode.isElement() && !xmlPostalcode.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(" ");
                        }
                        address.append(xmlPostalcode.text());
                        isNotFirst = true;
                    }
                    if (xmlAdminCode1.isElement() && !xmlAdminCode1.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(", ");
                        }
                        address.append(xmlAdminCode1.text());
                        isNotFirst = true;
                    }
                    if (xmlAdminName1.isElement() && !xmlAdminName1.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(" ");
                        }
                        address.append(xmlAdminName1.text());
                        isNotFirst = true;
                    }
                    if (xmlAdminCode2.isElement() && !xmlAdminCode2.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(", ");
                        }
                        address.append(xmlAdminCode2.text());
                        isNotFirst = true;
                    }
                    if (xmlAdminName2.isElement() && !xmlAdminName2.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(" ");
                        }
                        address.append(xmlAdminName2.text());
                        isNotFirst = true;
                    }
                    if (xmlAdminCode3.isElement() && !xmlAdminCode3.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(", ");
                        }
                        address.append(xmlAdminCode3.text());
                        isNotFirst = true;
                    }
                    if (xmlCountryCode.isElement() && !xmlCountryCode.text().isEmpty())
                    {
                        if (isNotFirst)
                        {
                            address.append(", ");
                        }
                        address.append(xmlCountryCode.text());
//                        isNotFirst = true;
                    }

                    qreal lon = xmlLng.text().toDouble();
                    qreal lat = xmlLat.text().toDouble();

                    new CGisItemWpt(QPointF(lon, lat), address, actSymbol->objectName(), this);
                }
            }
        }
    }
}

void CGeoSearch::parseNominatim(const QByteArray& data)
{
    QString status;
    QDomDocument xml;

    xml.setContent(data);

    QDomElement root = xml.documentElement();

    if(root.tagName() != "searchresults")
    {
        status = tr("Unknown response");
        createErrorItem(status);
        return;
    }

    QDomNodeList xmlEntries = root.elementsByTagName("place");
    const qint32 N = xmlEntries.size();
    if(N)
    {
        for(int i = 0; i < N; i++)
        {
            QString address;
            bool isNotFirst = false;
            bool hasPostcode = false;
            QDomElement xmlEntry = xmlEntries.item(i).toElement();

            if (xmlEntry.isElement())
            {
                qreal lon = xmlEntry.attribute("lon").toDouble();
                qreal lat = xmlEntry.attribute("lat").toDouble();

                QDomElement xmlHouseNumber = xmlEntry.namedItem("house_number").toElement();
                QDomElement xmlRoad = xmlEntry.namedItem("road").toElement();
                QDomElement xmlVillage = xmlEntry.namedItem("village").toElement();
                QDomElement xmlTown = xmlEntry.namedItem("town").toElement();
                QDomElement xmlCity = xmlEntry.namedItem("city").toElement();
                QDomElement xmlCounty = xmlEntry.namedItem("county").toElement();
                QDomElement xmlPostcode = xmlEntry.namedItem("postcode").toElement();
                QDomElement xmlCountry = xmlEntry.namedItem("country").toElement();
                QDomElement xmlCountryCode = xmlEntry.namedItem("country_code").toElement();

                if (xmlHouseNumber.isElement() && !xmlHouseNumber.text().isEmpty())
                {
                    address.append(xmlHouseNumber.text());
                    isNotFirst = true;
                }
                if (xmlRoad.isElement() && !xmlRoad.text().isEmpty())
                {
                    if (isNotFirst)
                    {
                        address.append(" ");
                    }
                    address.append(xmlRoad.text());
                    isNotFirst = true;
                }
                if (xmlPostcode.isElement() && !xmlPostcode.text().isEmpty())
                {
                    if (isNotFirst)
                    {
                        address.append(", ");
                    }
                    address.append(xmlPostcode.text());
                    isNotFirst = false;
                    hasPostcode = true;
                }
                if (xmlVillage.isElement() && !xmlVillage.text().isEmpty())
                {
                    if (hasPostcode)
                    {
                        address.append(" ");
                        hasPostcode = false;
                    }
                    else if (isNotFirst)
                    {
                        address.append(", ");
                    }
                    address.append(xmlVillage.text());
                    isNotFirst = true;
                }
                if (xmlTown.isElement() && !xmlTown.text().isEmpty())
                {
                    if (hasPostcode)
                    {
                        address.append(" ");
                        hasPostcode = false;
                    }
                    else if (isNotFirst)
                    {
                        address.append(", ");
                    }
                    address.append(xmlTown.text());
                    isNotFirst = true;
                }
                if (xmlCity.isElement() && !xmlCity.text().isEmpty())
                {
                    if (hasPostcode)
                    {
                        address.append(" ");
//                        hasPostcode = false;
                    }
                    else if (isNotFirst)
                    {
                        address.append(", ");
                    }
                    address.append(xmlCity.text());
                    isNotFirst = true;
                }
                if (xmlCounty.isElement() && !xmlCounty.text().isEmpty())
                {
                    if (isNotFirst)
                    {
                        address.append(", ");
                    }
                    address.append(xmlCounty.text());
                    isNotFirst = true;
                }
                if (xmlCountry.isElement() && !xmlCountry.text().isEmpty())
                {
                    if (isNotFirst)
                    {
                        address.append(", ");
                    }
                    address.append(xmlCountry.text());
                    isNotFirst = true;
                }
                if (xmlCountryCode.isElement() && !xmlCountryCode.text().isEmpty())
                {
                    if (isNotFirst)
                    {
                        address.append(", ");
                    }
                    address.append(xmlCountryCode.text());
//                    isNotFirst = true;
                }

                new CGisItemWpt(QPointF(lon, lat), address, actSymbol->objectName(), this);
            }
        }
    }
}

void CGeoSearch::createErrorItem(const QString& status)
{
    itemStatus = new QTreeWidgetItem(this);
    itemStatus->setText(CGisListWks::eColumnName, status);
    itemStatus->setToolTip(CGisListWks::eColumnName, status);
    itemStatus->setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/Error.png"));
}

void CGeoSearch::slotConfigChanged()
{
    QPointF focus;
    actSymbol->setIcon(CWptIconManager::self().getWptIconByName(searchConfig->symbolName, focus));
    actSymbol->setObjectName(searchConfig->symbolName);
    setIcon();
}

void CGeoSearch::slotAccuResults(bool yes)
{
    searchConfig->accumulativeResults = yes;
    setIcon();
}

void CGeoSearch::slotResetResults()
{
    qDeleteAll(takeChildren());
    updateDecoration();
}
