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

using fTrkPtSetVal = std::function<void(CTrackData::trkpt_t&, qreal)>;

struct extension_t
{
    /// the tag as used in the SML file
    QString tag;
    /// a scale factor to be applied to the value stored in the SML file
    qreal scale;
    /// an offset to be applied to the value stored in the SML file
    qreal offset;
    /// an assignment function that assigns a value to a member of a trkpt_t object
    fTrkPtSetVal func;
};

static const QList<extension_t> extensions =
{
    // example how to do it without a lambda function
    //{"Latitude", RAD_TO_DEG, 0.0,assignLat}

    // unit [°]
    {"Latitude", RAD_TO_DEG, 0.0,
     // this is a c++11 lambda function.
     // think of it like a fTrkPtSetVal function that does
     // not need a function name. Of course I could define
     // those functions like a normal function and use their
     // name here. The lambda function simply allows me to
     // define what has to be done inline.
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.lat = val;
         }
     }
    },

    // unit [°]
    {"Longitude", RAD_TO_DEG, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.lon = val;
         }
     }
    },

    // unit [m]
    {"Altitude", 1.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.ele = val;
         }
     }
    },

    // unit [m/h]
    {"VerticalSpeed", 1.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:verticalSpeed"] = val;
         }
     }
    },

    // unit [bpm]
    {"HR", 60.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = val;
         }
     }
    },

    // unit [bpm]
    {"Cadence", 60.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:cadence"] = val;
         }
     }
    },

    // unit [°C]
    {"Temperature", 1.0, -273.15,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:temp"] = val;
         }
     }
    },

    // unit [hPa]
    {"SeaLevelPressure", 0.01, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:seaLevelPressure"] = val;
         }
     }
    },

    // unit [m/s]
    {"Speed", 1.0, 0.0,
     [](CTrackData::trkpt_t &pt, qreal val)
     {
         if(val != NOFLOAT)
         {
             pt.extensions["gpxdata:speed"] = val;
         }
     }
    },

    // unit [kCal/min]
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
                    else // samples without "Events" are the ones containing position, heart rate, etc... that we want to store
                    {
                        for (const extension_t& ext  : extensions)
                        {
                            if (xmlSample.namedItem(ext.tag).isElement())
                            {
                                const QDomNode& xmlSampleData = xmlSample.namedItem(ext.tag);
                                sample[ext.tag] = xmlSampleData.toElement().text().toDouble() * ext.scale + ext.offset;
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

                for(const sml_sample_t& sample : samplesList)
                {
                    if (sample.time > lapsList[lap])
                    {
                        lap++;
                        seg = &(trk.segs[lap]);
                    }

                    CTrackData::trkpt_t trkpt;
                    trkpt.time = sample.time;

                    for(const extension_t& ext : extensions)
                    {
                        if(sample.data.contains(ext.tag))
                        {
                            ext.func(trkpt, sample[ext.tag]);
                        }
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
{   // Suunto samples contain lat/lon OR heart rate, elevation, etc.., each one with its own timestamp.
    // The purpose of the code below is to "spread" data among samples.
    // At the end each sample contains data, linearly interpolated from its neighbors according to timestamps.
    QList<sml_sample_t> collect;
    QList<sml_sample_t> result;
    sml_sample_t previousSampleWithData;

    for(sml_sample_t& sample : samplesList)
    {
        collect << sample;

        if (sample.data.contains(dataField))
        {
            if (!previousSampleWithData.data.contains(dataField))
            {   // case where, at the beginning, first samples have no data
                for(sml_sample_t& collectedSample : collect)
                {
                    collectedSample[dataField] = sample[dataField];
                }
            }
            else
            {   // case where linear interpolation can be applied
                qreal dT = ((qreal)(collect.last().time.toMSecsSinceEpoch() - previousSampleWithData.time.toMSecsSinceEpoch())) / 1000.0;
                if (dT != 0) // dT == 0 when samples have the same timestamps ; this is managed later in deleteSamplesWithDuplicateTimestamps function
                {
                    qreal dY = collect.last().data[dataField] - previousSampleWithData.data[dataField];
                    qreal slope = dY / dT;
                    qreal offsetAt0 = previousSampleWithData.data[dataField]
                                      - slope * ( (  (qreal)(previousSampleWithData.time.toMSecsSinceEpoch())  ) / 1000.0 );

                    for(sml_sample_t& collectedSample : collect)
                    {   // apply interpolation to collected samples
                        collectedSample[dataField] = (qreal)(slope * (qreal)((collectedSample.time.toMSecsSinceEpoch()) / 1000.0) + offsetAt0  );
                    }
                }
            }

            previousSampleWithData = collect.last();
            result << collect;
            collect.clear();
        }
     }

    if (previousSampleWithData.data.contains(dataField))
    {
        for(sml_sample_t& collectedSample : collect)
        {   // processing last remaining collected samples without data
            collectedSample[dataField] = previousSampleWithData[dataField];
        }
    }

    result << collect;
    samplesList = result;
}


void CSmlProject::deleteSamplesWithDuplicateTimestamps(QList<sml_sample_t> &samples)
{
    QList<sml_sample_t> result;
    QList<sml_sample_t> collect;


    for(sml_sample_t& sample : samples)
    {
        if(!collect.isEmpty())
        {
            if(sample.time != collect.first().time)
            {
                result << sumUpSamples(collect);
                collect.clear();
            }
        }
        collect << sample;
    }

    result << sumUpSamples(collect);
    samples = result;
}


CSmlProject::sml_sample_t CSmlProject::sumUpSamples(QList<sml_sample_t> samples)
{
    if(samples.count() == 1)
    {
        return samples.first();
    }

    sml_sample_t result;

    result.time = samples.first().time;

    for (const extension_t& ext  : extensions)
    {
        qreal  sum = 0;
        qint32 cnt = 0;

        for(const sml_sample_t& sample : samples)
        {
            if(sample.data.contains(ext.tag))
            {
                sum += sample[ext.tag];
                cnt++;
            }
        }

        if(cnt != 0)
        {
            result[ext.tag] = sum/cnt;
        }
    }

    return result;
}
