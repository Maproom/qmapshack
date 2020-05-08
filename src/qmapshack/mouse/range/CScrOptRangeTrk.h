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

#ifndef CSCROPTRANGETRK_H
#define CSCROPTRANGETRK_H

#include "gis/trk/CTrackData.h"
#include "mouse/IScrOpt.h"
#include "ui_IScrOptRangeTrk.h"

class CGisItemTrk;

class CScrOptRangeTrk : public IScrOpt, public Ui::IScrOptRangeTrk
{
    Q_OBJECT

public:
    CScrOptRangeTrk(const QPointF& point, CGisItemTrk *trk, IMouse *mouse, QWidget *parent = nullptr);
    virtual ~CScrOptRangeTrk();

    void draw(QPainter& p) override;
};

#endif //CSCROPTRANGETRK_H

