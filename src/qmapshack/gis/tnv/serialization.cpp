/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

struct twonav_icon_t {
  const char* twonav;
  const char* qlgt;
};

static const twonav_icon_t TwoNavIcons[] = {{"City (Capitol)", "City (Capitol)"},
                                            {"City (Large)", "City (Large)"},
                                            {"City (Medium)", "City (Medium)"},
                                            {"City (Small)", "City (Small)"},
                                            {"City (Small)", "Small City"},
                                            {"Closed Box", "Geocache"},
                                            {"Open Box", "Geocache Found"},
                                            {"Red Flag", "Flag, Red"},
                                            {"Blue Flag", "Flag, Blue"},
                                            {"Green Flag", "Flag, Green"},
                                            {"Red Booble", "Pin, Red"},
                                            {"Blue Booble", "Pin, Blue"},
                                            {"Green Booble", "Pin, Green"},
                                            {"Red Cube", "Block, Red"},
                                            {"Blue Cube", "Block, Blue"},
                                            {"Green Cube", "Block, Green"},
                                            {"Blue Diamond", "Blue Diamond"},
                                            {"Green Diamond", "Green Diamond"},
                                            {"Red Diamond", "Red Diamond"},
                                            {"Traditional Cache", "Traditional Cache"},
                                            {"Multi-cache", "Multi-cache"},
                                            {"Unknown Cache", "Unknown Cache"},
                                            {"Wherigo", "Wherigo Cache"},
                                            {"Event Cache", "Event Cache"},
                                            {"Earthcache", "Earthcache"},
                                            {"Letterbox", "Letterbox Hybrid"},
                                            {"Virtual Cache", "Virtual Cache"},
                                            {"Webcam Cache", "Webcam Cache"},
                                            {0, 0}};

