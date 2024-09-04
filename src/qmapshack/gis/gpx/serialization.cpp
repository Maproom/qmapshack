/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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

#include <QtXml>

#include "device/CDeviceGarmin.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CWptIconManager.h"
#include "version.h"

const QString IGisProject::gpx_ns = "http://www.topografix.com/GPX/1/1";
const QString IGisProject::xsi_ns = "http://www.w3.org/2001/XMLSchema-instance";
const QString IGisProject::gpxx_ns = "http://www.garmin.com/xmlschemas/GpxExtensions/v3";
const QString IGisProject::gpxtpx_ns = "http://www.garmin.com/xmlschemas/TrackPointExtension/v1";
const QString IGisProject::wptx1_ns = "http://www.garmin.com/xmlschemas/WaypointExtension/v1";
const QString IGisProject::rmc_ns = "urn:net:trekbuddy:1.0:nmea:rmc";
const QString IGisProject::ql_ns = "http://www.qlandkarte.org/xmlschemas/v1.1";
const QString IGisProject::gs_ns = "http://www.groundspeak.com/cache/1/0";
const QString IGisProject::tp1_ns = "http://www.garmin.com/xmlschemas/TrackPointExtension/v1";
const QString IGisProject::gpxdata_ns = "http://www.cluetrust.com/XML/GPXDATA/1/0";

static void readXml(const QDomNode& xml, const QString& tag, qint32& value) {
  if (xml.namedItem(tag).isElement()) {
    bool ok = false;
    qint32 tmp = xml.namedItem(tag).toElement().text().toInt(&ok);
    if (!ok) {
      tmp = qRound(xml.namedItem(tag).toElement().text().toDouble(&ok));
    }
    if (ok) {
      value = tmp;
    }
  }
}

static void readXml(const QDomNode& xml, const QString& tag, trkact_t& value) {
  if (xml.namedItem(tag).isElement()) {
    bool ok = false;
    qint32 tmp = xml.namedItem(tag).toElement().text().toInt(&ok);
    if (!ok) {
      value = CTrackData::trkpt_t::eAct20None;
    } else {
      value = trkact_t(tmp);
    }
  }
}

template <typename T>
static void readXml(const QDomNode& xml, const QString& tag, T& value) {
  if (xml.namedItem(tag).isElement()) {
    bool ok = false;
    T tmp;

    if (std::is_same<T, quint32>::value) {
      tmp = xml.namedItem(tag).toElement().text().toUInt(&ok);
    } else if (std::is_same<T, quint64>::value) {
      tmp = xml.namedItem(tag).toElement().text().toULongLong(&ok);
    } else if (std::is_same<T, qreal>::value) {
      tmp = xml.namedItem(tag).toElement().text().toDouble(&ok);
    } else if (std::is_same<T, bool>::value) {
      tmp = xml.namedItem(tag).toElement().text().toInt(&ok);
    }

    if (ok) {
      value = tmp;
    }
  }
}

static void readXml(const QDomNode& xml, const QString& tag, QString& value) {
  if (xml.namedItem(tag).isElement()) {
    value = xml.namedItem(tag).toElement().text();
  }
}

static void readXml(const QDomNode& xml, const QString& tag, QString& value, bool& isHtml) {
  if (xml.namedItem(tag).isElement()) {
    const QDomNamedNodeMap& attr = xml.namedItem(tag).toElement().attributes();
    isHtml = (attr.namedItem("html").nodeValue().toLocal8Bit().toLower() == "true");
    value = xml.namedItem(tag).toElement().text();
  }
}

static void readXml(const QDomNode& xml, const QString& tag, QDateTime& value) {
  if (xml.namedItem(tag).isElement()) {
    QString time = xml.namedItem(tag).toElement().text();
    IUnit::parseTimestamp(time, value);
  }
}

static void readXml(const QDomNode& xml, const QString& tag, QList<IGisItem::link_t>& l) {
  if (xml.namedItem(tag).isElement()) {
    const QDomNodeList& links = xml.toElement().elementsByTagName(tag);
    int N = links.count();
    for (int n = 0; n < N; ++n) {
      const QDomNode& link = links.item(n);

      if (xml != link.parentNode()) {
        continue;
      }

      IGisItem::link_t tmp;
      tmp.uri.setUrl(link.attributes().namedItem("href").nodeValue());
      readXml(link, "text", tmp.text);
      readXml(link, "type", tmp.type);

      l << tmp;
    }
  }
}

static void readXml(const QDomNode& xml, IGisItem::history_t& history) {
  if (xml.namedItem("ql:history").isElement()) {
    const QDomElement& xmlHistory = xml.namedItem("ql:history").toElement();

    const QDomNodeList& xmlEntries = xmlHistory.elementsByTagName("ql:event");
    for (int n = 0; n < xmlEntries.count(); ++n) {
      const QDomNode& xmlEntry = xmlEntries.item(n);
      IGisItem::history_event_t entry;
      readXml(xmlEntry, "ql:icon", entry.icon);
      readXml(xmlEntry, "ql:time", entry.time);
      readXml(xmlEntry, "ql:comment", entry.comment);

      history.events << entry;
    }

    history.histIdxInitial = history.events.size() - 1;
    history.histIdxCurrent = history.histIdxInitial;
  }
}

