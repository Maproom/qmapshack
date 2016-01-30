/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/trk/CActivityTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "units/IUnit.h"

CActivityTrk::desc_t CActivityTrk::actDescriptor[] =
{
    {
        "Foot"
        , CGisItemTrk::trkpt_t::eActFoot
        , QObject::tr("Foot")
        , "://icons/48x48/ActFoot.png"
        , "://icons/16x16/ActFoot.png"
    },
    {
        "Cycle"
        , CGisItemTrk::trkpt_t::eActCycle
        , QObject::tr("Bicycle")
        , "://icons/48x48/ActCycle.png"
        , "://icons/16x16/ActCycle.png"
    },
    {
        "Bike"
        , CGisItemTrk::trkpt_t::eActBike
        , QObject::tr("Motor Bike")
        , "://icons/48x48/ActBike.png"
        , "://icons/16x16/ActBike.png"
    },
    {
        "Car"
        , CGisItemTrk::trkpt_t::eActCar
        , QObject::tr("Car")
        , "://icons/48x48/ActCar.png"
        , "://icons/16x16/ActCar.png"
    },
    {
        "Cable"
        , CGisItemTrk::trkpt_t::eActCable
        , QObject::tr("Cable Car")
        , "://icons/48x48/ActCable.png"
        , "://icons/16x16/ActCable.png"
    },
    {
        "Swim"
        , CGisItemTrk::trkpt_t::eActSwim
        , QObject::tr("Swim")
        , "://icons/48x48/ActSwim.png"
        , "://icons/16x16/ActSwim.png"
    },
    {
        "Ship"
        , CGisItemTrk::trkpt_t::eActShip
        , QObject::tr("Ship")
        , "://icons/48x48/ActShip.png"
        , "://icons/16x16/ActShip.png"
    },
    {
        "Aeronautik"
        , CGisItemTrk::trkpt_t::eActAero
        , QObject::tr("Aeronautik")
        , "://icons/48x48/ActAero.png"
        , "://icons/16x16/ActAero.png"
    },
    {
        "Ski/Winter"
        , CGisItemTrk::trkpt_t::eActSki
        , QObject::tr("Ski/Winter")
        , "://icons/48x48/ActSki.png"
        , "://icons/16x16/ActSki.png"
    },
    {
        ""
        , 0
        , ""
        , ""
    }
};

CActivityTrk::CActivityTrk(CGisItemTrk * trk)
    : trk(trk)
    , allFlags(0)
    , activitySummary(CGisItemTrk::trkpt_t::eActMaxNum + 1)
{
    actDescriptor[0].name = QObject::tr("Foot");
    actDescriptor[1].name = QObject::tr("Bicycle");
    actDescriptor[2].name = QObject::tr("Motor Bike");
    actDescriptor[3].name = QObject::tr("Car");
    actDescriptor[4].name = QObject::tr("Cable Car");
    actDescriptor[5].name = QObject::tr("Swim");
    actDescriptor[6].name = QObject::tr("Ship");
    actDescriptor[7].name = QObject::tr("Aeronautics");
    actDescriptor[8].name = QObject::tr("Ski/Winter");
}


