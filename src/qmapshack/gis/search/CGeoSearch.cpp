/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisListWks.h"
#include "gis/WptIcons.h"
#include "gis/search/CGeoSearch.h"
#include "gis/search/CGeoSearchConfig.h"
#include "gis/search/CGeoSearchConfigDialog.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconDialog.h"
#include "version.h"

#include <QMutexLocker>
#include <QtNetwork>
#include <QtWidgets>
#include <QtXml>

CGeoSearch::CGeoSearch(CGisListWks * parent, CGeoSearchConfig* config)
    : IGisProject(eTypeGeoSearch, "", parent)
{
    searchConfig = config;

    QPointF focus;

    parent->takeTopLevelItem(parent->indexOfTopLevelItem(this));
    parent->insertTopLevelItem(0, this);

    edit = new QLineEdit(parent);
    actSymbol = edit->addAction(getWptIconByName(config->symbolName, focus), QLineEdit::TrailingPosition);
    actSymbol->setObjectName(config->symbolName);
    connect(actSymbol, &QAction::triggered, this, &CGeoSearch::slotChangeSymbol);
    QAction* actSetup = edit->addAction(QIcon("://icons/32x32/Apply.png"), QLineEdit::LeadingPosition);
    actSetup->setToolTip(tr("Setup Geosearch"));
    connect(actSetup, &QAction::triggered, this, &CGeoSearch::slotSelectService);

    parent->setItemWidget(this, CGisListWks::eColumnName, edit);

    connect(edit, &QLineEdit::returnPressed, this, &CGeoSearch::slotStartSearch);
    connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &CGeoSearch::slotRequestFinished);
    connect(searchConfig, &CGeoSearchConfig::sigConfigChanged, this, &CGeoSearch::slotConfigChanged);

    setIcon(CGisListWks::eColumnDecoration, searchConfig->getCurrentIcon());
}

CGeoSearch::~CGeoSearch()
{
}

void CGeoSearch::slotChangeSymbol()
{
    CWptIconDialog dlg(actSymbol);
    dlg.exec();
    searchConfig->symbolName = actSymbol->objectName();
    searchConfig->emitChanged();
}

void CGeoSearch::slotSelectService()
{
    QMenu * menu = new QMenu(edit);

    menu->addSection(tr("select Geosearch service"));

    QAction* actNominatim = menu->addAction(tr("OSM Nominatim"));
    actNominatim->setEnabled(!searchConfig->nominatimEmail.isEmpty());
    actNominatim->setCheckable(true);
    actNominatim->setChecked(searchConfig->currentService == CGeoSearchConfig::eNominatim);
    connect(actNominatim, &QAction::triggered, this, &CGeoSearch::slotNominatimSelected);

    QAction* actGeoSearch = menu->addAction(tr("Geonames Places search"));
    actGeoSearch->setEnabled(!searchConfig->geonamesUsername.isEmpty());
    actGeoSearch->setCheckable(true);
    actGeoSearch->setChecked(searchConfig->currentService == CGeoSearchConfig::eGeonamesSearch);
    connect(actGeoSearch, &QAction::triggered, this, &CGeoSearch::slotGeonamesSearchSelected);

    QAction* actGeoAddress = menu->addAction(tr("Geonames Address search"));
    actGeoAddress->setEnabled(!searchConfig->geonamesUsername.isEmpty());
    actGeoAddress->setCheckable(true);
    actGeoAddress->setChecked(searchConfig->currentService == CGeoSearchConfig::eGeonamesAddress);
    connect(actGeoAddress, &QAction::triggered, this, &CGeoSearch::slotGeonamesAddressSelected);

    QAction* actGoogle = menu->addAction(tr("Google Geocoding API"));
    actGoogle->setEnabled(!searchConfig->googleApiKey.isEmpty());
    actGoogle->setCheckable(true);
    actGoogle->setChecked(searchConfig->currentService == CGeoSearchConfig::eGoogle);
    connect(actGoogle, &QAction::triggered, this, &CGeoSearch::slotGoogleSelected);

    menu->addSeparator();
    QAction* actSetup = menu->addAction(QIcon("://icons/32x32/Apply.png"),tr("Configure Services"));
    actSetup->setToolTip(tr("configure providers of geocoding search services"));

    connect(actSetup, &QAction::triggered, this, &CGeoSearch::slotSetupGeoSearch);

    menu->move(edit->geometry().topLeft());
    menu->exec();

    return;
}

