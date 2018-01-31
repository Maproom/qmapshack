/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CGISSUMMARYSETUP_H
#define CGISSUMMARYSETUP_H

#include "ui_IGisSummarySetup.h"

class CGisSummary;

class CGisSummarySetup : public QDialog, private Ui::IGisSummarySetup
{
    Q_OBJECT
public:
    CGisSummarySetup(CGisSummary& parent);
    virtual ~CGisSummarySetup() = default;

private slots:
    void slotAdd();
    void slotDel();

private:
    CGisSummary& summary;
};

#endif //CGISSUMMARYSETUP_H

