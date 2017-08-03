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

#include <QSet>

#include "gis/trk/CGisItemTrk.h"

class CKnownExtension
{
    Q_DECLARE_TR_FUNCTIONS(CKnownExtension)
public:
    /**
       @brief Initialize using the default list of known namespaces
       @param units  The unit class currently used
     */
    static void init(const IUnit &units);

    /**
       @brief Register the Garmin TPX v1 for namespace ns
       @param units  The unit class currently used
       @param ns     The namespace to be used for Garmin TPX v1
     */
    static void initGarminTPXv1(const IUnit &units, const QString &ns);

    static void initClueTrustTPXv1(const IUnit &units, const QString &ns);

    static const QString internalSlope;    //< name of internally derived slope
    static const QString internalSpeed;    //< name of internally derived speed
    static const QString internalEle;      //< name of internally derived elevation (DEM)
    static const QString internalProgress; //< name of internally derived progress
    static const QString internalTerrainSlope; //< name of internally derived terrain slope

    /**
       @brief Get extension descriptor for name

       If there is no known descriptor for an extension with `name`, a valid dummy descriptor
       will be returned.
       The dummy descriptor contains valid entries, `valueFunc` can be used just as if the
       extension was known.

       @param name  The name of the requested extension (incl. namespace)

       @return a valid extension descriptor (for both known and unknown extensions)
     */
    static const CKnownExtension get(const QString &name);
    static bool isKnown(const QString &name);

    QString name;              //< userfriendly name ("Speed" "Heart Rate")
    int order;                 //< the order used for exporting ("relative position")
    qreal minimum;             //< hard (enforced) minimum, cannot go lower
    qreal maximum;             //< hard (enforced) maximum, cannot go higher
    qreal factor;              //< factor used to convert a value to match the users' units
    QString unit;              //< the unit (to be displayed)
    QString icon;              //< path to an icon
    bool known;                //< true if extension is known, false otherwise
    bool derivedQMS;           //< if set to true the value is derived by QMS (p.x. slope*)
    fTrkPtGetVal valueFunc;    //< the function used to retrieve the value

private:

    static bool registerNS(const QString &ns);

    static QHash<QString, CKnownExtension> knownExtensions;
    static QSet<QString> registeredNS;

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

    static void initMioTPX(const IUnit &units);
};

#endif // CKNOWNEXTENSION_H
