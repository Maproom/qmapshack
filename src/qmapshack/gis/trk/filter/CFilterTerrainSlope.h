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

#ifndef CFILTERTERRAINSLOPE_H
#define CFILTERTERRAINSLOPE_H

#include "ui_IFilterTerrainSlope.h"
#include <QWidget>

class CGisItemTrk;

class CFilterTerrainSlope : public QWidget, private Ui::IFilterTerrainSlope
{
    Q_OBJECT
public:
    CFilterTerrainSlope(CGisItemTrk& trk, QWidget * parent);
    virtual ~CFilterTerrainSlope() = default;

private slots:
    void slotApply();

private:
    CGisItemTrk& trk;
};

#endif //CFILTERTERRAINSLOPE_H

