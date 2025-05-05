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

#ifndef CFIT2PROJECT_H
#define CFIT2PROJECT_H

#include <QtCore>
#include <fit_activity_mesg_listener.hpp>
#include <fit_date_time.hpp>
#include <fit_device_info_mesg_listener.hpp>
#include <fit_event_mesg_listener.hpp>
#include <fit_file_creator_mesg_listener.hpp>
#include <fit_file_id_mesg_listener.hpp>
#include <fit_lap_mesg_listener.hpp>
#include <fit_mesg_listener.hpp>
#include <fit_record_mesg_listener.hpp>
#include <fit_session_mesg_listener.hpp>

#include "gis/prj/IGisProject.h"
#include "gis/trk/CTrackData.h"

class CFit2Project : public fit::FileIdMesgListener,
                     public fit::DeviceInfoMesgListener,
                     public fit::MesgListener,
                     public fit::RecordMesgListener,
                     public fit::ActivityMesgListener,
                     public fit::SessionMesgListener,
                     public fit::LapMesgListener,
                     public fit::EventMesgListener,
                     public fit::FileCreatorMesgListener,
                     public IGisProject {
  Q_DECLARE_TR_FUNCTIONS(CFit2Project)
 public:
  CFit2Project(QFile& file, const QString& filename, IDevice* parent);
  CFit2Project(const QString& filename, CGisListWks* parent);
  virtual ~CFit2Project() = default;

  const QString getFileDialogFilter() const override { return IGisProject::filedialogFilterFIT; }
  const QString getFileExtension() const override { return "fit"; }
  bool canSave() const override { return false; }

 private:
  constexpr static qreal degrees = 180.0;
  constexpr static qreal twoPow31 = 2147483648;

  void loadFitFromFile(const QString& filename, bool showErrorMsg);
  void decodeFile(const QString& filename);

  void OnMesg(fit::FileIdMesg& mesg) override;
  void OnMesg(fit::DeviceInfoMesg& mesg) override;
  void OnMesg(fit::Mesg& mesg) override;
  void OnMesg(fit::RecordMesg& mesg) override;
  void OnMesg(fit::ActivityMesg& mesg) override;
  void OnMesg(fit::SessionMesg& mesg) override;
  void OnMesg(fit::LapMesg& mesg) override;
  void OnMesg(fit::EventMesg& mesg) override;
  void OnMesg(fit::FileCreatorMesg& mesg) override;

  inline QDateTime dateTimeFromFitToQt(FIT_DATE_TIME t, const QTimeZone& tz = QTimeZone::UTC) {
    return QDateTime::fromSecsSinceEpoch(fit::DateTime(t).GetTimeT(), tz);
  }
  inline qreal semicircleToDegree(qint32 semicircles) { return semicircles * (degrees / twoPow31); }

  static const QSet<std::string> knownMessages;

  CTrackData track;
  CTrackData::trkseg_t segment;
};

#endif  // CFIT2PROJECT_H
