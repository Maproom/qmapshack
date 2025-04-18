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

#include "gis/suunto/CSmlProject.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/suunto/ISuuntoProject.h"
#include "gis/trk/CGisItemTrk.h"

const QList<extension_t> CSmlProject::extensions = {
    {"Latitude", RAD_TO_DEG, 0.0, ASSIGN_VALUE(lat, NIL)}  // unit [°]
    ,
    {"Longitude", RAD_TO_DEG, 0.0, ASSIGN_VALUE(lon, NIL)}  // unit [°]
    ,
    {"Altitude", 1.0, 0.0, ASSIGN_VALUE(ele, NIL)}  // unit [m]
    ,
    {"VerticalSpeed", 1.0, 0.0, ASSIGN_VALUE(extensions["gpxdata:verticalSpeed"], NIL)}  // unit [m/h]
    ,
    {"HR", 60.0, 0.0, ASSIGN_VALUE(extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"], qRound)}  // unit [bpm]
    ,
    {"Cadence", 60.0, 0.0, ASSIGN_VALUE(extensions["gpxdata:cadence"], NIL)}  // unit [bpm]
    ,
    {"Temperature", 1.0, -273.15, ASSIGN_VALUE(extensions["gpxdata:temp"], NIL)}  // unit [°C]
    ,
    {"SeaLevelPressure", 0.01, 0.0, ASSIGN_VALUE(extensions["gpxdata:seaLevelPressure"], NIL)}  // unit [hPa]
    ,
    {"Speed", 1.0, 0.0, ASSIGN_VALUE(extensions["gpxdata:speed"], NIL)}  // unit [m/s]
    ,
    {"EnergyConsumption", 60.0 / 4184.0, 0.0, ASSIGN_VALUE(extensions["gpxdata:energy"], NIL)}  // unit [kCal/min]
};

CSmlProject::CSmlProject(const QString& filename, CGisListWks* parent) : ISuuntoProject(eTypeSml, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/SmlProject.png"));
  blockUpdateItems(true);
  loadSml(filename);
  blockUpdateItems(false);
  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}

void CSmlProject::loadSml(const QString& filename) {
  try {
    loadSml(filename, this);
  } catch (QString& errormsg) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to load file %1...").arg(filename),
                          errormsg, QMessageBox::Abort);
    valid = false;
  }
}

