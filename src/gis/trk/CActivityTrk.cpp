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

CActivityTrk::CActivityTrk(CGisItemTrk* trk)
    : trk(trk)
    , summaries(CGisItemTrk::trkpt_t::eActMaxNum + 1)
{
}

CActivityTrk::~CActivityTrk()
{
}

void CActivityTrk::update()
{
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
