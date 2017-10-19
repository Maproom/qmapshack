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

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/sml/CSmlProject.h"
#include "gis/trk/CGisItemTrk.h"

#include <QtWidgets>
#include <functional>

using fTrkPtSetVal = std::function<void(CTrackData::trkpt_t&, qreal val)>;

struct extension_t
{
    QString tag;
    qreal scale;
    qreal offset;
    fTrkPtSetVal func;
};

static const QList<extension_t> extensions =
{
    {"Latitude", RAD_TO_DEG, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.lat = val;
         }
     }
    },
    {"Longitude", RAD_TO_DEG, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.lon = val;
         }
     }
    },
    {"Altitude", 1.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.ele = val;
         }
     }
    },
    {"VerticalSpeed", 1.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:verticalSpeed"] = val;
         }
     }
    },
    {"HR", 60.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = val;
         }
     }
    },
    {"Cadence", 60.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:cadence"] = val;
         }
     }
    },
    {"Temperature", 1.0, -273.15,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:temp"] = val;
         }
     }
    },
    {"SeaLevelPressure", 0.01, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:seaLevelPressure"] = val;
         }
     }
    },
    {"Speed", 1.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:speed"] = val;
         }
     }
    },
    {"EnergyConsumption", 60.0 / 4184.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:energy"] = val;
         }
     }
    }
};


CSmlProject::CSmlProject(const QString &filename, CGisListWks * parent)
    : IGisProject(eTypeSml, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/SmlProject.png"));
    blockUpdateItems(true);
    loadSml(filename);
    blockUpdateItems(false);
    setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}


void CSmlProject::loadSml(const QString& filename)
{
    try
    {
        loadSml(filename, this);
    }
    catch(QString &errormsg)
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                              tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
        valid = false;
    }
}



