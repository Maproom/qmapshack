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

#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/CGisProject.h"

#include <QtWidgets>

CGisItemOvlArea::CGisItemOvlArea(const QPolygonF& line, const QString &name, CGisProject * project, int idx)
    : IGisItem(project, eTypeOvl, -1)
{

}

CGisItemOvlArea::~CGisItemOvlArea()
{

}

void CGisItemOvlArea::genKey()
{

}

bool CGisItemOvlArea::isCloseTo(const QPointF& pos)
{

    return false;
}

void CGisItemOvlArea::save(QDomNode& gpx)
{

}

void CGisItemOvlArea::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis)
{

}

void CGisItemOvlArea::drawLabel(QPainter& p, const QRectF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis)
{

}

void CGisItemOvlArea::drawHighlight(QPainter& p)
{

}

void CGisItemOvlArea::gainUserFocus(bool yes)
{

}

const QString& CGisItemOvlArea::getName()
{
    return area.name;
}

QString CGisItemOvlArea::getInfo()
{
    QString str;

    return str;
}

void CGisItemOvlArea::getData(QPolygonF& line)
{

}

void CGisItemOvlArea::setData(const QPolygonF& line)
{

}
