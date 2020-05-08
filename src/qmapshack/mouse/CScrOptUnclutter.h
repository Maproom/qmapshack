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

#ifndef CSCROPTUNCLUTTER_H
#define CSCROPTUNCLUTTER_H

#include "gis/IGisItem.h"
#include "mouse/IScrOpt.h"


class CScrOptUnclutter : public IScrOpt
{
public:
    CScrOptUnclutter(IMouse *mouse);
    virtual ~CScrOptUnclutter();

    struct item_t
    {
        bool hasUserFocus;
        QString name;
        IGisItem::key_t key;
        QPixmap icon;
        QRect area;
        QRect text;
        QRect active;
    };

    virtual void clear();
    virtual int  size()
    {
        return items.size();
    }

    void addItem(IGisItem * gisItem);
    IGisItem::key_t getItemKey(int index = 0);
    const item_t *selectItem(const QPoint& point);

    void draw(QPainter& p) override;
    void mouseMove(const QPoint& pos) override;

private:
    static const QPoint positions[9][8];
    QList<item_t> items;
    bool doSpecialCursor = false;
};

#endif //CSCROPTUNCLUTTER_H

