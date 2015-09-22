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

const CActivityTrk::desc_t CActivityTrk::actDescriptor[] =
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
        "Ship"
        , CGisItemTrk::trkpt_t::eActShip
        , QObject::tr("Ship")
        , "://icons/48x48/ActShip.png"
        , "://icons/16x16/ActShip.png"
    },

    {
        ""
        , 0
        , ""
        , ""
    }

};

CActivityTrk::CActivityTrk(CGisItemTrk* trk)
    : trk(trk)
    , allFlags(0)
    , summaries(CGisItemTrk::trkpt_t::eActMaxNum + 1)
{
}

CActivityTrk::~CActivityTrk()
{
}

void CActivityTrk::update()
{
    allFlags = 0;
    activities.clear();
    for(int i = 0; i < summaries.size(); i++)
    {
        summaries[i].reset();
    }

    const CGisItemTrk::trk_t&       data = trk->getTrackData();
    const CGisItemTrk::trkpt_t *    lastTrkpt = 0;
    const CGisItemTrk::trkpt_t *    startTrkpt = 0;

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
                if(startTrkpt != 0)
                {
                    summary_t& summary = getSummary(lastFlag);
                    summary.distance += pt.distance - startTrkpt->distance;
                    summary.ascend += pt.ascend - startTrkpt->ascend;
                    summary.descend += pt.descend - startTrkpt->descend;
                    summary.ellapsedSeconds += pt.elapsedSeconds - startTrkpt->elapsedSeconds;
                    summary.ellapsedSecondsMoving += pt.elapsedSecondsMoving - startTrkpt->elapsedSecondsMoving;
                }

                startTrkpt  = &pt;
                lastFlag    = pt.flags;
            }
        }
    }
    summary_t& summary = getSummary(lastFlag);
    summary.distance += lastTrkpt->distance - startTrkpt->distance;
    summary.ascend += lastTrkpt->ascend - startTrkpt->ascend;
    summary.descend += lastTrkpt->descend - startTrkpt->descend;
    summary.ellapsedSeconds += lastTrkpt->elapsedSeconds - startTrkpt->elapsedSeconds;
    summary.ellapsedSecondsMoving += lastTrkpt->elapsedSecondsMoving - startTrkpt->elapsedSecondsMoving;

    allFlags &= CGisItemTrk::trkpt_t::eActMask;

    for(int i = 0; i < 9; i++)
    {
        summary_t& stat   = summaries[i];
        qDebug() << "--------------" << i << "--------------";
        qDebug() << "stat.distance" << stat.distance;
        qDebug() << "stat.ascend" << stat.ascend;
        qDebug() << "stat.descend" << stat.descend;
        qDebug() << "stat.timeMoving" << stat.ellapsedSecondsMoving;
        qDebug() << "stat.timeTotal" << stat.ellapsedSeconds;
    }
}

void CActivityTrk::printSummary(QString& str) const
{
    quint32 mask;
    QString val, unit;

    str += "<table>";

    // ############### build header ###############
    str += "<tr>";
    str += "<th></th>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            str += QString("<th align='center'><img src='%1'/></th>").arg(actDescriptor[i].iconSmall);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Distance row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Distance:") + "</td>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            const summary_t& summary = getSummary(mask);
            IUnit::self().meter2distance(summary.distance, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Ascend row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Ascend:") + "</td>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            const summary_t& summary = getSummary(mask);
            IUnit::self().meter2elevation(summary.ascend, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Descend row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Descend:") + "</td>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            const summary_t& summary = getSummary(mask);
            IUnit::self().meter2elevation(summary.descend, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Speed Moving row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Speed Moving:") + "</td>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            const summary_t& summary = getSummary(mask);
            IUnit::self().meter2speed(summary.distance/summary.ellapsedSecondsMoving, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Speed row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Speed Total:") + "</td>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            const summary_t& summary = getSummary(mask);
            IUnit::self().meter2speed(summary.distance/summary.ellapsedSeconds, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Time Moving row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Time Moving:") + "</td>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            const summary_t& summary = getSummary(mask);
            IUnit::self().seconds2time(summary.ellapsedSecondsMoving, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    // ############### build Time Moving row ###############
    str += "<tr>";
    str += "<td>" + QObject::tr("Time Total:") + "</td>";
    mask = 0x80000000;
    for(unsigned i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if(actDescriptor[i].objName.isEmpty())
        {
            break;
        }

        if((allFlags & mask) != 0)
        {
            const summary_t& summary = getSummary(mask);
            IUnit::self().seconds2time(summary.ellapsedSeconds, val, unit);
            str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
        }

        mask >>= 1;
    }
    str += "</tr>";

    str += "</table>";
}

CActivityTrk::summary_t &CActivityTrk::getSummary(quint32 flag)
{
    quint32 cnt = 0;
    flag >>= 24;

    while(((flag & 0x01) == 0) && (cnt < CGisItemTrk::trkpt_t::eActMaxNum))
    {
        cnt++;
        flag >>= 1;
    }

    return summaries[cnt];
}

const CActivityTrk::summary_t &CActivityTrk::getSummary(quint32 flag) const
{
    quint32 cnt = 0;
    flag >>= 24;

    while(((flag & 0x01) == 0) && (cnt < CGisItemTrk::trkpt_t::eActMaxNum))
    {
        cnt++;
        flag >>= 1;
    }

    return summaries[cnt];
}
