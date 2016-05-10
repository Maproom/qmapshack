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

#include "TestHelper.h"
#include "test_QMapShack.h"

#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/fit/CFitProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/slf/CSlfProject.h"
#include "gis/slf/CSlfReader.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"
#include "setup/IAppSetup.h"

QString testInput;

void test_QMapShack::initTestCase()
{
    IAppSetup* env = IAppSetup::getPlatformInstance();
    env->processArguments();
    env->initLogHandler();
    env->initQMapShack();

    SETTINGS;
    IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt(), nullptr);
    CKnownExtension::init(IUnit::self());

    testInput = QCoreApplication::applicationDirPath() + "/input/";

    inputFiles =
    {
        "qtt_gpx_file0.gpx"
        , "gpx_ext_GarminTPX1_gpxtpx.gpx"
        , "gpx_ext_GarminTPX1_tp1.gpx"
        , "V1.6.0_file1.qms"
        , "V1.6.0_file2.qms"
    };
}

void test_QMapShack::verify(expectedGisProject exp, const IGisProject &proj)
{
    VERIFY_EQUAL(true,        proj.isValid());
    VERIFY_EQUAL(exp.changed, proj.isChanged());

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
            for(const CGisItemTrk::trkseg_t &seg : trk.segs)
            {
                trkptCount += seg.pts.count();

                for(const CGisItemTrk::trkpt_t &trkpt : seg.pts)
                {
                    SUBVERIFY((0. != trkpt.lat) || (0. != trkpt.lon), "Trackpoint has position 0/0");

                    for(const QString &key : expTrk.extensions.keys())
                    {
                        VERIFY_EQUAL(expTrk.extensions[key].known, CKnownExtension::isKnown(key));
                        if(expTrk.extensions[key].everyPoint)
                        {
                            SUBVERIFY(trkpt.extensions.contains(key), QString("Missing extension `%1`on trackpoint").arg(key));
                        }
                    }
                }
            }

            VERIFY_EQUAL(expTrk.segCount, trk.segs.count());
            VERIFY_EQUAL(expTrk.ptCount,  trkptCount);
            VERIFY_EQUAL(expTrk.colorIdx, itemTrk->getColorIdx());

            QStringList existingSources = itemTrk->getExistingDataSources();
            for(const QString &ext : expTrk.extensions.keys())
            {
                SUBVERIFY(existingSources.contains(ext), QString("Missing extension `%1`").arg(ext));
                existingSources.removeOne(ext);
            }

            auto accuFunc = [](const QString &accu, const QString &b) { return accu.isEmpty() ? b : QString("%1, %2").arg(accu).arg(b); };
            QString remainingExts = std::accumulate(existingSources.cbegin(), existingSources.cend(), QString(), accuFunc);

            SUBVERIFY(existingSources.isEmpty(), QString("existingSources still contains: ") +  remainingExts);
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

void test_QMapShack::verify(const QString &projFile, const IGisProject &proj)
{
    expectedGisProject exp = TestHelper::readExpProj(fileToPath(projFile) + ".xml");
    verify(exp, proj);
}

void test_QMapShack::verify(const QString &projFile)
{
    IGisProject        *proj = readProjFile(projFile);
    expectedGisProject exp  = TestHelper::readExpProj(fileToPath(projFile) + ".xml");

    verify(exp, *proj);
    delete proj;
}

QString test_QMapShack::fileToPath(const QString &file)
{
    if(!QFileInfo(file).exists())
    {
        return testInput + "/" + file.right(3) + "/" + file;
    }
    return file;
}

IGisProject* test_QMapShack::readProjFile(const QString &file, bool valid, bool forceVerify)
{
    IGisProject *proj = nullptr;

    try
    {
        if(file.endsWith(".gpx"))
        {
            CGpxProject *gpxProj = new CGpxProject("a very random string to prevent loading via constructor", (CGisListWks*) nullptr);
            gpxProj->blockUpdateItems(true);
            CGpxProject::loadGpx(fileToPath(file), gpxProj);
            gpxProj->blockUpdateItems(false);
            proj = gpxProj;
            SUBVERIFY(IGisProject::eTypeGpx == proj->getType(), "Project has invalid type");
        }
        else if(file.endsWith(".qms"))
        {
            proj = new CQmsProject(fileToPath(file), (CGisListWks*) nullptr);
            SUBVERIFY(IGisProject::eTypeQms == proj->getType(), "Project has invalid type");
        }
        else if(file.endsWith(".slf"))
        {
            CSlfProject *slfProj = new CSlfProject("a very random string to prevent loading via constructor", false);
            proj = slfProj;
            CSlfReader::readFile(fileToPath(file), slfProj);
            SUBVERIFY(IGisProject::eTypeSlf == proj->getType(), "Project has invalid type");
        }
        else if(file.endsWith(".fit"))
        {
            proj = new CFitProject(fileToPath(file), (CGisListWks*) nullptr);
            SUBVERIFY(IGisProject::eTypeFit == proj->getType(), "Project has invalid type");
        }
        else
        {
            SUBVERIFY(false, "Internal error: Can't read project file `" + file + "`");
        }
    }
    catch(QString &errormsg)
    {
        SUBVERIFY(!valid, "Expected `" + file + "` to be valid, error while reading: " + errormsg);
        delete proj;
        proj = nullptr;
    }

    SUBVERIFY(valid || nullptr == proj, "File is neither valid, nor an exception was thrown");

    if(nullptr != proj)
    {
        const QString &projPath = fileToPath(file);
        SUBVERIFY(QFile(projPath + ".xml").exists() || !forceVerify, "Can't verify file `" + file + "`, .xml does not exist");

        if(QFile(projPath + ".xml").exists())
        {
            verify(file, *proj);
        }
    }

    return proj;
}

QTEST_MAIN(test_QMapShack)
