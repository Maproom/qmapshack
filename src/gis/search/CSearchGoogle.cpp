/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/search/CSearchGoogle.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconDialog.h"

#include <QtNetwork>
#include <QtWidgets>
#include <QtXml>

CSearchGoogle::CSearchGoogle(CGisListWks * parent)
    : IGisProject(eTypeGoogle, "", parent)
{
    QPointF focus;

    SETTINGS;
    QString symName = cfg.value("Search/symbol","Default").toString();

    parent->takeTopLevelItem(parent->indexOfTopLevelItem(this));
    parent->insertTopLevelItem(0, this);

    edit = new QLineEdit(parent);
    actSymbol = edit->addAction(getWptIconByName(symName, focus), QLineEdit::TrailingPosition);
    actSymbol->setObjectName(symName);
    connect(actSymbol, SIGNAL(triggered()), this, SLOT(slotChangeSymbol()));

    parent->setItemWidget(this, CGisListWks::eColumnName, edit);

    connect(edit, SIGNAL(returnPressed()), this, SLOT(slotStartSearch()));
    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(slotRequestFinished(QNetworkReply*)));

    setIcon(CGisListWks::eColumnDecoration, QIcon("://icons/32x32/SearchGoogle.png"));
}

CSearchGoogle::~CSearchGoogle()
{
}

void CSearchGoogle::slotChangeSymbol()
{
    CWptIconDialog dlg(actSymbol);
    dlg.exec();

    SETTINGS;
    cfg.setValue("Search/symbol", actSymbol->objectName());
}

void CSearchGoogle::slotStartSearch()
{
    qDeleteAll(takeChildren());

    QString addr = edit->text();

    QUrl url("http://maps.googleapis.com");
    url.setPath("/maps/api/geocode/xml");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("address",addr.replace(" ","+"));
    urlQuery.addQueryItem("sensor","false");
    url.setQuery(urlQuery);

    QNetworkRequest request;

    request.setUrl(url);
    networkAccessManager.get(request);

    edit->setEnabled(false);
}

void CSearchGoogle::slotRequestFinished(QNetworkReply* reply)
{
    edit->setEnabled(true);

    if(reply->error() != QNetworkReply::NoError)
    {
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    if(data.isEmpty())
    {
        return;
    }

    QString status;
    QDomDocument xml;

    xml.setContent(data);
//    qDebug() << xml.toString();

    QDomElement root = xml.documentElement();

    if(root.tagName() != "GeocodeResponse")
    {
        status = tr("Unknown response");
        QTreeWidgetItem * item = new QTreeWidgetItem(this);
        item->setText(CGisListWks::eColumnName, status);
        item->setIcon(CGisListWks::eColumnName,QIcon("://icons/32x32/Error.png"));
        return;
    }

    status = root.namedItem("status").toElement().text();
    if(status != "OK")
    {
        status  = tr("Error: ");
        status += root.namedItem("error_message").toElement().text();
        QTreeWidgetItem * item = new QTreeWidgetItem(this);
        item->setText(CGisListWks::eColumnName, status);
        item->setIcon(CGisListWks::eColumnName,QIcon("://icons/32x32/Error.png"));
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

    setExpanded(true);
    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
}
