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

#ifndef WPTICONS_H
#define WPTICONS_H

#include <QPixmap>
#include <QString>

struct icon_t
{
    icon_t() : focus(16,16)
    {
    }
    icon_t(const QString& path, int x, int y) : path(path), focus(x,y)
    {
    }
    QString path;
    QPoint focus;
};


void initWptIcons();
const QMap<QString, icon_t> &getWptIcons();
QPixmap getWptIconByName(const QString& name, QPointF &focus, QString * src = nullptr);
void setWptIconByName(const QString& name, const QString& filename);
void setWptIconByName(const QString& name, const QPixmap& icon);
QPixmap loadIcon(const QString& path);

#endif //WPTICONS_H

