/**********************************************************************************************
    Copyright (C) 2025 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/fit2/CFit2Project.h"

#include <fit_decode.hpp>
#include <fit_mesg_broadcaster.hpp>
#include <fstream>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/trk/CGisItemTrk.h"

const QSet<std::string> CFit2Project::knownMessages = {"file_id",  "session", "lap",         "event",
                                                       "activity", "record",  "device_info", "file_creator"};

CFit2Project::CFit2Project(QFile& file, const QString& filename, IDevice* parent)
    : IGisProject(eTypeFit, filename, parent) {
  if (file.isOpen()) {
    file.close();
  }
  loadFitFromFile(file.fileName(), false);

  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}

CFit2Project::CFit2Project(const QString& filename, CGisListWks* parent) : IGisProject(eTypeFit, filename, parent) {
  loadFitFromFile(filename, false);
  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}

void CFit2Project::loadFitFromFile(const QString& filename, bool showErrorMsg) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/FitProject.png"));
  blockUpdateItems(true);
  try {
    decodeFile(filename);
    markAsSaved();
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
  } catch (const std::exception& e) {
    if (showErrorMsg) {
      QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to load file %1...").arg(filename),
                            e.what(), QMessageBox::Abort);
    } else {
      qWarning() << "Failed to load FIT file:" << e.what();
    }
    valid = false;
  }

  sortItems();
  blockUpdateItems(false);
}

void CFit2Project::decodeFile(const QString& filename) {
  // if the file does not exist, the filename is assumed to be a name for a new project
  if (!QFile::exists(filename)) {
    IGisProject::filename.clear();
    setupName(filename);
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
    return;
  }

  std::fstream file;
  file.open(filename.toStdString(), std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error(("Error opening file " + filename).toStdString());
  }

  fit::Decode decode;

  if (!decode.CheckIntegrity(file)) {
    qWarning() << "FIT file integrity failed. Attempting to decode...";
  }

  // Geeee, call the 90th to collect their old fashioned c++ code!
  // This is depressing.....
  fit::MesgBroadcaster mesgBroadcaster;
  mesgBroadcaster.AddListener((fit::FileIdMesgListener&)*this);
  mesgBroadcaster.AddListener((fit::DeviceInfoMesgListener&)*this);
  mesgBroadcaster.AddListener((fit::RecordMesgListener&)*this);
  mesgBroadcaster.AddListener((fit::MesgListener&)*this);
  mesgBroadcaster.AddListener((fit::ActivityMesgListener&)*this);
  mesgBroadcaster.AddListener((fit::SessionMesgListener&)*this);
  mesgBroadcaster.AddListener((fit::LapMesgListener&)*this);
  mesgBroadcaster.AddListener((fit::EventMesgListener&)*this);
  mesgBroadcaster.AddListener((fit::FileCreatorMesgListener&)*this);

  try {
    decode.Read(&file, &mesgBroadcaster, &mesgBroadcaster, nullptr);
  } catch (const fit::RuntimeException& e) {
    throw std::runtime_error((QStringLiteral("Exception decoding file: ") + e.what()).toStdString());
  }
}

void CFit2Project::OnMesg(fit::Mesg& mesg) {
  if (knownMessages.contains(mesg.GetName())) {
    return;
  }
  qDebug() << "Mesg" << mesg.GetNumFields() << mesg.GetName();
  for (int i = 0; i < mesg.GetNumFields(); i++) {
    fit::Field* filed = mesg.GetFieldByIndex(i);
    qDebug() << "  " << filed->GetName();
  }
}

void CFit2Project::OnMesg(fit::FileIdMesg& mesg) { /*qDebug() << mesg.GetName();*/ }

void CFit2Project::OnMesg(fit::DeviceInfoMesg& mesg) {
  // qDebug() << mesg.GetName() << dateTimeFromFitToQt(mesg.GetTimestamp());
}

void CFit2Project::OnMesg(fit::RecordMesg& mesg) {
  // qDebug() << mesg.GetName();
  // for (int i = 0; i < mesg.GetNumFields(); i++) {
  //   fit::Field* filed = mesg.GetFieldByIndex(i);
  //   qDebug() << "  " << filed->GetName();
  // }

  CTrackData::trkpt_t trkpt;
  if (mesg.IsTimestampValid()) {
    trkpt.time = dateTimeFromFitToQt(mesg.GetTimestamp());
  }
  if (mesg.IsPositionLatValid() && mesg.IsPositionLongValid()) {
    trkpt.lon = semicircleToDegree(mesg.GetPositionLong());
    trkpt.lat = semicircleToDegree(mesg.GetPositionLat());

    trkpt.valid |= (trkpt.lat < -90) || (trkpt.lat > 90) || (trkpt.lon < -180) || (trkpt.lon > 180)
                       ? quint32(CTrackData::trkpt_t::eInvalidPos)
                       : quint32(CTrackData::trkpt_t::eValidPos);
  }
  if (mesg.IsAltitudeValid()) {
    trkpt.ele = mesg.GetAltitude();
  }
  if (mesg.IsEnhancedAltitudeValid()) {
    trkpt.ele = mesg.GetEnhancedAltitude();
  }
  if (mesg.IsSpeedValid()) {
    trkpt.extensions["speed"] = mesg.GetSpeed();
  }
  if (mesg.IsEnhancedSpeedValid()) {
    trkpt.extensions["fit:speed"] = mesg.GetEnhancedSpeed();
  }
  if (mesg.IsDistanceValid()) {
    trkpt.extensions["fit:distance"] = mesg.GetDistance();
  }
  if (mesg.IsTemperatureValid()) {
    trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:atemp"] = mesg.GetTemperature();
  }
  if (mesg.IsHeartRateValid()) {
    trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = mesg.GetHeartRate();
  }
  if (mesg.IsCadenceValid()) {
    trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"] = mesg.GetCadence();
  }
  if (mesg.IsPowerValid()) {
    trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:power"] = mesg.GetPower();
  }

  if (trkpt.isValid(CTrackData::trkpt_t::eValidPos)) {
    segment.pts.append(trkpt);
  } else {
    qWarning() << "invalid track point in FIT record" << trkpt.time << trkpt << trkpt.ele << trkpt.extensions
               << "- skip";
  }
}

