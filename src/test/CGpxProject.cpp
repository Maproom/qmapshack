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

#include "test/test_QMapShack.h"

#include "gis/gpx/CGpxProject.h"

#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CKnownExtension.h"

void test_QMapShack::readValidGPXFile()
{
    // this does not ready anything, a bare CSlfProject is created
    CGpxProject *proj = new CGpxProject("qtt_gpx_file0.gpx", (CGisListWks*) nullptr);

    try
    {
        proj->blockUpdateItems(true);
        CGpxProject::loadGpx(testInput + "qtt_gpx_file0.gpx", proj);
        proj->blockUpdateItems(false);
    }
    catch(QString &errormsg)
    {
        QFAIL(QString("Did not expect any error, but got: `%1`").arg(errormsg).toStdString().c_str());
    }

    QVERIFY(  proj->isValid()   );
    QVERIFY( !proj->isChanged() );

    VERIFY_EQUAL(QString("QTTest gpx file 0"),       proj->getName());
    VERIFY_EQUAL(QString("QTTest gpx input file 0"), proj->getDescription());

    QVERIFY( IGisProject::eTypeGpx == proj->getType() );

    VERIFY_EQUAL(0, proj->getItemCountByType(IGisItem::eTypeWpt));
    VERIFY_EQUAL(1, proj->getItemCountByType(IGisItem::eTypeTrk));
    VERIFY_EQUAL(0, proj->getItemCountByType(IGisItem::eTypeRte));
    VERIFY_EQUAL(0, proj->getItemCountByType(IGisItem::eTypeOvl));

    for(int i = 0; i < proj->childCount(); i++)
    {
        IGisItem *item = dynamic_cast<IGisItem*>(proj->child(i));

        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(item);
        if(nullptr != wpt)
        {
            QVERIFY( wpt->getPosition() != QPointF(0., 0.) );
            QVERIFY( wpt->getName()     != "" );
        }

        CGisItemTrk *itemTrk = dynamic_cast<CGisItemTrk*>(item);
        if(nullptr != itemTrk)
        {
            const CGisItemTrk::trk_t &trk = itemTrk->getTrackData();

            VERIFY_EQUAL(1, trk.segs.count());

            int trkptCount = 0;
            foreach(const CGisItemTrk::trkseg_t &seg, trk.segs)
            {
                trkptCount += seg.pts.count();

                foreach(const CGisItemTrk::trkpt_t &trkpt, seg.pts)
                {
                    QVERIFY( (0. != trkpt.lat) && (0. != trkpt.lon) );
                }
            }

            // there are 521 trkpts (.gpx files are not filtered)
            VERIFY_EQUAL(521, trkptCount);
        }
    }

    delete proj;
}

