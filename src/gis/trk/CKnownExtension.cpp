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


#include "gis/trk/CKnownExtension.h"
#include "units/IUnit.h"
#include <QStringBuilder>

const QString CKnownExtension::internalSlope    = "::ql:slope";
const QString CKnownExtension::internalSpeedDist    = "::ql:speeddist";
const QString CKnownExtension::internalSpeedTime    = "::ql:speedtime";
const QString CKnownExtension::internalEle      = "::ql:ele";
const QString CKnownExtension::internalProgress = "::ql:progress";
const QString CKnownExtension::internalTerrainSlope = "::ql:terrainslope";

QHash<QString, CKnownExtension> CKnownExtension::knownExtensions;
QSet<QString> CKnownExtension::registeredNS;

static const int NOORDER = std::numeric_limits<int>::max();

static fTrkPtGetVal getExtensionValueFunc(const QString ext)
{
    return [ext](const CTrackData::trkpt_t &p)
           {
               bool ok;
               qreal val = p.extensions.value(ext).toReal(&ok);
               return ok ? val : NOFLOAT;
           };
}

bool CKnownExtension::registerNS(const QString &ns)
{
    if(!registeredNS.contains(ns))
    {
        registeredNS.insert(ns);
        return true;
    }

    return false;
}

void CKnownExtension::initGarminTPXv1(const IUnit &units, const QString &ns)
{
    if(!registerNS(ns))
    {
        return;
    }

    // support for the Garmin TrackPointExtension v1
    //  https://www8.garmin.com/xmlschemas/TrackPointExtensionv1.xsd
    knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":atemp",
                           { tr("Air Temp."), tr("Garmin Air Temperature"), 0, -100., 100., 1., "°C", "://icons/32x32/CSrcATemp.png", true, false,
                             getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":atemp")});

    knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":wtemp",
                           { tr("Water Temp."), tr("Garmin Water Temperature"), 1, -100., 100., 1., "°C", "://icons/32x32/CSrcWTemp.png", true, false,
                             getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":wtemp")});

    knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":depth",
                           { tr("Depth"), tr("Garmin Depth"), 2, 0., 12000., units.basefactor, units.baseunit, "://icons/32x32/CSrcDepth.png", true, false,
                             getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":depth")});

    knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":hr",
                           { tr("Heart R."), tr("Garmin Heart Rate"), 3, 0., 300., 1., "bpm", "://icons/32x32/CSrcHR.png", true, false,
                             getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":hr")});

    knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":cad",
                           { tr("Cadence"), tr("Garmin Cadence"), 4, 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true, false,
                             getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":cad")});
}

void CKnownExtension::initMioTPX(const IUnit &units)
{
    // support for extensions used by MIO Cyclo ver. 4.2 (who needs xml namespaces?!)
    knownExtensions.insert("heartrate",
                           { tr("Heart R."), tr("Mio Heart Rate"), NOORDER, 0., 300., 1., "bpm", "://icons/32x32/CSrcHR.png", true, false,
                             getExtensionValueFunc("heartrate")});

    knownExtensions.insert("cadence",
                           { tr("Cadence"), tr("Mio Cadence"), NOORDER, 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true, false,
                             getExtensionValueFunc("cadence")});

    knownExtensions.insert("speed",
                           { tr("Speed"), tr("Mio Speed"), NOORDER, 0., 600., units.speedfactor, units.speedunit, "://icons/32x32/CSrcSpeed.png", true, false,
                             getExtensionValueFunc("speed")});

    knownExtensions.insert("acceleration",
                           { tr("Accel."), tr("Mio Acceleration"), NOORDER, std::numeric_limits<qreal>::lowest(), std::numeric_limits<qreal>::max(), units.basefactor, units.baseunit + "/s²", "://icons/32x32/CSrcAccel.png", true, false,
                             getExtensionValueFunc("acceleration")});

    knownExtensions.insert("course",
                           { tr("Course"), tr("Mio Course"), NOORDER, -3.2, 3.2, 1., "rad", "://icons/32x32/CSrcCourse.png", true, false,
                             getExtensionValueFunc("course")});
}

