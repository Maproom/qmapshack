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
#include "helpers/CElevationDialog.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <proj_api.h>

CElevationDialog::CElevationDialog(QWidget * parent, QVariant &val, const QVariant &reset, const QPointF &pos)
    : QDialog(parent)
    , val(val)
    , reset(reset)
    , pos(pos)
{
    setupUi(this);

    QPushButton * pushReset = buttonBox->addButton(QDialogButtonBox::Reset);
    connect(pushReset,  &QPushButton::clicked, this, &CElevationDialog::slotReset);
    connect(toolGetEle, &QToolButton::clicked, this, &CElevationDialog::slotGetEle);

    QString str, unit;
    IUnit::self().meter2elevation(100, str, unit);

    labelUnit->setText(unit);
    if(val != NOINT)
    {
        IUnit::self().meter2elevation(val.toDouble(), str, unit);
        lineValue->setText(str);
    }
}

CElevationDialog::~CElevationDialog()
{
}

void CElevationDialog::accept()
{
    if(lineValue->text().isEmpty())
    {
        val = reset;
    }
    else
    {
        val.setValue(lineValue->text().toDouble() / IUnit::self().basefactor);
    }

    QDialog::accept();
}

void CElevationDialog::slotReset()
{
    if(reset == NOINT)
    {
        lineValue->clear();
    }
    else
    {
        QString str, unit;
        IUnit::self().meter2elevation(val.toDouble(), str, unit);
        lineValue->setText(str);
    }
}

void CElevationDialog::slotGetEle()
{
    QVariant ele = CMainWindow::self().getElevationAt(pos * DEG_TO_RAD);
    if(ele != NOFLOAT)
    {
        QString str, unit;
        IUnit::self().meter2elevation(ele.toDouble(), str, unit);
        lineValue->setText(str);
    }
    else
    {
        labelMessage->setText(tr("No DEM data found for that point."));
    }
}
