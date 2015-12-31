/**********************************************************************************************
    Copyright (C) 2015 Ivo Kronenberg

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

#include "CMainWindow.h"
#include "gis/WptIcons.h"
#include "gis/fit/CFitProject.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"


static const qreal degrees = 180.0;
static const qreal twoPow31 = qPow(2, 31);
static const uint sec1970to1990 = QDateTime(QDate(1989, 12, 31), QTime(0, 0, 0),Qt::UTC).toTime_t();


class CFitDataConverter
{
public:

    /**
     * converts the semicircle to the WGS-84 geoids (Degrees Decimal Minutes (DDD MM.MMM)).
     * North latitude +, South latitute -
     * East longitude +, West longitude -
     *
       return: the given semicircle value converted to degree.
     */
    static qreal toDegree(int32_t semicircles)
    {
        qreal degree = 0;
        degree = semicircles * (degrees / twoPow31);
        return degree;
    }

    /**
       timestamp: seconds since UTC 00:00 Dec 31 1989
     */
    static QDateTime toDateTime(uint32_t timestamp)
    {
        QDateTime dateTime;
        dateTime.setTime_t(sec1970to1990 + timestamp);
        dateTime.setTimeSpec(Qt::UTC);
        return dateTime;
    }
};


bool readFitRecord(const CFitMessage &mesg, IGisItem::wpt_t &pt)
{
    if(mesg.isFieldValueValid(eRecordPositionLong) && mesg.isFieldValueValid(eRecordPositionLat))
    {
        pt.lon = CFitDataConverter::toDegree(mesg.getFieldIntValue(eRecordPositionLong));
        pt.lat = CFitDataConverter::toDegree(mesg.getFieldIntValue(eRecordPositionLat));
        pt.ele = (int) mesg.getFieldDoubleValue(eRecordAltitude);
        pt.time = CFitDataConverter::toDateTime(mesg.getFieldUIntValue(eRecordTimestamp));
        //pt.speed = mesg.getFieldDoubleValue(RecordSpeed);

        // see gis/trk/CKnownExtension for the keys of the extensions
        if(mesg.isFieldValueValid(eRecordHeartRate))
        {
            pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = QVariant(mesg.getFieldIntValue(eRecordHeartRate));
        }
        if(mesg.isFieldValueValid(eRecordTemperature))
        {
            pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:atemp"] = QVariant(mesg.getFieldIntValue(eRecordTemperature));
        }

        return true;
    }
    return false;
}

bool readFitRecord(const CFitMessage &mesg, CGisItemTrk::trkpt_t &pt)
{
    if(readFitRecord(mesg, (IGisItem::wpt_t &)pt))
    {
        pt.speed = mesg.getFieldDoubleValue(eRecordSpeed);

        // see gis/trk/CKnownExtension for the keys of the extensions
        if(mesg.isFieldValueValid(eRecordHeartRate))
        {
            pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = QVariant(mesg.getFieldIntValue(eRecordHeartRate));
        }
        if(mesg.isFieldValueValid(eRecordTemperature))
        {
            pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:atemp"] = QVariant(mesg.getFieldIntValue(eRecordTemperature));
        }

        pt.extensions.squeeze();
        return true;
    }
    return false;
}

void readFitCoursePoint(const CFitMessage &mesg, IGisItem::wpt_t &wpt)
{
    if(mesg.isFieldValueValid(eCoursePointName))
    {
        wpt.name = mesg.getFieldString(eCoursePointName);
    }
    if(mesg.isFieldValueValid(eCoursePointTimestamp))
    {
        wpt.time = CFitDataConverter::toDateTime(mesg.getFieldUIntValue(eCoursePointTimestamp));
    }

    if(mesg.isFieldValueValid(eCoursePointPositionLong) && mesg.isFieldValueValid(eCoursePointPositionLat))
    {
        wpt.lon = CFitDataConverter::toDegree(mesg.getFieldIntValue(eCoursePointPositionLong));
        wpt.lat = CFitDataConverter::toDegree(mesg.getFieldIntValue(eCoursePointPositionLat));
    }
    // TODO find appropriate icon for different CoursePointType (CoursePoint***)
    // see WptIcons.initWptIcons() for all values
    wpt.sym = "Waypoint";
}

void CGisItemTrk::readTrkFromFit(CFitStream &stream)
{
    const CFitMessage& mesg = stream.firstMesgOf(eMesgNumCourse);
    if(mesg.isValid() && mesg.isFieldValueValid(eCourseName))
    {
        trk.name = mesg.getFieldString(eCourseName);
    }
    else
    {
        // course files can have a name but activities don't, so use just the filename
        trk.name = QFileInfo(stream.getFileName()).baseName().replace("_", " ");
    }

    trk.name = QFileInfo(stream.getFileName()).baseName().replace("_", " ");
    // note to the FIT specification: the specification alllows different ordering of the messages.
    // Record messages can either be at the beginning or in chronological order within the record
    // messages. Garmin devices uses the chronological ordering. We only consider the chronological
    // order, otherwise timestamps (of records and events) must be compared to each other.
    trkseg_t seg = trkseg_t();
    do
    {
        const CFitMessage& mesg = stream.nextMesg();
        if(mesg.getGlobalMesgNr() == eMesgNumRecord)
        {
            // for documentation: MesgNumActivity, MesgNumSession, MesgNumLap, MesgNumLength could also contain data
            CGisItemTrk::trkpt_t pt = CGisItemTrk::trkpt_t();
            if(readFitRecord(mesg, pt))
            {
                seg.pts.append(pt);
            }
        }
        else if(mesg.getGlobalMesgNr() ==  eMesgNumEvent)
        {
            if(mesg.getFieldUIntValue(eEventEvent) == eEventTimer)
            {
                if(mesg.getFieldUIntValue(eEventEventType) == eEventTypeStop ||
                   mesg.getFieldUIntValue(eEventEventType) == eEventTypeStopAll ||
                   mesg.getFieldUIntValue(eEventEventType) == eEventTypeStopDisableAll)
                {
                    trk.segs.append(seg);
                    seg = trkseg_t();
                }
            }
        }
    }
    while (stream.hasMoreMesg());

    if(trk.segs.size() == 0)
    {
        // navigation course files do not have to have start / stop event, so add it now.
        trk.segs.append(seg);
    }
}


void CGisItemWpt::readWptFromFit(CFitStream &stream)
{
    const CFitMessage& mesg = stream.lastMesg();
    readFitCoursePoint(mesg, wpt);
}


void CGisItemRte::readRteFromFit(CFitStream &stream)
{
    // a course file could be considered as a route...
    const CFitMessage& mesg = stream.firstMesgOf(eMesgNumCourse);
    if(mesg.isFieldValueValid(eCourseName))
    {
        rte.name = mesg.getFieldString(eCourseName);
    }
    do
    {
        const CFitMessage& mesg = stream.nextMesg();
        if(mesg.getGlobalMesgNr() == eMesgNumRecord)
        {
            rtept_t pt = rtept_t();
            if(readFitRecord(mesg, pt))
            {
                rte.pts.append(pt);
            }
        }
    }
    while (stream.hasMoreMesg());
}


