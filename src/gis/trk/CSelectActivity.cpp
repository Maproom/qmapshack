/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CSelectActivity.h"

#include <QtWidgets>

CSelectActivity::CSelectActivity(quint32 &flag, QString &name, QString &icon, QWidget * parent)
    : QDialog(parent)
    , flag(flag)
    , name(name)
    , icon(icon)
{
    setupUi(this);

    checkActivityNone->setProperty("flag", CGisItemTrk::trkpt_t::eActNone);
    checkActivityNone->setProperty("name", checkActivityNone->text());
    checkActivityNone->setProperty("symbol", QString("://icons/48x48/ActNone.png"));

    checkActivityFoot->setProperty("flag", CGisItemTrk::trkpt_t::eActFoot);
    checkActivityFoot->setProperty("name", checkActivityFoot->text());
    checkActivityFoot->setProperty("symbol", QString("://icons/48x48/ActFoot.png"));

    checkActivityCycle->setProperty("flag", CGisItemTrk::trkpt_t::eActCycle);
    checkActivityCycle->setProperty("name", checkActivityCycle->text());
    checkActivityCycle->setProperty("symbol", QString("://icons/48x48/ActCycle.png"));

    checkActivityBike->setProperty("flag", CGisItemTrk::trkpt_t::eActBike);
    checkActivityBike->setProperty("name", checkActivityBike->text());
    checkActivityBike->setProperty("symbol", QString("://icons/48x48/ActBike.png"));

    checkActivityCar->setProperty("flag", CGisItemTrk::trkpt_t::eActCar);
    checkActivityCar->setProperty("name", checkActivityCar->text());
    checkActivityCar->setProperty("symbol", QString("://icons/48x48/ActCar.png"));

    checkActivityCable->setProperty("flag", CGisItemTrk::trkpt_t::eActCable);
    checkActivityCable->setProperty("name", checkActivityCable->text());
    checkActivityCable->setProperty("symbol", QString("://icons/48x48/ActCable.png"));

    checkActivityShip->setProperty("flag", CGisItemTrk::trkpt_t::eActShip);
    checkActivityShip->setProperty("name", checkActivityShip->text());
    checkActivityShip->setProperty("symbol", QString("://icons/48x48/ActShip.png"));

    connect(checkActivityNone, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));
    connect(checkActivityFoot, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));
    connect(checkActivityCycle, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));
    connect(checkActivityBike, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));
    connect(checkActivityCar, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));
    connect(checkActivityCable, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));
    connect(checkActivityShip, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));

}

CSelectActivity::~CSelectActivity()
{
}

void CSelectActivity::slotActivitySelected(bool)
{
    QObject * s = sender();
    bool ok = false;
    flag = s->property("flag").toUInt(&ok);
    if(ok)
    {
        name = s->property("name").toString();
        icon = s->property("symbol").toString();
    }
    QDialog::accept();
}