static QStringList writeCompeTime(const QDateTime& t, bool isTrack) {
  QStringList result;
  QString dateFormat;

  if (!t.isValid()) {
    if (isTrack) {
      result << "01-Jan-1970"
             << "00:00:00.000";
    } else {
      result << "01-Jan-1970"
             << "00:00:00";
    }
    return result;
  }

  QDateTime timestamp = t.toTimeSpec(Qt::UTC);

  QString monthStrs[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  QString monthStr = monthStrs[timestamp.date().month()];

  if (isTrack) {
    dateFormat = QString("dd-'%1'-yy").arg(monthStr);
  } else {
    dateFormat = QString("dd-'%1'-yyyy").arg(monthStr);
  }

  result << timestamp.toString(dateFormat);

  if (isTrack) {
    result << timestamp.toString("hh:mm:ss.000");
  } else {
    result << timestamp.toString("hh:mm:ss");
  }

  return result;
}

static QDateTime readCompeTime(QString str, bool isTrack) {
  QDateTime timestamp;
  static const QRegularExpression re(QRegularExpression::anchoredPattern("([0-9]{2})-([A-Za-z]{3})-.*"));

  const QRegularExpressionMatch& match = re.match(str);
  if (match.hasMatch()) {
    QString monthStr = match.captured(2);

    QHash<QString, QString> monthStr2Num{{"JAN", "01"}, {"FEB", "02"}, {"MAR", "03"}, {"APR", "04"},
                                         {"MAY", "05"}, {"JUN", "06"}, {"JUL", "07"}, {"AUG", "08"},
                                         {"SEP", "09"}, {"OCT", "10"}, {"NOV", "11"}, {"DEC", "12"}};

    str.replace(monthStr, monthStr2Num.value(monthStr.toUpper()));

    if (isTrack) {
      timestamp = QDateTime::fromString(str, "dd-MM-yy hh:mm:ss.zzz");
      if (timestamp.isValid()) {
        timestamp = timestamp.addYears(100);
      }
    } else {
      timestamp = QDateTime::fromString(str, "dd-MM-yyyy hh:mm:ss");
    }
  }

  timestamp.setTimeSpec(Qt::UTC);
  return timestamp;
}

static QString iconTwoNav2QlGt(const QString& sym) {
  int i = 0;
  while (TwoNavIcons[i].qlgt) {
    if (sym == TwoNavIcons[i].twonav) {
      return TwoNavIcons[i].qlgt;
    }

    i++;
  }

  return sym;
}

static QString iconQlGt2TwoNav(const QString& sym) {
  int i = 0;
  while (TwoNavIcons[i].qlgt) {
    if (sym == TwoNavIcons[i].qlgt) {
      return TwoNavIcons[i].twonav;
    }

    i++;
  }

  return sym;
}

static QString makeUniqueName(const QString& name, const QDir& dir) {
  int cnt = 0;

  QFileInfo fi(name);
  QString tmp(name);

  while (dir.exists(tmp)) {
    tmp = QString("%1_%2.%3").arg(fi.baseName()).arg(cnt++).arg(fi.completeSuffix());
  }

  return tmp;
}

bool CGisItemTrk::saveTwoNav(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to open %1.").arg(filename),
                          QMessageBox::Abort);
    return false;
  }

  QDir dir(QFileInfo(filename).absoluteDir());
  IGisProject* project = getParentProject();

  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << Qt::bom;
  out << "B  UTF-8" << Qt::endl;
  out << "G  WGS 84" << Qt::endl;
  out << "U  1" << Qt::endl;

  QString name = getName();
  name = name.replace(" ", "_");

  QColor color = getColor();

  QStringList list;
  list << "C";
  list << QString::number(color.red());
  list << QString::number(color.green());
  list << QString::number(color.blue());
  list << "5";  // ???
  list << "1";  // ???
  out << list.join(" ") << Qt::endl;

  out << "s " << name << Qt::endl;
  out << "y " << getKey().item << Qt::endl;

  for (const CTrackData::trkseg_t& seg : std::as_const(trk.segs)) {
    for (const CTrackData::trkpt_t& trkpt : seg.pts) {
      list.clear();

      list << "T";
      list << "A";
      list << (trkpt.lat > 0 ? QString("%1%2N") : QString("%1%2S")).arg(trkpt.lat, 0, 'f').arg(QChar(186));
      list << (trkpt.lon > 0 ? QString("%1%2E") : QString("%1%2W")).arg(trkpt.lon, 0, 'f').arg(QChar(186));
      list << writeCompeTime(trkpt.time, true);
      list << "s";
      list << QString("%1").arg(trkpt.ele == NOINT ? 0 : trkpt.ele);
      list << "0.000000";
      list << "0.000000";
      list << "0.000000";
      list << "0";
      list << "-1000.000000";
      list << "-1.000000";
      list << "-1";
      list << "-1.000000";
      list << "-1";
      list << "-1";
      list << "-1";
      list << "-1.000000";

      out << list.join(" ") << Qt::endl;

      if (!trkpt.keyWpt.item.isEmpty() && project) {
        CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(project->getItemByKey(trkpt.keyWpt));
        if (wpt) {
          QString iconName = wpt->getIconName();
          QPixmap icon = wpt->getIcon();
          icon = icon.scaledToWidth(15, Qt::SmoothTransformation);
          iconName = iconQlGt2TwoNav(iconName);
          iconName = iconName.replace(" ", "_");

          icon.save(dir.absoluteFilePath(iconName + ".png"));

          list.clear();
          list << (iconName + ".png");
          list << "1";
          list << "3";
          list << "0";
          list << wpt->getName();
          out << "a " << list.join(",") << Qt::endl;

          for (const CGisItemWpt::image_t& img : wpt->getImages()) {
            QString fn = img.info;
            if (fn.isEmpty()) {
              fn = QString("picture.png");
            }

            QFileInfo fi(fn);

            if (!(fi.completeSuffix() == "png")) {
              fn = fi.baseName() + ".png";
            }

            fn = makeUniqueName(fn, dir);
            img.pixmap.save(dir.absoluteFilePath(fn));

            list.clear();
            list << fn;
            list << "1";
            list << "8";
            list << "0";
            out << "a " << list.join(",") << Qt::endl;
          }

          QString comment = wpt->getComment();
          if (!IGisItem::removeHtml(comment).isEmpty()) {
            QString filenameCmt = QString("QMS_CMT%1.html").arg(wpt->getKey().item);
            QFile fileCmt(dir.absoluteFilePath(filenameCmt));
            fileCmt.open(QIODevice::WriteOnly);

            QTextStream stream(&fileCmt);
            stream << Qt::bom << comment;

            fileCmt.close();

            out << "a .\\" << filenameCmt << ",0" << Qt::endl;
          }
        }
      }
    }
  }

  return true;
}