void CKnownExtension::initClueTrustTPXv1(const IUnit &units, const QString &ns)
{
    knownExtensions.insert(ns % ":cadence",
                           { tr("Cadence"), tr("Garmin Cadence"), 0, 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true, false,
                             getExtensionValueFunc(ns % ":cadence")});

    knownExtensions.insert(ns % ":temp",
                           { tr("Temp."), tr("Garmin Temperature"), 1, -100., 100., 1., "°C", "://icons/32x32/CSrcATemp.png", true, false,
                             getExtensionValueFunc(ns % ":temp")});

    knownExtensions.insert(ns % ":distance",
                           { tr("Dist."), tr("Garmin Distance"), 2, 0., +100000000., units.basefactor, units.baseunit, "://icons/32x32/CSrcDistance.png", true, false,
                             getExtensionValueFunc(ns % ":distance") });

    knownExtensions.insert(ns % ":altitude",
                           { tr("Elev."), tr("Garmin Elevation"), 3, -1000., +10000., units.basefactor, units.baseunit, "://icons/32x32/CSrcElevation.png", true, false,
                             getExtensionValueFunc(ns % ":altitude") });

    knownExtensions.insert(ns % ":energy",
                           { tr("Energy"), tr("Garmin Energy"), 4, 0., 10000., 1., "kcal/min", "://icons/32x32/CSrcEnergy.png", true, false,
                             getExtensionValueFunc(ns % ":energy") });


    knownExtensions.insert(ns % ":seaLevelPressure",
                           { tr("Sea Lev. Pres."), tr("Garmin Sea Level Pressure"), 5, 0., 1500., 1., "hPa", "://icons/32x32/CSrcSeaLevelPressure.png", true, false,
                             getExtensionValueFunc(ns % ":seaLevelPressure") });

    knownExtensions.insert(ns % ":speed",
                           { tr("Speed"), tr("Garmin Speed"), 6, 0., 600., units.speedfactor, units.speedunit, "://icons/32x32/CSrcSpeed.png", true, false,
                             getExtensionValueFunc(ns % ":speed")});

    knownExtensions.insert(ns % ":verticalSpeed",
                           { tr("v. Speed"), tr("Garmin vertical Speed"), 7, 0., 50., units.speedfactor, units.speedunit, "://icons/32x32/CSrcVertSpeed.png", true, false,
                             getExtensionValueFunc(ns % ":verticalSpeed")});
}

void CKnownExtension::init(const IUnit &units)
{
    knownExtensions =
    {
        {internalSlope,
         { tr("Slope"), tr("QMS Slope"), -1, -90., 90., 1., "°", "://icons/32x32/CSrcSlope.png", true, true,
           [](const CTrackData::trkpt_t &p) { return p.slope1; }}
        },

        {internalSpeedDist,
         { tr("Speed"), tr("QMS Speed over Distance"), -1, 0., 600., units.speedfactor, units.speedunit, "://icons/32x32/CSrcSpeed.png", true, true,
           [](const CTrackData::trkpt_t &p) { return p.speed; }}
        },

        {internalSpeedTime,
         { tr("Speed"), tr("QMS Speed over Time"), -1, 0., NOFLOAT, units.speedfactor, units.speedunit, "://icons/32x32/CSrcSpeed.png", true, true,
           [](const CTrackData::trkpt_t &p) { return p.speed; }}
        },

        {internalEle,
         { tr("Elev."), tr("QMS Elevation"), -1, 0., 100000., units.basefactor, units.baseunit, "://icons/32x32/CSrcElevation.png", true, true,
           [](const CTrackData::trkpt_t &p) { return (NOINT == p.ele) ? NOFLOAT : p.ele; }}
        },

        {internalProgress,
         { tr("Progress"), tr("QMS Progress"), -1, 0., NOFLOAT, units.basefactor, units.baseunit, "://icons/32x32/Progress.png", true, true,
           [](const CTrackData::trkpt_t &p) { return p.distance; }}
        },

        {internalTerrainSlope,
         { tr("Terr. Slope"), tr("QMS Terrain Slope"), -1, 0, 90., 1., "°", "://icons/32x32/CSrcSlope.png", true, false,
           getExtensionValueFunc(internalTerrainSlope)}
        }
    };

    initGarminTPXv1(units, "gpxtpx");
    initGarminTPXv1(units, "tp1");

    initMioTPX(units);
    initClueTrustTPXv1(units, "gpxdata");
}

const CKnownExtension CKnownExtension::get(const QString &key)
{
    CKnownExtension def("", "", NOORDER, -100000., 100000., 1., "", "://icons/32x32/CSrcUnknown.png", false, true,
                        getExtensionValueFunc(key)
                        );
    return knownExtensions.value(key, def);
}

bool CKnownExtension::isKnown(const QString &key)
{
    return knownExtensions.contains(key);
}