void CSmlProject::loadSml(const QString& filename, CSmlProject* project) {
  QFile file(filename);

  // if the file does not exist, the filename is assumed to be a name for a new project
  if (!file.exists() || QFileInfo(filename).suffix().toLower() != "sml") {
    project->filename.clear();
    project->setupName(filename);
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
    return;
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

  QDomElement xmlSml = xml.documentElement();
  if (xmlSml.tagName() != "sml") {
    throw tr("Not an sml file: %1").arg(filename);
  }

  if (xmlSml.namedItem("DeviceLog").isElement()) {
    CTrackData trk;
    QDateTime time0;  // start time of the track

    const QDomNode& xmlDeviceLog = xmlSml.namedItem("DeviceLog");
    if (xmlDeviceLog.namedItem("Header").isElement()) {
      const QDomNode& xmlHeader = xmlDeviceLog.namedItem("Header");
      if (xmlHeader.namedItem("DateTime").isElement()) {
        QString dateTimeStr = xmlHeader.namedItem("DateTime").toElement().text();
        trk.name = dateTimeStr;  // date (in local time) of beginning of recording is chosen as track name
        IUnit::parseTimestamp(dateTimeStr, time0);  // as local time
      }

      if (xmlHeader.namedItem("Activity").isElement()) {
        trk.desc = xmlHeader.namedItem("Activity").toElement().text();
      }

      if (xmlHeader.namedItem("RecoveryTime").isElement()) {
        trk.cmt =
            tr("Recovery time: %1 h<br/>").arg(xmlHeader.namedItem("RecoveryTime").toElement().text().toInt() / 3600);
      }

      if (xmlHeader.namedItem("PeakTrainingEffect").isElement()) {
        trk.cmt += tr("Peak Training Effect: %1<br/>")
                       .arg(xmlHeader.namedItem("PeakTrainingEffect").toElement().text().toDouble());
      }

      if (xmlHeader.namedItem("Energy").isElement()) {
        trk.cmt +=
            tr("Energy: %1 kCal<br/>").arg((int)xmlHeader.namedItem("Energy").toElement().text().toDouble() / 4184);
      }

      if (xmlHeader.namedItem("BatteryChargeAtStart").isElement() && xmlHeader.namedItem("BatteryCharge").isElement() &&
          xmlHeader.namedItem("Duration").isElement())

      {
        trk.cmt += tr("Battery usage: %1 %/hour")
                       .arg(100 *
                                (xmlHeader.namedItem("BatteryChargeAtStart").toElement().text().toDouble() -
                                 xmlHeader.namedItem("BatteryCharge").toElement().text().toDouble()) /
                                (xmlHeader.namedItem("Duration").toElement().text().toDouble() / 3600),
                            0, 'f', 1);
      }
    }

    if (xmlDeviceLog.namedItem("Device").isElement()) {
      const QDomNode& xmlDevice = xmlDeviceLog.namedItem("Device");
      if (xmlDevice.namedItem("Name").isElement()) {
        trk.cmt = tr("Device: %1<br/>").arg(xmlDevice.namedItem("Name").toElement().text()) + trk.cmt;
      }
    }

    if (xmlDeviceLog.namedItem("Samples").isElement()) {
      const QDomNode& xmlSamples = xmlDeviceLog.namedItem("Samples");
      const QDomNodeList& xmlSampleList = xmlSamples.toElement().elementsByTagName("Sample");

      if (xmlSampleList.count() > 0) {
        bool UTCtimeFound = false;
        for (int i = 0; i < xmlSampleList.count(); i++)  // browse XML samples
        {                                                // look for samples with UTC timestamp
          const QDomNode& xmlSample = xmlSampleList.item(i);

          if (xmlSample.namedItem("UTC").isElement()) {
            QString timeStr = xmlSample.namedItem("UTC").toElement().text();

            if (timeStr.indexOf("Z") != NOIDX)  // "Z" means "UTC timestamp" ; note the even this element is <UTC>, this
                                                // does not mean that time is expressed as UTC
            {
              if (xmlSample.namedItem("Time").isElement()) {
                IUnit::parseTimestamp(timeStr, time0);
                time0 = time0.addMSecs(-xmlSample.namedItem("Time").toElement().text().toDouble() *
                                       1000.0);  // substract current sample time to get start time
                UTCtimeFound = true;
                break;
              }
            }
          }
        }

        if (!UTCtimeFound) {
          QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Use of local time..."),
                               tr("No UTC time has been found in file %1. "
                                  "Local computer time will be used. "
                                  "You can adjust time using a time filter if needed.")
                                   .arg(filename),
                               QMessageBox::Ok);
        }

        bool sampleWithPositionFound = false;
        QList<sample_t> samplesList;
        QList<QDateTime> lapsList;

        for (int i = 0; i < xmlSampleList.count(); i++)  // browse XML samples
        {
          sample_t sample;
          const QDomNode& xmlSample = xmlSampleList.item(i);

          if (xmlSample.namedItem("Latitude").isElement()) {
            sampleWithPositionFound = true;
          }

          if (xmlSample.namedItem("Time").isElement()) {
            sample.time = time0.addMSecs(xmlSample.namedItem("Time").toElement().text().toDouble() * 1000.0);
          }

          if (xmlSample.namedItem("Events").isElement()) {
            const QDomNode& xmlEvents = xmlSample.namedItem("Events");
            if (xmlEvents.namedItem("Lap").isElement()) {
              lapsList << sample.time;  // stores timestamps of the samples where the the "Lap" button has been pressed
            }
          } else  // samples without "Events" are the ones containing position, heart rate, etc... that we want to store
          {
            for (const extension_t& ext : extensions) {
              if (xmlSample.namedItem(ext.tag).isElement()) {
                const QDomNode& xmlSampleData = xmlSample.namedItem(ext.tag);
                sample[ext.tag] = xmlSampleData.toElement().text().toDouble() * ext.scale + ext.offset;
              }
            }
            samplesList << sample;
          }
        }

        if (!sampleWithPositionFound) {
          throw tr("This SML file does not contain any position data and can not be displayed by QMapShack: %1")
              .arg(filename);
        }

        fillTrackPointsFromSamples(samplesList, lapsList, trk, extensions);

        new CGisItemTrk(trk, project);

        project->sortItems();
        project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
        project->setToolTip(CGisListWks::eColumnName, project->getInfo());
        project->valid = true;
      }
    }
  }
}
