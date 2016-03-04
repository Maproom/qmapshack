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

struct expectedWaypoint
{
    QString name;
};

struct expectedTrack
{
    QString name;
    int colorIdx;
    int segCount;
    int ptCount;
    QStringList colorSources;
};

struct expectedRoute
{
    QString name;
    int ptCount;
};

struct expectedArea
{
    QString name;
    int colorIdx;
    int ptCount;
};

CAppOpts *qlOpts;
QString testInput;

CGpxProject* test_QMapShack::readGpxFile(const QString &file, bool valid)
{
    // this does not read anything, a bare CSlfProject is created
    CGpxProject *proj = new CGpxProject("a very random string to prevent loading via constructor", (CGisListWks*) nullptr);

    bool hadExc = false;
    try
    {
        proj->blockUpdateItems(true);
        CGpxProject::loadGpx(file, proj);
        proj->blockUpdateItems(false);
    }
    catch(QString &errormsg)
    {
        SUBVERIFY(!valid, "Expected `" + file + "` to be valid, error while reading: " + errormsg);
        hadExc = true;
    }

    SUBVERIFY(valid || hadExc, "File is neither valid, nor an exception was thrown")
    SUBVERIFY(IGisProject::eTypeGpx == proj->getType(), "Project has invalid type");

    return proj;
}



void test_QMapShack::initTestCase()
{
    CCommandProcessor cmdParse;
    qlOpts = cmdParse.processOptions(QStringList(""));

    SETTINGS;
    IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt());
    CKnownExtension::init(IUnit::self());

    testInput = QCoreApplication::applicationDirPath() + "/input/";
}

static QString getAttribute(const QDomNode &node, const QString &name)
{
    const QDomNamedNodeMap &attrs = node.attributes();
    if(!attrs.contains(name))
    {
        QWARN( QString("Attribute `%1` does not exist in DomNode `%2`").arg(name).arg(node.nodeName()).toStdString().c_str() );
        return QString();
    }

    return attrs.namedItem(name).nodeValue();
}

static QHash<QString, expectedArea> getExpectedAreas(const QDomNode &exp)
{
    QHash<QString, expectedArea> expOvls;
    const QDomNodeList &ovlList = exp.namedItem("areas").childNodes();
    for(int i = 0; i < ovlList.length(); i++)
    {
        const QDomNode &node = ovlList.item(i);

        expectedArea ovl;
        ovl.name     = getAttribute(node, "name");
        ovl.colorIdx = getAttribute(node, "colorIdx").toInt();
        ovl.ptCount  = getAttribute(node, "pointcount").toInt();

        expOvls.insert(ovl.name, ovl);
    }

    return expOvls;
}

static QHash<QString, expectedRoute> getExpectedRoutes(const QDomNode &exp)
{
    QHash<QString, expectedRoute> expRtes;
    const QDomNodeList &rteList = exp.namedItem("routes").childNodes();
    for(int i = 0; i < rteList.length(); i++)
    {
        const QDomNode &node = rteList.item(i);

        expectedRoute rte;
        rte.name    = getAttribute(node, "name");
        rte.ptCount = getAttribute(node, "pointcount").toInt();

        expRtes.insert(rte.name, rte);
    }

    return expRtes;
}

static QHash<QString, expectedTrack> getExpectedTracks(const QDomNode &exp)
{
    QHash<QString, expectedTrack> expTrks;
    const QDomNodeList &trkList = exp.namedItem("tracks").childNodes();
    for(int i = 0; i < trkList.length(); i++)
    {
        const QDomNode &node = trkList.item(i);

        expectedTrack trk;
        trk.name     = getAttribute(node, "name");
        trk.segCount = getAttribute(node, "segcount"  ).toInt();
        trk.ptCount  = getAttribute(node, "pointcount").toInt();
        trk.colorIdx = getAttribute(node, "colorIdx"  ).toInt();

        QStringList colorSources;
        const QDomNodeList &extList = node.namedItem("colorSources").childNodes();
        for(int j = 0; j < extList.length(); j++)
        {
            colorSources << getAttribute(extList.item(j), "name");
        }
        colorSources.sort();
        trk.colorSources = colorSources;

        expTrks.insert(trk.name, trk);
    }

    return expTrks;
}

static QHash<QString, expectedWaypoint> getExpectedWaypoints(const QDomNode &exp)
{
    QHash<QString, expectedWaypoint> expWpts;
    const QDomNodeList &wptList = exp.namedItem("waypoints").childNodes();
    for(int i = 0; i < wptList.length(); i++)
    {
        const QDomNode &node = wptList.item(i);

        expectedWaypoint wpt;
        wpt.name = node.attributes().namedItem("name").nodeValue();

        expWpts.insert(wpt.name, wpt);
    }

    return expWpts;
}