static void readXml(const QDomNode& xml, const QString& tag, QPoint& offsetBubble, quint32& widthBubble) {
  if (xml.namedItem(tag).isElement()) {
    const QDomElement& xmlBubble = xml.namedItem(tag).toElement();
    int x = xmlBubble.attributes().namedItem("xoff").nodeValue().toInt();
    int y = xmlBubble.attributes().namedItem("yoff").nodeValue().toInt();
    offsetBubble = QPoint(x, y);
    widthBubble = xmlBubble.attributes().namedItem("width").nodeValue().toInt();
  }
}

static void writeXml(QDomNode& xml, const QString& tag, qint32 val) {
  if (val != NOINT) {
    QDomElement elem = xml.ownerDocument().createElement(tag);
    xml.appendChild(elem);
    QDomText text = xml.ownerDocument().createTextNode(QString::number(val));
    elem.appendChild(text);
  }
}

static void writeXml(QDomNode& xml, const QString& tag, quint32 val) {
  if (val != NOINT) {
    QDomElement elem = xml.ownerDocument().createElement(tag);
    xml.appendChild(elem);
    QDomText text = xml.ownerDocument().createTextNode(QString::number(val));
    elem.appendChild(text);
  }
}

static void writeXml(QDomNode& xml, const QString& tag, quint64 val) {
  if (val != 0) {
    QDomElement elem = xml.ownerDocument().createElement(tag);
    xml.appendChild(elem);
    QDomText text = xml.ownerDocument().createTextNode(QString::number(val));
    elem.appendChild(text);
  }
}

static void writeXml(QDomNode& xml, const QString& tag, const QString& val) {
  if (!val.isEmpty()) {
    QDomElement elem = xml.ownerDocument().createElement(tag);
    xml.appendChild(elem);
    QDomText text = xml.ownerDocument().createTextNode(val);
    elem.appendChild(text);
  }
}

static void writeXml(QDomNode& xml, const QString& tag, qreal val) {
  if (val != NOFLOAT) {
    QDomElement elem = xml.ownerDocument().createElement(tag);
    xml.appendChild(elem);
    QDomText text = xml.ownerDocument().createTextNode(QString("%1").arg(val, 0, 'f', 8));
    elem.appendChild(text);
  }
}

static void writeXml(QDomNode& xml, const QString& tag, const QString& val, bool isHtml) {
  if (!val.isEmpty()) {
    QDomElement elem = xml.ownerDocument().createElement(tag);
    xml.appendChild(elem);
    QDomText text = xml.ownerDocument().createCDATASection(val);
    elem.appendChild(text);
    elem.setAttribute("html", isHtml ? "True" : "False");
  }
}

static void writeXml(QDomNode& xml, const QString& tag, const QDateTime& time) {
  if (time.isValid()) {
    QDomElement elem = xml.ownerDocument().createElement(tag);
    xml.appendChild(elem);
    QDomText text = xml.ownerDocument().createTextNode(time.toString("yyyy-MM-dd'T'hh:mm:ss.zzz'Z'"));
    elem.appendChild(text);
  }
}

static void writeXml(QDomNode& xml, const QString& tag, const QList<IGisItem::link_t>& links) {
  if (!links.isEmpty()) {
    for (const IGisItem::link_t& link : links) {
      QDomElement elem = xml.ownerDocument().createElement(tag);
      xml.appendChild(elem);

      elem.setAttribute("href", link.uri.toString());
      writeXml(elem, "text", link.text);
      writeXml(elem, "type", link.type);
    }
  }
}

static void writeXml(QDomNode& xml, const IGisItem::history_t& history) {
  if (history.events.size() > 1) {
    QDomElement xmlHistory = xml.ownerDocument().createElement("ql:history");
    xml.appendChild(xmlHistory);
    for (int i = 0; i <= history.histIdxCurrent; i++) {
      const IGisItem::history_event_t& event = history.events[i];
      QDomElement xmlEvent = xml.ownerDocument().createElement("ql:event");
      xmlHistory.appendChild(xmlEvent);
      writeXml(xmlEvent, "ql:icon", event.icon);
      writeXml(xmlEvent, "ql:time", event.time);
      writeXml(xmlEvent, "ql:comment", event.comment);
    }
  }
}

static void writeXml(QDomNode& xml, const QString& tag, const QPoint& offsetBubble, quint32 widthBubble) {
  QDomElement elem = xml.ownerDocument().createElement(tag);
  xml.appendChild(elem);

  elem.setAttribute("xoff", offsetBubble.x());
  elem.setAttribute("yoff", offsetBubble.y());
  elem.setAttribute("width", widthBubble);
}

static void readXml(const QDomNode& node, const QString& parentTags, QHash<QString, QVariant>& extensions) {
  QString tag = node.nodeName();
  if ((tag.left(8) == "ql:flags") || (tag.left(11) == "ql:activity")) {
    return;
  }

  QString tags = parentTags.isEmpty() ? tag : parentTags + "|" + tag;
  const QDomNode& next = node.firstChild();
  if (next.isText()) {
    extensions[tags] = node.toElement().text();
  } else {
    const QDomNodeList& list = node.childNodes();
    for (int i = 0; i < list.size(); i++) {
      readXml(list.at(i), tags, extensions);
    }
  }
}

static void readXml(const QDomNode& ext, QHash<QString, QVariant>& extensions) {
  const QDomNodeList& list = ext.childNodes();
  for (int i = 0; i < list.size(); i++) {
    readXml(list.at(i), "", extensions);
  }

  extensions.squeeze();
}

