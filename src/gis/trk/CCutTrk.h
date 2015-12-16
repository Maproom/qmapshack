/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CCUTTRK_H
#define CCUTTRK_H

#include "ui_ICutTrk.h"
#include <QDialog>

class CCutTrk : public QDialog, private Ui::ICutTrk
{
    Q_OBJECT
public:
    CCutTrk(QWidget * parent);
    virtual ~CCutTrk() = default;

    enum mode_e
    {
        eModeNone         = 0
        , eModeKeepFirst  = 1
        , eModeKeepBoth   = 2
        , eModeKeepSecond = 4
    };

    mode_e getMode() const
    {
        return mode;
    }

    bool createClone()
    {
        return checkCreateClone->isChecked();
    }

public slots:
    virtual void accept() override;

private slots:
    void slotClicked();

private:
    mode_e mode = eModeNone;
};

#endif //CCUTTRK_H

