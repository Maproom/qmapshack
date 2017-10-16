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

    if (xmlSml.elementsByTagName("Latitude").count() == 0)
    {
        throw tr("This SML file does not contain any position data and can not be displayed by QMapShack: %1").arg(filename);
    }

    CTrackData trk;
      
    trk.name = xmlSml.elementsByTagName("DateTime").item(0).firstChild().nodeValue(); // date of beginning of recording is chosen as track name
    trk.desc = xmlSml.elementsByTagName("Activity").item(0).firstChild().nodeValue();
    
    trk.cmt = tr("Device: %1<br>"
        "Recovery time: %2 h<br>"
        "Peak Training Effect: %3<br>"
        "Energy: %4 kCal<br>"
        "Battery usage: %5 %/hour")
        .arg(xmlSml.elementsByTagName("Name").item(0).firstChild().nodeValue())
        .arg(xmlSml.elementsByTagName("RecoveryTime").item(0).firstChild().nodeValue().toInt() / 3600)
        .arg(xmlSml.elementsByTagName("PeakTrainingEffect").item(0).firstChild().nodeValue().toDouble())
        .arg((int)(xmlSml.elementsByTagName("Energy").item(0).firstChild().nodeValue().toDouble()/ 4184))
        .arg( 100*(xmlSml.elementsByTagName("BatteryChargeAtStart").item(0).firstChild().nodeValue().toDouble() 
            - xmlSml.elementsByTagName("BatteryCharge").item(0).firstChild().nodeValue().toDouble())
            / (xmlSml.elementsByTagName("Duration").item(0).firstChild().nodeValue().toDouble() / 3600), 0, 'f', 1);
    
    const QDomNodeList& smlSamples = xmlSml.elementsByTagName("Sample");

    QDateTime Time0;
    IUnit::parseTimestamp(smlSamples.item(0).toElement().elementsByTagName("UTC").item(0).firstChild().nodeValue(), Time0);

    QList<sml_sample_t> samplesList;
    QList<QDateTime> lapsList;

    QStringList extensionsNames;
    extensionsNames << "Latitude"<<"Longitude"<<"Altitude"<<"VerticalSpeed"<<"HR"<<"Cadence"<<"Temperature"<<"SeaLevelPressure"<<"Speed"<<"EnergyConsumption";


    for (int i = 0; i < smlSamples.count(); i++)	// browse XML samples
    {
        sml_sample_t sample;
        sample.time = Time0.addMSecs(smlSamples.item(i).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue().toDouble() * 1000.0);


        if (smlSamples.item(i).toElement().elementsByTagName("Events").item(0).toElement().isElement())
        {
            if (smlSamples.item(i).toElement().elementsByTagName("Events").item(0).toElement().elementsByTagName("Lap").item(0).toElement().isElement())
            {
                lapsList << sample.time; // stores timestamps of the samples where the the "Lap" button has been pressed
            }
        }
        else // samples without "event" are the ones containing position, heartrate, etc... that we want to store
        {
            QList<qreal> extensionsScalingFactors;
            QList<qreal> extensionsOffsets;
            extensionsScalingFactors << RAD_TO_DEG<<RAD_TO_DEG<<1.0<<1.0<<60.0<<60.0<<1.0<<0.01<<1.0<< (60.0 / 4184.0);
            extensionsOffsets << 0.0<<0.0<<0.0<<0.0<<0.0<<0.0<<-273.15<<0.0<<0.0<<0.0;

            for (int j = 0 ; j < extensionsNames.size() ; j++)
            {
                if ( smlSamples.item(i).toElement().elementsByTagName(extensionsNames[j]).item(0).toElement().isElement() )
                {
                     sample.data.insert(extensionsNames[j],
                                        smlSamples.item(i).toElement().elementsByTagName(extensionsNames[j]).item(0).firstChild().nodeValue().toDouble()
                                         * extensionsScalingFactors[j] + extensionsOffsets[j]);
                }
            }
            samplesList << sample;
        }
    }

    for (int i = 0 ; i < extensionsNames.size() ; i++)
    {
       fillMissingData(extensionsNames[i], samplesList);
    }


    if (samplesList.count() >= 2)
    {   // code below merges samples with identical timestamps.
        // Samples with identical timestamps are found when "pause" button is pressed (and maybe in some other cases, I can not say)
        QList<sml_sample_t *> samplesWithSameTimestampList;
        samplesWithSameTimestampList << &samplesList[0];
        for (int i = 1; i < samplesList.size(); i++)
        {
            if (samplesWithSameTimestampList[0]->time == samplesList[i].time) // a sample with identical timestamp has been found
            {// todo case where samplesList[i] is the last one (easy example : activity with 2 samples only)
                samplesWithSameTimestampList << &samplesList[i];
            }
            else if ( (samplesWithSameTimestampList.count() == 1) && (samplesWithSameTimestampList[0]->time != samplesList[i].time) )
            {   // the single stored sample and current sample have different timestamps
                samplesWithSameTimestampList.clear();
                samplesWithSameTimestampList << &samplesList[i];
            }
            else if  ( (samplesWithSameTimestampList.count() >= 2) && (samplesWithSameTimestampList[0]->time != samplesList[i].time) )
            {   // samples with identical timestamps have been found, and the current sample has a different timestamp
                for(int j = 0; j < extensionsNames.count(); j++)
                {
                    qreal sum = 0;
                    qreal samplesWithDataCount = 0;
                    for(int k = 0; k < samplesWithSameTimestampList.size(); k++)
                    {
                        if ( samplesWithSameTimestampList[k]->data.contains(extensionsNames[j]) )
                        {
                            samplesWithDataCount++;
                            sum += samplesWithSameTimestampList[k]->data.value(extensionsNames[j]);
                        }
                    }
                    if ( samplesWithDataCount != 0)
                    {
                        samplesWithSameTimestampList[0]->data.insert(extensionsNames[j], sum / samplesWithDataCount); // first sample gets the averaged value
                    }
                 }

                // remove samples with same timestamp but the first one
                for (int j = 0 ; j < samplesWithSameTimestampList.size() - 1 ; j++)
                {
                    samplesList.removeAt(1+i-samplesWithSameTimestampList.size());
                }

                i -= samplesWithSameTimestampList.count() - 1; // index i has to be moved because of removed samples

                samplesWithSameTimestampList.clear();
                samplesWithSameTimestampList << &samplesList[i]; // and current sample has to be stored

            }
        }
    }

    lapsList << samplesList.last().time.addSecs(1); // a last dummy lap button push is added with timestamp = 1 s later than the last sample timestamp

    trk.segs.resize(lapsList.size() ); // segments are created and each of them contains 1 lap

    int lap = 0;
    CTrackData::trkseg_t *seg = &(trk.segs[lap]);

    for (int j = 0; j < samplesList.size(); j++)
    {
        if (samplesList[j].time > lapsList[lap])
        {
            lap++;
            seg = &(trk.segs[lap]);
        }

        CTrackData::trkpt_t trkpt;
        trkpt.time = samplesList[j].time;
        trkpt.lat = samplesList[j].data.value("Latitude");
        trkpt.lon = samplesList[j].data.value("Longitude");
        if (samplesList[j].data.contains("Altitude"))  { trkpt.ele = samplesList[j].data.value("Altitude"); }
        if (samplesList[j].data.contains("VerticalSpeed"))  { trkpt.extensions["gpxdata:verticalSpeed"] = samplesList[j].data.value("VerticalSpeed"); }
        if (samplesList[j].data.contains("HR"))  { trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = (int)samplesList[j].data.value("HR"); }
        if (samplesList[j].data.contains("Cadence"))  { trkpt.extensions["gpxdata:cadence"] = samplesList[j].data.value("Cadence"); }
        if (samplesList[j].data.contains("Temperature"))  { trkpt.extensions["gpxdata:temp"] = samplesList[j].data.value("Temperature"); }
        if (samplesList[j].data.contains("SeaLevelPressure"))  { trkpt.extensions["gpxdata:seaLevelPressure"] = samplesList[j].data.value("SeaLevelPressure"); }
        if (samplesList[j].data.contains("Speed"))  { trkpt.extensions["gpxdata:speed"] = samplesList[j].data.value("Speed"); }
        if (samplesList[j].data.contains("EnergyConsumption"))  { trkpt.extensions["gpxdata:energy"] = samplesList[j].data.value("EnergyConsumption"); }

        seg->pts.append(trkpt);
    }

    new CGisItemTrk(trk, project);

    project->sortItems();
    project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
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
        if (!currentSample->data.contains(dataField)) // sample with missing data found
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
                    samplesWithMissingDataList[j]->data.insert(dataField, currentSample->data.value(dataField));
                }
                previousSampleWithData = currentSample;
                samplesWithMissingDataList.clear();
            }
            else
            {
                qreal dY = currentSample->data.value(dataField) - previousSampleWithData->data.value(dataField);
                qreal dT = ((qreal)(currentSample->time.toMSecsSinceEpoch() - previousSampleWithData->time.toMSecsSinceEpoch())) / 1000.0;
                qreal slope = dY / dT;
                qreal offsetAt0 = previousSampleWithData->data.value(dataField)
                                  - slope * ( (  (qreal)(previousSampleWithData->time.toMSecsSinceEpoch())  ) / 1000.0 );

                int j;
                for (j = 0; j < samplesWithMissingDataList.size(); j++)
                {
                 //    interpolate data and apply them to samples in-between

                   samplesWithMissingDataList[j]->data.insert(dataField, (qreal)(
                                        slope * (qreal)((samplesWithMissingDataList[j]->time.toMSecsSinceEpoch()) / 1000.0) + offsetAt0  ));
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
                    samplesWithMissingDataList[j]->data.insert(dataField, previousSampleWithData->data.value(dataField));// apply previous data found to all of the previous samples with no data
                }
            }
        }
        else
        {
            currentSample = &(samplesList[i]);
        }
    }
}
