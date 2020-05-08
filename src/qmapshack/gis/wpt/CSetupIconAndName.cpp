/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/wpt/CSetupIconAndName.h"
#include "helpers/CWptIconManager.h"

#include <QtWidgets>

CSetupIconAndName::CSetupIconAndName(QString &icon, QString &name, QWidget *parent)
    : QDialog(parent)
    , icon(icon)
    , name(name)

{
    setupUi(this);
    toolIcon->setObjectName(icon);
    QPointF focus;
    toolIcon->setIcon(CWptIconManager::self().getWptIconByName(icon, focus));
    lineName->setText(name);

    connect(lineName, &QLineEdit::textEdited, this, &CSetupIconAndName::slotEditName);
    connect(toolIcon, &QToolButton::clicked,  this, &CSetupIconAndName::slotChangeIcon);

    checkInput();
}

void CSetupIconAndName::slotEditName(const QString& str)
{
    checkInput();
}

void CSetupIconAndName::slotChangeIcon()
{
    QString iconName = CWptIconManager::self().selectWptIcon(this);
    if(!iconName.isEmpty())
    {
        QPointF focus;
        toolIcon->setObjectName(iconName);
        toolIcon->setIcon(CWptIconManager::self().getWptIconByName(iconName, focus));
    }
}

void CSetupIconAndName::checkInput()
{
    bool isEnabled = !lineName->text().isEmpty();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabled);
}

void CSetupIconAndName::accept()
{
    icon = toolIcon->objectName();
    name = lineName->text();

    QDialog::accept();
}

void CSetupIconAndName::reject()
{
    name.clear();
    icon.clear();

    QDialog::reject();
}