static void writeXml(QDomNode& ext, const QHash<QString, QVariant>& extensions) {
  if (extensions.isEmpty()) {
    return;
  }

  QDomDocument doc = ext.ownerDocument();

  QStringList keys = extensions.keys();
  std::sort(keys.begin(), keys.end(), [](const QString& k1, const QString& k2) {
    return CKnownExtension::get(k1).order < CKnownExtension::get(k2).order;
  });

  for (const QString& key : std::as_const(keys)) {
    QStringList tags = key.split('|', Qt::SkipEmptyParts);

    if (tags.size() == 1) {
      QDomElement elem = doc.createElement(tags.first());
      ext.appendChild(elem);
      QDomText text = doc.createTextNode(extensions[key].toString());
      elem.appendChild(text);
    } else {
      QDomNode node = ext;

      QString lastTag = tags.last();
      tags.pop_back();
      for (const QString& tag : std::as_const(tags)) {
        QDomNode child = node.firstChildElement(tag);
        if (child.isNull()) {
          QDomElement elem = doc.createElement(tags.first());
          node.appendChild(elem);
          node = elem;
        } else {
          node = child;
        }
      }
      QDomElement elem = doc.createElement(lastTag);
      node.appendChild(elem);

      QDomText text = doc.createTextNode(extensions[key].toString());
      elem.appendChild(text);
    }
  }
}

void IGisProject::readMetadata(const QDomNode& xml, metadata_t& metadata) {
  readXml(xml, "name", metadata.name);
  readXml(xml, "desc", metadata.desc);

  const QDomNode& xmlAuthor = xml.namedItem("author");
  if (xmlAuthor.isElement()) {
    readXml(xml, "name", metadata.author.name);

    const QDomNode& xmlEmail = xmlAuthor.namedItem("email");
    if (xmlEmail.isElement()) {
      const QDomNamedNodeMap& attr = xmlEmail.attributes();
      metadata.author.id = attr.namedItem("id").nodeValue();
      metadata.author.domain = attr.namedItem("domain").nodeValue();
    }

    const QDomNode& xmlLink = xmlAuthor.namedItem("link");
    if (xmlLink.isElement()) {
      metadata.author.link.uri.setUrl(xmlLink.attributes().namedItem("href").nodeValue());
      readXml(xmlLink, "text", metadata.author.link.text);
      readXml(xmlLink, "type", metadata.author.link.type);
    }
  }

  const QDomNode& xmlCopyright = xml.namedItem("copyright");
  if (xmlCopyright.isElement()) {
    metadata.copyright.author = xmlCopyright.attributes().namedItem("author").nodeValue();
    readXml(xmlCopyright, "year", metadata.copyright.year);
    readXml(xmlCopyright, "license", metadata.copyright.license);
  }

  readXml(xml, "link", metadata.links);
  readXml(xml, "time", metadata.time);
  readXml(xml, "keywords", metadata.keywords);

  const QDomNode& xmlBounds = xml.namedItem("bounds");
  if (xmlBounds.isElement()) {
    const QDomNamedNodeMap& attr = xmlBounds.attributes();
    metadata.bounds.setLeft(attr.namedItem("minlon").nodeValue().toDouble());
    metadata.bounds.setTop(attr.namedItem("maxlat").nodeValue().toDouble());
    metadata.bounds.setRight(attr.namedItem("maxlon").nodeValue().toDouble());
    metadata.bounds.setBottom(attr.namedItem("minlat").nodeValue().toDouble());
  }
}

