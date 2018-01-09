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

#include "helpers/CPositionDialog.h"
#include "units/IUnit.h"

#include <QtWidgets>


CPositionDialog::CPositionDialog(QWidget * parent, QPointF &pos)
    : QDialog(parent)
    , pos(pos)
{
    setupUi(this);
    QString str;
    IUnit::degToStr(pos.x(), pos.y(), str);
    lineEdit->setText(str);

    labelWarning->hide();

    connect(lineEdit, &QLineEdit::textEdited, this, &CPositionDialog::slotEdit);
}

CPositionDialog::~CPositionDialog()
{
}

void CPositionDialog::accept()
{
    if(getPosition(pos, lineEdit->text()))
    {
        QDialog::accept();
    }
}

bool CPositionDialog::getPosition(QPointF& pt, const QString& str)
{
    qreal lon, lat;

    bool res = IUnit::strToDeg(str, lon, lat);

    if(res)
    {
        pt.rx() = lon;
        pt.ry() = lat;
    }

    return res;
}



void CPositionDialog::slotEdit(const QString& str)
{
    bool isValid = IUnit::isValidCoordString(str);

    labelWarning->setVisible(!isValid);
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}
