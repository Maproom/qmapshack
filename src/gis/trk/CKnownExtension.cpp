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
        {"slope",
         { "Slope (directed)", -10., 10., -90., 90., 1., "°", "://icons/32x32/CSrcSlope.png", true,
           [] (const CGisItemTrk::trkpt_t &p) { return p.slope1; }
         }
        },

        {"speed",
         { "Speed", 1., 14., 0., 100., speedfactor, speedunit, "://icons/32x32/CSrcSpeed.png", true,
           [] (const CGisItemTrk::trkpt_t &p) { return p.speed; }
         }
        },

        {"ele",
         { "Elevation", 200., 800., 0., 5000., basefactor, baseunit, "://icons/32x32/CSrcElevation.png", true,
           [] (const CGisItemTrk::trkpt_t &p) { return (NOINT == p.ele) ? NOFLOAT : p.ele; }
         }
        },

        // support for the Garmin TrackPointExtension v1
        //  https://www8.garmin.com/xmlschemas/TrackPointExtensionv1.xsd
        {"gpxtpx:TrackPointExtension|gpxtpx:hr",
         { "Heart Rate", 100., 200., 0., 300., 1., "bpm", "://icons/32x32/CSrcHR.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:hr")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:cad",
         { "Cadence", 50., 110., 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:cad")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:atemp",
         { "Air Temperature", 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcATemp.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:atemp")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:wtemp",
         { "Water Temperature", 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcWTemp.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:wtemp")}
        },

        {"gpxtpx:TrackPointExtension|gpxtpx:depth",
         { "Depth", 0., 200., 0., 5000., basefactor, baseunit, "://icons/32x32/CSrcDepth.png", true,
           getExtensionValueFunc("gpxtpx:TrackPointExtension|gpxtpx:depth")}
        },


        {"tp1:TrackPointExtension|tp1:hr",
         { "Heart Rate", 100., 200., 0., 300., 1., "bpm", "://icons/32x32/CSrcHR.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:hr")}
        },

        {"tp1:TrackPointExtension|tp1:cad",
         { "Cadence", 50., 110., 0., 500., 1., "rpm", "://icons/32x32/CSrcCAD.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:cad")}
        },

        {"tp1:TrackPointExtension|tp1:atemp",
         { "Air Temperature", 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcATemp.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:atemp")}
        },

        {"tp1:TrackPointExtension|tp1:wtemp",
         { "Water Temperature", 10., 30., -100., 100., 1., "°C", "://icons/32x32/CSrcWTemp.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:wtemp")}
        },

        {"tp1:TrackPointExtension|tp1:depth",
         { "Depth", 0., 200., 0., 5000., basefactor, baseunit, "://icons/32x32/CSrcDepth.png", true,
           getExtensionValueFunc("tp1:TrackPointExtension|tp1:depth")}
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
