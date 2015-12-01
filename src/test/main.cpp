#include <QTest>

#include "helpers/CAppSetup.h"
#include "helpers/CCommandProcessor.h"

#include "gis/slf/CSlfProject.h"
#include "gis/slf/CSlfReader.h"

#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

#include "units/IUnit.h"
#include "gis/trk/CKnownExtension.h"
#include "helpers/CSettings.h"
#include "helpers/CAppSetup.h"
#include "helpers/CCommandProcessor.h"


CAppOpts *qlOpts;

template<typename T>
static void VERIFY_EQUAL(const T &expected, const T &actual)
{
    QVERIFY2( (expected == actual), QTest::toString(QString("Expected `%1`, got `%2`").arg(expected).arg(actual)) );
}

class test_CSlfReader : public QObject {
    Q_OBJECT

private:
    QString testInput;

private slots:

    void initTestCase()
    {
        CCommandProcessor cmdParse;
        qlOpts = cmdParse.processOptions(QStringList(""));

        SETTINGS;
        IUnit::self().setUnitType((IUnit::type_e)cfg.value("MainWindow/units",IUnit::eTypeMetric).toInt(), this);
        CKnownExtension::init(IUnit::self());

        testInput = QCoreApplication::applicationDirPath() + "/input/";
    }

    void readValidSLFFile()
    {
        // this does not ready anything, a bare CSlfProject is created
        CSlfProject *proj = new CSlfProject("qtt_slf_file0.slf", false);

        try
        {
            CSlfReader::readFile(testInput + "qtt_slf_file0.slf", proj);
        }
        catch(QString &errormsg)
        {
            QFAIL(QString("Did not expect any error, but got: `%1`").arg(errormsg).toStdString().c_str());
        }

        QVERIFY(  proj->isValid()   );
        QVERIFY( !proj->isChanged() );

        VERIFY_EQUAL(QString("qttest slf file 0"),        proj->getName());
        VERIFY_EQUAL(QString("QTTest .slf input file 0"), proj->getDescription());

	QVERIFY( IGisProject::eTypeSlf == proj->getType() );

        VERIFY_EQUAL(3, proj->getItemCountByType(IGisItem::eTypeWpt));
        VERIFY_EQUAL(1, proj->getItemCountByType(IGisItem::eTypeTrk)); // 2817 trkpts
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

                // there are only 2 segments, even if there are 3 waypoints, as 2 of them are at the very end of the track
                VERIFY_EQUAL(2, trk.segs.count());

                int trkptCount = 0;
                foreach(const CGisItemTrk::trkseg_t &seg, trk.segs)
                {
                    trkptCount += seg.pts.count();

                    foreach(const CGisItemTrk::trkpt_t &trkpt, seg.pts)
                    {
                        QVERIFY( (0. != trkpt.lat) && (0. != trkpt.lon) );
                    }
                }

                // there are 2764 trkpts (filtered)
                VERIFY_EQUAL(2764, trkptCount);
            }
        }

        delete proj;
    }

    void readNonExistingSLFFile()
    {
        // this does not ready anything, a bare CSlfProject is created
        CSlfProject *proj = new CSlfProject("qtt_slf_DOES_NOT_EXIST.slf", false);

        bool hadException = false;
        try
        {
            CSlfReader::readFile(testInput + "qtt_slf_DOES_NOT_EXIST.slf", proj);
        }
        catch(QString &errormsg)
        {
            QVERIFY( errormsg.contains("does not exist") );
            hadException = true;
        }
        QVERIFY( hadException );

        delete proj;
    }
};

QTEST_MAIN(test_CSlfReader)

#include "main.moc"
