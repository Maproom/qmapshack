/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
                  2019 Johannes Zellner johannes@zellner.org

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

#ifndef CDEMPROPSETUP_H
#define CDEMPROPSETUP_H

#define SLOPE_LEVELS 5

#include "dem/IDemProp.h"
#include "ui_IDemPropSetup.h"

class CDemPropSetup : public IDemProp, private Ui::IDemPropSetup
{
    Q_OBJECT
public:
    CDemPropSetup(IDem* demfile, CDemDraw* dem);
    virtual ~CDemPropSetup();

protected slots:
    void slotPropertiesChanged() override;

private slots:
    void slotScaleChanged(const QPointF& s);
    void slotSetMinScale(bool checked);
    void slotSetMaxScale(bool checked);

    void slotGradeIndex(int idx);
    void slotSlopeValiddateAfterEdit();
    void slotSlopeChanged(int val);
    void slotElevationAfterEdit();

private:
    CTinySpinBox* slopeSpins[SLOPE_LEVELS];

    static QPointF scale;
};

#endif //CDEMPROPSETUP_H

