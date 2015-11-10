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

CKnownExtension::CKnownExtension(QString name,
        qreal defLimitLow, qreal defLimitHigh,
        qreal minimum, qreal maximum,
        QString unit, QString icon, bool known,
        colorFunc_t colorFunc
    ) : name(name), defLimitLow(defLimitLow), defLimitHigh(defLimitHigh),
        minimum(minimum), maximum(maximum), unit(unit), icon(icon), known(known), colorFunc(colorFunc)
{
}

QHash<QString, CKnownExtension> CKnownExtension::knownExtensions
{
    {"slope",
        { "Slope (directed)", -10., 10., -90., 90., "°", "://icons/32x32/CSrcSlope.png", true,
            [](const CGisItemTrk::trkpt_t &pp, const CGisItemTrk::trkpt_t &p) { return pp.ele < p.ele ? p.slope1 : -p.slope1; }
        }
    },

    {"speed",
        { "Speed", 1., 14., 0., 100., "m/s", "://icons/32x32/CSrcSpeed.png", true,
            [](const CGisItemTrk::trkpt_t &pp, const CGisItemTrk::trkpt_t &p) { return p.speed; }
        }
    },

    {"ele",
        { "Elevation", 200., 800., 0., 5000., "m", "://icons/32x32/CSrcElevation.png", true,
            [](const CGisItemTrk::trkpt_t &pp, const CGisItemTrk::trkpt_t &p) { return p.ele; }
        }
    },

    {"gpxtpx:TrackPointExtension|gpxtpx:hr",
        { "Heart Rate", 100., 200., 0., 300., "bpm", "://icons/32x32/CSrcHR.png", true,
            [](const CGisItemTrk::trkpt_t &pp, const CGisItemTrk::trkpt_t &p) { return p.extensions.value("gpxtpx:TrackPointExtension|gpxtpx:hr").toReal(); }
        }
    },

    {"tp1:TrackPointExtension|tp1:hr",
        { "Heart Rate", 100., 200., 0., 300., "bpm", "://icons/32x32/CSrcHR.png", true,
            [](const CGisItemTrk::trkpt_t &pp, const CGisItemTrk::trkpt_t &p) { return p.extensions.value("tp1:TrackPointExtension|tp1:hr").toReal(); }
        }
    }
};

const CKnownExtension CKnownExtension::get(const QString &name)
{
    CKnownExtension def("", 0., 100., -100000., 100000, "", "://icons/32x32/CSrcUnknown.png", false,
        [name] (const CGisItemTrk::trkpt_t &pp, const CGisItemTrk::trkpt_t &p) { return p.extensions.value(name).toReal(); }
    );
    return knownExtensions.value(name, def);
}

bool CKnownExtension::isKnown(const QString &name)
{
    return knownExtensions.contains(name);
}
