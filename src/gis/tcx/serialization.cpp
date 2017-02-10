/**********************************************************************************************
Copyright (C) 2017 Michel Durand zero@cms123.fr

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

#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"


void CGisItemWpt::saveTCX(QDomNode& tcx, const QDateTime crsPtDateTimeToBeSaved)
{
    QDomDocument doc = tcx.ownerDocument();

    QDomElement xmlCrsPt = doc.createElement("CoursePoint");
    tcx.lastChild().lastChild().appendChild(xmlCrsPt);

    xmlCrsPt.appendChild(doc.createElement("Name"));
    QString str = wpt.name;
    str.truncate(10);
    xmlCrsPt.lastChild().appendChild(doc.createTextNode(str));

    xmlCrsPt.appendChild(doc.createElement("Time"));
    xmlCrsPt.lastChild().appendChild(doc.createTextNode(crsPtDateTimeToBeSaved.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));

    xmlCrsPt.appendChild(doc.createElement("Position"));

    xmlCrsPt.lastChild().appendChild(doc.createElement("LatitudeDegrees"));
    str.sprintf("%1.8f", wpt.lat);
    xmlCrsPt.lastChild().lastChild().appendChild(doc.createTextNode(str));

    xmlCrsPt.lastChild().appendChild(doc.createElement("LongitudeDegrees"));
    str.sprintf("%1.8f", wpt.lon);
    xmlCrsPt.lastChild().lastChild().appendChild(doc.createTextNode(str));

    if (wpt.ele != NOINT)
    {
        xmlCrsPt.appendChild(doc.createElement("AltitudeMeters"));
        xmlCrsPt.lastChild().appendChild(doc.createTextNode(QString::number(wpt.ele)));
    }


    QString pointTypeToBeWritten;

    if (wpt.sym != "1st Category" &&
        wpt.sym != "2nd Category" &&
        wpt.sym != "3rd Category" &&
        wpt.sym != "4th Category" &&
        wpt.sym != "Danger" &&
        wpt.sym != "First Aid" &&
        wpt.sym != "Food" &&
        wpt.sym != "Hors Category" &&
        wpt.sym != "Left" &&
        wpt.sym != "Right" &&
        wpt.sym != "Sprint" &&
        wpt.sym != "Straight" &&
        wpt.sym != "Summit" &&
        wpt.sym != "Valley" &&
        wpt.sym != "Water")
    {
        pointTypeToBeWritten = "Generic";
    }
    else
    {
        pointTypeToBeWritten = wpt.sym;
    }

    xmlCrsPt.appendChild(doc.createElement("PointType"));
    xmlCrsPt.lastChild().appendChild(doc.createTextNode(pointTypeToBeWritten));

}




void CGisItemTrk::saveTCX(QDomNode& tcx, QList<QDateTime>& trkPtToOverwriteDateTimes, QList<qint32>& trkPtToOverwriteElevations)
{
    QDomDocument doc = tcx.ownerDocument();

    QDomElement xmlTrk = doc.createElement("Track");
    tcx.lastChild().lastChild().appendChild(xmlTrk);

    int i = 0;

    for (const CTrackData::trkseg_t &seg : trk.segs)
    {
        for (const CTrackData::trkpt_t &pt : seg.pts)
        {
            QDomElement xmlTrkpt = doc.createElement("Trackpoint");
            xmlTrk.appendChild(xmlTrkpt);

            xmlTrkpt.appendChild(doc.createElement("Time"));
            xmlTrkpt.lastChild().appendChild(doc.createTextNode(pt.time.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));

            xmlTrkpt.appendChild(doc.createElement("Position"));

            xmlTrkpt.lastChild().appendChild(doc.createElement("LatitudeDegrees"));
            QString str;
            str.sprintf("%1.8f", pt.lat);
            xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));

            xmlTrkpt.lastChild().appendChild(doc.createElement("LongitudeDegrees"));
            str.sprintf("%1.8f", pt.lon);
            xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));

            qint32 eleToBeWritten = NOINT;

            if ((i < trkPtToOverwriteDateTimes.size()) && (pt.time == trkPtToOverwriteDateTimes[i])) // if trackpoint corresponds to one of the waypoints to be attached to the track
            {
                if (trkPtToOverwriteElevations[i] != NOINT) // if waypoint has elevation
                {
                    eleToBeWritten = trkPtToOverwriteElevations[i]; // take elevation of the waypoint
                }
                else
                {
                    eleToBeWritten = pt.ele; // if not, take elevation on the trackpoint
                }
                i++;
            }
            else
            {
                if (pt.ele != NOINT) // if this trackpoint has elevation
                {
                    eleToBeWritten = pt.ele;
                }
            }


            if (eleToBeWritten != NOINT) // if valid elevation has been found
            {
                xmlTrkpt.appendChild(doc.createElement("AltitudeMeters"));
                xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(eleToBeWritten)));
            }

            xmlTrkpt.appendChild(doc.createElement("DistanceMeters"));
            xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(pt.distance)));
        }
    }
}
