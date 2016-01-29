/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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

#ifndef CFILTERDELETEEXTENSION_H
#define CFILTERDELETEEXTENSION_H

#include "ui_IFilterDeleteExtension.h"
#include <QWidget>

class CGisItemTrk;

class CFilterDeleteExtension : public QWidget, private Ui::IFilterDeleteExtension
{
    Q_OBJECT
public:
    CFilterDeleteExtension(CGisItemTrk& trk, QWidget * parent);
    virtual ~CFilterDeleteExtension();

    void update();

private slots:
    void slotApply();

private:
    CGisItemTrk& trk;
};

#endif //CFILTERDELETEEXTENSION_H

