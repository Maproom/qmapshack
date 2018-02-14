/**********************************************************************************************
    Copyright (C) 2014-2018 Michel Durand zero@cms123.fr

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

#include "gis/trk/CColorTrk.h"
#include "gis/trk/CGisItemTrk.h"


quint32 CColorTrk::selectColor(QWidget *parent)
{
    QMenu menu(parent);
    QAction * act;

    for(int i=0; i < TRK_N_COLORS; ++i)
    {
        QPixmap icon(14,14);
        icon.fill(IGisItem::colorMap[i].color);
        act = menu.addAction(icon, IGisItem::colorMap[i].name);
        act->setData(QVariant(i));
    }

    act = menu.exec(QCursor::pos());
    if(nullptr != act)
    {
        return act->data().toUInt(nullptr);
    }

    return 0xFFFFFFFF;
}



