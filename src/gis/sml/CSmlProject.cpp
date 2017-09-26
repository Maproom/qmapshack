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

    CTrackData trk;

  
    trk.name = xmlSml.elementsByTagName("DateTime").item(0).firstChild().nodeValue(); // date of beginning of recording is chosen as track name

    const QDomNodeList& smlSamples = xmlSml.elementsByTagName("Sample");

    QDateTime Time0;
    IUnit::parseTimestamp(smlSamples.item(0).toElement().elementsByTagName("UTC").item(0).firstChild().nodeValue(), Time0);

    QList<smlSample_t> samplesList;
    QList<QDateTime> lapsList;

    for (int i = 0; i < smlSamples.count(); i++)	// browse XML samples
    {
        smlSample_t sample;
        sample.time = Time0.addMSecs(smlSamples.item(i).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue().toDouble() * 1000);
    
        /*
          	<Sample>
				<UTC>2017-09-22T14:57:28.896Z</UTC>
				<Time>85.896000000000001</Time>
				<Events>
					<Lap>
						<Type>Manual</Type>
						<Duration>86.5</Duration>
						<Distance>586</Distance>
					</Lap>
				</Events>
			</Sample>
        
        */
        if (smlSamples.item(i).toElement().elementsByTagName("Events").item(0).toElement().isElement())
        {
            if (smlSamples.item(i).toElement().elementsByTagName("Events").item(0).toElement().elementsByTagName("Laps").item(0).toElement().isElement())
            {
                lapsList << sample.time; // stores timestamps of the samples where the the "Lap" button has been pressed
            }
        }
        else // samples without "event" are the ones containing position, heartrate, etc... that we want to store
        {
//   const QString &lat = (y < 0) ? "S" : "N";
            if (smlSamples.item(i).toElement().elementsByTagName("Latitude").item(0).toElement().isElement() )
            {   sample.latitude = (smlSamples.item(i).toElement().elementsByTagName("Latitude").item(0).firstChild().nodeValue().toDouble() / (2 * 3.141592654)) * 360; }
            else
            {    sample.latitude = NOFLOAT; }
        
            if (smlSamples.item(i).toElement().elementsByTagName("Longitude").item(0).toElement().isElement())
            {   sample.longitude = (smlSamples.item(i).toElement().elementsByTagName("Longitude").item(0).firstChild().nodeValue().toDouble() / (2 * 3.141592654)) * 360; }
            else
            {   sample.longitude = NOFLOAT;  }

            if (smlSamples.item(i).toElement().elementsByTagName("Altitude").item(0).toElement().isElement())
            {   sample.altitude = smlSamples.item(i).toElement().elementsByTagName("Altitude").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.altitude = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("VerticalSpeed").item(0).toElement().isElement())
            {   sample.verticalSpeed = smlSamples.item(i).toElement().elementsByTagName("VerticalSpeed").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.verticalSpeed = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("HR").item(0).toElement().isElement())
            {   sample.HR = smlSamples.item(i).toElement().elementsByTagName("HR").item(0).firstChild().nodeValue().toDouble() * 60; }
            else
            {   sample.HR = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("Cadence").item(0).toElement().isElement())
            {
                sample.cadence = smlSamples.item(i).toElement().elementsByTagName("Cadence").item(0).firstChild().nodeValue().toDouble() * 60;
            }
            else
            {   sample.cadence = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("Temperature").item(0).toElement().isElement())
            {   sample.temperature = smlSamples.item(i).toElement().elementsByTagName("Temperature").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.temperature = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("SeaLevelPressure").item(0).toElement().isElement())
            {   sample.seaLevelPressure = smlSamples.item(i).toElement().elementsByTagName("SeaLevelPressure").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.seaLevelPressure = NOFLOAT; }
        
            if (smlSamples.item(i).toElement().elementsByTagName("Speed").item(0).toElement().isElement())
            {   sample.speed = smlSamples.item(i).toElement().elementsByTagName("Speed").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.speed = NOFLOAT; }

             samplesList << sample;

        }
    }

    fillMissingData("Altitude", samplesList);
    fillMissingData("Latitude", samplesList);
    fillMissingData("Longitude", samplesList);
    fillMissingData("HR", samplesList);


    trk.segs.resize(1);
    CTrackData::trkseg_t *seg = &(trk.segs[0]);

    for (int i = 0; i < samplesList.size(); i++)
    {
        if (samplesList[i].longitude != NOFLOAT)
        {
            CTrackData::trkpt_t trkpt;
            trkpt.time = samplesList[i].time;
            trkpt.lat = samplesList[i].latitude;
            trkpt.lon = samplesList[i].longitude;
            trkpt.ele = samplesList[i].altitude;
            trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = samplesList[i].HR;
    
            seg->pts.append(trkpt);
        }
    }
    
    new CGisItemTrk(trk, project);

    project->sortItems();
    project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
}

