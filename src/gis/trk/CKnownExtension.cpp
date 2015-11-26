/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

const QString CKnownExtension::internalSlope = "::ql:slope";
const QString CKnownExtension::internalSpeed = "::ql:speed";
const QString CKnownExtension::internalEle   = "::ql:ele";

QHash<QString, CKnownExtension> CKnownExtension::knownExtensions;

CKnownExtension::CKnownExtension(QString name,
                                 qreal defLimitLow, qreal defLimitHigh,
                                 qreal minimum, qreal maximum,
                                 qreal factor, QString unit,
                                 QString icon, bool known,
                                 fTrkPtGetVal valueFunc
                                 ) : name(name), defLimitLow(defLimitLow), defLimitHigh(defLimitHigh),
    minimum(minimum), maximum(maximum), factor(factor), unit(unit),
    icon(icon), known(known), valueFunc(valueFunc)
{
}

static fTrkPtGetVal getExtensionValueFunc(const QString ext)
{
    return [ext](const CGisItemTrk::trkpt_t &p)
           {
               bool ok;
               qreal val = p.extensions.value(ext).toReal(&ok);
               return ok ? val : NOFLOAT;
           };
}

void CKnownExtension::init(IUnit &units)
{
    const QString &speedunit   = units.speedunit;
    const qreal   &speedfactor = units.speedfactor;

    const QString &baseunit    = units.baseunit;
    const qreal   &basefactor  = units.basefactor;

    knownExtensions =
    {
        {internalSlope,
         { QObject::tr("Slope (directed, derived)"), -10., 10., -90., 90., 1., "°", "://icons/32x32/CSrcSlope.png", true,
           [](const CGisItemTrk::trkpt_t &p) { return p.slope1; }}
        },

        {internalSpeed,
         { QObject::tr("Speed (derived)"), 1., 14., 0., 600., speedfactor, speedunit, "://icons/32x32/CSrcSpeed.png", true,
           [](const CGisItemTrk::trkpt_t &p) { return p.speed; }}
        },

        {internalEle,
         { QObject::tr("Elevation"), 200., 800., 0., 100000., basefactor, baseunit, "://icons/32x32/CSrcElevation.png", true,
           [](const CGisItemTrk::trkpt_t &p) { return (NOINT == p.ele) ? NOFLOAT : p.ele; }}
        },

        // support for the Garmin TrackPointExtension v1
        //  https://www8.garmin.com/xmlschemas/TrackPointExtensionv1.xsd
        {"gpxtpx:TrackPointExtension|gpxtpx:hr",
         { QObject::tr("Heart Rate"), 100., 200., 0., 300., 1., "bpm", "://icons/32x32/CSrcHR.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:hr")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:cad",
         { QObject::tr("Cadence"), 50., 110., 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:cad")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:atemp",
         { QObject::tr("Air Temperature"), 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcATemp.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:atemp")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:wtemp",
         { QObject::tr("Water Temperature"), 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcWTemp.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:wtemp")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:depth",
         { QObject::tr("Depth"), 0., 200., 0., 12000., basefactor, baseunit, "://icons/32x32/CSrcDepth.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:depth")}
        },


        {"tp1:TrackPointExtension|tp1:hr",
         { QObject::tr("Heart Rate"), 100., 200., 0., 300., 1., "bpm", "://icons/32x32/CSrcHR.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:hr")}
        },

        {"tp1:TrackPointExtension|tp1:cad",
         { QObject::tr("Cadence"), 50., 110., 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:cad")}
        },

        {"tp1:TrackPointExtension|tp1:atemp",
         { QObject::tr("Air Temperature"), 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcATemp.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:atemp")}
        },

        {"tp1:TrackPointExtension|tp1:wtemp",
         { QObject::tr("Water Temperature"), 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcWTemp.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:wtemp")}
        },

        {"tp1:TrackPointExtension|tp1:depth",
         { QObject::tr("Depth"), 0., 200., 0., 12000., basefactor, baseunit, "://icons/32x32/CSrcDepth.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:depth")}
        },

        // support for extensions used by MIO Cyclo ver. 4.2 (who needs xml namespaces?!)
        {"heartrate",
         { QObject::tr("Heart Rate"), 100., 200., 0., 300., 1., "bpm", "://icons/32x32/CSrcHR.png", true,
           getExtensionValueFunc("heartrate")}
        },

        {"cadence",
         { QObject::tr("Cadence"), 50., 110., 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true,
           getExtensionValueFunc("cadence")}
        }
    };
}

const CKnownExtension CKnownExtension::get(const QString &name)
{
    CKnownExtension def("", 0., 100., -100000., 100000., 1., "", "://icons/32x32/CSrcUnknown.png", false,
                        getExtensionValueFunc(name)
                        );
    return knownExtensions.value(name, def);
}

bool CKnownExtension::isKnown(const QString &name)
{
    return knownExtensions.contains(name);
}
