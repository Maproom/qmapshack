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

#ifndef CMAPPROPSETUP_H
#define CMAPPROPSETUP_H

#include <QWidget>
#include <ui_IMapPropSetup.h>

class IMap;
class CMap;

class CMapPropSetup : public QWidget, private Ui::IMapPropSetup
{
    Q_OBJECT
    public:
        CMapPropSetup(IMap * mapfile, CMap * map);
        virtual ~CMapPropSetup();

    private slots:
        void slotScaleChanged(const QPointF& scale);

    private:
        IMap * mapfile;
        CMap * map;
};

#endif //CMAPPROPSETUP_H