void CSmlProject::loadSml(const QString &filename, CSmlProject *project)
{
    QFile file(filename);

    // if the file does not exist, the filename is assumed to be a name for a new project
    if (!file.exists() || QFileInfo(filename).suffix().toLower() != "sml")
    {
        project->filename.clear();
        project->setupName(filename);
        project->setToolTip(CGisListWks::eColumnName, project->getInfo());
        project->valid = true;
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        throw tr("Failed to open %1").arg(filename);
    }

    // load file content to xml document
    QDomDocument xml;
    QString msg;
    int line;
    int column;
    if (!xml.setContent(&file, false, &msg, &line, &column))
    {
        file.close();
        throw tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg);
    }
    file.close();

    QDomElement xmlSml = xml.documentElement();
    if (xmlSml.tagName() != "sml")
    {
        throw tr("Not an sml file: %1").arg(filename);
    }

    if(xmlSml.namedItem("DeviceLog").isElement())
    {
        CTrackData trk;

        const QDomNode& xmlDeviceLog = xmlSml.namedItem("DeviceLog");
        if(xmlDeviceLog.namedItem("Header").isElement())
        {
            const QDomNode& xmlHeader = xmlDeviceLog.namedItem("Header");
            if(xmlHeader.namedItem("DateTime").isElement())
            {
                trk.name = xmlHeader.namedItem("DateTime").toElement().text();
            }                                                                  // date of beginning of recording is chosen as track name
            if(xmlHeader.namedItem("Activity").isElement())
            {
                trk.desc = xmlHeader.namedItem("Activity").toElement().text();
            }

            if(xmlHeader.namedItem("RecoveryTime").isElement())
            {
                trk.cmt = tr("Recovery time: %1 h<br>").arg(xmlHeader.namedItem("RecoveryTime").toElement().text().toInt() / 3600);
            }

            if(xmlHeader.namedItem("PeakTrainingEffect").isElement())
            {
                trk.cmt += tr("Peak Training Effect: %1<br>").arg(xmlHeader.namedItem("PeakTrainingEffect").toElement().text().toDouble());
            }

            if(xmlHeader.namedItem("Energy").isElement())
            {
                trk.cmt += tr("Energy: %1 kCal<br>").arg((int)xmlHeader.namedItem("Energy").toElement().text().toDouble() / 4184);
            }


            if(xmlHeader.namedItem("BatteryChargeAtStart").isElement() &&
               xmlHeader.namedItem("BatteryCharge").isElement() &&
               xmlHeader.namedItem("Duration").isElement() )

            {
                trk.cmt += tr("Battery usage: %1 %/hour")
                           .arg( 100*(xmlHeader.namedItem("BatteryChargeAtStart").toElement().text().toDouble()
                                      - xmlHeader.namedItem("BatteryCharge").toElement().text().toDouble())
                                 / (xmlHeader.namedItem("Duration").toElement().text().toDouble() / 3600), 0, 'f', 1);
            }
        }

        if(xmlDeviceLog.namedItem("Device").isElement())
        {
            const QDomNode& xmlDevice = xmlDeviceLog.namedItem("Device");
            if(xmlDevice.namedItem("Name").isElement())
            {
                trk.cmt =  tr("Device: %1<br>").arg(xmlDevice.namedItem("Name").toElement().text()) + trk.cmt;
            }
        }

        if(xmlDeviceLog.namedItem("Samples").isElement())
        {
            const QDomNode& xmlSamples = xmlDeviceLog.namedItem("Samples");
            const QDomNodeList& xmlSampleList = xmlSamples.toElement().elementsByTagName("Sample");

            if (xmlSampleList.count() > 0)
            {
                QDateTime time0;

                const QDomNode& xmlSample = xmlSampleList.item(0);
                if(xmlSample.namedItem("UTC").isElement())
                {
                    IUnit::parseTimestamp(xmlSample.namedItem("UTC").toElement().text(), time0);
                }

                QList<sml_sample_t> samplesList;
                QList<QDateTime> lapsList;

                bool sampleWithPositionFound = false;

                for (int i = 0; i < xmlSampleList.count(); i++) // browse XML samples
                {
                    sml_sample_t sample;
                    const QDomNode& xmlSample = xmlSampleList.item(i);

                    if(xmlSample.namedItem("Latitude").isElement())
                    {
                        sampleWithPositionFound = true;
                    }

                    if(xmlSample.namedItem("Time").isElement())
                    {
                        sample.time = time0.addMSecs(xmlSample.namedItem("Time").toElement().text().toDouble() * 1000.0);
                    }

                    if(xmlSample.namedItem("Events").isElement())
                    {
                        const QDomNode& xmlEvents = xmlSample.namedItem("Events");
                        if(xmlEvents.namedItem("Lap").isElement())
                        {
                            lapsList << sample.time; // stores timestamps of the samples where the the "Lap" button has been pressed
                        }
                    }
                    else // samples without "Events" are the ones containing position, heartrate, etc... that we want to store
                    {
                        for (const extension_t& ext  : extensions)
                        {
                            if (xmlSample.namedItem(ext.tag).isElement())
                            {
                                const QDomNode& xmlSampleData = xmlSample.namedItem(ext.tag);
                                sample.data[ext.tag] = xmlSampleData.toElement().text().toDouble() * ext.scale + ext.offset;
                            }
                        }
                        samplesList << sample;
                    }
                }

                if (!sampleWithPositionFound)
                {
                    throw tr("This SML file does not contain any position data and can not be displayed by QMapShack: %1").arg(filename);
                }


                for (const extension_t& ext  : extensions)
                {
                    fillMissingData(ext.tag, samplesList);
                }

                deleteSamplesWithDuplicateTimestamps(samplesList);


                lapsList << samplesList.last().time.addSecs(1); // a last dummy lap button push is added with timestamp = 1 s later than the last sample timestamp

                trk.segs.resize(lapsList.size() ); // segments are created and each of them contains 1 lap

                int lap = 0;
                CTrackData::trkseg_t *seg = &(trk.segs[lap]);

                for (int i = 0; i < samplesList.size(); i++)
                {
                    if (samplesList[i].time > lapsList[lap])
                    {
                        lap++;
                        seg = &(trk.segs[lap]);
                    }

                    CTrackData::trkpt_t trkpt;
                    trkpt.time = samplesList[i].time;

                    for(const extension_t& ext : extensions)
                    {
                        ext.func(trkpt, samplesList[i][ext.tag]);
                    }

                    seg->pts.append(trkpt);
                }

                new CGisItemTrk(trk, project);

                project->sortItems();
                project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
                project->setToolTip(CGisListWks::eColumnName, project->getInfo());
                project->valid = true;
            }
        }
    }
}