QDomNode IGisProject::writeMetadata(QDomDocument& doc, bool strictGpx11) {
  QDomElement gpx = doc.createElement("gpx");
  doc.appendChild(gpx);

  gpx.setAttribute("version", "1.1");
  gpx.setAttribute("creator", "QMapShack " VER_STR " http://www.qlandkarte.org/");
  gpx.setAttribute("xmlns", gpx_ns);
  gpx.setAttribute("xmlns:xsi", xsi_ns);

  QString schemaLocation;
  if (!strictGpx11) {
    gpx.setAttribute("xmlns:gpxx", gpxx_ns);
    gpx.setAttribute("xmlns:gpxtpx", gpxtpx_ns);
    gpx.setAttribute("xmlns:wptx1", wptx1_ns);
    gpx.setAttribute("xmlns:rmc", rmc_ns);
    gpx.setAttribute("xmlns:ql", ql_ns);
    gpx.setAttribute("xmlns:tp1", tp1_ns);
    gpx.setAttribute("xmlns:gpxdata", gpxdata_ns);

    schemaLocation = QString() + gpx_ns + " http://www.topografix.com/GPX/1/1/gpx.xsd " + gpxx_ns +
                     " http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd " + gpxtpx_ns +
                     " http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd " + wptx1_ns +
                     " http://www.garmin.com/xmlschemas/WaypointExtensionv1.xsd " + ql_ns +
                     " http://www.qlandkarte.org/xmlschemas/v1.1/ql-extensions.xsd " + gpxdata_ns +
                     " http://www.cluetrust.com/Schemas/gpxdata10.xsd";
  } else {
    schemaLocation = QString() + gpx_ns + " http://www.topografix.com/GPX/1/1/gpx.xsd ";
  }

  gpx.setAttribute("xsi:schemaLocation", schemaLocation);

  QDomElement xmlMetadata = doc.createElement("metadata");
  gpx.appendChild(xmlMetadata);

  writeXml(xmlMetadata, "name", metadata.name);
  writeXml(xmlMetadata, "desc", html2Dev(metadata.desc));

  if (!metadata.author.name.isEmpty()) {
    QDomElement xmlAuthor = doc.createElement("author");
    xmlMetadata.appendChild(xmlAuthor);

    writeXml(xmlAuthor, "name", metadata.author.name);

    if (!metadata.author.id.isEmpty() && !metadata.author.domain.isEmpty()) {
      QDomElement xmlEmail = doc.createElement("email");
      xmlAuthor.appendChild(xmlEmail);
      xmlEmail.setAttribute("id", metadata.author.id);
      xmlEmail.setAttribute("domain", metadata.author.domain);
    }

    if (metadata.author.link.uri.isValid()) {
      QDomElement xmlLink = doc.createElement("link");
      xmlAuthor.appendChild(xmlLink);

      xmlLink.setAttribute("href", metadata.author.link.uri.toString());
      writeXml(xmlLink, "text", metadata.author.link.text);
      writeXml(xmlLink, "type", metadata.author.link.type);
    }
  }

  if (!metadata.copyright.author.isEmpty()) {
    QDomElement xmlCopyright = doc.createElement("copyright");
    xmlMetadata.appendChild(xmlCopyright);

    xmlCopyright.setAttribute("author", metadata.copyright.author);
    writeXml(xmlCopyright, "year", metadata.copyright.year);
    writeXml(xmlCopyright, "license", metadata.copyright.license);
  }
  writeXml(xmlMetadata, "link", metadata.links);
  writeXml(xmlMetadata, "time", metadata.time);
  writeXml(xmlMetadata, "keywords", metadata.keywords);

  if (metadata.bounds.isValid()) {
    QDomElement xmlBounds = doc.createElement("bounds");
    xmlMetadata.appendChild(xmlBounds);

    xmlBounds.setAttribute("minlat", metadata.bounds.bottom());
    xmlBounds.setAttribute("minlon", metadata.bounds.left());
    xmlBounds.setAttribute("maxlat", metadata.bounds.top());
    xmlBounds.setAttribute("maxlon", metadata.bounds.right());
  }

  return gpx;
}

void CGisItemWpt::readGpx(const QDomNode& xml) {
  readWpt(xml, wpt);
  // decode some well known extensions
  if (xml.namedItem("extensions").isElement()) {
    const QDomNode& ext = xml.namedItem("extensions");
    readXml(ext, "ql:key", key.item);
    readXml(ext, "ql:flags", flags);
    readXml(ext, "ql:bubble", offsetBubble, widthBubble);
    readXml(ext, history);

    const QDomNode& wptx1 = ext.namedItem("wptx1:WaypointExtension");
    readXml(wptx1, "wptx1:Proximity", proximity);

    const QDomNode& xmlCache = ext.namedItem("cache");
    if (!xmlCache.isNull()) {
      // read OC cache extensions
    }
  }

  const QDomNode& xmlCache = xml.namedItem("groundspeak:cache");
  if (!xmlCache.isNull() && !geocache.hasData) {
    readGcExt(xmlCache);
  }
}

void CGisItemWpt::save(QDomNode& gpx, bool strictGpx11) {
  QDomDocument doc = gpx.ownerDocument();

  QDomElement xmlWpt = doc.createElement("wpt");
  gpx.appendChild(xmlWpt);
  writeWpt(xmlWpt, wpt, strictGpx11);

  if (!strictGpx11) {
    // write the key as extension tag
    QDomElement xmlExt = doc.createElement("extensions");
    xmlWpt.appendChild(xmlExt);
    writeXml(xmlExt, "ql:key", key.item);
    writeXml(xmlExt, "ql:flags", flags);
    writeXml(xmlExt, "ql:bubble", offsetBubble, widthBubble);
    writeXml(xmlExt, history);

    // write other well known extensions
    QDomElement wptx1 = doc.createElement("wptx1:WaypointExtension");
    xmlExt.appendChild(wptx1);
    writeXml(wptx1, "wptx1:Proximity", proximity);

    if (geocache.hasData /*&& geocache.service == eGC*/) {
      QDomElement xmlCache = doc.createElement("groundspeak:cache");
      writeGcExt(xmlCache);
      xmlWpt.appendChild(xmlCache);
    }
  }
}

