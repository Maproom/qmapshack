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
#include "helpers/CSettings.h"
#include "units/IUnit.h"

QVector<CActivityTrk::desc_t> CActivityTrk::actDescriptor =
{
    { // 0
        "Foot"
        , CGisItemTrk::trkpt_t::eActFoot
        , tr("Foot")
        , "://icons/48x48/ActFoot.png"
        , "://icons/16x16/ActFoot.png"
        , IGisItem::colorMap[0].color
    },
    { // 1
        "Cycle"
        , CGisItemTrk::trkpt_t::eActCycle
        , tr("Bicycle")
        , "://icons/48x48/ActCycle.png"
        , "://icons/16x16/ActCycle.png"
        , IGisItem::colorMap[1].color
    },
    { // 2
        "Bike"
        , CGisItemTrk::trkpt_t::eActBike
        , tr("Motor Bike")
        , "://icons/48x48/ActBike.png"
        , "://icons/16x16/ActBike.png"
        , IGisItem::colorMap[2].color
    },
    { // 3
        "Car"
        , CGisItemTrk::trkpt_t::eActCar
        , tr("Car")
        , "://icons/48x48/ActCar.png"
        , "://icons/16x16/ActCar.png"
        , IGisItem::colorMap[3].color
    },
    { // 4
        "Cable"
        , CGisItemTrk::trkpt_t::eActCable
        , tr("Cable Car")
        , "://icons/48x48/ActCable.png"
        , "://icons/16x16/ActCable.png"
        , IGisItem::colorMap[4].color
    },
    { // 5
        "Swim"
        , CGisItemTrk::trkpt_t::eActSwim
        , tr("Swim")
        , "://icons/48x48/ActSwim.png"
        , "://icons/16x16/ActSwim.png"
        , IGisItem::colorMap[5].color
    },
    { // 6
        "Ship"
        , CGisItemTrk::trkpt_t::eActShip
        , tr("Ship")
        , "://icons/48x48/ActShip.png"
        , "://icons/16x16/ActShip.png"
        , IGisItem::colorMap[6].color
    },
    { // 7
        "Aeronautik"
        , CGisItemTrk::trkpt_t::eActAero
        , tr("Aeronautik")
        , "://icons/48x48/ActAero.png"
        , "://icons/16x16/ActAero.png"
        , IGisItem::colorMap[7].color
    },
    { // 8
        "Ski/Winter"
        , CGisItemTrk::trkpt_t::eActSki
        , tr("Ski/Winter")
        , "://icons/48x48/ActSki.png"
        , "://icons/16x16/ActSki.png"
        , IGisItem::colorMap[8].color
    }
};

CActivityTrk::CActivityTrk(CGisItemTrk * trk)
    : trk(trk)
    , allFlags(0)
{
}

void CActivityTrk::init()
{
    actDescriptor[0].name = tr("Foot");
    actDescriptor[1].name = tr("Bicycle");
    actDescriptor[2].name = tr("Motor Bike");
    actDescriptor[3].name = tr("Car");
    actDescriptor[4].name = tr("Cable Car");
    actDescriptor[5].name = tr("Swim");
    actDescriptor[6].name = tr("Ship");
    actDescriptor[7].name = tr("Aeronautics");
    actDescriptor[8].name = tr("Ski/Winter");

    SETTINGS;
    cfg.beginGroup("Activities");
    int i = 0;
    for(desc_t &desc : actDescriptor)
    {
        desc.color = QColor(cfg.value(QString("color%1").arg(i), desc.color.name()).toString());
        ++i;
    }
    cfg.endGroup(); // Activities
}

void CActivityTrk::release()
{
    SETTINGS;
    cfg.beginGroup("Activities");
    int i = 0;
    for(desc_t &desc : actDescriptor)
    {
        cfg.setValue(QString("color%1").arg(i), desc.color.name());
        ++i;
    }
    cfg.endGroup(); // Activities
}


