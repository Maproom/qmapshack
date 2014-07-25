/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CSCROPTTRK_H
#define CSCROPTTRK_H

#include "mouse/IScrOpt.h"

#include <QWidget>
#include "ui_IScrOptTrk.h"

class CGisItemTrk;
class IMouse;

class CScrOptTrk : public IScrOpt , public QWidget, private Ui::IScrOptTrk
{
    public:
        CScrOptTrk(CGisItemTrk * trk, IMouse *parent);
        virtual ~CScrOptTrk();

        void draw(QPainter& p);

    private:
        CGisItemTrk * trk;
};

#endif //CSCROPTTRK_H