void CActivityTrk::update()
{
    allFlags = 0;
    activityRanges.clear();
    for(int i = 0; i < activitySummary.size(); i++)
    {
        activitySummary[i].reset();
    }

    const CGisItemTrk::trk_t&       data = trk->getTrackData();
    const CGisItemTrk::trkpt_t *    lastTrkpt = nullptr;
    const CGisItemTrk::trkpt_t *    startTrkpt = nullptr;

    quint32 lastFlag = 0xFFFFFFFF;
    foreach(const CGisItemTrk::trkseg_t &seg, data.segs)
    {
        foreach(const CGisItemTrk::trkpt_t &pt, seg.pts)
        {
            allFlags |= pt.flags;

            if(pt.flags & CGisItemTrk::trkpt_t::eHidden)
            {
                continue;
            }
            lastTrkpt = &pt;
            if(pt.flags != lastFlag)
            {
                if(startTrkpt != nullptr)
                {
                    activity_summary_t& summary = getSummary(activitySummary, lastFlag);
                    summary.distance += pt.distance - startTrkpt->distance;
                    summary.ascend += pt.ascend - startTrkpt->ascend;
                    summary.descend += pt.descend - startTrkpt->descend;
                    summary.ellapsedSeconds += pt.elapsedSeconds - startTrkpt->elapsedSeconds;
                    summary.ellapsedSecondsMoving += pt.elapsedSecondsMoving - startTrkpt->elapsedSecondsMoving;

                    activityRanges << activity_range_t();
                    activity_range_t& activity = activityRanges.last();

                    activity.d1 = startTrkpt->distance;
                    activity.d2 = pt.distance;
                    activity.t1 = startTrkpt->time.toTime_t();
                    activity.t2 = pt.time.toTime_t();

                    const desc_t& desc = getDescriptor(lastFlag);
                    activity.name = desc.name;
                    activity.icon = desc.iconSmall;
                }

                startTrkpt  = &pt;
                lastFlag    = pt.flags;
            }
        }
    }

    if(lastTrkpt == nullptr)
    {
        return;
    }

    activity_summary_t& summary = getSummary(activitySummary, lastFlag);
    summary.distance += lastTrkpt->distance - startTrkpt->distance;
    summary.ascend += lastTrkpt->ascend - startTrkpt->ascend;
    summary.descend += lastTrkpt->descend - startTrkpt->descend;
    summary.ellapsedSeconds += lastTrkpt->elapsedSeconds - startTrkpt->elapsedSeconds;
    summary.ellapsedSecondsMoving += lastTrkpt->elapsedSecondsMoving - startTrkpt->elapsedSecondsMoving;

    activityRanges << activity_range_t();
    activity_range_t& activity = activityRanges.last();

    activity.d1 = startTrkpt->distance;
    activity.d2 = lastTrkpt->distance;
    activity.t1 = startTrkpt->time.toTime_t();
    activity.t2 = lastTrkpt->time.toTime_t();

    const desc_t& desc = getDescriptor(lastFlag);
    activity.name = desc.name;
    activity.icon = desc.iconSmall;



    allFlags &= CGisItemTrk::trkpt_t::eActMask;

//    for(int i = 0; i < 9; i++)
//    {
//        activity_summary_t& stat   = summaries[i];
//        qDebug() << "--------------" << i << "--------------";
//        qDebug() << "stat.distance" << stat.distance;
//        qDebug() << "stat.ascend" << stat.ascend;
//        qDebug() << "stat.descend" << stat.descend;
//        qDebug() << "stat.timeMoving" << stat.ellapsedSecondsMoving;
//        qDebug() << "stat.timeTotal" << stat.ellapsedSeconds;
//    }
}

void CActivityTrk::printSummary(QString& str) const
{
    printSummary(activitySummary, allFlags, str);
}

