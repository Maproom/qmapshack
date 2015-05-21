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

#include "gis/rte/router/CRouterMapQuest.h"
#include "gis/rte/router/CRouterRoutino.h"
#include "gis/rte/router/CRouterSetup.h"

#include <QtWidgets>

CRouterSetup::CRouterSetup(QWidget * parent)
    : QWidget(parent)
{
    setupUi(this);

    comboRouter->addItem(tr("Routino (offline)"));
    comboRouter->addItem(tr("MapQuest (online)"));

    stackedWidget->addWidget(new CRouterRoutino(this));
    stackedWidget->addWidget(new CRouterMapQuest(this));

    connect(comboRouter, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSelectRouter(int)));
}

CRouterSetup::~CRouterSetup()
{
}

void CRouterSetup::slotSelectRouter(int i)
{
    stackedWidget->setCurrentIndex(i);
}
