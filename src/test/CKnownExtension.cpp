/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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

#include "test/test_QMapShack.h"
#include "test/TestHelper.h"

#include "gis/gpx/CGpxProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"

static QStringList GarminTPX1Exts = {"atemp", "wtemp", "depth", "hr", "cad"};

void test_QMapShack::readExtGarminTPX1(const QString &file, const QString &ns)
{
    CGpxProject *proj = readGpxFile(file, true);

    VERIFY_EQUAL(1, proj->childCount());

    for(int i = 0; i < proj->childCount(); i++)
    {
        IGisItem *item = dynamic_cast<IGisItem*>(proj->child(i));

        CGisItemTrk *itemTrk = dynamic_cast<CGisItemTrk*>(item);
        if(nullptr != itemTrk)
        {
            const CGisItemTrk::trk_t &trk = itemTrk->getTrackData();

            // filter all internal extensions (starting with ::ql:)
            QStringList extensions = itemTrk->getExistingDataSources().filter(QRegExp("^((?!::ql:).)*$"));

            VERIFY_EQUAL(GarminTPX1Exts.size(), extensions.size());
            for(const QString &exp : GarminTPX1Exts)
            {
                const QString &fullExp = ns + ":TrackPointExtension|" + ns + ":" + exp;
                SUBVERIFY(extensions.contains(fullExp), "Missing extension " + fullExp)
            }

            const fTrkPtGetVal &getAtemp = CKnownExtension::get(ns + ":TrackPointExtension|" + ns + ":atemp").valueFunc;
            const fTrkPtGetVal &getWtemp = CKnownExtension::get(ns + ":TrackPointExtension|" + ns + ":wtemp").valueFunc;
            const fTrkPtGetVal &getDepth = CKnownExtension::get(ns + ":TrackPointExtension|" + ns + ":depth").valueFunc;
            const fTrkPtGetVal &getHR    = CKnownExtension::get(ns + ":TrackPointExtension|" + ns + ":hr").valueFunc;
            const fTrkPtGetVal &getCad   = CKnownExtension::get(ns + ":TrackPointExtension|" + ns + ":cad").valueFunc;

            int i = 0;
            for(const CGisItemTrk::trkseg_t &seg : trk.segs)
            {
                for(const CGisItemTrk::trkpt_t &trkpt : seg.pts)
                {
                    SUBVERIFY((0. != trkpt.lat) || (0. != trkpt.lon), "Trackpoint has position 0/0");

                    for(const QString &ext : trkpt.extensions.keys())
                    {
                        VERIFY_EQUAL(true, CKnownExtension::isKnown(ext));
                    }

                    // try to read values from the file, they start at a specific value and are incremented from trkpt to trkpt
                    VERIFY_EQUAL( 20 + i, getAtemp(trkpt));
                    VERIFY_EQUAL( 10 + i, getWtemp(trkpt));
                    VERIFY_EQUAL(100 + i, getDepth(trkpt));
                    VERIFY_EQUAL( 90 + i, getHR(trkpt));
                    VERIFY_EQUAL( 60 + i, getCad(trkpt));

                    ++i;
                }
            }
        }
    }

    delete proj;
}

void test_QMapShack::_readExtGarminTPX1_tp1()
{
    readExtGarminTPX1(testInput + "gpx_ext_GarminTPX1_tp1.gpx", "tp1");
}

void test_QMapShack::_readExtGarminTPX1_gpxtpx()
{
    readExtGarminTPX1(testInput + "gpx_ext_GarminTPX1_gpxtpx.gpx", "gpxtpx");
}
