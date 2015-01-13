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

#ifndef IDEMPROP_H
#define IDEMPROP_H

#include <QWidget>

class IDem;
class CDemDraw;

class IDemProp : public QWidget
{
    Q_OBJECT
public:
    IDemProp(IDem *demfile, CDemDraw *dem);
    virtual ~IDemProp();

protected slots:
    virtual void slotPropertiesChanged()= 0;

protected:
    IDem * demfile;
    CDemDraw * dem;
};

#endif //IDEMPROP_H

