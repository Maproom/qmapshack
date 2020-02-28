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

#include "gis/IGisItem.h"
#include "mouse/IScrOpt.h"
#include "ui_IScrOptTrk.h"

class CGisItemTrk;
class IMouse;

class CScrOptTrk : public IScrOpt, private Ui::IScrOptTrk
{
    Q_OBJECT
public:
    CScrOptTrk(CGisItemTrk * trk, const QPoint &point, IMouse *parent);
    virtual ~CScrOptTrk();

    void draw(QPainter& p) override;

private slots:
    void slotDelete();
    void slotCopy();
    void slotEditDetails();
    void slotProfile(bool on);
    void slotCut();
    void slotEdit();
    void slotReverse();
    void slotCombine();
    void slotRange();
    void slotActivity();
    void slotColor();
    void slotCopyWithWpt();
    void slotNogo();
    void slotAddElevation();
    void slotAddInfo();
    void slotTags();


private:
    IGisItem::key_t key;
    QPointF anchor;
};

#endif //CSCROPTTRK_H