void CGisItemWpt::readGcExt(const QDomNode& xmlCache) {
  // Geocaches only have one link
  if (wpt.links.isEmpty()) {
    geocache.service = eUnknown;
  } else {
    if (wpt.links.first().uri.url(QUrl::RemovePath).contains("geocaching.com")) {
      geocache.service = eGcCom;
    } else if (wpt.links.first().uri.url(QUrl::RemovePath).contains("opencaching")) {
      geocache.service = eOc;
    } else if (wpt.links.first().uri.url(QUrl::RemovePath).contains("geocaching.su")) {
      geocache.service = eGcSu;
    } else {
      geocache.service = eUnknown;
    }
  }

  const QDomNamedNodeMap& attr = xmlCache.attributes();
  geocache.id = attr.namedItem("id").nodeValue().toInt();

  QDomNode geocacheAttributes = xmlCache.namedItem("groundspeak:attributes");

  geocache.archived = attr.namedItem("archived").nodeValue().toLocal8Bit().toLower() == "true";
  geocache.available = attr.namedItem("available").nodeValue().toLocal8Bit().toLower() == "true";

  for (QDomNode xmlAttribute = geocacheAttributes.firstChild(); !xmlAttribute.isNull();
       xmlAttribute = xmlAttribute.nextSibling()) {
    quint8 id = xmlAttribute.attributes().namedItem("id").nodeValue().toUInt();
    if (id >= geocache.attributeMeanings.size()) {
      qWarning() << "CGisItemWpt::readGcExt(): Ignore unknown attribute ID " << id;
      continue;
    }

    qint8 intvalue = xmlAttribute.attributes().namedItem("inc").nodeValue().toUInt();
    geocache.attributes[id] = (intvalue == 1);
    if (id == 42)  // 42 is the code for 'Needs maintenance' and it only appears, when there attribute is set
    {
      geocache.needsMaintenance = true;
    }
  }

  readXml(xmlCache, "groundspeak:name", geocache.name);
  readXml(xmlCache, "groundspeak:placed_by", geocache.owner);
  readXml(xmlCache, "groundspeak:type", geocache.type);
  readXml(xmlCache, "groundspeak:container", geocache.container);
  readXml(xmlCache, "groundspeak:difficulty", geocache.difficulty);
  readXml(xmlCache, "groundspeak:terrain", geocache.terrain);
  readXml(xmlCache, "groundspeak:short_description", geocache.shortDesc, geocache.shortDescIsHtml);
  readXml(xmlCache, "groundspeak:long_description", geocache.longDesc, geocache.longDescIsHtml);
  readXml(xmlCache, "groundspeak:encoded_hints", geocache.hint);
  readXml(xmlCache, "groundspeak:country", geocache.country);
  readXml(xmlCache, "groundspeak:state", geocache.state);

  const QDomNodeList& logs = xmlCache.toElement().elementsByTagName("groundspeak:log");
  uint N = logs.count();

  for (uint n = 0; n < N; ++n) {
    const QDomNode& xmlLog = logs.item(n);
    const QDomNamedNodeMap& attr = xmlLog.attributes();

    geocachelog_t log;
    log.id = attr.namedItem("id").nodeValue().toUInt();
    readXml(xmlLog, "groundspeak:date", log.date);
    readXml(xmlLog, "groundspeak:type", log.type);
    if (xmlLog.namedItem("groundspeak:finder").isElement()) {
      const QDomNamedNodeMap& attr = xmlLog.namedItem("groundspeak:finder").attributes();
      log.finderId = attr.namedItem("id").nodeValue();
    }

    readXml(xmlLog, "groundspeak:finder", log.finder);
    readXml(xmlLog, "groundspeak:text", log.text, log.textIsHtml);

    geocache.logs << log;
  }
  geocache.hasData = true;
}

void CGisItemWpt::writeGcExt(QDomNode& xmlCache) {
  QString str;
  xmlCache.toElement().setAttribute("xmlns:groundspeak", "http://www.groundspeak.com/cache/1/0");
  xmlCache.toElement().setAttribute("id", geocache.id);
  xmlCache.toElement().setAttribute("archived", geocache.archived ? "True" : "False");
  xmlCache.toElement().setAttribute("available", geocache.available ? "True" : "False");

  writeXml(xmlCache, "groundspeak:name", geocache.name);
  writeXml(xmlCache, "groundspeak:placed_by", geocache.owner);
  writeXml(xmlCache, "groundspeak:type", geocache.type);
  writeXml(xmlCache, "groundspeak:container", geocache.container);

  QDomElement xmlAttributes = xmlCache.ownerDocument().createElement("groundspeak:attributes");
  const QList<quint8>& attributes = geocache.attributes.keys();
  for (quint8 attribute : attributes) {
    QDomElement xmlAttribute = xmlCache.ownerDocument().createElement("groundspeak:attribute");
    xmlAttribute.setAttribute("id", attribute);
    qint8 inc = geocache.attributes[attribute] ? 1 : 0;
    xmlAttribute.setAttribute("inc", inc);
    QDomText text;
    if (attribute < geocache.attributeMeanings.size()) {
      text = xmlCache.ownerDocument().createTextNode(geocache.attributeMeanings[attribute]);
    }
    xmlAttribute.appendChild(text);
    xmlAttributes.appendChild(xmlAttribute);
    xmlCache.appendChild(xmlAttributes);
  }

  if (geocache.difficulty == int(geocache.difficulty)) {
    str = QString::asprintf("%1.0f", geocache.difficulty);
  } else {
    str = QString::asprintf("%1.1f", geocache.difficulty);
  }
  writeXml(xmlCache, "groundspeak:difficulty", str);

  if (geocache.terrain == int(geocache.terrain)) {
    str = QString::asprintf("%1.0f", geocache.terrain);
  } else {
    str = QString::asprintf("%1.1f", geocache.terrain);
  }
  writeXml(xmlCache, "groundspeak:terrain", str);
  writeXml(xmlCache, "groundspeak:short_description", geocache.shortDesc, geocache.shortDescIsHtml);
  writeXml(xmlCache, "groundspeak:long_description", geocache.longDesc, geocache.longDescIsHtml);
  writeXml(xmlCache, "groundspeak:encoded_hints", geocache.hint);

  if (!geocache.logs.isEmpty()) {
    QDomElement xmlLogs = xmlCache.ownerDocument().createElement("groundspeak:logs");
    xmlCache.appendChild(xmlLogs);

    for (const geocachelog_t& log : std::as_const(geocache.logs)) {
      QDomElement xmlLog = xmlCache.ownerDocument().createElement("groundspeak:log");
      xmlLogs.appendChild(xmlLog);

      xmlLog.setAttribute("id", log.id);
      writeXml(xmlLog, "groundspeak:date", log.date);
      writeXml(xmlLog, "groundspeak:type", log.type);

      QDomElement xmlFinder = xmlCache.ownerDocument().createElement("groundspeak:finder");
      xmlLog.appendChild(xmlFinder);

      QDomText _finder_ = xmlCache.ownerDocument().createCDATASection(log.finder);
      xmlFinder.appendChild(_finder_);
      xmlFinder.setAttribute("id", log.finderId);

      writeXml(xmlLog, "groundspeak:text", log.text, log.textIsHtml);
    }
  }
}

