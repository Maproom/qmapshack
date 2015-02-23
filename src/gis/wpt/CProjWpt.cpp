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
#include "GeoMath.h"
#include "gis/prj/IGisProject.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CProjWpt.h"
#include "helpers/CWptIconDialog.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <proj_api.h>

CProjWpt::CProjWpt(CGisItemWpt& wpt, QWidget *parent)
    : QDialog(parent)
    , wpt(wpt)
{
    setupUi(this);

    name = wpt.getName();

    toolIcon->setIcon(wpt.getIcon());
    toolIcon->setObjectName(wpt.getIconName());
    labelName->setText(QString("<a href='name'>%2</a>").arg(name));

    QString val, unit;
    IUnit::self().meter2distance(0,val,unit);
    labelDistUnit->setText(unit);

    connect(labelName, SIGNAL(linkActivated(QString)), this, SLOT(slotChangeName()));
    connect(toolIcon, SIGNAL(clicked()), this, SLOT(slotChangeIcon()));
}

CProjWpt::~CProjWpt()
{
}

void CProjWpt::slotChangeIcon()
{
    CWptIconDialog dlg(toolIcon);
    dlg.exec();
}

void CProjWpt::slotChangeName()
{
    QString n = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new waypoint name."), QLineEdit::Normal, wpt.getName());
    if(n.isEmpty())
    {
        return;
    }
    name = n;
    labelName->setText(QString("<a href='name'>%2</a>").arg(name));
}


void CProjWpt::accept()
{
    qreal dist = lineDist->text().toDouble();
    qreal bearing = lineBearing->text().toDouble();

    if((dist <= 0) || (bearing > 180) || (bearing < -180))
    {
        return;
    }

    IGisProject * project = dynamic_cast<IGisProject*>(wpt.parent());
    if(project == 0)
    {
        return;
    }

    QPointF pos = wpt.getPosition() * DEG_TO_RAD;
    pos = GPS_Math_Wpt_Projection(pos, dist, bearing * DEG_TO_RAD) * RAD_TO_DEG;

    CGisItemWpt * newWpt = new CGisItemWpt(pos, wpt, project);

    if(name != newWpt->getName())
    {
        newWpt->setName(name);
    }

    if(toolIcon->objectName() != newWpt->getIconName())
    {
        newWpt->setIcon(toolIcon->objectName());
    }

    QDialog::accept();
}
