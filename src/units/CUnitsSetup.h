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
#ifndef CUNITSSETUP_H
#define CUNITSSETUP_H

#include "ui_IUnitsSetup.h"
#include <QDialog>

class CUnitsSetup : public QDialog, private Ui::IUnitsSetup
{
public:
    CUnitsSetup(QWidget * parent);
    virtual ~CUnitsSetup();

public slots:
    virtual void accept() override;
};

#endif //CUNITSSETUP_H

