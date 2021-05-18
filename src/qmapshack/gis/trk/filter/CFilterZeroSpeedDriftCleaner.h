/**********************************************************************************************
    Copyright (C) 2018 Michel Durand <zero@cms123.fr>

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

#ifndef CFILTERZEROSPEEDDRIFTCLEANER_H
#define CFILTERZEROSPEEDDRIFTCLEANER_H

#include "CMainWindow.h"
#include "ui_IFilterZeroSpeedDriftCleaner.h"
#include <QWidget>

class CGisItemTrk;

class CFilterZeroSpeedDriftCleaner : public QWidget, private Ui::IFilterZeroSpeedDriftCleaner
{
    Q_OBJECT
public:
    CFilterZeroSpeedDriftCleaner(CGisItemTrk& trk, QWidget* parent);
    virtual ~CFilterZeroSpeedDriftCleaner();

private slots:
    void slotApply();
    void showHelp();

private:
    CGisItemTrk& trk;
};

#endif //CFILTERZEROSPEEDDRIFTCLEANER_H

