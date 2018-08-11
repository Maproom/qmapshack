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

#include "gis/wpt/CSetupNewWpt.h"
#include "gis/WptIcons.h"
#include "helpers/CPositionDialog.h"
#include "helpers/CWptIconDialog.h"
#include "units/IUnit.h"

#include <QtWidgets>


CSetupNewWpt::CSetupNewWpt(QPointF &pt, QString &icon, QString &name, QWidget *parent)
    : QDialog(parent)
    , pt(pt)
    , icon(icon)
    , name(name)
{
    QPointF focus;

    setupUi(this);
    toolIcon->setObjectName(icon);
    toolIcon->setIcon(getWptIconByName(icon, focus));

    if(pt != NOPOINTF)
    {
        QString pos;
        IUnit::degToStr(pt.x(), pt.y(), pos);
        linePosition->setText(pos);
    }
    else
    {
        linePosition->setDisabled(true);
    }

    labelWarning->hide();
    lineName->setText(name);

    connect(linePosition, &QLineEdit::textEdited, this, &CSetupNewWpt::slotEditPosition);
    connect(lineName,     &QLineEdit::textEdited, this, &CSetupNewWpt::slotEditName);
    connect(toolIcon,     &QToolButton::clicked,  this, &CSetupNewWpt::slotChangeIcon);

    checkInput();
}

CSetupNewWpt::~CSetupNewWpt()
{
}


void CSetupNewWpt::accept()
{
    if((pt == NOPOINTF) || CPositionDialog::getPosition(pt, linePosition->text()))
    {
        icon = toolIcon->objectName();
        name = lineName->text();

        QDialog::accept();
    }
}

void CSetupNewWpt::reject()
{
    pt = NOPOINTF;
    name.clear();
    icon.clear();

    QDialog::reject();
}

void CSetupNewWpt::slotEditPosition(const QString& str)
{
    labelWarning->setVisible(!IUnit::isValidCoordString(str));
    checkInput();
}

void CSetupNewWpt::slotEditName(const QString& str)
{
    checkInput();
}

void CSetupNewWpt::slotChangeIcon()
{
    CWptIconDialog dlg(toolIcon);
    dlg.exec();
}


void CSetupNewWpt::checkInput()
{
    bool isEnabled = (labelWarning->isHidden() && !lineName->text().isEmpty());
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabled);
}