void CGisItemTrk::readTrk(const QDomNode& xml, CTrackData& trk) {
  readXml(xml, "name", trk.name);
  readXml(xml, "cmt", trk.cmt);
  readXml(xml, "desc", trk.desc);
  readXml(xml, "src", trk.src);
  readXml(xml, "link", trk.links);
  readXml(xml, "number", trk.number);
  readXml(xml, "type", trk.type);

  const QDomNodeList& trksegs = xml.toElement().elementsByTagName("trkseg");
  int N = trksegs.count();
  trk.segs.resize(N);
  for (int n = 0; n < N; ++n) {
    const QDomNode& trkseg = trksegs.item(n);
    CTrackData::trkseg_t& seg = trk.segs[n];

    const QDomNodeList& xmlTrkpts = trkseg.toElement().elementsByTagName("trkpt");
    int M = xmlTrkpts.count();
    seg.pts.resize(M);
    for (int m = 0; m < M; ++m) {
      CTrackData::trkpt_t& trkpt = seg.pts[m];
      const QDomNode& xmlTrkpt = xmlTrkpts.item(m);
      readWpt(xmlTrkpt, trkpt);

      const QDomNode& ext = xmlTrkpt.namedItem("extensions");
      if (ext.isElement()) {
        readXml(ext, "ql:flags", trkpt.flags);
        readXml(ext, "ql:activity", trkpt.activity);
        trkpt.sanitizeFlags();
        readXml(ext, trkpt.extensions);
      }
    }
  }

  // decode some well known extensions
  const QDomNode& ext = xml.namedItem("extensions");
  if (ext.isElement()) {
    readXml(ext, "ql:key", key.item);
    readXml(ext, "ql:flags", flags);
    readXml(ext, history);

    const QDomNode& gpxx = ext.namedItem("gpxx:TrackExtension");
    readXml(gpxx, "gpxx:DisplayColor", trk.color);
    setColor(str2color(trk.color));
  }

  deriveSecondaryData();
}

void CGisItemTrk::save(QDomNode& gpx, bool strictGpx11) {
  QDomDocument doc = gpx.ownerDocument();

  QDomElement xmlTrk = doc.createElement("trk");
  gpx.appendChild(xmlTrk);

  writeXml(xmlTrk, "name", trk.name);
  writeXml(xmlTrk, "cmt", html2Dev(trk.cmt, strictGpx11));
  writeXml(xmlTrk, "desc", html2Dev(trk.desc, strictGpx11));
  writeXml(xmlTrk, "src", trk.src);
  writeXml(xmlTrk, "link", trk.links);
  writeXml(xmlTrk, "number", trk.number);
  writeXml(xmlTrk, "type", trk.type);

  if (!strictGpx11) {
    // write the key as extension tag
    QDomElement xmlExt = doc.createElement("extensions");
    xmlTrk.appendChild(xmlExt);
    writeXml(xmlExt, "ql:key", key.item);
    writeXml(xmlExt, "ql:flags", flags);
    writeXml(xmlExt, history);

    // write other well known extensions
    QDomElement gpxx = doc.createElement("gpxx:TrackExtension");
    xmlExt.appendChild(gpxx);
    writeXml(gpxx, "gpxx:DisplayColor", trk.color);
  }

  for (const CTrackData::trkseg_t& seg : std::as_const(trk.segs)) {
    QDomElement xmlTrkseg = doc.createElement("trkseg");
    xmlTrk.appendChild(xmlTrkseg);

    for (const CTrackData::trkpt_t& pt : seg.pts) {
      QDomElement xmlTrkpt = doc.createElement("trkpt");
      xmlTrkseg.appendChild(xmlTrkpt);
      writeWpt(xmlTrkpt, pt, strictGpx11);

      if (!strictGpx11) {
        QDomElement xmlExt = doc.createElement("extensions");
        xmlTrkpt.appendChild(xmlExt);
        writeXml(xmlExt, "ql:flags", pt.flags);
        writeXml(xmlExt, "ql:activity", pt.activity);
        writeXml(xmlExt, pt.extensions);
      }
    }
  }
}

