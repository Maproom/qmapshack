/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CGRIDSETUP_H
#define CGRIDSETUP_H

#include "ui_IGridSetup.h"
#include <QDialog>

class CGrid;
class CMapDraw;

class CGridSetup : public QDialog, private Ui::IGridSetup
{
    Q_OBJECT
public:
    CGridSetup(CGrid * grid, CMapDraw *map);
    virtual ~CGridSetup();

public slots:
    void accept() override;

private slots:
    void slotProjWizard();
    void slotSelectGridColor();
    void slotRestoreDefault();
    void slotProjFromMap();

private:
    CGrid * grid;
    CMapDraw * map;
};

#endif //CGRIDSETUP_H

