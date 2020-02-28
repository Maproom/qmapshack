/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#ifndef CSCROPTWPT_H
#define CSCROPTWPT_H

#include "gis/IGisItem.h"
#include "mouse/IScrOpt.h"

#include "ui_IScrOptWpt.h"
#include <QWidget>

class CGisItemWpt;
class IMouse;

class CScrOptWpt : public IScrOpt, private Ui::IScrOptWpt
{
    Q_OBJECT
public:
    CScrOptWpt(CGisItemWpt * wpt, const QPoint &point, IMouse *parent);
    virtual ~CScrOptWpt();

    void draw(QPainter& p) override;

private slots:
    void slotDelete();
    void slotEdit();
    void slotCopy();
    void slotCoordToClipboard();
    void slotMove();
    void slotProj();
    void slotBubble();
    void slotDeleteRadius();
    void slotNogoArea();
    void slotEditRadius();
    void slotAddElevation();
    void slotSearchWeb();
    void slotTags();

private:
    IGisItem::key_t key;
    QPointF anchor;
};

#endif //CSCROPTWPT_H

