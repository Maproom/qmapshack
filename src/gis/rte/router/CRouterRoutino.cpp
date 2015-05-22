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

#include "gis/rte/router/CRouterRoutino.h"
#include <QtWidgets>


CRouterRoutino::CRouterRoutino(QWidget *parent)
    : IRouter(parent)
{
    setupUi(this);

    comboProfile->addItem(tr("Foot"));
    comboProfile->addItem(tr("Horse"));
    comboProfile->addItem(tr("Wheelchair"));
    comboProfile->addItem(tr("Bicycle"));
    comboProfile->addItem(tr("Moped"));
    comboProfile->addItem(tr("Motorcycle"));
    comboProfile->addItem(tr("Motorcar"));
    comboProfile->addItem(tr("Goods"));

    comboMode->addItem(tr("Shortest"));
    comboMode->addItem(tr("Quickest"));

    QFile _profiles("://xml/routino/routino-profiles.xml");
    _profiles.open(QIODevice::ReadOnly);
    QTemporaryFile profiles;
    profiles.open();
    profiles.write(_profiles.readAll());
    profiles.close();

    QFile _translations("://xml/routino/routino-translations.xml");
    _translations.open(QIODevice::ReadOnly);
    QTemporaryFile translations;
    translations.open();
    translations.write(_translations.readAll());
    translations.close();

    RoutinoInit(profiles.fileName().toUtf8(), translations.fileName().toUtf8());
}

CRouterRoutino::~CRouterRoutino()
{
    RoutinoRelease();
}


