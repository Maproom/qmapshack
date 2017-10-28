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
#include "gis/log/CLogProject.h"
#include "gis/trk/CGisItemTrk.h"

#include <QtWidgets>
#include <functional>

using fTrkPtSetVal = std::function<void(CTrackData::trkpt_t&, qreal)>;

struct extension_t
{
    /// the tag as used in the LOG file
    QString tag;
    /// a scale factor to be applied to the value stored in the LOG file
    qreal scale;
    /// an offset to be applied to the value stored in the LOG file
    qreal offset;
    /// an assignment function that assigns a value to a member of a trkpt_t object
    fTrkPtSetVal func;
};

#define NIL ///< this is to silence the MSVC compiler
#define ASSIGN_VALUE(var, op) \
    [](CTrackData::trkpt_t &pt, qreal val) \
    { \
        if(val != NOFLOAT) \
        { \
            pt.var = op(val); \
        } \
    } \

static const QList<extension_t> extensions =
{
     {"Latitude",           0.0000001,      0.0,        ASSIGN_VALUE(lat,NIL)}  // unit [°]
    ,{"Longitude",          0.0000001,      0.0,        ASSIGN_VALUE(lon,NIL)}  // unit [°]
    ,{"Altitude",           1.0,            0.0,        ASSIGN_VALUE(ele,NIL)}  // unit [m]
    ,{"VerticalSpeed",      0.01,           0.0,        ASSIGN_VALUE(extensions["gpxdata:verticalSpeed"],NIL)}                  // unit [m/h]
    ,{"HR",                 1.0,            0.0,        ASSIGN_VALUE(extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"],qRound)}   // unit [bpm]
    ,{"Cadence",            1.0,            0.0,        ASSIGN_VALUE(extensions["gpxdata:cadence"],NIL)}                        // unit [bpm]
    ,{"Temperature",        0.1,            0.0,        ASSIGN_VALUE(extensions["gpxdata:temp"],NIL)}                           // unit [°C]
    ,{"SeaLevelPressure",   0.1,            0.0,        ASSIGN_VALUE(extensions["gpxdata:seaLevelPressure"],NIL)}               // unit [hPa]
    ,{"Speed",              0.01,           0.0,        ASSIGN_VALUE(extensions["gpxdata:speed"],NIL)}                          // unit [m/s]
    ,{"EnergyConsumption",  0.1,            0.0,        ASSIGN_VALUE(extensions["gpxdata:energy"],NIL)}                         // unit [kCal/min]
};


CLogProject::CLogProject(const QString &filename, CGisListWks * parent)
    : IGisProject(eTypeLog, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/LogProject.png"));
    blockUpdateItems(true);
    loadLog(filename);
    blockUpdateItems(false);
    setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}


void CLogProject::loadLog(const QString& filename)
{
    try
    {
        loadLog(filename, this);
    }
    catch(QString &errormsg)
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                              tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
        valid = false;
    }
}


void CLogProject::loadLog(const QString &filename, CLogProject *project)
{
    QFile file(filename);

    // if the file does not exist, the filename is assumed to be a name for a new project
    if (!file.exists() || QFileInfo(filename).suffix().toLower() != "log")
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

    QDomElement xmlOpenambitlog = xml.documentElement();
    if (xmlOpenambitlog.tagName() != "openambitlog")
    {
        throw tr("Not an Openambit log file: %1").arg(filename);
    }

    CTrackData trk;

    if(xmlOpenambitlog.namedItem("Time").isElement())
    {
        trk.name = xmlOpenambitlog.namedItem("Time").toElement().text();                            // date of beginning of recording is chosen as track name
    }

    if(xmlOpenambitlog.namedItem("DeviceInfo").isElement())
    {
        const QDomNode& xmlDeviceInfo = xmlOpenambitlog.namedItem("DeviceInfo");
        if(xmlDeviceInfo.namedItem("Name").isElement())
        {
            trk.cmt =  tr("Device: %1<br>").arg(xmlDeviceInfo.namedItem("Name").toElement().text());
        }
    }

    if(xmlOpenambitlog.namedItem("Log").isElement())
    {
        const QDomNode& xmlLog = xmlOpenambitlog.namedItem("Log");
        if(xmlLog.namedItem("Header").isElement())
        {
            const QDomNode& xmlHeader = xmlLog.namedItem("Header");

            if(xmlHeader.namedItem("Activity").isElement())
            {
                trk.desc = xmlHeader.namedItem("Activity").toElement().text();
            }

            if(xmlHeader.namedItem("RecoveryTime").isElement())
            {
                trk.cmt += tr("Recovery time: %1 h<br>").arg(xmlHeader.namedItem("RecoveryTime").toElement().text().toInt() / 3600000);
            }

            if(xmlHeader.namedItem("PeakTrainingEffect").isElement())
            {
                trk.cmt += tr("Peak Training Effect: %1<br>").arg(xmlHeader.namedItem("PeakTrainingEffect").toElement().text().toDouble()/10.0);
            }

            if(xmlHeader.namedItem("Energy").isElement())
            {
                trk.cmt += tr("Energy: %1 kCal<br>").arg((int)xmlHeader.namedItem("Energy").toElement().text().toDouble() );
            }
        }

        if(xmlLog.namedItem("Samples").isElement())
        {
            const QDomNode& xmlSamples = xmlLog.namedItem("Samples");
            const QDomNodeList& xmlSampleList = xmlSamples.toElement().elementsByTagName("Sample");

            if (xmlSampleList.count() > 0)
            {
                QDateTime time0;

                const QDomNode& xmlSample = xmlSampleList.item(0);
                if(xmlSample.namedItem("UTC").isElement())
                {
                    IUnit::parseTimestamp(xmlSample.namedItem("UTC").toElement().text(), time0);
                }

                QList<log_sample_t> samplesList;
                QList<QDateTime> lapsList;

                bool sampleWithPositionFound = false;

                for (int i = 0; i < xmlSampleList.count(); i++) // browse XML samples
                {
                    log_sample_t sample;
                    const QDomNode& xmlSample = xmlSampleList.item(i);

                    if(xmlSample.namedItem("Latitude").isElement())
                    {
                        sampleWithPositionFound = true;
                    }

                    if(xmlSample.namedItem("Time").isElement())
                    {
                        sample.time = time0.addMSecs(xmlSample.namedItem("Time").toElement().text().toDouble() );
                    }

                    if(xmlSample.namedItem("Type").isElement())
                    {
                        if ( xmlSample.namedItem("Type").toElement().text() == "lap-info" )
                        {
                            if ( xmlSample.namedItem("Lap").isElement() )
                            {
                                const QDomNode& xmlLap = xmlSample.namedItem("Lap");
                                if(xmlLap.namedItem("Type").isElement())
                                {
                                    if (xmlLap.namedItem("Type").toElement().text() == "Manual")
                                    {
                                        lapsList << sample.time; // stores timestamps of the samples where the the "Lap" button has been pressed
                                    }
                                }
                             }
                         }
                        else if (xmlSample.namedItem("Type").toElement().text() == "gps-small"
                              || xmlSample.namedItem("Type").toElement().text() == "gps-base"
                              || xmlSample.namedItem("Type").toElement().text() == "gps-tiny"
                              || xmlSample.namedItem("Type").toElement().text() == "position"
                              || xmlSample.namedItem("Type").toElement().text() == "periodic")
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
                }

                if (!sampleWithPositionFound)
                {
                    throw tr("This LOG file does not contain any position data and can not be displayed by QMapShack: %1").arg(filename);
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

                for(const log_sample_t& sample : samplesList)
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


void CLogProject::fillMissingData(const QString &dataField, QList<log_sample_t> &samplesList)
{   // Suunto samples contain lat/lon OR heart rate, elevation, etc.., each one with its own timestamp.
    // The purpose of the code below is to "spread" data among samples.
    // At the end each sample contains data, linearly interpolated from its neighbors according to timestamps.
    QList<log_sample_t> collect;
    QList<log_sample_t> result;
    log_sample_t previousSampleWithData;

    for(log_sample_t& sample : samplesList)
    {
        collect << sample;

        if (sample.data.contains(dataField))
        {
            if (!previousSampleWithData.data.contains(dataField))
            {   // case where, at the beginning, first samples have no data
                for(log_sample_t& collectedSample : collect)
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

                    for(log_sample_t& collectedSample : collect)
                    {   // apply interpolation to collected samples
                        collectedSample[dataField] = previousSampleWithData.data[dataField] + slope * ( (qreal)(collectedSample.time.toMSecsSinceEpoch() - previousSampleWithData.time.toMSecsSinceEpoch()) / 1000.0 );
                    }
                }
            }

            previousSampleWithData = sample;
            result << collect;
            collect.clear();
        }
    }

    if (previousSampleWithData.data.contains(dataField))
    {
        for(log_sample_t& collectedSample : collect)
        {   // processing last remaining collected samples without data
            collectedSample[dataField] = previousSampleWithData[dataField];
        }
    }

    result << collect;
    samplesList = result;
}


void CLogProject::deleteSamplesWithDuplicateTimestamps(QList<log_sample_t> &samples)
{
    QList<log_sample_t> result;
    QList<log_sample_t> collect;


    for(log_sample_t& sample : samples)
    {
        if(!collect.isEmpty())
        {
            if(sample.time != collect.first().time)
            {
                result << mergeSamples(collect);
                collect.clear();
            }
        }
        collect << sample;
    }

    result << mergeSamples(collect);
    samples = result;
}


CLogProject::log_sample_t CLogProject::mergeSamples(QList<log_sample_t> samples)
{
    if(samples.count() == 1)
    {
        return samples.first();
    }

    log_sample_t result;

    result.time = samples.first().time;

    for (const extension_t& ext  : extensions)
    {
        qreal sum = 0;
        qint32 cnt = 0;

        for(const log_sample_t& sample : samples)
        {
            if(sample.data.contains(ext.tag))
            {
                sum += sample[ext.tag];
                cnt++;
            }
        }

        if(cnt != 0)
        {
            result[ext.tag] = sum/cnt; // averaged value is assigned to the merged sample
        }
    }

    return result;
}
