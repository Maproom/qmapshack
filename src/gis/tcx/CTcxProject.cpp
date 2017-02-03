/**********************************************************************************************
    Copyright (C) 2016 Michel Durand zero@cms123.fr

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
   	QFile file(filename);
	
	// if the file does not exist, the file name is assumed to be a name for a new project
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

	const QDomNodeList& tcxActivitys = xmlTcx.elementsByTagName("Activity");
	const QDomNodeList& tcxCourses = xmlTcx.elementsByTagName("Course");
	if (!tcxActivitys.item(0).isElement() && !tcxCourses.item(0).isElement()) // Only the first activity or course is parsed as TCX files coming from GPSr (as Edge 705) only contain one activity or course. TCX files can also contain full exports of Garmin Training Center databases ; this use case is not handled.
	{
		throw tr("This TCX file does not contain any activity or course: %1").arg(filename); // maybe it is a "workout", not handled because workouts do not contain position data
	}

	CTrackData trk;
		
	if (tcxActivitys.item(0).isElement()) // parsing of the first activity found, if any
	{ 
		trk.name = tcxActivitys.item(0).toElement().elementsByTagName("Id").item(0).firstChild().nodeValue(); // activities does not have a "Name" but an "Id" instead (containing start date-time)

		const QDomNodeList& tcxLaps = tcxActivitys.item(0).toElement().elementsByTagName("Lap");

		trk.segs.resize(tcxLaps.count());
		for (int i = 0; i < tcxLaps.count(); i++)	// browse laps
		{
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

					seg->pts.append(trkpt); // 1 TCX lap gives 1 GPX track segment 
				}
			}
		}
	}
	
	else // parsing of the first course found
	{
		trk.name = tcxCourses.item(0).toElement().elementsByTagName("Name").item(0).firstChild().nodeValue();
		QString timeString;
		QDateTime trkPtTimestamp;
		trk.segs.resize(1);
		CTrackData::trkseg_t *seg = &(trk.segs[0]);

		const QDomNodeList& tcxTrackpts = tcxCourses.item(0).toElement().elementsByTagName("Trackpoint");

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
			qreal lat = tcxCoursePts.item(i).toElement().elementsByTagName("Position").item(0).toElement().elementsByTagName("LatitudeDegrees").item(0).firstChild().nodeValue().toDouble();
			qreal lon = tcxCoursePts.item(i).toElement().elementsByTagName("Position").item(0).toElement().elementsByTagName("LongitudeDegrees").item(0).firstChild().nodeValue().toDouble();
			qreal ele = tcxCoursePts.item(i).toElement().elementsByTagName("AltitudeMeters").item(0).firstChild().nodeValue().toDouble();
			QString icon = tcxCoursePts.item(i).toElement().elementsByTagName("PointType").item(0).firstChild().nodeValue(); // there is no "icon" in course points ;  "PointType" is used instead (can be "turn left", "turn right", etc... See list in http://www8.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd)

			new CGisItemWpt(QPointF(lon, lat), ele, QDateTime::currentDateTime().toUTC(), name, icon, project); // 1 TCX course point gives 1 GPX waypoint
		}
	}
	
	
	new CGisItemTrk(trk, project); 

    project->sortItems();
    project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
}




bool CTcxProject::saveAs(const QString& fn, IGisProject& project)
{
	QString _fn_ = fn;
	QFileInfo fi(_fn_);
	if (fi.suffix().toLower() != "tcx")
	{
		_fn_ += ".tcx";
	}

	project.mount();

	// safety check for existing files
	QFile file(_fn_);

	//  ---- start content of tcx
	QDomDocument doc;
	QDomElement tcx = doc.createElement("TrainingCenterDatabase");
	doc.appendChild(tcx);

	tcx.setAttribute("xmlns", "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");
	tcx.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	tcx.setAttribute("xsi:schemaLocation", "http://www.garmin.com/xmlschemas/ProfileExtension/v1 http://www.garmin.com/xmlschemas/UserProfilePowerExtensionv1.xsd http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd http://www.garmin.com/xmlschemas/UserProfile/v2 http://www.garmin.com/xmlschemas/UserProfileExtensionv2.xsd");

	
	CGisItemTrk *trkItem;
	for (int i = 0; i < project.childCount(); i++) // find 1st project track
	{
		trkItem = dynamic_cast<CGisItemTrk*>(project.child(i));
		if (nullptr == trkItem)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	if (!trkItem->isTrkTimeValid())
	{
		int res = QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Track with invalid timestamps...")
			, tr("The track you have selected contains trackpoints with "
			"invalid timestamps. "
			"Device might not accept the generated TCX course file if left as is. "
			"<b>Do you want to apply a filter with constant speed (10 m/s) and continue?</b>")
			, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (res == QMessageBox::No)
		{
			return false;
		}

		if (res == QMessageBox::Yes)
		{
			trkItem->filterSpeed(10);
		}
	}

	QVector<QDateTime> ptDateTimes;
	QVector<qint32> ptElevations;
	int j = 0;

	for (int i = 0; i < project.childCount(); i++) // browse waypoints 
	{
		CGisItemWpt *wptItem = dynamic_cast<CGisItemWpt*>(project.child(i));
		if (nullptr == wptItem)
		{
			continue;
		}
		else
		{
			j++;
			ptDateTimes.resize(j);
			ptElevations.resize(j);
			ptDateTimes[j - 1] = trkItem->getCloserPtDateTime(wptItem->getPosition());
			ptElevations[j - 1] = wptItem->getElevation();
		}
	}
	

	tcx.appendChild(doc.createElement("Courses"));
	tcx.lastChild().appendChild(doc.createElement("Course"));

	tcx.lastChild().lastChild().appendChild(doc.createElement("Name"));
	QString str = trkItem->getName();
	str.truncate(15);
	tcx.lastChild().lastChild().lastChild().appendChild(doc.createTextNode(str));

			
	QDomElement lapElmt = doc.createElement("Lap");
	tcx.lastChild().lastChild().appendChild(lapElmt);

	lapElmt.appendChild(doc.createElement("TotalTimeSeconds"));
	lapElmt.lastChild().appendChild(doc.createTextNode(QString::number(trkItem->getTotalElapsedSeconds())));

	lapElmt.appendChild(doc.createElement("DistanceMeters"));
	lapElmt.lastChild().appendChild(doc.createTextNode(QString::number(trkItem->getTotalDistance())));
			
	lapElmt.appendChild(doc.createElement("Intensity"));
	lapElmt.lastChild().appendChild(doc.createTextNode("Active"));

	trkItem->saveTCX(tcx, ptDateTimes, ptElevations);

	

	j = 0;
	for (int i = 0; i < project.childCount(); i++)
	{
		CGisItemWpt *item = dynamic_cast<CGisItemWpt*>(project.child(i));
		if (nullptr == item)
		{
			continue;
		}

		item->saveTCX(tcx, ptDateTimes[j]);
		j++;
	}



	bool res = true;
	try
	{
		if (!file.open(QIODevice::WriteOnly))
		{
			throw tr("Failed to create file '%1'").arg(_fn_);
		}
		QTextStream out(&file);
		out.setCodec("UTF-8");
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << endl;

		out << doc.toString();
		file.close();
		if (file.error() != QFile::NoError)
		{
			throw tr("Failed to write file '%1'").arg(_fn_);
		}
	}
	catch (const QString& msg)
	{
		// as saveAs() can be called from the thread that exports a database showing the
		// message box will crash the app. Therefore we test if the current thread is the
		// application's main thread. If not we forward the exception.
		//
		// Not sure if that is a good concept.
		if (QThread::currentThread() == qApp->thread())
		{
			QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Saving GIS data failed..."), msg, QMessageBox::Abort);
		}
		else
		{
			throw msg;
		}
		res = false;
	}
	project.umount();
	return res;
}


void CGisItemTrk::saveTCX(QDomNode& tcx, QVector<QDateTime> ptDateTimes, QVector<qint32> ptElevations)
{
	QDomDocument doc = tcx.ownerDocument();

	QDomElement xmlTrk = doc.createElement("Track");
	tcx.lastChild().lastChild().appendChild(xmlTrk);

	int i = 0;

	for (const CTrackData::trkseg_t &seg : trk.segs)
	{
		for (const CTrackData::trkpt_t &pt : seg.pts)
		{
			QDomElement xmlTrkpt = doc.createElement("Trackpoint");
			xmlTrk.appendChild(xmlTrkpt);

			xmlTrkpt.appendChild(doc.createElement("Time"));
			xmlTrkpt.lastChild().appendChild(doc.createTextNode(pt.time.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));
		
			xmlTrkpt.appendChild(doc.createElement("Position"));
			
			xmlTrkpt.lastChild().appendChild(doc.createElement("LatitudeDegrees"));
			QString str;
			str.sprintf("%1.8f", pt.lat);
			xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));
						
			xmlTrkpt.lastChild().appendChild(doc.createElement("LongitudeDegrees"));
			str.sprintf("%1.8f", pt.lon);
			xmlTrkpt.lastChild().lastChild().appendChild(doc.createTextNode(str));

			qint32 eleToBeWritten = NOINT;
			
			if ((i < ptDateTimes.size()) && (pt.time == ptDateTimes[i])) // if trackpoint corresponds to one of the waypoints to be attached to the track
			{
				if (ptElevations[i] != NOINT) // if waypoint has elevation
				{
					eleToBeWritten = ptElevations[i]; // take elevation of the waypoint
				}
				else 
				{
					eleToBeWritten = pt.ele; // if not, take elevation on the trackpoint
				}
				i++;
			}
			else
			{
				if (pt.ele != NOINT) // if this trackpoint has elevation
				{
					eleToBeWritten = pt.ele;
				}
			}
			
			
			if (eleToBeWritten != NOINT) // if valid elevation has been found
			{
				xmlTrkpt.appendChild(doc.createElement("AltitudeMeters"));
				xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(eleToBeWritten)));
			}

			xmlTrkpt.appendChild(doc.createElement("DistanceMeters"));
			xmlTrkpt.lastChild().appendChild(doc.createTextNode(QString::number(pt.distance)));
		}
	}
}


QDateTime CGisItemTrk::getCloserPtDateTime(const QPointF inputPoint)
{
	qreal shortestDistFound;
	QDateTime shortestDistPtDateTime;
	bool firstCycle = true;

	for (const CTrackData::trkseg_t &seg : trk.segs)
	{
		for (const CTrackData::trkpt_t &pt : seg.pts)
		{
			qreal dist = GPS_Math_Distance(pt.lon, pt.lat, inputPoint.x(), inputPoint.y());
			if (firstCycle)
			{
				shortestDistFound = dist;
				firstCycle = false;
			}
			else
			{
				if (dist < shortestDistFound)
				{
					shortestDistFound = dist;
					shortestDistPtDateTime = pt.time;
				}
			}
		}
	}

	return shortestDistPtDateTime;
}



void CGisItemWpt::saveTCX(QDomNode& tcx, const QDateTime crsPtDateTimeToBeSaved)
{
	QDomDocument doc = tcx.ownerDocument();

	QDomElement xmlCrsPt = doc.createElement("CoursePoint");
	tcx.lastChild().lastChild().appendChild(xmlCrsPt);

	xmlCrsPt.appendChild(doc.createElement("Name"));
	QString str = wpt.name;
	str.truncate(10);
	xmlCrsPt.lastChild().appendChild(doc.createTextNode(str));

	xmlCrsPt.appendChild(doc.createElement("Time"));
	xmlCrsPt.lastChild().appendChild(doc.createTextNode(crsPtDateTimeToBeSaved.toString("yyyy-MM-dd'T'hh:mm:ss'Z'")));

	xmlCrsPt.appendChild(doc.createElement("Position"));

	xmlCrsPt.lastChild().appendChild(doc.createElement("LatitudeDegrees"));
	str.sprintf("%1.8f", wpt.lat);
	xmlCrsPt.lastChild().lastChild().appendChild(doc.createTextNode(str));

	xmlCrsPt.lastChild().appendChild(doc.createElement("LongitudeDegrees"));
	str.sprintf("%1.8f", wpt.lon);
	xmlCrsPt.lastChild().lastChild().appendChild(doc.createTextNode(str));

	if (wpt.ele != NOINT)
	{
		xmlCrsPt.appendChild(doc.createElement("AltitudeMeters"));
		xmlCrsPt.lastChild().appendChild(doc.createTextNode(QString::number(wpt.ele)));
	}


	QString pointTypeToBeWritten;

	if (wpt.sym != "1st Category" &&
		wpt.sym != "2nd Category" &&
		wpt.sym != "3rd Category" &&
		wpt.sym != "4th Category" &&
		wpt.sym != "Danger" &&
		wpt.sym != "First Aid" &&
		wpt.sym != "Food" &&
		wpt.sym != "Hors Category" &&
		wpt.sym != "Left" &&
		wpt.sym != "Right" &&
		wpt.sym != "Sprint" &&
		wpt.sym != "Straight" &&
		wpt.sym != "Summit" &&
		wpt.sym != "Valley" &&
		wpt.sym != "Water")
	{
		pointTypeToBeWritten = "Generic";
	}
	else
	{
		pointTypeToBeWritten = wpt.sym;
	}

	xmlCrsPt.appendChild(doc.createElement("PointType"));
	xmlCrsPt.lastChild().appendChild(doc.createTextNode(pointTypeToBeWritten));

}