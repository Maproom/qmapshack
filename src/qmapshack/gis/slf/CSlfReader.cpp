/**********************************************************************************************
    Copyright (C) 2015-2016 Christian Eichler <code@christian-eichler.de>

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

#include "gis/slf/CSlfReader.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "gis/slf/CSlfProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

// .slf does not know extensions, but we are using them internally.
// This makes saving as .gpx easy and conforms to the internal way of doing
// things.
const QHash<QString, QString> CSlfReader::attrToExt = {{"temperature", "gpxtpx:TrackPointExtension|gpxtpx:atemp"},
                                                       {"heartrate", "gpxtpx:TrackPointExtension|gpxtpx:hr"},
                                                       {"cadence", "gpxtpx:TrackPointExtension|gpxtpx:cad"},
                                                       {"speed", "speed"}};

void CSlfReader::readFile(const QString& file, CSlfProject* proj) { CSlfReader reader(file, proj); }

QDateTime CSlfReader::parseTimestamp(const QString& ts) {
  int posOfGMT = ts.indexOf("GMT");
  int deltaGMT = 0;
  if (' ' != ts[posOfGMT + 3]) {
    QString deltaGMTStr = ts.mid(posOfGMT + 3);
    QTextStream(&deltaGMTStr) >> deltaGMT;
  }

  QString pts = ts.left(posOfGMT - 1) + ts.mid(posOfGMT + 8);

  QLocale locale(QLocale::C);
  const QDateTime& baseTime = locale.toDateTime(pts, "ddd MMM d HH:mm:ss yyyy");
  if (!baseTime.isValid()) {
    throw tr("Failed to parse timestamp `%1`").arg(ts);
  }

  return baseTime.addSecs((deltaGMT / 100) * 60 * 60);
}

CSlfReader::CSlfReader(const QString& filename, CSlfProject* proj) : proj(proj) {
  QFile file(filename);

  // if the file does not exist, the filename is assumed to be a name for a new project
  if (!file.exists()) {
    throw tr("%1 does not exist").arg(filename);
  }

  if (!file.open(QIODevice::ReadOnly)) {
    throw tr("Failed to open %1").arg(filename);
  }

  // load file content to xml document
  QDomDocument xml;
  const QDomDocument::ParseResult& result = xml.setContent(&file);
  if (!result) {
    file.close();
    throw tr("Failed to read: %1\nline %2, column %3:\n %4")
        .arg(filename)
        .arg(result.errorLine)
        .arg(result.errorColumn)
        .arg(result.errorMessage);
  }
  file.close();

  QDomElement xmlAct = xml.documentElement();
  if (xmlAct.tagName() != "Activity") {
    throw tr("Not a SLF file: %1").arg(filename);
  }

  // Ensure we only open files with the correct version (only revision 400 supported atm)
  const int revision = xmlAct.attributes().namedItem("revision").nodeValue().toInt();
  if (400 != revision) {
    throw tr("Unsupported revision %1: %2").arg(revision).arg(filename);
  }

  // Parse the file's dateCode
  // This is a crucial step, as all the other timestamps are relative to this one
  const QString& dateCode = xmlAct.namedItem("Computer").attributes().namedItem("dateCode").nodeValue();
  baseTime = parseTimestamp(dateCode);

  const QDomNode& xmlGI = xmlAct.namedItem("GeneralInformation");
  if (xmlGI.isElement()) {
    readMetadata(xmlGI, proj->metadata);
  }

  // Read all the Markers
  // This needs to be done prior to reading the Entries, as they contain the
  // locations at which we need to split the track into segments
  readMarkers(xmlAct.namedItem("Markers"));

  // Now read the "Entries"
  // Entries are the actual waypoints, which make up the track(s)
  readEntries(xmlAct.namedItem("Entries"));
}

void CSlfReader::readMarkers(const QDomNode& xml) {
  const QDomNodeList& xmlMrks = xml.childNodes();
  long markerTimeSum = 0;

  // iterate over all markers and add them to a QMap (this will sort them by timeAbsolute)
  QMap<long, QDomNode> markers;
  for (int i = 0; i < xmlMrks.count(); i++) {
    const QDomNode& marker = xmlMrks.item(i);
    const QDomNamedNodeMap& attr = marker.attributes();

    long time = attr.namedItem("timeAbsolute").nodeValue().toLong();

    markers.insert(time, marker);
  }

  offsetsTime.append(0);
  for (const QDomNode& marker : markers) {
    const QDomNamedNodeMap& attr = marker.attributes();

    bool ok;
    long markerTime = attr.namedItem("duration").nodeValue().toLong(&ok);
    if (ok) {
      markerTimeSum += markerTime;
    }

    // type="l" indicates a marker, which is used to separate two laps
    const QString& type = attr.namedItem("type").nodeValue();
    if ("l" == type || "p" == type) {
      // filter out duplicate (and invalid) laps
      long time = attr.namedItem("timeAbsolute").nodeValue().toLong();
      if (laps.isEmpty() || (laps.last() < time)) {
        laps.append(time);
        offsetsTime.append(markerTimeSum);
      }
    }
    if ("p" == type || "l" == type) {
      qreal lat = attr.namedItem("latitude").nodeValue().toDouble();
      qreal lon = attr.namedItem("longitude").nodeValue().toDouble();

      QString name = attr.namedItem("title").nodeValue();
      if (name.isEmpty()) {
        if ("p" == type) {
          name = tr("Break %1").arg(attr.namedItem("number").nodeValue());
        }
        if ("l" == type) {
          name = tr("Lap %1").arg(attr.namedItem("number").nodeValue());
        }
      }

      qreal ele = attr.namedItem("altitude").nodeValue().toDouble() / 1000.;
      const QDateTime& time = baseTime.addSecs(attr.namedItem("timeAbsolute").nodeValue().toDouble() / 100.);
      new CGisItemWpt(QPointF(lon, lat), ele, time, name, "", proj);
    }
  }

  // the last lap ends at positive infinite
  laps.append(std::numeric_limits<long>::max());
}

QSet<QString> CSlfReader::findUsedAttributes(const QDomNodeList& xmlEntrs) {
  QSet<QString> usedAttr;
  for (int i = 0; i < xmlEntrs.count(); i++) {
    const QDomNamedNodeMap& attr = xmlEntrs.item(i).attributes();
    const QStringList& keys = attrToExt.keys();
    for (const QString& key : keys) {
      if (attr.contains(key) && ("0" != attr.namedItem(key).nodeValue())) {
        usedAttr.insert(key);
      }
    }
  }

  return usedAttr;
}

void CSlfReader::readEntries(const QDomNode& xml) {
  const QDomNodeList& xmlEntrs = xml.childNodes();

  const QSet<QString>& usedAttr = findUsedAttributes(xmlEntrs);

  // Now generate the track / segments
  int lap = 0;

  CTrackData trk;
  trk.segs.resize(laps.count() + 1);

  CTrackData::trkseg_t* seg = &(trk.segs[0]);
  long breakTime = offsetsTime[0];

  qreal prevLon = NOFLOAT;
  qreal prevLat = NOFLOAT;
  for (int i = 0; i < xmlEntrs.count(); i++) {
    CTrackData::trkpt_t trkpt;

    const QDomNamedNodeMap& attr = xmlEntrs.item(i).attributes();
    trkpt.lat = attr.namedItem("latitude").nodeValue().toDouble();
    trkpt.lon = attr.namedItem("longitude").nodeValue().toDouble();

    if ((0. == trkpt.lat && 0. == trkpt.lon) || (prevLat == trkpt.lat && prevLon == trkpt.lon)) {
      continue;
    }
    prevLat = trkpt.lat;
    prevLon = trkpt.lon;

    trkpt.ele = attr.namedItem("altitude").nodeValue().toDouble() / 1000.;

    for (const QString& key : usedAttr) {
      if (attr.contains(key) && attrToExt.contains(key)) {
        trkpt.extensions[attrToExt[key]] = attr.namedItem(key).nodeValue().toDouble();
      }
    }

    const long trainingTime = attr.namedItem("trainingTimeAbsolute").nodeValue().toLong();
    while (trainingTime > laps[lap]) {
      lap++;
      seg = &(trk.segs[lap]);
      breakTime = offsetsTime[lap];
    }

    trkpt.time = baseTime.addSecs((breakTime + trainingTime) / 100.);

    seg->pts.append(trkpt);
  }

  trk.name = proj->metadata.name;
  new CGisItemTrk(trk, proj);
  proj->updateItemCounters();
}

void CSlfReader::readMetadata(const QDomNode& xml, IGisProject::metadata_t& metadata) {
  metadata.name = xml.namedItem("name").firstChild().nodeValue();
  metadata.desc = xml.namedItem("description").firstChild().nodeValue();
  metadata.keywords = xml.namedItem("sport").firstChild().nodeValue();
}