bool CGisItemTrk::readTwoNav(const QString& filename) {
  QString line("start");

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                             tr("Failed to open %1.").arg(filename), QMessageBox::Abort, QMessageBox::Abort);
    return false;
  }
  QTextStream in(&file);
  in.setEncoding(QStringConverter::Utf8);

  CTrackData::trkseg_t seg;

  while (!line.isEmpty()) {
    line = in.readLine();
    switch (line[0].toLatin1()) {
      case 'B': {
        QString name = line.mid(1).simplified();
        auto codec = QStringConverter::encodingForName(name.toLatin1());
        if (codec.has_value()) {
          in.setEncoding(codec.value());
        }
        break;
      }

      case 'G': {
        QString name = line.mid(1).simplified();
        if (name != "WGS 84") {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                                   tr("Only support lon/lat WGS 84 format."), QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }
        break;
      }

      case 'U': {
        QString name = line.mid(1).simplified();
        if (name != "1") {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                                   tr("Only support lon/lat WGS 84 format."), QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }
        break;
      }

      case 'C': {
        QStringList values = line.split(' ', Qt::SkipEmptyParts);
        if (values.size() > 2) {
          QColor c(values[1].toInt(), values[2].toInt(), values[3].toInt());
          setColor(c);
        } else {
          values = values[1].split(',', Qt::SkipEmptyParts);
          if (values.size() >= 3) {
            QColor c(values[0].toInt(), values[1].toInt(), values[2].toInt());
            setColor(c);
          }
        }
        break;
      }

      case 'T': {
        CTrackData::trkpt_t pt;
        QStringList values = line.split(' ', Qt::SkipEmptyParts);

        const int N = values.size();
        if (N < 8) {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read data."),
                                   QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }

        QString lat = values[2].replace(QChar(186), "").replace(QChar(-3), "");
        QString lon = values[3].replace(QChar(186), "").replace(QChar(-3), "");
        IUnit::strToDeg(lat + " " + lon, pt.lon, pt.lat);

        pt.time = readCompeTime(values[4] + " " + values[5], true);
        pt.ele = qRound(values[7].toFloat());

        if (N > 13) {
          pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:atemp"] = values[13].toFloat();
        }

        if (N > 14) {
          pt.sat = values[14].toInt();
        }

        seg.pts << pt;
        break;
      }

      case 's': {
        trk.name = line.mid(1).simplified();
        trk.name = trk.name.replace("_", " ");
        break;
      }

      case 'y': {
        key.item = line.mid(1).simplified();
        break;
      }

      case 'x': {
        if (seg.pts.isEmpty()) {
          break;
        }
        QStringList values = line.mid(1).split(",");
        CTrackData::trkpt_t& pt = seg.pts.last();

        const int N = values.size();
        if (N > 0) {
          pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:atemp"] = values[0].toFloat() / 10;
        }
        if (N > 1) {
          pt.hdop = values[1].toInt();
        }
        if (N > 2) {
          pt.vdop = values[2].toInt();
        }

        break;
      }
    }
  }
  file.close();

  trk.segs << seg;

  if (trk.name.isEmpty()) {
    QFileInfo fi(filename);
    trk.name = fi.baseName();
  }

  deriveSecondaryData();

  return true;
}

