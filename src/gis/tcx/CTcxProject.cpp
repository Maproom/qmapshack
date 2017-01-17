/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/tcx/CTcxProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

#include <QtWidgets>



CTcxProject::CTcxProject(const QString &filename, CGisListWks * parent)
    : IGisProject(eTypeTcx, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/TcxProject.png"));
    blockUpdateItems(true);
    loadTcx(filename);
    blockUpdateItems(false);
    setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}


void CTcxProject::loadTcx(const QString& filename)
{
    try
    {
        loadTcx(filename, this);
    }
    catch(QString &errormsg)
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                              tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
        valid = false;
    }
}


void CTcxProject::loadTcx(const QString &filename, CTcxProject *project)
{
   	// create file instance
	QFile file(filename);

	// if the file does not exist, the filename is assumed to be a name for a new project
	if (!file.exists() || QFileInfo(filename).suffix().toLower() != "tcx")
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

	QDomElement xmlTcx = xml.documentElement();
	if (xmlTcx.tagName() != "TrainingCenterDatabase")
	{
		throw tr("Not a TCX file: %1").arg(filename);
	}

	if ((xmlTcx.firstChild().toElement().tagName() != "Activities") && (xmlTcx.firstChild().toElement().nextSibling().toElement().tagName() != "Courses"))
	{
		throw tr("This TCX file does not contain any activity or course: %1").arg(filename);
	}

	
	CTrackData trk;

	
	if (xmlTcx.firstChild().toElement().tagName() == "Activities") // if activity TCX
	{ 
		trk.name = xmlTcx.elementsByTagName("Id").item(0).firstChild().nodeValue();

		const QDomNodeList& tcxLaps = xmlTcx.elementsByTagName("Lap");

		for (int i = 0; i < tcxLaps.count(); i++)	// browse laps
		{
			trk.segs.resize(i + 1);
			CTrackData::trkseg_t *seg = &(trk.segs[i]);

			const QDomNodeList& tcxLapTrackpts = tcxLaps.item(i).toElement().elementsByTagName("Trackpoint");

			for (int j = 0; j < tcxLapTrackpts.count(); j++) // browse trackpoints
			{
				const QDomElement positionElement = tcxLapTrackpts.item(j).toElement().elementsByTagName("Position").item(0).toElement();

				if (positionElement.isElement()) // if this trackpoint contains position, i.e. GPSr was able to capture position
				{
					CTrackData::trkpt_t trkpt;

					QString timeString = tcxLapTrackpts.item(j).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue();
					QDateTime trkPtTimestamp;
					IUnit::parseTimestamp(timeString, trkPtTimestamp);
					trkpt.time = trkPtTimestamp;


					trkpt.lat = positionElement.elementsByTagName("LatitudeDegrees").item(0).firstChild().nodeValue().toDouble();
					trkpt.lon = positionElement.elementsByTagName("LongitudeDegrees").item(0).firstChild().nodeValue().toDouble();
					trkpt.ele = tcxLapTrackpts.item(j).toElement().elementsByTagName("AltitudeMeters").item(0).firstChild().nodeValue().toDouble();

					const QDomElement HRElement = tcxLapTrackpts.item(j).toElement().elementsByTagName("HeartRateBpm").item(0).toElement();
					if (HRElement.isElement()) // if this trackpoint contains heartrate data, i.e. heartrate sensor data has been captured
					{
						trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = HRElement.elementsByTagName("Value").item(0).firstChild().nodeValue().toDouble();
					}

					const QDomElement CADElement = tcxLapTrackpts.item(j).toElement().elementsByTagName("Cadence").item(0).toElement();
					if (CADElement.isElement()) // if this trackpoint contains cadence data, i.e. cadence sensor data has been captured
					{
						trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"] = CADElement.firstChild().nodeValue().toDouble();
					}
					seg->pts.append(trkpt);
				}
			}
		}
	}


	if (xmlTcx.firstChild().toElement().nextSibling().toElement().tagName() == "Courses") // if course TCX
	{
		trk.name = xmlTcx.elementsByTagName("Name").item(0).firstChild().nodeValue();
		QString timeString;
		QDateTime trkPtTimestamp;
		trk.segs.resize(1);
		CTrackData::trkseg_t *seg = &(trk.segs[0]);

		const QDomNodeList& tcxTrackpts = xmlTcx.elementsByTagName("Trackpoint");

		for (int i = 0; i < tcxTrackpts.count(); i++) // browse trackpoints
		{
			const QDomElement positionElement = tcxTrackpts.item(i).toElement().elementsByTagName("Position").item(0).toElement();

			if (positionElement.isElement()) // if this trackpoint contains position, i.e. GPSr was able to capture position
			{
				CTrackData::trkpt_t trkpt;

				timeString = tcxTrackpts.item(i).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue();
				IUnit::parseTimestamp(timeString, trkPtTimestamp);
				trkpt.time = trkPtTimestamp;


				trkpt.lat = positionElement.elementsByTagName("LatitudeDegrees").item(0).firstChild().nodeValue().toDouble();
				trkpt.lon = positionElement.elementsByTagName("LongitudeDegrees").item(0).firstChild().nodeValue().toDouble();
				trkpt.ele = tcxTrackpts.item(i).toElement().elementsByTagName("AltitudeMeters").item(0).firstChild().nodeValue().toDouble();

				const QDomElement HRElement = tcxTrackpts.item(i).toElement().elementsByTagName("HeartRateBpm").item(0).toElement();
				if (HRElement.isElement()) // if this trackpoint contains heartrate data, i.e. heartrate sensor data has been captured
				{
					trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] = HRElement.elementsByTagName("Value").item(0).firstChild().nodeValue().toDouble();
				}

				const QDomElement CADElement = tcxTrackpts.item(i).toElement().elementsByTagName("Cadence").item(0).toElement();
				if (CADElement.isElement()) // if this trackpoint contains cadence data, i.e. cadence sensor data has been captured
				{
					trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"] = CADElement.firstChild().nodeValue().toDouble();
				}
				seg->pts.append(trkpt);
			}
		}

		const QDomNodeList& tcxCoursePts = xmlTcx.elementsByTagName("CoursePoint");
		for (int i = 0; i < tcxCoursePts.count(); i++) // browse course points
		{
			QString name = tcxCoursePts.item(i).toElement().elementsByTagName("Name").item(0).firstChild().nodeValue();
			
			timeString = tcxCoursePts.item(i).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue();
			IUnit::parseTimestamp(timeString, trkPtTimestamp);
			QDateTime &time = trkPtTimestamp;
			qreal lat = tcxCoursePts.item(i).toElement().elementsByTagName("Position").item(0).toElement().elementsByTagName("LatitudeDegrees").item(0).firstChild().nodeValue().toDouble();
			qreal lon = tcxCoursePts.item(i).toElement().elementsByTagName("Position").item(0).toElement().elementsByTagName("LongitudeDegrees").item(0).firstChild().nodeValue().toDouble();
			qreal ele = tcxCoursePts.item(i).toElement().elementsByTagName("AltitudeMeters").item(0).firstChild().nodeValue().toDouble();

			QString icon = tcxCoursePts.item(i).toElement().elementsByTagName("PointType").item(0).firstChild().nodeValue();

			new CGisItemWpt(QPointF(lon, lat), ele, time, name, icon, project);
		}
	}
	
	
	new CGisItemTrk(trk, project); 

    qDebug() << "tcx";

    project->sortItems();
    project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;


}
