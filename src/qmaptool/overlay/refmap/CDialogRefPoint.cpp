/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "overlay/refmap/CDialogRefPoint.h"
#include "units/IUnit.h"

#include <QtWidgets>

CDialogRefPoint::CDialogRefPoint(QPointF& ptPtx, QPointF& ptRef, QWidget *parent)
    : QDialog(parent)
    , ptPtx(ptPtx)
    , ptRef(ptRef)
{
    setupUi(this);
    connect(lineCoord, &QLineEdit::textEdited, this, &CDialogRefPoint::slotEditPosition);

    lineX->setText(QString::number(qRound(ptPtx.x())));
    lineY->setText(QString::number(qRound(ptPtx.y())));
    if(ptRef != NOPOINTF)
    {
        QString str;
        if(IUnit::degToStr(ptRef.x(), ptRef.y(), str))
        {
            str = tr("bad coordinate");
        }
        lineCoord->setText(str);
    }

    labelWarning->hide();
}

void CDialogRefPoint::slotEditPosition(const QString& str)
{
    labelWarning->setVisible(!IUnit::isValidCoordString(str));
}

void CDialogRefPoint::accept()
{
    bool ok;
    ptPtx.setX(lineX->text().toInt(&ok));
    if(!ok)
    {
        QMessageBox::warning(this, tr("Error"), tr("Bad value for X pixel."), QMessageBox::Ok);
        return;
    }
    ptPtx.setY(lineY->text().toInt(&ok));
    if(!ok)
    {
        QMessageBox::warning(this, tr("Error"), tr("Bad value for Y pixel."), QMessageBox::Ok);
        return;
    }

    if(!IUnit::strToDeg(lineCoord->text(), ptRef.rx(), ptRef.ry()))
    {
        return;
    }

    QDialog::accept();
}
