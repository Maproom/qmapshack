/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CCREATEROUTEFROMWPT_H
#define CCREATEROUTEFROMWPT_H

#include "gis/IGisItem.h"

#include "ui_ICreateRouteFromWpt.h"
#include <QDialog>


class CCreateRouteFromWpt : public QDialog, private Ui::ICreateRouteFromWpt
{
    Q_OBJECT
public:
    CCreateRouteFromWpt(const QList<IGisItem::key_t>& keys, QWidget * parent);
    virtual ~CCreateRouteFromWpt();

public slots:
    void accept() override;

private slots:
    void slotSelectionChanged();
    void slotUp();
    void slotDown();
};

#endif //CCREATEROUTEFROMWPT_H

