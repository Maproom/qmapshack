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

#ifndef CDEMDRAW_H
#define CDEMDRAW_H

#include "IDrawContext.h"

class QPainter;
class CDemList;
class CCanvas;

class CDemDraw : public IDrawContext
{
    public:
        CDemDraw(CCanvas * canvas);
        virtual ~CDemDraw();

        void saveConfig(QSettings& cfg);
        void loadConfig(QSettings& cfg);

        qreal getElevation(const QPointF& pos);

    protected:
        void drawt(buffer_t& currentBuffer){}

    private:
        CDemList * demList;

        static QStringList demPaths;

        static QList<CDemDraw*> dems;

};

#endif //CDEMDRAW_H