void CSmlProject::fillMissingData(const QString &dataField, QList<sml_sample_t> &samplesList)
{   // Suunto samples contain lat/lon OR heartrate, elevation, etc.., each one with its own timestamp.
    // The purpose of the code below is to "spread" data among samples.
    // At the end each sample contains data, linearly interpolated from its neighbours according to timestamps.
    QList<sml_sample_t*> samplesWithMissingDataList;
    sml_sample_t* currentSample = &samplesList.first();
    sml_sample_t* previousSampleWithData = nullptr;
    bool keepBrowsing = samplesList.size() > 0;
    int i = 0;

    while (keepBrowsing)
    {
        if (currentSample->data[dataField] == NOFLOAT) // sample with missing data found
        {
            samplesWithMissingDataList << currentSample;
        }
        else
        {
            if (nullptr == previousSampleWithData) // if this is the first sample containing data found
            {
                int j;
                for (j = 0; j < samplesWithMissingDataList.size(); j++)
                {
                    samplesWithMissingDataList[j]->data[dataField] =  currentSample->data[dataField];
                }
                previousSampleWithData = currentSample;
                samplesWithMissingDataList.clear();
            }
            else
            {
                qreal dY = currentSample->data[dataField] - previousSampleWithData->data[dataField];
                qreal dT = ((qreal)(currentSample->time.toMSecsSinceEpoch() - previousSampleWithData->time.toMSecsSinceEpoch())) / 1000.0;
                qreal slope = dY / dT;
                qreal offsetAt0 = previousSampleWithData->data[dataField]
                                  - slope * ( (  (qreal)(previousSampleWithData->time.toMSecsSinceEpoch())  ) / 1000.0 );

                int j;
                for (j = 0; j < samplesWithMissingDataList.size(); j++)
                {
                    //    interpolate data and apply them to samples in-between

                    samplesWithMissingDataList[j]->data[dataField] = (qreal)(
                        slope * (qreal)((samplesWithMissingDataList[j]->time.toMSecsSinceEpoch()) / 1000.0) + offsetAt0  );
                }
                previousSampleWithData = currentSample;
                samplesWithMissingDataList.clear();
            }
        }

        if (++i >= samplesList.size())
        {
            keepBrowsing = false;

            if (nullptr != previousSampleWithData)
            {
                int j;
                for (j = 0; j < samplesWithMissingDataList.size(); j++) //  case where samplesWithMissingDataList is not empty at the end can happen when last samples contain no data
                {
                    samplesWithMissingDataList[j]->data[dataField] = previousSampleWithData->data[dataField];// apply previous data found to all of the previous samples with no data
                }
            }
        }
        else
        {
            currentSample = &(samplesList[i]);
        }
    }
}


void CSmlProject::deleteSamplesWithDuplicateTimestamps(QList<sml_sample_t> &samplesList)
{
    sml_sample_t lastDummySample;
    lastDummySample.time = samplesList.last().time.addSecs(1);
    samplesList << lastDummySample; // this dummy sample will force the processing of the last samples when they have identical timestamps

    if (samplesList.count() >= 3) // = dummy sample + at least 2 samples with hypothetical identical timestamp
    {   // code below merges samples with identical timestamps.
        // Samples with identical timestamps are found when "pause" button is pressed (and maybe in some other cases, I can not say)
        QList<sml_sample_t *> samplesWithSameTimestampList;

        samplesWithSameTimestampList << &samplesList[0];
        for (int i = 1; i < samplesList.size(); i++)
        {
            if (samplesWithSameTimestampList[0]->time == samplesList[i].time) // a sample with identical timestamp has been found
            {
                samplesWithSameTimestampList << &samplesList[i];
            }
            else if ( (samplesWithSameTimestampList.count() == 1) && (samplesWithSameTimestampList[0]->time != samplesList[i].time) )
            {   // the single stored sample and current sample have different timestamps
                samplesWithSameTimestampList.clear();
                samplesWithSameTimestampList << &samplesList[i];
            }
            else if  ( (samplesWithSameTimestampList.count() >= 2) && (samplesWithSameTimestampList[0]->time != samplesList[i].time) )
            {   // samples with identical timestamps have been found, and the current sample has a different timestamp (current sample can be the last dummy sample, see above)
                for (const extension_t& ext  : extensions)
                {
                    qreal sum = 0;
                    qreal samplesWithDataCount = 0;

                    for(int j = 0; j < samplesWithSameTimestampList.size(); j++)
                    {
                        if ( samplesWithSameTimestampList[j]->data[ext.tag] != NOFLOAT)
                        {
                            samplesWithDataCount++;
                            sum += samplesWithSameTimestampList[j]->data[ext.tag];
                        }
                    }
                    if ( samplesWithDataCount != 0)
                    {
                        samplesWithSameTimestampList[0]->data[ext.tag] = sum / samplesWithDataCount; // the first sample gets the averaged value
                    }
                }

                // remove samples with same timestamp but the first one
                for (int j = 0; j < samplesWithSameTimestampList.size() - 1; j++)
                {
                    samplesList.removeAt(1+i-samplesWithSameTimestampList.size());
                }

                i -= samplesWithSameTimestampList.count() - 1; // index i has to be moved because of removed samples

                samplesWithSameTimestampList.clear();
                samplesWithSameTimestampList << &samplesList[i]; // and current sample has to be stored
            }
        }
    }
    samplesList.removeLast(); // remove dummy sample
}
