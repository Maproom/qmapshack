/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>
                  2019 Johannes Zellner johannes@zellner.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

**********************************************************************************************/
#ifndef CUNITAVIATION_H
#define CUNITAVIATION_H

#include "IUnit.h"

class CUnitAviation : public IUnit
{
public:
    CUnitAviation(QObject * parent);
    virtual ~CUnitAviation() = default;

    void meter2distance(qreal meter, QString& val, QString& unit) const override;
    void meter2distance(qreal meter, qreal& val, QString& unit) const override;
    void meter2area(qreal meter, QString& val, QString& unit) const override;
    void meter2area(qreal meter, qreal& val, QString& unit) const override;
    qreal elevation2meter(const QString& val) const override;
    void meter2unit(qreal meter, qreal& scale, QString&  unit) const override;
};
#endif //CUNITAVIATION_H
