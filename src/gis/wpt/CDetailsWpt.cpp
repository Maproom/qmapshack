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
#include "helpers/CInputDialog.h"
#include "helpers/CPositionDialog.h"
#include "helpers/CWptIconDialog.h"


#include <QtWidgets>

CDetailsWpt::CDetailsWpt(CGisItemWpt &wpt, QWidget *parent)
    : QDialog(parent)
    , wpt(wpt)
{
    setupUi(this);
    setupGui();
    connect(labelName, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(labelPositon, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(labelElevation, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(labelProximity, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(toolIcon, SIGNAL(clicked()), this, SLOT(slotChangeIcon()));
}

CDetailsWpt::~CDetailsWpt()
{

}

void CDetailsWpt::setupGui()
{
    setWindowTitle(wpt.getName());

    QString val, unit;
    QString strPos;
    QPointF pos = wpt.getPosition();
    GPS_Math_Deg_To_Str(pos.x(), pos.y(), strPos);

    toolIcon->setIcon(wpt.getIcon());
    toolIcon->setObjectName(wpt.getIconName());
    labelName->setText(QString("<a href='name'>%1</a>").arg(wpt.getName()));
    labelPositon->setText(QString("<a href='position'>%1</a>").arg(strPos));

    if(wpt.getElevation() != NOINT)
    {
        IUnit::self().meter2elevation(wpt.getElevation(), val, unit);
        labelElevation->setText(QString("<a href='elevation'>%1</a> %2").arg(val).arg(unit));
    }
    else
    {
        labelElevation->setText(QString("<a href='elevation'>--</a>"));
    }

    if(wpt.getProximity() != NOFLOAT)
    {
        IUnit::self().meter2elevation(wpt.getProximity(), val, unit);
        labelProximity->setText(QString("<a href='proximity'>%1</a> %2").arg(val).arg(unit));
    }
    else
    {
        labelProximity->setText(QString("<a href='proximity'>--</a>"));
    }

    if(wpt.getComment().isEmpty())
    {
        labelCmt->setText(tr("no comment"));
    }
    else
    {
        labelCmt->setText(wpt.getComment());
    }

    if(wpt.getDescription().isEmpty())
    {
        labelDesc->setText(tr("no description"));
    }
    else
    {
        labelDesc->setText(wpt.getDescription());
    }

}

void CDetailsWpt::slotLinkActivated(const QString& link)
{
    if(link == "name")
    {
        QString name = QInputDialog::getText(0, tr("Edit name..."), tr("Enter new waypoint name."), QLineEdit::Normal, wpt.getName());
        if(name.isEmpty())
        {
            return;
        }
        wpt.setName(name);
    }
    else if(link == "elevation")
    {
        QVariant var(wpt.getElevation());
        CInputDialog dlg(0, tr("Enter new elevation."), var, QVariant(NOINT));
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setElevation(var.toInt());
        }
    }
    else if(link == "proximity")
    {
        QVariant var(wpt.getProximity());
        CInputDialog dlg(0, tr("Enter new proximity range."), var, QVariant(NOFLOAT));
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setProximity(var.toDouble());
        }
    }
    else if(link == "position")
    {
        QPointF pos = wpt.getPosition();
        CPositionDialog dlg(0, pos);
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setPosition(pos);
        }
    }

    setupGui();
}

void CDetailsWpt::slotChangeIcon()
{

    CWptIconDialog dlg(toolIcon);
    if(dlg.exec() == QDialog::Accepted)
    {
        wpt.setIcon(toolIcon->objectName());
        setupGui();
    }
}