void CGisItemRte::readRte(const QDomNode& xml, rte_t& rte) {
  readXml(xml, "name", rte.name);
  readXml(xml, "cmt", rte.cmt);
  readXml(xml, "desc", rte.desc);
  readXml(xml, "src", rte.src);
  readXml(xml, "link", rte.links);
  readXml(xml, "number", rte.number);
  readXml(xml, "type", rte.type);

  const QDomNodeList& xmlRtepts = xml.toElement().elementsByTagName("rtept");
  int M = xmlRtepts.count();
  rte.pts.resize(M);
  for (int m = 0; m < M; ++m) {
    rtept_t& rtept = rte.pts[m];
    const QDomNode& xmlRtept = xmlRtepts.item(m);
    readWpt(xmlRtept, rtept);
    rtept.icon = CWptIconManager::self().getWptIconByName(rtept.sym, rtept.focus);
  }

  // decode some well known extensions
  if (xml.namedItem("extensions").isElement()) {
    const QDomNode& ext = xml.namedItem("extensions");
    readXml(ext, "ql:key", key.item);
  }
}

void CGisItemRte::save(QDomNode& gpx, bool strictGpx11) {
  QDomDocument doc = gpx.ownerDocument();

  QDomElement xmlRte = doc.createElement("rte");
  gpx.appendChild(xmlRte);

  writeXml(xmlRte, "name", rte.name);
  writeXml(xmlRte, "cmt", html2Dev(rte.cmt, strictGpx11));
  writeXml(xmlRte, "desc", html2Dev(rte.desc, strictGpx11));
  writeXml(xmlRte, "src", rte.src);
  writeXml(xmlRte, "link", rte.links);
  writeXml(xmlRte, "number", rte.number);
  writeXml(xmlRte, "type", rte.type);

  if (!strictGpx11) {
    // write the key as extension tag
    QDomElement xmlExt = doc.createElement("extensions");
    xmlRte.appendChild(xmlExt);
    writeXml(xmlExt, "ql:key", key.item);
  }

  for (const rtept_t& pt : std::as_const(rte.pts)) {
    QDomElement xmlRtept = doc.createElement("rtept");
    xmlRte.appendChild(xmlRtept);
    writeWpt(xmlRtept, pt, strictGpx11);
  }
}

void CGisItemOvlArea::readArea(const QDomNode& xml, area_t& area) {
  readXml(xml, "ql:name", area.name);
  readXml(xml, "ql:cmt", area.cmt);
  readXml(xml, "ql:desc", area.desc);
  readXml(xml, "ql:src", area.src);
  readXml(xml, "ql:link", area.links);
  readXml(xml, "ql:number", area.number);
  readXml(xml, "ql:type", area.type);
  readXml(xml, "ql:color", area.color);
  readXml(xml, "ql:width", area.width);
  readXml(xml, "ql:style", area.style);
  readXml(xml, "ql:opacity", area.opacity);
  readXml(xml, "ql:key", key.item);
  readXml(xml, "ql:flags", flags);
  readXml(xml, history);

  const QDomNodeList& xmlPts = xml.toElement().elementsByTagName("ql:point");
  int M = xmlPts.count();
  area.pts.resize(M);
  for (int m = 0; m < M; ++m) {
    pt_t& pt = area.pts[m];
    const QDomNode& xmlPt = xmlPts.item(m);
    readWpt(xmlPt, pt);
  }

  setColor(str2color(area.color));

  deriveSecondaryData();
}

void CGisItemOvlArea::save(QDomNode& gpx, bool strictGpx11) {
  QDomDocument doc = gpx.ownerDocument();

  QDomElement xmlArea = doc.createElement("ql:area");
  gpx.appendChild(xmlArea);

  writeXml(xmlArea, "ql:name", area.name);
  writeXml(xmlArea, "ql:cmt", area.cmt);
  writeXml(xmlArea, "ql:desc", area.desc);
  writeXml(xmlArea, "ql:src", area.src);
  writeXml(xmlArea, "ql:link", area.links);
  writeXml(xmlArea, "ql:number", area.number);
  writeXml(xmlArea, "ql:type", area.type);
  writeXml(xmlArea, "ql:color", area.color);
  writeXml(xmlArea, "ql:width", area.width);
  writeXml(xmlArea, "ql:style", area.style);
  writeXml(xmlArea, "ql:opacity", area.opacity);
  writeXml(xmlArea, "ql:key", key.item);
  writeXml(xmlArea, "ql:flags", flags);
  writeXml(xmlArea, history);

  for (const pt_t& pt : std::as_const(area.pts)) {
    QDomElement xmlPt = doc.createElement("ql:point");
    xmlArea.appendChild(xmlPt);
    writeWpt(xmlPt, pt, strictGpx11);
  }
}

