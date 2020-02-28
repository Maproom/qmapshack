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

#ifndef CSCROPTRTE_H
#define CSCROPTRTE_H

#include "gis/IGisItem.h"
#include "mouse/IScrOpt.h"

#include "ui_IScrOptRte.h"
#include <QWidget>

class CGisItemRte;
class IMouse;

class CScrOptRte : public IScrOpt, private Ui::IScrOptRte
{
    Q_OBJECT
public:
    CScrOptRte(CGisItemRte * rte, const QPoint &point, IMouse *parent);
    virtual ~CScrOptRte();

    void draw(QPainter& p) override;

private slots:
    void slotEditDetails();
    void slotDelete();
    void slotCopy();
    void slotCalc();
    void slotReset();
    void slotEdit();
    void slotReverse();
    void slotInstruction(bool on);
    void slotToTrack();
    void slotNogo();
    void slotTags();

private:
    IGisItem::key_t key;
    QPointF anchor;
};

#endif //CSCROPTRTE_H