void CActivityTrk::update()
{
    allFlags = 0;
    activityRanges.clear();
    activitySummary.clear();

    const CGisItemTrk::trk_t&   data       = trk->getTrackData();
    const CGisItemTrk::trkpt_t *lastTrkpt  = nullptr;
    const CGisItemTrk::trkpt_t *startTrkpt = nullptr;

    quint32 lastFlag = 0xFFFFFFFF;
    for(const CGisItemTrk::trkseg_t &seg : data.segs)
    {
        for(const CGisItemTrk::trkpt_t &pt : seg.pts)
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
                    activity_summary_t& summary = activitySummary[lastFlag];
                    summary.distance += pt.distance - startTrkpt->distance;
                    summary.ascend   += pt.ascend   - startTrkpt->ascend;
                    summary.descend  += pt.descend  - startTrkpt->descend;
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

    activity_summary_t& summary = activitySummary[lastFlag];
    summary.distance += lastTrkpt->distance - startTrkpt->distance;
    summary.ascend   += lastTrkpt->ascend   - startTrkpt->ascend;
    summary.descend  += lastTrkpt->descend  - startTrkpt->descend;
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

void CActivityTrk::printSummary(const QMap<uint32_t, activity_summary_t>& summary, quint32 flags, QString& str)
{
    QString val, unit;

    if(flags == 0)
    {
        const activity_summary_t& s = summary[CGisItemTrk::trkpt_t::eActMaxNum];

        str += "<table>";
        IUnit::self().meter2distance(s.distance, val, unit);
        str += "<tr><td>" + tr("Distance:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2elevation(s.ascend, val, unit);
        str += "<tr><td>" + tr("Ascend:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2elevation(s.descend, val, unit);
        str += "<tr><td>" + tr("Descend:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2speed(s.distance/s.ellapsedSecondsMoving, val, unit);
        str += "<tr><td>" + tr("Speed Moving:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().meter2speed(s.distance/s.ellapsedSeconds, val, unit);
        str += "<tr><td>" + tr("Speed Total:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().seconds2time(s.ellapsedSecondsMoving, val, unit);
        str += "<tr><td>" + tr("Time Moving:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        IUnit::self().seconds2time(s.ellapsedSeconds, val, unit);
        str += "<tr><td>" + tr("Time Total:") + QString("&nbsp;&nbsp;</td><td>%1 %2</td></tr>").arg(val).arg(unit);
        str += "</table>";
        return;
    }

    str += "<table>";


    // gather any used activities
    QVector<const desc_t*> descs;
    for(const desc_t &desc : actDescriptor)
    {
        if(flags & desc.flag)
        {
            descs << &desc;
        }
    }


    // ############### build header ###############
    str += "<tr>";
    str += "<th></th>";
    for(const desc_t *desc : descs)
    {
        str += QString("<th align='right'><img src='%1'/></th>").arg(desc->iconSmall);
    }
    str += "</tr>";

    // ############### build Distance row ###############
    str += "<tr>";
    str += "<td>" + tr("Distance:") + "</td>";
    for(const desc_t *desc : descs)
    {
        const activity_summary_t& s = summary[desc->flag];
        IUnit::self().meter2distance(s.distance, val, unit);
        str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
    }
    str += "</tr>";

    // ############### build Ascend row ###############
    str += "<tr>";
    str += "<td>" + tr("Ascend:") + "</td>";
    for(const desc_t *desc : descs)
    {
        const activity_summary_t& s = summary[desc->flag];
        IUnit::self().meter2elevation(s.ascend, val, unit);
        str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
    }
    str += "</tr>";

    // ############### build Descend row ###############
    str += "<tr>";
    str += "<td>" + tr("Descend:") + "</td>";
    for(const desc_t *desc : descs)
    {
        const activity_summary_t& s = summary[desc->flag];
        IUnit::self().meter2elevation(s.descend, val, unit);
        str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
    }
    str += "</tr>";

    // ############### build Speed Moving row ###############
    str += "<tr>";
    str += "<td>" + tr("Speed Moving:") + "</td>";
    for(const desc_t *desc : descs)
    {
       const activity_summary_t& s = summary[desc->flag];
       IUnit::self().meter2speed(s.distance/s.ellapsedSecondsMoving, val, unit);
       str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
    }
    str += "</tr>";

    // ############### build Speed row ###############
    str += "<tr>";
    str += "<td>" + tr("Speed Total:") + "</td>";
    for(const desc_t *desc : descs)
    {
        const activity_summary_t& s = summary[desc->flag];
        IUnit::self().meter2speed(s.distance/s.ellapsedSeconds, val, unit);
        str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
    }
    str += "</tr>";

    // ############### build Time Moving row ###############
    str += "<tr>";
    str += "<td>" + tr("Time Moving:") + "</td>";
    for(const desc_t *desc : descs)
    {
        const activity_summary_t& s = summary[desc->flag];
        IUnit::self().seconds2time(s.ellapsedSecondsMoving, val, unit);
        str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
    }
    str += "</tr>";

    // ############### build Time Moving row ###############
    str += "<tr>";
    str += "<td>" + tr("Time Total:") + "</td>";
    for(const desc_t *desc : descs)
    {
        const activity_summary_t& s = summary[desc->flag];
        IUnit::self().seconds2time(s.ellapsedSeconds, val, unit);
        str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val).arg(unit);
    }
    str += "</tr>";

    str += "</table>";
}

void CActivityTrk::sumUp(QMap<uint32_t, activity_summary_t> &summary) const
{
    for(uint32_t flag : activitySummary.keys())
    {
        const activity_summary_t &src = activitySummary[flag];
        activity_summary_t       &dst = summary[flag];

        dst.distance += src.distance;
        dst.ascend   += src.ascend;
        dst.descend  += src.descend;
        dst.ellapsedSeconds += src.ellapsedSeconds;
        dst.ellapsedSecondsMoving += src.ellapsedSecondsMoving;
    }
}

const CActivityTrk::desc_t& CActivityTrk::getDescriptor(quint32 flag)
{
    auto it = std::find_if(actDescriptor.begin(), actDescriptor.end(), [flag](const desc_t &desc) {
        return desc.flag == flag;
    });

    static desc_t dummyDesc;
    return actDescriptor.end() == it ? dummyDesc : *it;
}

void CActivityTrk::setColor(quint32 flag, const QString& color)
{
    for(desc_t &desc : actDescriptor)
    {
        if(desc.flag == flag)
        {
            desc.color = QColor(color);
            return;
        }
    }
}

void CActivityTrk::getActivityNames(QStringList& names) const
{
    for(const desc_t &desc : actDescriptor)
    {
        if((allFlags & desc.flag) != 0)
        {
            names << desc.name;
        }
    }
}
