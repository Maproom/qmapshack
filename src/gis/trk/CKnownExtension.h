/**********************************************************************************************
    Copyright (C) 2015-2016 Christian Eichler code@christian-eichler.de

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

#ifndef CKNOWNEXTENSION_H
#define CKNOWNEXTENSION_H

#include "gis/trk/CGisItemTrk.h"
#include <functional>

class CKnownExtension
{
public:
    static void init(IUnit &units);

    static const QString internalSlope;
    static const QString internalSpeed;
    static const QString internalEle;
    static const QString internalProgress;

    static const CKnownExtension get(const QString &name);
    static bool isKnown(const QString &name);

    QString name;              //< userfriendly name ("Speed" "Heart Rate")
    int order;                 //< the order used for exporting ("relative position")
    qreal minimum;             //< hard (enforced) minimum, cannot go lower
    qreal maximum;             //< hard (enforced) maximum, cannot go higher
    qreal factor;              //< factor used to convert a value to match the users' units
    QString unit;              //< the unit (to be displayed)
    QString icon;              //< path to an icon
    bool known;
    bool derivedQMS;           //< if set to true the value is derived by QMS (p.x. slope*)
    fTrkPtGetVal valueFunc;    //< the function used to retrieve the value

private:
    static QHash<QString, CKnownExtension> knownExtensions;

    CKnownExtension(QString name,      int order,
                    qreal minimum,     qreal maximum,
                    qreal factor,      QString unit,
                    QString icon,      bool known,
                    bool derivedQMS,   fTrkPtGetVal valueFunc
                    )
        : name(name), order(order), minimum(minimum), maximum(maximum), factor(factor),
          unit(unit), icon(icon), known(known), derivedQMS(derivedQMS), valueFunc(valueFunc)
    {
    }

    static void initGarminTPXv1(IUnit &units, const QString &ns);
    static void initMioTPX(IUnit &units);
};

#endif // CKNOWNEXTENSION_H
