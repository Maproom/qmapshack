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

#include <QDebug>

#include "test/test_QMapShack.h"

#include "helpers/CAppSetup.h"
#include "helpers/CCommandProcessor.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

#include "gis/trk/CKnownExtension.h"

#include "gis/prj/IGisProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

struct expectedWaypoint
{
    QString name;
};

struct expectedTrack
{
    QString name;
    int     colorIdx;
    int     segCount;
    int     ptCount;
};

CAppOpts *qlOpts;
QString testInput;

void test_QMapShack::initTestCase()
{
    CCommandProcessor cmdParse;
    qlOpts = cmdParse.processOptions(QStringList(""));

    SETTINGS;
    IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt(), this);
    CKnownExtension::init(IUnit::self());

    testInput = QCoreApplication::applicationDirPath() + "/input/";
}

void test_QMapShack::verify(const QString &expectFile, const IGisProject &proj)
{
    QFile file(expectFile);

    QDomDocument xml;
    QString msg;
    int line;
    int column;
    if(!xml.setContent(&file, false, &msg, &line, &column))
    {
        QFAIL( QString("[%1:%2] %3").arg(expectFile).arg(line).arg(msg).toStdString().c_str() );
    }

    const QDomNode &exp = xml.namedItem("expected");

    const QString &expName = exp.namedItem("name").firstChild().nodeValue();
    const QString &expDesc = exp.namedItem("desc").firstChild().nodeValue();

    QHash<QString, expectedWaypoint> expWpts;
    const QDomNodeList &wptList = exp.namedItem("waypoints").childNodes();
    for(int i = 0; i < wptList.length(); i++)
    {
        const QDomNode &node = wptList.item(i);

        expectedWaypoint wpt;
        wpt.name = node.attributes().namedItem("name").nodeValue();

        expWpts.insert(wpt.name, wpt);
    }

    QHash<QString, expectedTrack> expTrks;
    const QDomNodeList &trkList = exp.namedItem("tracks").childNodes();
    for(int i = 0; i < trkList.length(); i++)
    {
        const QDomNode &node = trkList.item(i);

        expectedTrack trk;
        trk.name     = node.attributes().namedItem("name"      ).nodeValue();
        trk.segCount = node.attributes().namedItem("segcount"  ).nodeValue().toInt();
        trk.ptCount  = node.attributes().namedItem("pointcount").nodeValue().toInt();
        trk.colorIdx = node.attributes().namedItem("colorIdx"  ).nodeValue().toInt();

        expTrks.insert(trk.name, trk);
    }

    //const QDomNodeList &expRtes = exp.namedItem("routes"   ).childNodes(); TODO
    //const QDomNodeList &expOvls = exp.namedItem("areas"    ).childNodes(); TODO


    QVERIFY(  proj.isValid()   );
    QVERIFY( !proj.isChanged() );

    VERIFY_EQUAL(expName, proj.getName());
    VERIFY_EQUAL(expDesc, proj.getDescription());

    VERIFY_EQUAL(expWpts.count(), proj.getItemCountByType(IGisItem::eTypeWpt));
    VERIFY_EQUAL(expTrks.count(), proj.getItemCountByType(IGisItem::eTypeTrk));
    VERIFY_EQUAL(0, proj.getItemCountByType(IGisItem::eTypeRte)); // TODO
    VERIFY_EQUAL(0, proj.getItemCountByType(IGisItem::eTypeOvl)); // TODO

    for(int i = 0; i < proj.childCount(); i++)
    {
        IGisItem *item = dynamic_cast<IGisItem*>(proj.child(i));

        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(item);
        if(nullptr != wpt)
        {
            VERIFY_EQUAL(true, expWpts.contains(wpt->getName()));
            expWpts.remove(wpt->getName());

            QVERIFY( wpt->getPosition() != QPointF(0., 0.) );
        }

        CGisItemTrk *itemTrk = dynamic_cast<CGisItemTrk*>(item);
        if(nullptr != itemTrk)
        {
            const CGisItemTrk::trk_t &trk = itemTrk->getTrackData();

            if(!expTrks.contains(itemTrk->getName()))
            {
                QFAIL( QString("Found track `%1`, there shouldn't be any track with that name").arg(itemTrk->getName()).toStdString().c_str() );
            }
            else
            {
                const expectedTrack &expTrk = expTrks.take(itemTrk->getName());

                VERIFY_EQUAL(expTrk.colorIdx, itemTrk->getColorIdx());

                VERIFY_EQUAL(expTrk.segCount, trk.segs.count());


                int trkptCount = 0;
                foreach(const CGisItemTrk::trkseg_t &seg, trk.segs)
                {
                    trkptCount += seg.pts.count();

                    foreach(const CGisItemTrk::trkpt_t &trkpt, seg.pts)
                    {
                        QVERIFY( (0. != trkpt.lat) && (0. != trkpt.lon) );
                    }
                }

                VERIFY_EQUAL(expTrk.ptCount, trkptCount);
            }
        }
    }

    // ensure all expected waypoints/tracks actually exist
    QVERIFY( expWpts.isEmpty() );
    QVERIFY( expTrks.isEmpty() );
}

QTEST_MAIN(test_QMapShack)
