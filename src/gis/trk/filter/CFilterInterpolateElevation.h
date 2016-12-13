/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CFILTERINTERPOLATEELEVATION_H
#define CFILTERINTERPOLATEELEVATION_H

#include "ui_IFilterInterpolateElevation.h"
#include <QWidget>

class CGisItemTrk;

class CFilterInterpolateElevation : public QWidget, private Ui::IFilterInterpolateElevation
{
    Q_OBJECT
public:
    CFilterInterpolateElevation(CGisItemTrk& trk, QWidget * parent);
    virtual ~CFilterInterpolateElevation();

private slots:
    void slotApply();
    void slotPreview();

private:

    CGisItemTrk& trk;
};

#endif //CFILTERINTERPOLATEELEVATION_H