void test_QMapShack::verify(const QString &expectFile, const IGisProject &proj)
{
    // step 0: read expected values from .xml file
    QFile file(expectFile);

    QDomDocument xml;
    QString msg;
    int line;
    int column;
    SUBVERIFY(xml.setContent(&file, false, &msg, &line, &column), QString("[%1:%2] %3").arg(expectFile).arg(line).arg(msg));

    const QDomNode &exp = xml.namedItem("expected");

    const QString &expName = exp.namedItem("name").firstChild().nodeValue();
    const QString &expDesc = exp.namedItem("desc").firstChild().nodeValue();

    QHash<QString, expectedWaypoint> expWpts = getExpectedWaypoints(exp);
    QHash<QString, expectedTrack>    expTrks = getExpectedTracks(exp);
    QHash<QString, expectedRoute>    expRtes = getExpectedRoutes(exp);
    QHash<QString, expectedArea>     expOvls = getExpectedAreas(exp);

    // step 1: do the actual verification
    VERIFY_EQUAL(true,  proj.isValid());
    VERIFY_EQUAL(false, proj.isChanged()); //< projects should never be changed after loading

    VERIFY_EQUAL(expName, proj.getName());
    VERIFY_EQUAL(expDesc, proj.getDescription());

    VERIFY_EQUAL(expWpts.count(), proj.getItemCountByType(IGisItem::eTypeWpt));
    VERIFY_EQUAL(expTrks.count(), proj.getItemCountByType(IGisItem::eTypeTrk));
    VERIFY_EQUAL(expRtes.count(), proj.getItemCountByType(IGisItem::eTypeRte));
    VERIFY_EQUAL(expOvls.count(), proj.getItemCountByType(IGisItem::eTypeOvl));

    for(int i = 0; i < proj.childCount(); i++)
    {
        IGisItem *item = dynamic_cast<IGisItem*>(proj.child(i));

        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(item);
        if(nullptr != wpt)
        {
            VERIFY_EQUAL(true, expWpts.contains(wpt->getName()));
            expWpts.remove(wpt->getName());

            SUBVERIFY(wpt->getPosition() != QPointF(0., 0.), "Waypoint has position 0/0");
        }

        CGisItemTrk *itemTrk = dynamic_cast<CGisItemTrk*>(item);
        if(nullptr != itemTrk)
        {
            const CGisItemTrk::trk_t &trk = itemTrk->getTrackData();

            SUBVERIFY(expTrks.contains(itemTrk->getName()), QString("Found track `%1`, there shouldn't be any track with that name").arg(itemTrk->getName()));

            const expectedTrack &expTrk = expTrks.take(itemTrk->getName());

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
            SUBVERIFY(expRtes.contains(itemRte->getName()), QString("Found route `%1`, there shouldn't be any route with that name").arg(itemRte->getName()));
            const CGisItemRte::rte_t &rte = itemRte->getRoute();

            const expectedRoute &expRte = expRtes.take(itemRte->getName());

            VERIFY_EQUAL(expRte.ptCount, rte.pts.size());
        }

        CGisItemOvlArea *itemOvl = dynamic_cast<CGisItemOvlArea*>(item);
        if(nullptr != itemOvl)
        {
            SUBVERIFY(expOvls.contains(itemOvl->getName()), QString("Found area `%1`, there shouldn't be any area with that name").arg(itemOvl->getName()));

            const expectedArea &expOvl = expOvls.take(itemOvl->getName());
            VERIFY_EQUAL(expOvl.colorIdx, itemOvl->getColorIdx());

            const CGisItemOvlArea::area_t &area = itemOvl->getAreaData();
            VERIFY_EQUAL(expOvl.ptCount, area.pts.size());
        }
    }

    // ensure all expected waypoints/tracks actually exist
    SUBVERIFY(expWpts.isEmpty(), "Not all expected waypoints found");
    SUBVERIFY(expTrks.isEmpty(), "Not all expected tracks found");
    SUBVERIFY(expRtes.isEmpty(), "Not all expected routes found");
    SUBVERIFY(expOvls.isEmpty(), "Not all expected areas found");
}

QString test_QMapShack::getTempFileName(const QString &ext)
{
    QTemporaryFile tmp("qtt_XXXXXX." + ext);
    tmp.open();
    QString tempFile = tmp.fileName();
    tmp.remove();

    return tempFile;
}

QTEST_MAIN(test_QMapShack)