void IGisItem::readWpt(const QDomNode& xml, wpt_t& wpt) {
  const QDomNamedNodeMap& attr = xml.attributes();
  wpt.lat = attr.namedItem("lat").nodeValue().toDouble();
  wpt.lon = attr.namedItem("lon").nodeValue().toDouble();

  readXml(xml, "ele", wpt.ele);
  readXml(xml, "time", wpt.time);
  readXml(xml, "magvar", wpt.magvar);
  readXml(xml, "geoidheight", wpt.geoidheight);
  readXml(xml, "name", wpt.name);
  readXml(xml, "cmt", wpt.cmt);
  readXml(xml, "desc", wpt.desc);
  readXml(xml, "src", wpt.src);
  readXml(xml, "link", wpt.links);
  readXml(xml, "sym", wpt.sym);
  readXml(xml, "type", wpt.type);
  readXml(xml, "fix", wpt.fix);
  readXml(xml, "sat", wpt.sat);
  readXml(xml, "hdop", wpt.hdop);
  readXml(xml, "vdop", wpt.vdop);
  readXml(xml, "pdop", wpt.pdop);
  readXml(xml, "ageofdgpsdata", wpt.ageofdgpsdata);
  readXml(xml, "dgpsid", wpt.dgpsid);

  // some GPX 1.0 backward compatibility
  QString url;
  readXml(xml, "url", url);
  if (!url.isEmpty()) {
    link_t link;
    link.uri.setUrl(url);
    readXml(xml, "urlname", link.text);

    wpt.links << link;
  }
}

void IGisItem::writeWpt(QDomElement& xml, const wpt_t& wpt, bool strictGpx11) {
  QString str;

  str = QString::asprintf("%1.8f", wpt.lat);
  xml.setAttribute("lat", str);
  str = QString::asprintf("%1.8f", wpt.lon);
  xml.setAttribute("lon", str);

  writeXml(xml, "ele", wpt.ele);
  writeXml(xml, "time", wpt.time);
  writeXml(xml, "magvar", wpt.magvar);
  writeXml(xml, "geoidheight", wpt.geoidheight);
  writeXml(xml, "name", wpt.name);
  writeXml(xml, "cmt", html2Dev(wpt.cmt, strictGpx11));
  writeXml(xml, "desc", html2Dev(wpt.desc, strictGpx11));
  if (isOnDevice() != IDevice::eTypeGarmin) {
    writeXml(xml, "src", wpt.src);
  }
  writeXml(xml, "link", wpt.links);
  writeXml(xml, "sym", wpt.sym);
  writeXml(xml, "type", wpt.type);
  writeXml(xml, "fix", wpt.fix);
  writeXml(xml, "sat", wpt.sat);
  writeXml(xml, "hdop", wpt.hdop);
  writeXml(xml, "vdop", wpt.vdop);
  writeXml(xml, "pdop", wpt.pdop);
  writeXml(xml, "ageofdgpsdata", wpt.ageofdgpsdata);
  writeXml(xml, "dgpsid", wpt.dgpsid);
}

void CDeviceGarmin::createAdventureFromProject(IGisProject* project, const QString& gpxFilename) {
  if (pathAdventures.isEmpty()) {
    return;
  }

  QDomDocument doc;

  QDomElement adventure = doc.createElement("Adventure");
  doc.appendChild(adventure);
  adventure.setAttribute("xmlns", "http://www.garmin.com/xmlschemas/GarminAdventure/v1");

  writeXml(adventure, "GlobalId", project->getKey());
  writeXml(adventure, "Name", project->getName());

  QDomElement item = doc.createElement("Item");
  adventure.appendChild(item);
  writeXml(item, "DataType", "GPSData");
  writeXml(item, "Location", gpxFilename);

  writeXml(adventure, "Description", IGisItem::removeHtml(project->getDescription()));

  const int N = project->childCount();
  for (int i = 0; i < N; i++) {
    CGisItemTrk* track = dynamic_cast<CGisItemTrk*>(project->child(i));
    if (track != nullptr) {
      const CTrackData& trk = track->getTrackData();
      if (trk.segs.isEmpty()) {
        continue;
      }

      if (trk.segs.first().pts.isEmpty()) {
        continue;
      }

      const CTrackData::trkpt_t& origin = trk.segs.first().pts.first();

      QDomElement startPosition = doc.createElement("StartPosition");
      adventure.appendChild(startPosition);
      writeXml(startPosition, "Lat", origin.lat);
      writeXml(startPosition, "Lon", origin.lon);

      writeXml(adventure, "Activity", tr("Unknown"));
      writeXml(adventure, "Distance", track->getTotalDistance());
      writeXml(adventure, "Duration", track->getTotalElapsedSecondsMoving());
      writeXml(adventure, "Ascent", track->getTotalAscent());
      writeXml(adventure, "Descent", track->getTotalDescent());
      writeXml(adventure, "Difficulty", 1);
      writeXml(adventure, "NumRatings", 0);
      writeXml(adventure, "MainTrackId", track->getName());

      QDomElement waypointOrder = doc.createElement("WaypointOrder");
      adventure.appendChild(waypointOrder);

      for (const CTrackData::trkpt_t& trkpt : trk) {
        if (trkpt.keyWpt.item.isEmpty()) {
          continue;
        }

        const CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(project->getItemByKey(trkpt.keyWpt));
        if (wpt == nullptr) {
          continue;
        }

        QDomElement waypoints = doc.createElement("Waypoints");
        waypointOrder.appendChild(waypoints);

        writeXml(waypoints, "ID", wpt->getName());
        writeXml(waypoints, "DistanceFromOrigin", trkpt.distance);
      }

      break;
    }
  }

  const QDir dirAdventures(dir.absoluteFilePath(pathAdventures));
  QString filename = dirAdventures.absoluteFilePath(project->getKey() + ".adv");
  QFile file(filename);

  CDeviceMountLock mountLock(*this);

  file.open(QIODevice::WriteOnly);
  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << Qt::endl;
  out << doc.toString();
  file.close();
}
