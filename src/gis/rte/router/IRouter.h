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

#ifndef IROUTER_H
#define IROUTER_H

#include "gis/IGisItem.h"
#include <QWidget>

class IRouter : public QWidget
{
    Q_OBJECT
public:
    IRouter(bool fastRouting, QWidget * parent);
    virtual ~IRouter();

    virtual void calcRoute(const IGisItem::key_t& key) = 0;
    virtual int calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords) = 0;
    virtual bool hasFastRouting()
    {
        return fastRouting;
    }

    virtual QString getOptions() = 0;

private:
    bool fastRouting;
};

#endif //IROUTER_H

