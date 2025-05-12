/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/rte/router/CRouterSetup.h"

#include <QtWidgets>

#include "gis/rte/router/CRouterBRouter.h"
#include "gis/rte/router/CRouterRoutino.h"
#include "helpers/CSettings.h"

CRouterSetup* CRouterSetup::pSelf = nullptr;

CRouterSetup::CRouterSetup(QWidget* parent) : QWidget(parent) {
  setupUi(this);
  pSelf = this;

  comboRouter->addItem(tr("Routino (offline)"));
  comboRouter->addItem(tr("BRouter (online)"));

  stackedWidget->addWidget(new CRouterRoutino(this));
  stackedWidget->addWidget(new CRouterBRouter(this));

  connect(comboRouter, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &CRouterSetup::slotSelectRouter);

  SETTINGS;
  comboRouter->setCurrentIndex(cfg.value("Route/current", 0).toInt());
}

CRouterSetup::~CRouterSetup() {
  SETTINGS;
  cfg.setValue("Route/current", comboRouter->currentIndex());
}

bool CRouterSetup::hasFastRouting() {
  IRouter* router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
  if (router) {
    return router->hasFastRouting();
  }
  return false;
}

void CRouterSetup::slotSelectRouter(int i) {
  stackedWidget->setCurrentIndex(i);
  IRouter* router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
  if (router != nullptr) {
    router->routerSelected();
  }
}

void CRouterSetup::calcRoute(const IGisItem::key_t& key) {
  IRouter* router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
  if (router) {
    router->calcRoute(key);
  }
}

int CRouterSetup::calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords, qreal* costs) {
  IRouter* router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
  if (router) {
    return router->calcRoute(p1, p2, coords, costs);
  }

  return false;
}

QString CRouterSetup::getOptions() {
  IRouter* router = dynamic_cast<IRouter*>(stackedWidget->currentWidget());
  if (router) {
    return router->getOptions();
  }

  return "";
}

void CRouterSetup::setRouterTitle(const router_e router, const QString title) {
  comboRouter->setItemText(router, title);
}
