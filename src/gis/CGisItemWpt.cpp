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

#include "gis/CGisItemWpt.h"
#include "gis/CGisProject.h"
#include "gis/CGisDraw.h"
#include "gis/WptIcons.h"


#include <QtWidgets>
#include <QtXml>

CGisItemWpt::CGisItemWpt(const QDomNode &xml, CGisProject *parent)
    : IGisItem(parent)
{
    readWpt(xml, wpt);
    icon = getWptIconByName(wpt.sym, focus);

    setText(0, wpt.name);
    setIcon(0, icon);
    getKey();


    if(icon.width() > 22 || icon.height() > 22)
    {
        qreal s;
        if(icon.width() > icon.height())
        {
            s = 22.0/icon.width();
        }
        else
        {
            s = 22.0/icon.height();
        }

        focus = focus * s;
        icon  = icon.scaled(icon.size()*s,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
}

CGisItemWpt::~CGisItemWpt()
{

}

void CGisItemWpt::genKey()
{
    if(key.isEmpty())
    {
        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData((const char*)&wpt, sizeof(wpt));
        key = md5.result().toHex();
    }
}

void CGisItemWpt::draw(QPainter& p, const QRectF& viewport, CGisDraw *gis)
{
    QPointF pt(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
    if(!viewport.contains(pt))
    {
        return;
    }
    gis->convertRad2Px(pt);
    p.drawPixmap(pt - focus, icon);
}
