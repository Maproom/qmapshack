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

#ifndef CDIALOGREFPOINT_H
#define CDIALOGREFPOINT_H

#include "ui_IDialogRefPoint.h"
#include <QDialog>

class QPointF;

class CDialogRefPoint : public QDialog, private Ui::IDialogRefPoint
{
    Q_OBJECT
public:
    CDialogRefPoint(QPointF& ptPtx, QPointF& ptRef, QWidget* parent);
    virtual ~CDialogRefPoint() = default;

public slots:
    void accept() override;

private slots:
    void slotEditPosition(const QString& str);

private:
    QPointF& ptPtx;
    QPointF& ptRef;
};

#endif //CDIALOGREFPOINT_H

