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

#ifndef CFILTERLOOPSCUT_H
#define CFILTERLOOPSCUT_H

#include "CMainWindow.h"
#include "ui_IFilterLoopsCut.h"
#include <QWidget>

class CGisItemTrk;

class CFilterLoopsCut : public QWidget, private Ui::IFilterLoopsCut
{
    Q_OBJECT
public:
    CFilterLoopsCut(CGisItemTrk& trk, QWidget *parent);
    virtual ~CFilterLoopsCut();

private slots:
    void slotApply();
    void showHelp();

private:
    CGisItemTrk& trk;
};

#endif //CFILTERLOOPSCUT_H

