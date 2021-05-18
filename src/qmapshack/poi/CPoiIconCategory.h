/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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
#ifndef CPOIICONCATEGORY_H
#define CPOIICONCATEGORY_H

#include <QMap>
#include <QPixmap>

class CPoiIconCategory
{
public:
    //Only to avoid compiler errors due to maps assignment
    CPoiIconCategory(){}
    CPoiIconCategory(const QPixmap& baseIcon, const QString& garminSym = "", const QMap<QString, QPixmap>& subCategories = QMap<QString, QPixmap>())
        : baseIcon(baseIcon), garminSym(garminSym), subCategories(subCategories){}
    //Convenience constructor to be able to omit garminSym when specifying child categories
    CPoiIconCategory(const QPixmap& baseIcon, const QMap<QString, QPixmap>& subCategories)
        : baseIcon(baseIcon), garminSym(""), subCategories(subCategories){}
    QPixmap getIcon(const QStringList& additionalTags) const;
    const QString& getGarminSym() const
    {
        return garminSym;
    }
private:
    QPixmap baseIcon;
    QString garminSym;
    QMap<QString, QPixmap> subCategories;
};

#endif // CPOIICONCATEGORY_H
