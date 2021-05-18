/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CPOIPROPSETUP_H
#define CPOIPROPSETUP_H

#include "poi/IPoiProp.h"
#include "ui_IPoiPropSetup.h"

class CPoiPropSetup : public IPoiProp, private Ui::IPoiPropSetup
{
    Q_OBJECT
public:
    enum treeColumn_e
    {
        eTreeColumnCheckbox,
        eTreeColumnDisplayName,
    };

    CPoiPropSetup(IPoi* poifile, CPoiDraw* poi);
    virtual ~CPoiPropSetup() = default;

protected slots:
    void slotPropertiesChanged() override;

private:
};

#endif //CPOIPROPSETUP_H

