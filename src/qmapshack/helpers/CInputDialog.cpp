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

#include "CInputDialog.h"

#include <QtWidgets>

CInputDialog::CInputDialog(QWidget *parent, const QString& text, QVariant& val, const QVariant& reset)
    : QDialog(parent)
    , val(val)
    , reset(reset)
{
    setupUi(this);
    QPushButton * pushReset = buttonBox->addButton(QDialogButtonBox::Reset);
    connect(pushReset, &QPushButton::clicked, this, &CInputDialog::slotReset);

    label->setText(text);
    if(val != reset)
    {
        lineEdit->setText(val.toString());
    }
}

CInputDialog::~CInputDialog()
{
}

void CInputDialog::accept()
{
    if(lineEdit->text().isEmpty())
    {
        val = reset;
    }
    else
    {
        val.setValue(lineEdit->text());
    }

    QDialog::accept();
}

void CInputDialog::slotReset()
{
    lineEdit->clear();
}
