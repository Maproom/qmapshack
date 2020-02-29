/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef MISC_H
#define MISC_H

#include <QCollator>

#define QMS_DELETE(p) \
    delete p; \
    p = nullptr



inline bool sortByString(const QString& str1, const QString& str2)
{
    static QCollator collator;
    // this will set collator to natural sorting mode (instead of lexical)
    collator.setNumericMode(true);
    return collator.compare(str1, str2) < 0;
}

template<typename T>
bool sortByName(T * item1, T * item2)
{
    return sortByString(item1->getName(), item2->getName());
}



#endif //MISC_H