void CSmlProject::fillMissingData(const QString &dataField, QList<smlSample_t> &samplesList)
{
    QList<smlSample_t*> samplesWithMissingDataList;
    smlSample_t* currentSample = &samplesList.first();
    smlSample_t* previousSampleWithData = nullptr;
    bool keepBrowsing = samplesList.size() > 0;
    int i = 0;
    
    while (keepBrowsing)
    {
        if (getDataFromSmlSample(dataField, currentSample) == NOFLOAT) // sample with missing data found
        {
            samplesWithMissingDataList << currentSample;

        }
        else if (getDataFromSmlSample(dataField, currentSample) != NOFLOAT) // sample with data found
        {
            if (nullptr == previousSampleWithData) // if this is the first sample containing data found
            {
                int j;
                for (j = 0; j < samplesWithMissingDataList.size(); j++)
                {
                    setDataToSmlSample(dataField, samplesWithMissingDataList[j], getDataFromSmlSample(dataField, currentSample));// apply current sample data to all of the previous samples with missing data
                }
                previousSampleWithData = currentSample;
                samplesWithMissingDataList.clear();
            }
            else
            {
                double dY = getDataFromSmlSample(dataField, currentSample) - getDataFromSmlSample(dataField, previousSampleWithData);
                double dT = ((double)(currentSample->time.toMSecsSinceEpoch() - previousSampleWithData->time.toMSecsSinceEpoch())) / 1000;
                double slope = dY / dT;

                int j;
                for (j = 0; j < samplesWithMissingDataList.size(); j++)
                {
                    // interpolate data and apply them to position samples in-between
                    setDataToSmlSample(dataField, samplesWithMissingDataList[j],
                        getDataFromSmlSample(dataField, previousSampleWithData) + slope * (double)((samplesWithMissingDataList[j]->time.toMSecsSinceEpoch() - previousSampleWithData->time.toMSecsSinceEpoch()) / 1000));
                }
                previousSampleWithData = currentSample;
                samplesWithMissingDataList.clear();
            }
        }


        if (++i >= samplesList.size())
        {
            keepBrowsing = FALSE;

            if (nullptr != previousSampleWithData)
            {
                int j;
                for (j = 0; j < samplesWithMissingDataList.size(); j++) //  case where samplesWithMissingDataList is not empty at the end can happen when last samples contain no data
                {
                    setDataToSmlSample(dataField, samplesWithMissingDataList[j], getDataFromSmlSample(dataField, previousSampleWithData));// apply previous data found to all of the previous samples with no data 
                }
            }
        }
        else
        {
            currentSample = &(samplesList[i]);
        }
    }



}


