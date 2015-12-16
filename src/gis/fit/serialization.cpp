/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/fit/CFitProject.h"
#include "gis/fit/CFitStream.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"
#include "CMainWindow.h"


static const qreal twoPow31 = 180.0 / qPow(2, 31);
static const uint sec1970to1990 = QDateTime(QDate(1989, 12, 31), QTime(0, 0, 0),Qt::UTC).toTime_t();


class FitTypeHandler {
public:

    /**
     return: the given semicircle value converted to degree. 0 if the semicircle is not valid.
     */
    static qreal toDegree(int32_t semicircles) {
        qreal degree = 0;
        degree = semicircles * twoPow31;

        qDebug() << semicircles << " " << degree;
        return degree;
    }

    /**
     timestamp: seconds since UTC 00:00 Dec 31 1989
    */
    static QDateTime toDateTime(uint32_t timestamp) {
        QDateTime dateTime;
        dateTime.setTime_t(sec1970to1990 + timestamp);
        dateTime.setTimeSpec(Qt::UTC);
        return dateTime;
    }
};



void CGisItemTrk::readFit(CFitStream& stream)
{
    // in the loop are all message types for a activity listed, althogh not all of them are used.
    // note so the fIT specification: the specification alllows different ordering of the messages.
    // Record messages can either be at the beginning or in chronological order within the record
    // messages. GArmin devices uses the chronological ordering. we only consider the chronological
    // order.
        stream.reset();
        trkseg_t seg = trkseg_t();
        do
        {
            CFitMessage* mesg = stream.nextMesg();
            switch (mesg->getGlobalMesgNr())
            {
                case MesgNumActivity:
                    break;
                case MesgNumSession:
                    break;
                case MesgNumLap:
                    break;
                case MesgNumLength:
                    break;
                case MesgNumRecord:
                {
                    qDebug() << mesg->messageInfo();
                    trkpt_t pt = trkpt_t();

                    pt.lon = FitTypeHandler::toDegree(mesg->getFieldIntValue(RecordPositionLong));
                    pt.lat = FitTypeHandler::toDegree(mesg->getFieldIntValue(RecordPositionLat));
                    pt.ele = (int)mesg->getFieldDoubleValue(RecordAltitude);
                    pt.speed = mesg->getFieldDoubleValue(RecordSpeed);
                    pt.time = FitTypeHandler::toDateTime(mesg->getFieldUIntValue(RecordTimestamp));

                    if(mesg->isFieldValueValid(RecordHeartRate))
                        pt.extensions["heart_rate"] = QVariant(mesg->getFieldIntValue(RecordHeartRate));
                    if(mesg->isFieldValueValid(RecordTemperature))
                        pt.extensions["temperature"] = QVariant(mesg->getFieldIntValue(RecordTemperature));
                    pt.extensions.squeeze();
                    if (pt.lon != 0 && pt.lat != 0)
                    {
                        seg.pts.append(pt);
                    }
                    break;
                }
                case MesgNumEvent:
                {
                    if(mesg->getFieldUIntValue(EventEvent) == EventTimer)
                    {
                        if(mesg->getFieldUIntValue(EventEventType) == EventTypeStop
                           || mesg->getFieldUIntValue(EventEventType) == EventTypeStopAll)
                        {
                            trk.segs.append(seg);
                            seg = trkseg_t();
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        } while (stream.hasMoreMesg());
        
        trk.name = QFileInfo(stream.getFileName()).baseName().replace("_", " ");

    deriveSecondaryData();
}


void CGisItemWpt::readFit(CFitStream& stream)
{
    // TODO for a track we check if lon and lat are not null. Here we can not help, becuae the wpt is alredy created
    // but the gps postion could be invalid.
    CFitMessage* mesg = stream.lastMesg();
    wpt.name = mesg->profile()->getName();
    wpt.lon = FitTypeHandler::toDegree(mesg->getFieldIntValue(RecordPositionLong));
    wpt.lat = FitTypeHandler::toDegree(mesg->getFieldIntValue(RecordPositionLat));
    wpt.time = FitTypeHandler::toDateTime(mesg->getFieldUIntValue(RecordTimestamp));
}

