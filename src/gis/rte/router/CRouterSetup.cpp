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

#include "gis/CGisWidget.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/router/CRouterMapQuest.h"
#include "gis/rte/router/CRouterRoutino.h"
#include "gis/rte/router/CRouterSetup.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CRouterSetup * CRouterSetup::pSelf = 0;

CRouterSetup::CRouterSetup(QWidget * parent)
    : QWidget(parent)
{
    setupUi(this);
    pSelf = this;

    comboRouter->addItem(tr("Routino (offline)"));
    comboRouter->addItem(tr("MapQuest (online)"));

    stackedWidget->addWidget(new CRouterRoutino(this));
    stackedWidget->addWidget(new CRouterMapQuest(this));

    connect(comboRouter, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSelectRouter(int)));

    SETTINGS;
    comboRouter->setCurrentIndex(cfg.value("Route/current",0).toInt());
}

CRouterSetup::~CRouterSetup()
{
    SETTINGS;
    cfg.setValue("Route/current", comboRouter->currentIndex());
}

bool CRouterSetup::hasFastRouting()
{
    IRouter * router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
    if(router)
    {
        return router->hasFastRouting();
    }
    return false;
}

void CRouterSetup::slotSelectRouter(int i)
{
    stackedWidget->setCurrentIndex(i);
}

void CRouterSetup::calcRoute(const IGisItem::key_t& key)
{
    IRouter * router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
    if(router)
    {
        router->calcRoute(key);
    }
}

int CRouterSetup::calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords)
{
    IRouter * router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
    if(router)
    {
        return router->calcRoute(p1, p2, coords);
    }

    return false;
}
