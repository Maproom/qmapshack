/**********************************************************************************************
    Copyright (C) 2014-2016 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CLIMIT_H
#define CLIMIT_H

#include <QSet>
#include <QVariant>
#include <QString>
#include <functional>

using fGetLimit = std::function<void(const QString&, QVariant&)>;

class CLimit
{
public:
    CLimit(const QString& cfgPath, fGetLimit getMin, fGetLimit getMax, fGetLimit getMinAuto, fGetLimit getMaxAuto);
    virtual ~CLimit();


    enum mode_e
    {
        eModeDefault
        , eModeUser
        , eModeAutomatic
    };

    void setup(const QString& source);

private:
    mode_e mode = eModeAutomatic;
    QString cfgPath;
    QVariant minUser;
    QVariant maxUser;

    fGetLimit getMin;
    fGetLimit getMax;
    fGetLimit getMinAuto;
    fGetLimit getMaxAuto;

    static QSet<CLimit*> allLimits;
};

#endif //CLIMIT_H