void CFit2Project::OnMesg(fit::ActivityMesg& mesg) {
  // qDebug() << mesg.GetName() << dateTimeFromFitToQt(mesg.GetTimestamp()) << mesg.GetEventType();

  // for (int i = 0; i < mesg.GetNumFields(); i++) {
  //   fit::Field* filed = mesg.GetFieldByIndex(i);
  //   qDebug() << "  " << filed->GetName();
  // }
}

void CFit2Project::OnMesg(fit::SessionMesg& mesg) {
  // qDebug() << mesg.GetName() << dateTimeFromFitToQt(mesg.GetTimestamp()) << mesg.GetEventType();
  // for (int i = 0; i < mesg.GetNumFields(); i++) {
  //   fit::Field* filed = mesg.GetFieldByIndex(i);
  //   qDebug() << "  " << filed->GetName();
  // }

  if (!segment.isEmpty()) {
    track.segs.append(segment);
    segment.pts.clear();
  }

  QString name = IUnit::datetime2string(track.segs.first().pts.first().time, IUnit::eTimeFormatShort);
  if (mesg.IsStartTimeValid()) {
    name = IUnit::datetime2string(dateTimeFromFitToQt(mesg.GetStartTime()), IUnit::eTimeFormatShort);
  }

  QString comment = "<div><b>Device Statistic</b><br/>";
  QString val, unit;
  if (mesg.IsTotalElapsedTimeValid()) {
    IUnit::self().seconds2time(mesg.GetTotalElapsedTime(), val, unit);
    comment += tr("total elapsed time: %1%2<br/>").arg(val, unit);
  }

  if (mesg.IsTotalDistanceValid()) {
    IUnit::self().meter2distance(mesg.GetTotalDistance(), val, unit);
    comment += tr("total distance: %1%2<br/>").arg(val, unit);
  }

  if (mesg.IsEnhancedAvgSpeedValid()) {
    IUnit::self().meter2speed(mesg.GetEnhancedAvgSpeed(), val, unit);
    comment += tr("enhanced average speed: %1%2<br/>").arg(val, unit);
  }

  if (mesg.IsEnhancedMaxSpeedValid()) {
    IUnit::self().meter2speed(mesg.GetEnhancedMaxSpeed(), val, unit);
    comment += tr("enhanced max speed: %1%2<br/>").arg(val, unit);
  }

  if (mesg.IsEnhancedMinAltitudeValid()) {
    IUnit::self().meter2elevation(mesg.GetEnhancedMinAltitude(), val, unit);
    comment += tr("enhanced min altitude: %1%2<br/>").arg(val, unit);
  }

  if (mesg.IsEnhancedMaxAltitudeValid()) {
    IUnit::self().meter2elevation(mesg.GetEnhancedMaxAltitude(), val, unit);
    comment += tr("enhanced max altitude: %1%2<br/>").arg(val, unit);
  }

  if (mesg.IsTotalCaloriesValid()) {
    comment += tr("total calories: %1kcal<br/>").arg(mesg.GetTotalCalories());
  }

  if (mesg.GetTotalAscent()) {
    IUnit::self().meter2elevation(mesg.GetTotalAscent(), val, unit);
    comment += tr("total ascent: %1%2<br/>").arg(val, unit);
  }

  if (mesg.GetTotalDescent()) {
    IUnit::self().meter2elevation(mesg.GetTotalDescent(), val, unit);
    comment += tr("total descent: %1%2<br/>").arg(val, unit);
  }

  if (mesg.IsNumLapsValid()) {
    comment += tr("number of laps: %1<br/>").arg(mesg.GetNumLaps());
  }

  // if (mesg.IsSportValid()) {
  //   comment += tr("sport: %1<br/>").arg(mesg.GetSport());
  // }

  comment += "</div>";

  if (!track.isEmpty()) {
    track.name = name;
    track.cmt = comment;
    new CGisItemTrk(track, this);
  }

  track = CTrackData();
}

void CFit2Project::OnMesg(fit::LapMesg& mesg) {
  // qDebug() << mesg.GetName() << dateTimeFromFitToQt(mesg.GetTimestamp());
  // for (int i = 0; i < mesg.GetNumFields(); i++) {
  //   fit::Field* filed = mesg.GetFieldByIndex(i);
  //   qDebug() << "  " << filed->GetName();
  // }

  if (!segment.isEmpty()) {
    track.segs.append(segment);
    segment.pts.clear();
  }
}

void CFit2Project::OnMesg(fit::EventMesg& mesg) {
  // qDebug() << mesg.GetName() << dateTimeFromFitToQt(mesg.GetTimestamp()) << mesg.GetEventType();

  if (mesg.IsEventTypeValid()) {
    switch (mesg.GetEventType()) {
      case FIT_EVENT_TYPE_START:
      case FIT_EVENT_TYPE_STOP:
      case FIT_EVENT_TYPE_STOP_ALL:
        if (!segment.isEmpty()) {
          track.segs.append(segment);
          segment.pts.clear();
        }
        break;
    }
  }
}

void CFit2Project::OnMesg(fit::FileCreatorMesg& mesg) { /*qDebug() << mesg.GetName();*/ }
