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
#include "helpers/CTextEditWidget.h"


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
    connect(toolEditCmt, SIGNAL(clicked()), this, SLOT(slotChangeCmt()));
    connect(toolEditDesc, SIGNAL(clicked()), this, SLOT(slotChangeDesc()));
    connect(toolLock, SIGNAL(toggled(bool)), this, SLOT(slotChangeReadOnlyMode(bool)));
}

CDetailsWpt::~CDetailsWpt()
{

}

QString toLink(bool isReadOnly, const QString& href, const QString& str)
{
    if(isReadOnly)
    {
        return QString("%1").arg(str);
    }

    return QString("<a href='%1'>%2</a>").arg(href).arg(str);
}

void CDetailsWpt::setupGui()
{
    setWindowTitle(wpt.getName());

    QString val, unit;
    QString strPos;
    QPointF pos = wpt.getPosition();
    GPS_Math_Deg_To_Str(pos.x(), pos.y(), strPos);

    bool isReadOnly = wpt.isReadOnly();

    toolIcon->setIcon(wpt.getIcon());
    toolIcon->setObjectName(wpt.getIconName());
    labelName->setText(toLink(isReadOnly, "name", wpt.getName()));
    labelPositon->setText(toLink(isReadOnly, "position", strPos));

    if(wpt.getElevation() != NOINT)
    {
        IUnit::self().meter2elevation(wpt.getElevation(), val, unit);
        labelElevation->setText(toLink(isReadOnly, "elevation", QString("%1 %2").arg(val).arg(unit)));
    }
    else
    {
        labelElevation->setText(toLink(isReadOnly, "elevation", "--"));
    }

    if(wpt.getProximity() != NOFLOAT)
    {
        IUnit::self().meter2elevation(wpt.getProximity(), val, unit);
        labelProximity->setText(toLink(isReadOnly, "proximity", QString("%1 %2").arg(val).arg(unit)));
    }
    else
    {
        labelProximity->setText(toLink(isReadOnly, "proximity", "--"));
    }

    if(IGisItem::removeHtml(wpt.getComment()).simplified().isEmpty())
    {
        labelCmt->setText(tr("no comment"));
    }
    else
    {
        labelCmt->setText(wpt.getComment());
    }

    if(IGisItem::removeHtml(wpt.getDescription()).simplified().isEmpty())
    {
        labelDesc->setText(tr("no description"));
    }
    else
    {
        labelDesc->setText(wpt.getDescription());
    }

    if(isReadOnly)
    {
        toolEditCmt->hide();
        toolEditDesc->hide();
    }
    else
    {
        toolEditCmt->show();
        toolEditDesc->show();
    }

    toolLock->setChecked(isReadOnly);
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

    if(wpt.isReadOnly())
    {
        return;
    }

    CWptIconDialog dlg(toolIcon);
    if(dlg.exec() == QDialog::Accepted)
    {
        wpt.setIcon(toolIcon->objectName());
        setupGui();
    }
}

void CDetailsWpt::slotChangeCmt()
{
    CTextEditWidget dlg(0);
    dlg.setHtml(wpt.getComment());
    if(dlg.exec() == QDialog::Accepted)
    {
        wpt.setComment(dlg.getHtml());
        setupGui();
    }
}

void CDetailsWpt::slotChangeDesc()
{
    CTextEditWidget dlg(0);
    dlg.setHtml(wpt.getDescription());
    if(dlg.exec() == QDialog::Accepted)
    {
        wpt.setDescription(dlg.getHtml());
        setupGui();
    }
}

void CDetailsWpt::slotChangeReadOnlyMode(bool on)
{
    wpt.setReadOnlyMode(on);
    setupGui();
}
