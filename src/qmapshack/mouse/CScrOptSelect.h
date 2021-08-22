/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSCROPTSELECT_H
#define CSCROPTSELECT_H


#include "gis/IGisItem.h"
#include "mouse/IScrOpt.h"
#include "ui_IScrOptSelect.h"

class CScrOptSelect : public IScrOpt, public Ui::IScrOptSelect
{
    Q_OBJECT
public:
    CScrOptSelect(IMouse* mouse);
    virtual ~CScrOptSelect();

    void draw(QPainter& p) override
    {
    }

    IGisItem::selflags_t getModeSelection() const
    {
        return modeSelection;
    }

private slots:
    void slotModeSwitch(IGisItem::selection_e mode, bool checked);


private:
    IGisItem::selflags_t modeSelection = IGisItem::eSelectionNone;
};

#endif //CSCROPTSELECT_H