void CActivityTrk::printSummary(const QVector<activity_summary_t>& summary, quint32 flags, QString& str)
{
    quint32 mask;
    QString val, unit;

    if((flags == 0) && (summary.size() >= (int)CGisItemTrk::trkpt_t::eActMaxNum))
    {
        const activity_summary_t& s = summary[CGisItemTrk::trkpt_t::eActMaxNum];

        str += "<table>";
        IUnit::self().meter2distance(s.distance, val, unit);
        str += "<tr><td>" + QObject::tr("Distance:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2elevation(s.ascend, val, unit);
        str += "<tr><td>" + QObject::tr("Ascend:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2elevation(s.descend, val, unit);
        str += "<tr><td>" + QObject::tr("Descend:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2speed(s.distance/s.ellapsedSecondsMoving, val, unit);
        str += "<tr><td>" + QObject::tr("Speed Moving:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2speed(s.distance/s.ellapsedSeconds, val, unit);
        str += "<tr><td>" + QObject::tr("Speed Total:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().seconds2time(s.ellapsedSecondsMoving, val, unit);
        str += "<tr><td>" + QObject::tr("Time Moving:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().seconds2time(s.ellapsedSeconds, val, unit);
        str += "<tr><td>" + QObject::tr("Time Total:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        str += "</table>";
        return;
    }


    const int N = qMin((int)CGisItemTrk::trkpt_t::eActMaxNum, summary.size());

    str += "<table>";

    // ############### build header ###############
    str += "<tr>";
    str += "<th></th>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            str += QString("<th align='right'><img src='%1'/></th>").arg(actDescriptor[i].iconSmall);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Distance row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Distance:") + "</td>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            const activity_summary_t& s = getSummary(summary, mask);
            IUnit::self().meter2distance(s.distance, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Ascend row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Ascend:") + "</td>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            const activity_summary_t& s = getSummary(summary, mask);
            IUnit::self().meter2elevation(s.ascend, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Descend row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Descend:") + "</td>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            const activity_summary_t& s = getSummary(summary, mask);
            IUnit::self().meter2elevation(s.descend, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Speed Moving row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Speed Moving:") + "</td>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            const activity_summary_t& s = getSummary(summary, mask);
            IUnit::self().meter2speed(s.distance/s.ellapsedSecondsMoving, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Speed row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Speed Total:") + "</td>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            const activity_summary_t& s = getSummary(summary, mask);
            IUnit::self().meter2speed(s.distance/s.ellapsedSeconds, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Time Moving row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Time Moving:") + "</td>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            const activity_summary_t& s = getSummary(summary, mask);
            IUnit::self().seconds2time(s.ellapsedSecondsMoving, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Time Moving row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Time Total:") + "</td>";
    mask = 0x80000000;
    for(int i = 0; i < N; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((flags & mask) != 0)
        {
            const activity_summary_t& s = getSummary(summary, mask);
            IUnit::self().seconds2time(s.ellapsedSeconds, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    str += "</table>";
}

void CActivityTrk::sumUp(QVector<activity_summary_t> &summary) const
{
    const int N = qMin(activitySummary.size(), summary.size());
    for(int i = 0; i < N; i++)
    {
        const activity_summary_t& sum1 = activitySummary[i];
        activity_summary_t& sum2 = summary[i];

        sum2.distance += sum1.distance;
        sum2.ascend += sum1.ascend;
        sum2.descend += sum1.descend;
        sum2.ellapsedSeconds += sum1.ellapsedSeconds;
        sum2.ellapsedSecondsMoving += sum1.ellapsedSecondsMoving;
    }
}

const CActivityTrk::activity_summary_t &CActivityTrk::getSummary(const QVector<activity_summary_t>& summary, quint32 flag)
{
    qint32 cnt = 0;
    flag >>= 24;

    while(((flag & 0x01) == 0) && (cnt < qMin((int)CGisItemTrk::trkpt_t::eActMaxNum,summary.size())))
    {
        cnt++;
        flag >>= 1;
    }

    return summary[cnt];
}


CActivityTrk::activity_summary_t& CActivityTrk::getSummary(QVector<activity_summary_t> &summary, quint32 flag)
{
    qint32 cnt = 0;
    flag >>= 24;

    while(((flag & 0x01) == 0) && (cnt < qMin((int)CGisItemTrk::trkpt_t::eActMaxNum,summary.size())))
    {
        cnt++;
        flag >>= 1;
    }

    return summary[cnt];
}

const CActivityTrk::desc_t& CActivityTrk::getDescriptor(quint32 flag)
{
    int i = 0;
    while(!actDescriptor[i].objName.isEmpty())
    {
        if(actDescriptor[i].flag == flag)
        {
            break;
        }

        i++;
    }

    return actDescriptor[i];
}