double CSmlProject::getDataFromSmlSample(const QString &dataField, smlSample_t * smlSample)
{
    /*if (dataField == "Time")
    {
        return smlSample->time.toMSecsSinceEpoch;
    }*/

    if (dataField == "Latitude")
    {
        return smlSample->latitude;
    }
    
    if (dataField == "Longitude")
    {
        return smlSample->longitude;
    }

    if (dataField == "Altitude")
    {
        return smlSample->altitude;
    }


    if (dataField == "VerticalSpeed")
    {
        return smlSample->verticalSpeed;
    }


    if (dataField == "HR")
    {
        return smlSample->HR;
    }


    if (dataField == "Cadence")
    {
        return smlSample->cadence;
    }


    if (dataField == "Temperature")
    {
        return smlSample->temperature;
    }


    if (dataField == "SeaLevelPressure")
    {
        return smlSample->seaLevelPressure;
    }


    if (dataField == "Speed")
    {
        return smlSample->speed;
    }

    return NOFLOAT;

}


void CSmlProject::setDataToSmlSample(const QString &dataField, smlSample_t * smlSample, const double data)
{/*
    if (dataField == "Time")
    {
        smlSample->time.fromMSecsSinceEpoch = data;
    }*/

    if (dataField == "Latitude")
    {
        smlSample->latitude = data;
    }

    if (dataField == "Longitude")
    {
        smlSample->longitude = data;
    }

    if (dataField == "Altitude")
    {
        smlSample->altitude = data;
    }


    if (dataField == "VerticalSpeed")
    {
        smlSample->verticalSpeed = data;
    }


    if (dataField == "HR")
    {
        smlSample->HR = data;
    }


    if (dataField == "Cadence")
    {
        smlSample->cadence = data;
    }


    if (dataField == "Temperature")
    {
        smlSample->temperature = data;
    }


    if (dataField == "SeaLevelPressure")
    {
        smlSample->seaLevelPressure = data;
    }


    if (dataField == "Speed")
    {
        smlSample->speed = data;
    }



}

/*
   if (smlSamples.item(i).toElement().elementsByTagName("Latitude").item(0).toElement().isElement() )
            {   sample.latitude = (smlSamples.item(i).toElement().elementsByTagName("Latitude").item(0).firstChild().nodeValue().toDouble() / (2 * 3.141592654)) * 360; }
            else
            {    sample.latitude = NOFLOAT; }
        
            if (smlSamples.item(i).toElement().elementsByTagName("Longitude").item(0).toElement().isElement())
            {   sample.longitude = (smlSamples.item(i).toElement().elementsByTagName("Longitude").item(0).firstChild().nodeValue().toDouble() / (2 * 3.141592654)) * 360; }
            else
            {   sample.longitude = NOFLOAT;  }

            if (smlSamples.item(i).toElement().elementsByTagName("Altitude").item(0).toElement().isElement())
            {   sample.altitude = smlSamples.item(i).toElement().elementsByTagName("Altitude").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.altitude = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("VerticalSpeed").item(0).toElement().isElement())
            {   sample.verticalSpeed = smlSamples.item(i).toElement().elementsByTagName("VerticalSpeed").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.verticalSpeed = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("HR").item(0).toElement().isElement())
            {   sample.HR = smlSamples.item(i).toElement().elementsByTagName("HR").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.HR = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("Cadence").item(0).toElement().isElement())
            {   sample.cadence = smlSamples.item(i).toElement().elementsByTagName("Cadence").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.cadence = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("Temperature").item(0).toElement().isElement())
            {   sample.temperature = smlSamples.item(i).toElement().elementsByTagName("Temperature").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.temperature = NOFLOAT; }

            if (smlSamples.item(i).toElement().elementsByTagName("SeaLevelPressure").item(0).toElement().isElement())
            {   sample.seaLevelPressure = smlSamples.item(i).toElement().elementsByTagName("SeaLevelPressure").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.seaLevelPressure = NOFLOAT; }
        
            if (smlSamples.item(i).toElement().elementsByTagName("Speed").item(0).toElement().isElement())
            {   sample.speed = smlSamples.item(i).toElement().elementsByTagName("Speed").item(0).firstChild().nodeValue().toDouble(); }
            else
            {   sample.speed = NOFLOAT; } */

