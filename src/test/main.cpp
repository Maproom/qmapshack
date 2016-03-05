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

#include <QDebug>
#include <QTemporaryFile>

#include "test/test_QMapShack.h"
#include "test/TestHelper.h"

#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CAppSetup.h"
#include "helpers/CCommandProcessor.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

CAppOpts *qlOpts;
QString testInput;

void test_QMapShack::initTestCase()
{
    CCommandProcessor cmdParse;
    qlOpts = cmdParse.processOptions(QStringList(""));

    SETTINGS;
    IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt());
    CKnownExtension::init(IUnit::self());

    testInput = QCoreApplication::applicationDirPath() + "/input/";
}

void test_QMapShack::verify(const QString &expectFile, const IGisProject &proj)
{
    // step 0: read expected values from .xml file
    expectedGisProject exp = TestHelper::readExpProj(expectFile);

    // step 1: do the actual verification
    VERIFY_EQUAL(true,  proj.isValid());
    VERIFY_EQUAL(false, proj.isChanged()); //< projects should never be changed after loading

    VERIFY_EQUAL(exp.name, proj.getName());
    VERIFY_EQUAL(exp.desc, proj.getDescription());

    VERIFY_EQUAL(exp.wpts.count(), proj.getItemCountByType(IGisItem::eTypeWpt));
    VERIFY_EQUAL(exp.trks.count(), proj.getItemCountByType(IGisItem::eTypeTrk));
    VERIFY_EQUAL(exp.rtes.count(), proj.getItemCountByType(IGisItem::eTypeRte));
    VERIFY_EQUAL(exp.ovls.count(), proj.getItemCountByType(IGisItem::eTypeOvl));

    for(int i = 0; i < proj.childCount(); i++)
    {
        IGisItem *item = dynamic_cast<IGisItem*>(proj.child(i));

        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(item);
        if(nullptr != wpt)
        {
            VERIFY_EQUAL(true, exp.wpts.contains(wpt->getName()));
            exp.wpts.remove(wpt->getName());

            SUBVERIFY(wpt->getPosition() != QPointF(0., 0.), "Waypoint has position 0/0");
        }

        CGisItemTrk *itemTrk = dynamic_cast<CGisItemTrk*>(item);
        if(nullptr != itemTrk)
        {
            const CGisItemTrk::trk_t &trk = itemTrk->getTrackData();

            SUBVERIFY(exp.trks.contains(itemTrk->getName()), QString("Found track `%1`, there shouldn't be any track with that name").arg(itemTrk->getName()));

            const expectedTrack &expTrk = exp.trks.take(itemTrk->getName());

            int trkptCount = 0;
            foreach(const CGisItemTrk::trkseg_t &seg, trk.segs)
            {
                trkptCount += seg.pts.count();

                foreach(const CGisItemTrk::trkpt_t &trkpt, seg.pts)
                {
                    SUBVERIFY((0. != trkpt.lat) || (0. != trkpt.lon), "Trackpoint has position 0/0");
                }
            }

            VERIFY_EQUAL(expTrk.segCount, trk.segs.count());
            VERIFY_EQUAL(expTrk.ptCount,  trkptCount);
            VERIFY_EQUAL(expTrk.colorIdx, itemTrk->getColorIdx());

            QStringList existingSources = itemTrk->getExistingDataSources();
            existingSources.sort();
            SUBVERIFY(expTrk.colorSources == existingSources, "Expected and existing list of colorSources do not match");
        }

        CGisItemRte *itemRte = dynamic_cast<CGisItemRte*>(item);
        if(nullptr != itemRte)
        {
            SUBVERIFY(exp.rtes.contains(itemRte->getName()), QString("Found route `%1`, there shouldn't be any route with that name").arg(itemRte->getName()));
            const CGisItemRte::rte_t &rte = itemRte->getRoute();

            const expectedRoute &expRte = exp.rtes.take(itemRte->getName());

            VERIFY_EQUAL(expRte.ptCount, rte.pts.size());
        }

        CGisItemOvlArea *itemOvl = dynamic_cast<CGisItemOvlArea*>(item);
        if(nullptr != itemOvl)
        {
            SUBVERIFY(exp.ovls.contains(itemOvl->getName()), QString("Found area `%1`, there shouldn't be any area with that name").arg(itemOvl->getName()));

            const expectedArea &expOvl = exp.ovls.take(itemOvl->getName());
            VERIFY_EQUAL(expOvl.colorIdx, itemOvl->getColorIdx());

            const CGisItemOvlArea::area_t &area = itemOvl->getAreaData();
            VERIFY_EQUAL(expOvl.ptCount, area.pts.size());
        }
    }

    // ensure all expected waypoints/tracks actually exist
    SUBVERIFY(exp.wpts.isEmpty(), "Not all expected waypoints found");
    SUBVERIFY(exp.trks.isEmpty(), "Not all expected tracks found");
    SUBVERIFY(exp.rtes.isEmpty(), "Not all expected routes found");
    SUBVERIFY(exp.ovls.isEmpty(), "Not all expected areas found");
}

QTEST_MAIN(test_QMapShack)