void CGisItemWpt::saveTwoNav(QTextStream& out, const QDir& dir) {
  QString name = getName();
  name = name.replace(" ", "_");

  QString description = getDescription();
  description = removeHtml(description);

  QStringList list;
  list << "W";
  list << name;
  list << "A";
  list << (wpt.lat > 0 ? QString("%1%2N") : QString("%1%2S")).arg(wpt.lat, 0, 'f').arg(QChar(186));
  list << (wpt.lon > 0 ? QString("%1%2E") : QString("%1%2W")).arg(wpt.lon, 0, 'f').arg(QChar(186));
  list << writeCompeTime(wpt.time, false);
  list << QString("%1").arg(wpt.ele == NOINT ? 0 : wpt.ele);

  out << list.join(" ") << " ";
  out << description << Qt::endl;

  list.clear();
  list << iconQlGt2TwoNav(getIconName());
  list << "0";  // test position
  list << "-1.0";
  list << "0";
  list << QString("%1").arg(QColor(Qt::darkBlue).value());
  list << "1";
  list << "37";  // 1 Name 2 Beschreibung 4 Symbol 8 Hhe 16 URL 32 Radius
  list << "";    // wpt->link;
  list << QString("%1").arg(proximity == NOFLOAT ? 0 : proximity, 0, 'f');
  list << getKey().item;

  out << "w ";
  out << list.join(",");
  out << Qt::endl;

  QString comment = getComment();
  if (!IGisItem::removeHtml(comment).isEmpty()) {
    QString filenameCmt = QString("QMS_CMT%1.html").arg(getKey().item);
    QFile fileCmt(dir.absoluteFilePath(filenameCmt));
    fileCmt.open(QIODevice::WriteOnly);

    QTextStream stream(&fileCmt);
    stream << Qt::bom << comment;

    fileCmt.close();

    out << "a .\\" << filenameCmt << ",0" << Qt::endl;
  }

  for (const image_t& img : std::as_const(images)) {
    QString fn = img.info;
    if (fn.isEmpty()) {
      fn = QString("picture.png");
    }

    QFileInfo fi(fn);

    if (!(fi.completeSuffix().toLower() == "png")) {
      fn = fi.baseName() + ".png";
    }

    fn = makeUniqueName(fn, dir);
    img.pixmap.save(dir.absoluteFilePath(fn));
    out << "a .\\" << fn << Qt::endl;
  }

  if (isGeocache()) {
    // write geocache data
    QDomDocument doc;
    QDomElement gpxCache = doc.createElement("groundspeak:cache");
    writeGcExt(gpxCache);
    doc.appendChild(gpxCache);

    out << "e" << Qt::endl;
    out << doc.toString();
    out << "ee" << Qt::endl;
  }
}

