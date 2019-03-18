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

#ifndef CROUTERMAPQUEST_H
#define CROUTERMAPQUEST_H

#include "gis/rte/router/IRouter.h"
#include "ui_IRouterMapQuest.h"

class QNetworkAccessManager;
class CGisItemRte;
class QNetworkReply;
class QTimer;

class CRouterMapQuest : public IRouter, private Ui::IRouterMapQuest
{
    Q_OBJECT
public:
    CRouterMapQuest(QWidget * parent);
    virtual ~CRouterMapQuest();

    void calcRoute(const IGisItem::key_t& key) override;
    int calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords) override
    {
        return -1;
    }

    QString getOptions() override;

private slots:
    void slotRequestFinished(QNetworkReply* reply);
    void slotCloseStatusMsg();

private:
    void addMapQuestLocations(QDomDocument& xml, QDomElement& locations, CGisItemRte& rte);

    static const QByteArray keyMapQuest;

    QNetworkAccessManager * networkAccessManager;

    QTimer * timerCloseStatusMsg;
};

#endif //CROUTERMAPQUEST_H

