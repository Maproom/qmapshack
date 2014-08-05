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

#include "gis/wpt/CDetailsWpt.h"
#include "gis/wpt/CGisItemWpt.h"
#include "GeoMath.h"
#include "units/IUnit.h"


#include <QtWidgets>

CDetailsWpt::CDetailsWpt(CGisItemWpt &wpt, QWidget *parent)
    : QDialog(parent)
    , wpt(wpt)
{
    setupUi(this);
    setWindowTitle(wpt.getName());

    QString val, unit;
    QString strPos;
    QPointF pos = wpt.getPosition();
    GPS_Math_Deg_To_Str(pos.x(), pos.y(), strPos);

    toolIcon->setIcon(wpt.getIcon());
    labelName->setText(QString("<a href='name'>%1</a>").arg(wpt.getName()));
    labelPositon->setText(QString("<a href='position'>%1</a>").arg(strPos));

    if(wpt.wpt.ele != NOINT)
    {
        IUnit::self().meter2elevation(wpt.wpt.ele, val, unit);
        labelElevation->setText(QString("<a href='elevation'>%1</a> %2").arg(val).arg(unit));
    }
    else
    {
        labelElevation->setText(QString("<a href='elevation'>-</a>"));
    }

    if(wpt.proximity != NOFLOAT)
    {
        IUnit::self().meter2elevation(wpt.proximity, val, unit);
        labelProximity->setText(QString("<a href='proximity'>%1</a> %2").arg(val).arg(unit));
    }
    else
    {
        labelProximity->setText(QString("<a href='proximity'>-</a>"));
    }

    labelCmt->setText(wpt.wpt.cmt);
    labelDesc->setText(wpt.wpt.desc);
}

CDetailsWpt::~CDetailsWpt()
{

}

