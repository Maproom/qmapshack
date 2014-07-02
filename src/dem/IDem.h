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

#ifndef IDEM_H
#define IDEM_H

#include "canvas/IDrawContext.h"
#include "canvas/IDrawObject.h"
#include <QObject>
#include <proj_api.h>

class CDemDraw;
class QSettings;


class IDem : public IDrawObject
{
    public:
        IDem(CDemDraw * parent);
        virtual ~IDem();

        virtual void draw(IDrawContext::buffer_t& buf) = 0;

        virtual qreal getElevation(const QPointF& pos) = 0;

        bool activated(){return isActivated;}

    protected:
        /**
           @brief Reproject (translate, rotate, scale) tile befor drwaing it.
           @param img   the tile as QImage
           @param l     a 4 point polygon to fit the tile in
           @param p     the QPainter used to paint the tile
         */
        void drawTile(QImage& img, QPolygonF& l, QPainter& p);

        CDemDraw * dem;

        /// source projection of the current map file
        /**
            Has to be set by subclass. Destruction has to be
            handeled by subclass.
        */
        projPJ  pjsrc;
        /// target projection
        /**
            Is set by IMap() to WGS84. Will be freed by ~IMap()
        */
        projPJ  pjtar;

        /**
           @brief True if map was loaded successfully
         */
        bool isActivated;
};

#endif //IDEM_H

