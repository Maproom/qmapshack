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

#ifndef IMAPPROPSETUP_H
#define IMAPPROPSETUP_H

#include <QWidget>

class IMap;
class CMapDraw;

class IMapPropSetup : public QWidget
{
    Q_OBJECT
    public:
        IMapPropSetup(IMap * mapfile, CMapDraw * map);
        virtual ~IMapPropSetup();


    protected slots:
        virtual void slotPropertiesChanged()= 0;

    protected:
        IMap * mapfile;
        CMapDraw * map;

};

#endif //IMAPPROPSETUP_H

