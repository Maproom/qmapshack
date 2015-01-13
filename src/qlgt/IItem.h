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

#ifndef IITEM_H
#define IITEM_H

#include <QtCore>
#include <QtGui>

#define WPT_NOFLOAT 1e25f

class IItem
{
public:
    IItem(quint64 id);
    virtual ~IItem();

    QString getInfo()
    {
        return( "no info");
    }

    quint32 timestamp;

    QString name;
    QString comment;
    QString description;
    QPixmap iconPixmap;
    QString iconString;

    QString parentWpt;

    QString key;

    quint64 id;
};

#endif //IITEM_H

