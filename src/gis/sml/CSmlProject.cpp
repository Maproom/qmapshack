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

    for (int i = 0; i < smlSamples.count(); i++)	// browse XML samples
    {
        smlSample_t sample;
        sample.time = Time0.addMSecs(smlSamples.item(i).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue().toDouble() * 1000);
     
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
        {   sample.speed = NOFLOAT; }

         samplesList << sample;
    }

    QList<smlSample_t*> firstSamplesWithNoAltList;

    int i = 0;
    while (samplesList[i].latitude == NOFLOAT && samplesList[i].altitude == NOFLOAT) // find first sample with alt or pos
    {   i++; }
        
    double previousAlt;
    if (samplesList[i].altitude != NOFLOAT)
    {
        previousAlt = samplesList[i].altitude;
    }
    else if (samplesList[i].latitude != NOFLOAT)
    {
        firstSamplesWithNoAltList << &samplesList[i];
    }

    while (samplesList[i].altitude == NOFLOAT) // find first sample with alt or pos
    {    i++; }



    while (samplesList[i].latitude == NOFLOAT)
    {
        previousAlt = samplesList[i].altitude;
    }



    /*
    trk.segs.resize(1);
    CTrackData::trkseg_t *seg = &(trk.segs[0]);

    double lastRecordedAltitude = NOFLOAT;
    for (int i = 0; i < samplesList.size(); i++)
    {
        if (samplesList[i].altitude != NOFLOAT)
        {
            lastRecordedAltitude = samplesList[i].altitude;
        }
    
        if (samplesList[i].longitude != NOFLOAT)
        {
            CTrackData::trkpt_t trkpt;
            trkpt.lon = samplesList[i].longitude;
            trkpt.lat = samplesList[i].latitude;

            if (lastRecordedAltitude != NOFLOAT)
            {
                trkpt.ele = lastRecordedAltitude;
            }

            trkpt.time = samplesList[i].time;

            seg->pts.append(trkpt);
        }
        

    }
    */







    new CGisItemTrk(trk, project);

    project->sortItems();
    project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
}