void CGeoSearch::slotNominatimSelected()
{
    searchConfig->currentService = CGeoSearchConfig::eNominatim;
    searchConfig->emitChanged();
}

void CGeoSearch::slotGeonamesSearchSelected()
{
    searchConfig->currentService = CGeoSearchConfig::eGeonamesSearch;
    searchConfig->emitChanged();
}

void CGeoSearch::slotGeonamesAddressSelected()
{
    searchConfig->currentService = CGeoSearchConfig::eGeonamesAddress;
    searchConfig->emitChanged();
}

void CGeoSearch::slotGoogleSelected()
{
    searchConfig->currentService = CGeoSearchConfig::eGoogle;
    searchConfig->emitChanged();
}

void CGeoSearch::slotSetupGeoSearch()
{
    CGeoSearchConfigDialog dlg(this->treeWidget(),searchConfig);
    dlg.exec();
}

void CGeoSearch::slotStartSearch()
{
    qDeleteAll(takeChildren());

    QString addr = edit->text();

    QString userAgent(WHAT_STR);
    userAgent.append(" (Nokia; Qt)");

    switch(searchConfig->currentService)
    {
    case CGeoSearchConfig::eNone:
    {
        this->createErrorItem(tr("no service configured - please click setup-icon in search-field"));
        setExpanded(true);
        break;
    }
    case CGeoSearchConfig::eGoogle:
    {
        QUrl url("https://maps.googleapis.com");
        url.setPath("/maps/api/geocode/xml");

        QUrlQuery urlQuery;
        urlQuery.addQueryItem("address",addr.replace(" ","+"));
        if (!searchConfig->googleApiKey.isEmpty())
        {
            urlQuery.addQueryItem("key",searchConfig->googleApiKey);
        }
        urlQuery.addQueryItem("sensor","false");
        url.setQuery(urlQuery);

        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
        QNetworkReply* reply = networkAccessManager.get(request);
        reply->setProperty("service",QVariant(CGeoSearchConfig::eGoogle));
        edit->setEnabled(false);
        break;
    }
    case CGeoSearchConfig::eGeonamesSearch:
    {
        QUrl url("https://secure.geonames.org");
        url.setPath("/search");

        QUrlQuery urlQuery;
        urlQuery.addQueryItem("q",addr.replace(" ","+"));
        if (!searchConfig->geonamesUsername.isEmpty())
        {
            urlQuery.addQueryItem("username",searchConfig->geonamesUsername);
        }
        url.setQuery(urlQuery);

        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
        QNetworkReply* reply = networkAccessManager.get(request);
        reply->setProperty("service",QVariant(CGeoSearchConfig::eGeonamesSearch));
        edit->setEnabled(false);
        break;
    }
    case CGeoSearchConfig::eGeonamesAddress:
    {
        QUrl url("http://api.geonames.org");
        url.setPath("/geoCodeAddress");

        QUrlQuery urlQuery;
        urlQuery.addQueryItem("q",addr.replace(" ","+"));
        if (!searchConfig->geonamesUsername.isEmpty())
        {
            urlQuery.addQueryItem("username",searchConfig->geonamesUsername);
        }
        url.setQuery(urlQuery);

        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
        QNetworkReply* reply = networkAccessManager.get(request);
        reply->setProperty("service",QVariant(CGeoSearchConfig::eGeonamesAddress));
        edit->setEnabled(false);
        break;
    }
    case CGeoSearchConfig::eNominatim:
    {
        QUrl url("https://nominatim.openstreetmap.org");
        url.setPath("/search");

        QUrlQuery urlQuery;
        urlQuery.addQueryItem("q",addr.replace(" ","+"));
        urlQuery.addQueryItem("format","xml");
        urlQuery.addQueryItem("addressdetails","1");
        if (!searchConfig->nominatimEmail.isEmpty())
        {
            urlQuery.addQueryItem("email", searchConfig->nominatimEmail);
        }
        url.setQuery(urlQuery);

        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
        QNetworkReply* reply = networkAccessManager.get(request);
        reply->setProperty("service",QVariant(CGeoSearchConfig::eNominatim));
        edit->setEnabled(false);
        break;
    }
    default:
        searchConfig->currentService = CGeoSearchConfig::eNone;
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
    reply->deleteLater();

    if(data.isEmpty())
    {
        createErrorItem("Response is empty");
        return;
    }

    CGeoSearchConfig::search_service_e service = CGeoSearchConfig::search_service_e(reply->property("service").toInt());
    switch(service)
    {
    case CGeoSearchConfig::eNone:
        break;
    case CGeoSearchConfig::eGoogle:
    {
        QString status;
        QDomDocument xml;

        xml.setContent(data);
    //    qDebug() << xml.toString();

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
            status  = tr("Error: ");
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
                    QDomElement xmlEntry   = xmlEntries.item(i).toElement();
                    QDomElement xmlAddress = xmlEntry.namedItem("formatted_address").toElement();
                    if(xmlAddress.isElement())
                    {
                        address = xmlAddress.text();
                    }

                    QDomNode xmlGeometry = xmlEntry.namedItem("geometry");
                    QDomNode xmlLocation = xmlGeometry.namedItem("location");
                    qreal lon = xmlLocation.namedItem("lng").toElement().text().toDouble();
                    qreal lat = xmlLocation.namedItem("lat").toElement().text().toDouble();


                    new CGisItemWpt(QPointF(lon,lat), address, actSymbol->objectName(), this);
                }
            }
        }
        break;
    }
    case CGeoSearchConfig::eGeonamesSearch:
    case CGeoSearchConfig::eGeonamesAddress:
    {
        const bool isAddress = (service == CGeoSearchConfig::eGeonamesAddress);
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
            status  = tr("Error: ");
            status.append(xmlStatus.attribute("message"));
            createErrorItem(status);
            return;
        }

        QDomNodeList xmlEntries = root.elementsByTagName(isAddress ? "address" : "geoname");
        const qint32 N = xmlEntries.size();
        if(N)
        {
            for(int i = 0; i < N; i++)
            {
                QString address;
                bool isNotFirst = false;
                QDomElement xmlEntry   = xmlEntries.item(i).toElement();

                if (xmlEntry.isElement())
                {
                    QDomElement xmlLng = xmlEntry.namedItem("lng").toElement();
                    QDomElement xmlLat = xmlEntry.namedItem("lat").toElement();

                    if (xmlLng.isElement() && xmlLat.isElement())
                    {
                        if (isAddress)
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
                                isNotFirst = true;
                            }
                        }
                        else // CSearchConfig::eGeoNamesSearch
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
                                isNotFirst = true;
                            }
                        }

                        qreal lon = xmlLng.text().toDouble();
                        qreal lat = xmlLat.text().toDouble();

                        new CGisItemWpt(QPointF(lon,lat), address, actSymbol->objectName(), this);
                    }
                }
            }
        }
        break;
    }
    case CGeoSearchConfig::eNominatim:
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
                QDomElement xmlEntry   = xmlEntries.item(i).toElement();

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
                            hasPostcode = false;
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
                        isNotFirst = true;
                    }

                    new CGisItemWpt(QPointF(lon,lat), address, actSymbol->objectName(), this);
                }
            }
        }
        break;
    }
    default:
        searchConfig->currentService = CGeoSearchConfig::eNone;
    }

    setExpanded(true);

    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CGeoSearch::createErrorItem(const QString& status)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(this);
    item->setText(CGisListWks::eColumnName, status);
    item->setToolTip(CGisListWks::eColumnName, status);
    item->setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/Error.png"));
    return;
}

void CGeoSearch::slotConfigChanged()
{
    QPointF focus;
    actSymbol->setIcon(getWptIconByName(searchConfig->symbolName, focus));
    actSymbol->setObjectName(searchConfig->symbolName);
    setIcon(CGisListWks::eColumnDecoration, searchConfig->getCurrentIcon());
}