bool CTwoNavProject::loadWpts(const QString& filename, const QDir& dir) {
  wpt_t wpt;
  QString line("start");
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                             tr("Failed to open %1.").arg(filename), QMessageBox::Abort, QMessageBox::Abort);
    return false;
  }
  QTextStream in(&file);
  in.setEncoding(QStringConverter::Utf8);

  while (!line.isEmpty()) {
    line = in.readLine();

    switch (line[0].toLatin1()) {
      case 'B': {
        QString name = line.mid(1).simplified();
        auto codec = QStringConverter::encodingForName(name.toLatin1());
        if (codec.has_value()) {
          in.setEncoding(codec.value());
        }
        break;
      }

      case 'G': {
        QString name = line.mid(1).simplified();
        if (name != "WGS 84") {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                                   tr("Only support lon/lat WGS 84 format."), QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }
        break;
      }

      case 'U': {
        QString name = line.mid(1).simplified();
        if (name != "1") {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                                   tr("Only support lon/lat WGS 84 format."), QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }
        break;
      }

      case 'W': {
        if (wpt.valid) {
          new CGisItemWpt(wpt, this);
        }

        wpt = wpt_t();
        QStringList values = line.split(' ', Qt::SkipEmptyParts);

        if (values.size() < 8) {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read data."),
                                   QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }

        wpt.name = values[1];

        QString lat = values[3].replace(QChar(186), "").replace(QChar(-3), "");
        QString lon = values[4].replace(QChar(186), "").replace(QChar(-3), "");
        IUnit::strToDeg(lat + " " + lon, wpt.lon, wpt.lat);

        wpt.time = readCompeTime(values[5] + " " + values[6], false);
        wpt.ele = values[7].toFloat();

        if (values.size() > 7) {
          QStringList list = values.mid(8);
          wpt.description = list.join(" ");
        }

        break;
      }

      case 'w': {
        QStringList values = line.mid(1).simplified().split(',', Qt::KeepEmptyParts);

        if (values.size() < 10) {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read data."),
                                   QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }

        wpt.symbol = iconTwoNav2QlGt(values[0]);

        wpt.url = values[7];
        wpt.prox = values[8].toFloat();
        wpt.key = values[9];

        if (wpt.prox == 0) {
          wpt.prox = NOFLOAT;
        }
        if (wpt.ele == 0) {
          wpt.ele = NOINT;
        }
        if (wpt.key == "0") {
          wpt.key.clear();
        }

        wpt.name = wpt.name.replace("_", " ");

        if (!wpt.key.isEmpty()) {
          QString filenameCmt = QString("QMS_CMT%1.html").arg(wpt.key);
          if (QFile::exists(dir.absoluteFilePath(filenameCmt))) {
            QFile fileCmt(dir.absoluteFilePath(filenameCmt));
            if (fileCmt.open(QIODevice::ReadOnly)) {
              wpt.comment = QTextStream(&fileCmt).readAll();
              fileCmt.close();
            }
          }
        }

        wpt.valid = true;
        break;
      }

      case 'e': {
        QString str;

        while (!in.atEnd()) {
          line = in.readLine();
          if (line == "ee") {
            break;
          }

          str += line;
        }

        QString errorMsg;
        int errorLine = 0;
        int errorColumn = 0;
        wpt.gpx.setContent(str, &errorMsg, &errorLine, &errorColumn);
        break;
      }

      case 'a': {
        img_t img;
        QStringList values = line.mid(1).simplified().split(',', Qt::KeepEmptyParts);
        if (values.size() < 1) {
          QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read data."),
                                   QMessageBox::Abort, QMessageBox::Abort);
          return false;
        }

        QString fn = values[0].simplified();

#ifndef Q_OS_WIN64
        fn = fn.replace("\\", "/");
#endif
        QFileInfo fi(dir.absoluteFilePath(fn));
        img.image.load(dir.absoluteFilePath(fn));
        if (!img.image.isNull()) {
          img.filename = fi.fileName();
          img.info = fi.baseName();
          wpt.images << img;
        }

        break;
      }
    }
  }

  if (wpt.valid) {
    new CGisItemWpt(wpt, this);
  }

  return true;
}

void CGisItemWpt::readTwoNav(const CTwoNavProject::wpt_t& tnvWpt) {
  wpt.lon = tnvWpt.lon;
  wpt.lat = tnvWpt.lat;
  wpt.ele = tnvWpt.ele;
  proximity = tnvWpt.prox;
  wpt.time = tnvWpt.time;
  wpt.name = tnvWpt.name;
  wpt.cmt = tnvWpt.comment;
  wpt.desc = tnvWpt.description;
  wpt.sym = tnvWpt.symbol;
  key.item = tnvWpt.key;

  for (const CTwoNavProject::img_t& img : tnvWpt.images) {
    CGisItemWpt::image_t image;
    image.fileName = img.filename;
    image.info = img.info;
    image.pixmap = img.image;
    images << image;
  }

  const QDomNode& xmlCache = tnvWpt.gpx.namedItem("groundspeak:cache");
  if (!xmlCache.isNull()) {
    readGcExt(xmlCache);
  }

  setIcon();
}
