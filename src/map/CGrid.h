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

#ifndef CGRID_H
#define CGRID_H

#include <QObject>
#include <QColor>
#include <proj_api.h>
class QPainter;
class QSettings;
class CMap;

class CGrid : public QObject
{
    Q_OBJECT
    public:
        CGrid(CMap * map);
        virtual ~CGrid();

        void saveConfig(QSettings& cfg);
        void loadConfig(QSettings& cfg);

        void draw(QPainter& p, const QRect &rect);

        void convertPos2Str(const QPointF& pos, QString& info);

    private:
        void setProjAndColor(const QString& proj, const QColor& c);
        void findGridSpace(double min, double max, double& xSpace, double& ySpace);
        bool calcIntersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double& x, double& y);

        CMap * map;

        projPJ  pjWGS84;
        projPJ  pjGrid;

        QString projstr;
        QColor color;


};

#endif //CGRID_H

