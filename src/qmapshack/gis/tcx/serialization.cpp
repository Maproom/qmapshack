/**********************************************************************************************
   Copyright (C) 2017 Michel Durand <zero@cms123.fr>

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

#include <QSet>
#include <QString>
#include <initializer_list>

#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

static std::initializer_list<QString> knownSymbols = {
    "1st Category", "2nd Category", "3rd Category", "4th Category", "Danger", "First Aid", "Food", "Hors Category",
    "Left",         "Right",        "Sprint",       "Straight",     "Summit", "Valley",    "Water"};

void CGisItemWpt::saveTCX(QDomNode& courseNode, const QDateTime crsPtDateTimeToBeSaved) {
  QDomDocument doc = courseNode.ownerDocument();

  QDomElement xmlCrsPt = doc.createElement("CoursePoint");
  courseNode.appendChild(xmlCrsPt);

  xmlCrsPt.appendChild(doc.createElement("Name"));
  QString str = wpt.name;
  str.truncate(10);  // course point name max size is 10 characters ; see
                     // http://www8.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd
  xmlCrsPt.lastChild().appendChild(doc.createTextNode(str));

  xmlCrsPt.appendChild(doc.createElement("Time"));
  xmlCrsPt.lastChild().appendChild(doc.createTextNode(crsPtDateTimeToBeSaved.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));

  xmlCrsPt.appendChild(doc.createElement("Position"));

  xmlCrsPt.lastChild().appendChild(doc.createElement("LatitudeDegrees"));
  str = QString::asprintf("%1.8f", wpt.lat);
  xmlCrsPt.lastChild().lastChild().appendChild(doc.createTextNode(str));

  xmlCrsPt.lastChild().appendChild(doc.createElement("LongitudeDegrees"));
  str = QString::asprintf("%1.8f", wpt.lon);
  xmlCrsPt.lastChild().lastChild().appendChild(doc.createTextNode(str));

  if (wpt.ele != NOINT) {
    xmlCrsPt.appendChild(doc.createElement("AltitudeMeters"));
    xmlCrsPt.lastChild().appendChild(doc.createTextNode(QString::number(wpt.ele)));
  }

  QString pointTypeToBeWritten;
  if (!QSet<QString>(knownSymbols).contains(wpt.sym)) {
    pointTypeToBeWritten = "Generic";
  } else {
    pointTypeToBeWritten = wpt.sym;
  }

  xmlCrsPt.appendChild(doc.createElement("PointType"));
  xmlCrsPt.lastChild().appendChild(doc.createTextNode(pointTypeToBeWritten));
}

void CGisItemTrk::saveTCXcourse(QDomNode& coursesNode) {
  IGisProject* project = getParentProject();
  if (nullptr == project) {
    return;
  }

  QDomDocument doc = coursesNode.ownerDocument();

  QDomElement courseNode = doc.createElement("Course");
  coursesNode.appendChild(courseNode);

  courseNode.appendChild(doc.createElement("Name"));
  QString str = this->getName();
  str.truncate(15);  // course name max size is 15 characters ; see
                     // http://www8.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd
  courseNode.lastChild().appendChild(doc.createTextNode(str));

  QDomElement lapElmt = doc.createElement("Lap");
  courseNode.appendChild(lapElmt);

  lapElmt.appendChild(doc.createElement("TotalTimeSeconds"));
  lapElmt.lastChild().appendChild(doc.createTextNode(QString::number(this->getTotalElapsedSeconds())));

  lapElmt.appendChild(doc.createElement("DistanceMeters"));
  lapElmt.lastChild().appendChild(doc.createTextNode(QString::number(this->getTotalDistance())));

  lapElmt.appendChild(doc.createElement("Intensity"));
  lapElmt.lastChild().appendChild(doc.createTextNode("Active"));

  QDomElement xmlTrk = doc.createElement("Track");
  courseNode.appendChild(xmlTrk);

  QList<QDateTime> trkPtToOverwriteDateTimes;
  QList<IGisItem::key_t> wptKeys;

  for (const CTrackData::trkpt_t& trkpt : trk) {
    QDomElement xmlTrkpt = doc.createElement("Trackpoint");
    xmlTrk.appendChild(xmlTrkpt);

    xmlTrkpt.appendChild(doc.createElement("Time"));
    xmlTrkpt.lastChild().appendChild(doc.createTextNode(trkpt.time.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));

    xmlTrkpt.appendChild(doc.createElement("Position"));

    xmlTrkpt.lastChild().appendChild(doc.createElement("LatitudeDegrees"));
    QString str;
    str = QString::asprintf("%1.8f", trkpt.lat);
    xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));

    xmlTrkpt.lastChild().appendChild(doc.createElement("LongitudeDegrees"));
    str = QString::asprintf("%1.8f", trkpt.lon);
    xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));

    qint32 eleToBeWritten = NOINT;
    if (NOINT != trkpt.ele)  // if this trackpoint has elevation
    {
      eleToBeWritten = trkpt.ele;  // take elevation on the trackpoint
    }
    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(project->getItemByKey(trkpt.keyWpt));
    if (nullptr != wpt)  // if trackpoint has an attached waypoint
    {
      wptKeys << trkpt.keyWpt;                  // store attached waypoint
      trkPtToOverwriteDateTimes << trkpt.time;  // store trackpoint dateTime

      if (NOINT != wpt->getElevation())  // if waypoint has elevation
      {
        eleToBeWritten = wpt->getElevation();  // take elevation of the waypoint
      }
    }
    if (eleToBeWritten != NOINT)  // if valid elevation has been found
    {
      xmlTrkpt.appendChild(doc.createElement("AltitudeMeters"));
      xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(eleToBeWritten)));
    }

    xmlTrkpt.appendChild(doc.createElement("DistanceMeters"));
    xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(trkpt.distance)));

    if (trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"].toString().size() != 0) {
      xmlTrkpt.appendChild(doc.createElement("HeartRateBpm"));
      xmlTrkpt.lastChild().appendChild(doc.createElement("Value"));
      xmlTrkpt.lastChild().lastChild().appendChild(
          doc.createTextNode(trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"].toString()));
    }

    if (trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"].toString().size() != 0) {
      xmlTrkpt.appendChild(doc.createElement("Cadence"));
      xmlTrkpt.lastChild().appendChild(
          doc.createTextNode(trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"].toString()));
    }
  }

  int i = 0;
  for (const IGisItem::key_t& wptKey : std::as_const(wptKeys))  // browse course points
  {
    CGisItemWpt* wptItem = dynamic_cast<CGisItemWpt*>(project->getItemByKey(wptKey));
    wptItem->saveTCX(courseNode, trkPtToOverwriteDateTimes[i++]);
  }
}

void CGisItemTrk::saveTCXactivity(QDomNode& activitiesNode) {
  IGisProject* project = getParentProject();
  if (nullptr == project) {
    return;
  }

  QDomDocument doc = activitiesNode.ownerDocument();

  QDomElement activityNode = doc.createElement("Activity");
  activitiesNode.appendChild(activityNode);

  activityNode.setAttribute("Sport", "Other");

  activityNode.appendChild(doc.createElement("Id"));
  activityNode.lastChild().appendChild(
      doc.createTextNode(trk.segs[0].pts[0].time.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));

  for (const CTrackData::trkseg_t& seg : std::as_const(trk.segs)) {
    QDomElement lapElmt = doc.createElement("Lap");
    activityNode.appendChild(lapElmt);
    lapElmt.setAttribute("StartTime", seg.pts[0].time.toString("yyyy-MM-dd'T'hh:mm:ss'Z'"));

    lapElmt.appendChild(doc.createElement("TotalTimeSeconds"));  // "totalTime" means "time of this lap"
    lapElmt.lastChild().appendChild(
        doc.createTextNode(QString::number(seg.pts.first().time.secsTo(seg.pts.last().time))));

    lapElmt.appendChild(doc.createElement("DistanceMeters"));
    lapElmt.lastChild().appendChild(
        doc.createTextNode(QString::number(seg.pts.last().distance - seg.pts.first().distance)));

    lapElmt.appendChild(doc.createElement("Calories"));
    lapElmt.lastChild().appendChild(
        doc.createTextNode("0"));  // calories are unknown but a "calories" element is mandatory

    lapElmt.appendChild(doc.createElement("Intensity"));
    lapElmt.lastChild().appendChild(doc.createTextNode("Active"));

    lapElmt.appendChild(doc.createElement("TriggerMethod"));
    lapElmt.lastChild().appendChild(doc.createTextNode("Manual"));

    QDomElement xmlTrk = doc.createElement("Track");
    lapElmt.appendChild(xmlTrk);

    for (const CTrackData::trkpt_t& trkpt : seg.pts) {
      QDomElement xmlTrkpt = doc.createElement("Trackpoint");
      xmlTrk.appendChild(xmlTrkpt);

      xmlTrkpt.appendChild(doc.createElement("Time"));
      xmlTrkpt.lastChild().appendChild(doc.createTextNode(trkpt.time.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));

      xmlTrkpt.appendChild(doc.createElement("Position"));

      xmlTrkpt.lastChild().appendChild(doc.createElement("LatitudeDegrees"));
      QString str;
      str = QString::asprintf("%1.8f", trkpt.lat);
      xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));

      xmlTrkpt.lastChild().appendChild(doc.createElement("LongitudeDegrees"));
      str = QString::asprintf("%1.8f", trkpt.lon);
      xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));

      if (NOINT != trkpt.ele)  // if this trackpoint has elevation
      {
        xmlTrkpt.appendChild(doc.createElement("AltitudeMeters"));
        xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(trkpt.ele)));
      }

      xmlTrkpt.appendChild(doc.createElement("DistanceMeters"));
      xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(trkpt.distance)));

      if (trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"].toString().size() != 0) {
        xmlTrkpt.appendChild(doc.createElement("HeartRateBpm"));
        xmlTrkpt.lastChild().appendChild(doc.createElement("Value"));
        xmlTrkpt.lastChild().lastChild().appendChild(
            doc.createTextNode(trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"].toString()));
      }

      if (trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"].toString().size() != 0) {
        xmlTrkpt.appendChild(doc.createElement("Cadence"));
        xmlTrkpt.lastChild().appendChild(
            doc.createTextNode(trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"].toString()));
      }
    }
  }
}
