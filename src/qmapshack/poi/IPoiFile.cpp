/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "poi/CPoiDraw.h"
#include "poi/CPoiPropSetup.h"
#include "poi/IPoiFile.h"

constexpr int iconsize = 22;

QSize IPoiFile::_iconSize = {iconsize, iconsize};
QImage IPoiFile::_iconHighlight;

void IPoiFile::init()
{
    // default sizes are for iconsize 22.
    qreal sx = qreal(_iconSize.width()) * 42.0 / 22.0;
    qreal sy = qreal(_iconSize.height()) * 42.0 / 22.0;
    _iconHighlight = QImage("://cursors/poiHighlightRed.png").scaled(sx, sy, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

IPoiFile::IPoiFile(CPoiDraw* parent)
    : IDrawObject(parent)
    , poi(parent)

{
}

IPoiProp* IPoiFile::getSetup()
{
    if(setup.isNull())
    {
        setup = new CPoiPropSetup(this, poi);
    }

    return setup;
}

