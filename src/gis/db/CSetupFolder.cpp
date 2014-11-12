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

#include "gis/db/CSetupFolder.h"

#include <QtWidgets>

CSetupFolder::CSetupFolder(IDBFolder::type_e& type, QString &name, QWidget *parent)
    : QDialog(parent)
    , type(type)
    , name(name)
{
    setupUi(this);

    connect(lineName, SIGNAL(textChanged(QString)), this, SLOT(slotNameChanged(QString)));

    lineName->setText(name);
    switch(type)
    {
        case IDBFolder::eTypeGroup:
            radioGroup->setChecked(true);
            break;
        case IDBFolder::eTypeProject:
            radioProject->setChecked(true);
            break;
        case IDBFolder::eTypeOther:
            radioOther->setChecked(true);
            break;
        default:;
    }

    slotNameChanged(name);
}

CSetupFolder::~CSetupFolder()
{

}

void CSetupFolder::accept()
{
    name    = lineName->text();
    if(radioGroup->isChecked())
    {
        type = IDBFolder::eTypeGroup;
    }
    else if(radioProject->isChecked())
    {
        type = IDBFolder::eTypeProject;
    }
    else if(radioOther->isChecked())
    {
        type = IDBFolder::eTypeOther;
    }

    QDialog::accept();
}

void CSetupFolder::slotNameChanged(const QString& text)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
