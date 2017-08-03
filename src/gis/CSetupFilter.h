/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CSETUPFILTER_H
#define CSETUPFILTER_H

#include "ui_ISetupFilter.h"
#include <QWidget>

class CGisWidget;

class CSetupFilter : public QWidget, private Ui::ISetupFilter
{
    Q_OBJECT
public:
    CSetupFilter(CGisWidget *parent);
    virtual ~CSetupFilter() = default;

private slots:
    void slotSelect();

private:
    CGisWidget * gisWidget;
};

#endif //CSETUPFILTER_H

