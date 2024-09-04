/**********************************************************************************************
    Copyright (C) 2016 Michel Durand <zero@cms123.fr>

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

#include "gis/tcx/CTcxProject.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "device/IDevice.h"
#include "gis/CGisListWks.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectCopyAction.h"
#include "version.h"

CTcxProject::CTcxProject(const QString& filename, CGisListWks* parent) : IGisProject(eTypeTcx, filename, parent) {
  setup();
}

CTcxProject::CTcxProject(const QString& filename, IDevice* parent) : IGisProject(eTypeGpx, filename, parent) {
  setup();
}

CTcxProject::CTcxProject(const QString& filename, const IGisProject* project, IDevice* parent)
    : IGisProject(eTypeGpx, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/TcxProject.png"));
  *(IGisProject*)this = *project;
  blockUpdateItems(project->blockUpdateItems());

  CSelectCopyAction::result_e res = CSelectCopyAction::eResultNone;
  const int N = project->childCount();
  for (int n = 0; n < N; n++) {
    IGisItem* item = dynamic_cast<IGisItem*>(project->child(n));
    if (item) {
      insertCopyOfItem(item, NOIDX, res);
    }
  }

  blockUpdateItems(false);
  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
  setToolTip(CGisListWks::eColumnName, getInfo());
  valid = true;
}

void CTcxProject::setup() {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/TcxProject.png"));
  blockUpdateItems(true);
  loadTcx(filename);
  blockUpdateItems(false);
  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}

void CTcxProject::loadTcx(const QString& filename) {
  try {
    loadTcx(filename, this);
  } catch (QString& errormsg) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to load file %1...").arg(filename),
                          errormsg, QMessageBox::Abort);
    valid = false;
  }
}

void CTcxProject::loadTcx(const QString& filename, CTcxProject* project) {
  QFile file(filename);

  // if the file does not exist, the file name is assumed to be a name for a new project
  if (!file.exists() || QFileInfo(filename).suffix().toLower() != "tcx") {
    project->filename.clear();
    project->setupName(filename);
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
    return;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    throw tr("Failed to open %1").arg(filename);
  }

  QDomDocument xml;
  QString msg;
  int line;
  int column;
  if (!xml.setContent(&file, false, &msg, &line, &column)) {
    file.close();
    throw tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg);
  }
  file.close();

  QDomElement xmlTcx = xml.documentElement();
  if (xmlTcx.tagName() != "TrainingCenterDatabase") {
    throw tr("Not a TCX file: %1").arg(filename);
  }

  const QDomNodeList& tcxActivities = xmlTcx.elementsByTagName("Activity");
  const QDomNodeList& tcxCourses = xmlTcx.elementsByTagName("Course");

  if (!tcxActivities.item(0).isElement() && !tcxCourses.item(0).isElement()) {
    if (xmlTcx.elementsByTagName("Workout").item(0).isElement()) {
      throw tr(
          "This TCX file contains at least 1 workout, but neither an activity nor a course. "
          "As workouts do not contain position data, they can not be imported to QMapShack.");
    } else {
      throw tr("This TCX file does not contain any activity or course: %1").arg(filename);
    }
  }

  for (int i = 0; i < tcxActivities.count(); i++) {
    project->loadActivity(tcxActivities.item(i));
  }

  for (int i = 0; i < tcxCourses.count(); i++) {
    project->loadCourse(tcxCourses.item(i));
  }

  project->sortItems();
  project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
  project->setToolTip(CGisListWks::eColumnName, project->getInfo());
  project->valid = true;
}

void CTcxProject::loadActivity(const QDomNode& activityRootNode) {
  if (activityRootNode.isElement()) {
    CTrackData trk;

    trk.name = activityRootNode.toElement()
                   .elementsByTagName("Id")
                   .item(0)
                   .firstChild()
                   .nodeValue();  // activities do not have a "Name" but an "Id" instead (containing start date-time)

    const QDomNodeList& tcxLaps = activityRootNode.toElement().elementsByTagName("Lap");

    trk.segs.resize(tcxLaps.count());
    for (int i = 0; i < tcxLaps.count(); i++)  // browse laps
    {
      CTrackData::trkseg_t* seg = &(trk.segs[i]);

      const QDomNodeList& tcxLapTrackpts = tcxLaps.item(i).toElement().elementsByTagName("Trackpoint");

      for (int j = 0; j < tcxLapTrackpts.count(); j++)  // browse trackpoints
      {
        const QDomElement& positionElement =
            tcxLapTrackpts.item(j).toElement().elementsByTagName("Position").item(0).toElement();

        if (positionElement
                .isElement())  // if this trackpoint contains position, i.e. GPSr was able to capture position
        {
          CTrackData::trkpt_t trkpt;

          IUnit::parseTimestamp(
              tcxLapTrackpts.item(j).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue(),
              trkpt.time);

          trkpt.lat = positionElement.elementsByTagName("LatitudeDegrees").item(0).firstChild().nodeValue().toDouble();
          trkpt.lon = positionElement.elementsByTagName("LongitudeDegrees").item(0).firstChild().nodeValue().toDouble();
          trkpt.ele = tcxLapTrackpts.item(j)
                          .toElement()
                          .elementsByTagName("AltitudeMeters")
                          .item(0)
                          .firstChild()
                          .nodeValue()
                          .toDouble();

          const QDomElement& HRElement =
              tcxLapTrackpts.item(j).toElement().elementsByTagName("HeartRateBpm").item(0).toElement();
          if (HRElement.isElement())  // if this trackpoint contains heartrate data, i.e. heartrate sensor data has been
                                      // captured
          {
            trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] =
                HRElement.elementsByTagName("Value").item(0).firstChild().nodeValue().toDouble();
          }

          const QDomElement& CADElement =
              tcxLapTrackpts.item(j).toElement().elementsByTagName("Cadence").item(0).toElement();
          if (CADElement
                  .isElement())  // if this trackpoint contains cadence data, i.e. cadence sensor data has been captured
          {
            trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"] = CADElement.firstChild().nodeValue().toDouble();
          }

          seg->pts.append(trkpt);  // 1 TCX lap gives 1 GPX track segment
        }
      }
    }

    CGisItemTrk* trkItem = new CGisItemTrk(trk, this);
    trackTypes.insert(trkItem->getKey().item, eActivity);  // store the track type according to its key
  }
}

void CTcxProject::loadCourse(const QDomNode& courseRootNode) {
  if (courseRootNode.isElement()) {
    CTrackData trk;

    trk.name = courseRootNode.toElement().elementsByTagName("Name").item(0).firstChild().nodeValue();
    trk.segs.resize(1);
    CTrackData::trkseg_t* seg = &(trk.segs[0]);

    const QDomNodeList& tcxTrackpts = courseRootNode.toElement().elementsByTagName("Trackpoint");

    for (int i = 0; i < tcxTrackpts.count(); i++)  // browse trackpoints
    {
      const QDomElement& positionElement =
          tcxTrackpts.item(i).toElement().elementsByTagName("Position").item(0).toElement();

      if (positionElement.isElement())  // if this trackpoint contains position, i.e. GPSr was able to capture position
      {
        CTrackData::trkpt_t trkpt;

        IUnit::parseTimestamp(
            tcxTrackpts.item(i).toElement().elementsByTagName("Time").item(0).firstChild().nodeValue(), trkpt.time);

        trkpt.lat = positionElement.elementsByTagName("LatitudeDegrees").item(0).firstChild().nodeValue().toDouble();
        trkpt.lon = positionElement.elementsByTagName("LongitudeDegrees").item(0).firstChild().nodeValue().toDouble();
        trkpt.ele = tcxTrackpts.item(i)
                        .toElement()
                        .elementsByTagName("AltitudeMeters")
                        .item(0)
                        .firstChild()
                        .nodeValue()
                        .toDouble();

        const QDomElement& HRElement =
            tcxTrackpts.item(i).toElement().elementsByTagName("HeartRateBpm").item(0).toElement();
        if (HRElement.isElement())  // if this trackpoint contains heartrate data, i.e. heartrate sensor data has been
                                    // captured
        {
          trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"] =
              HRElement.elementsByTagName("Value").item(0).firstChild().nodeValue().toDouble();
        }

        const QDomElement& CADElement =
            tcxTrackpts.item(i).toElement().elementsByTagName("Cadence").item(0).toElement();
        if (CADElement
                .isElement())  // if this trackpoint contains cadence data, i.e. cadence sensor data has been captured
        {
          trkpt.extensions["gpxtpx:TrackPointExtension|gpxtpx:cad"] = CADElement.firstChild().nodeValue().toDouble();
        }

        seg->pts.append(trkpt);
      }
    }

    CGisItemTrk* trkItem = new CGisItemTrk(trk, this);
    trackTypes.insert(trkItem->getKey().item, eCourse);  // store the track type according to its key

    const QDomNodeList& tcxCoursePts = courseRootNode.toElement().elementsByTagName("CoursePoint");
    for (int i = 0; i < tcxCoursePts.count(); i++)  // browse course points
    {
      QString name = tcxCoursePts.item(i).toElement().elementsByTagName("Name").item(0).firstChild().nodeValue();
      qreal lat = tcxCoursePts.item(i)
                      .toElement()
                      .elementsByTagName("Position")
                      .item(0)
                      .toElement()
                      .elementsByTagName("LatitudeDegrees")
                      .item(0)
                      .firstChild()
                      .nodeValue()
                      .toDouble();
      qreal lon = tcxCoursePts.item(i)
                      .toElement()
                      .elementsByTagName("Position")
                      .item(0)
                      .toElement()
                      .elementsByTagName("LongitudeDegrees")
                      .item(0)
                      .firstChild()
                      .nodeValue()
                      .toDouble();
      qreal ele = tcxCoursePts.item(i)
                      .toElement()
                      .elementsByTagName("AltitudeMeters")
                      .item(0)
                      .firstChild()
                      .nodeValue()
                      .toDouble();
      QString icon = tcxCoursePts.item(i)
                         .toElement()
                         .elementsByTagName("PointType")
                         .item(0)
                         .firstChild()
                         .nodeValue();  // there is no "icon" in course points ;  "PointType" is used instead (can be
                                        // "turn left", "turn right", etc... See list in
                                        // http://www8.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd)

      new CGisItemWpt(QPointF(lon, lat), ele, QDateTime::currentDateTimeUtc(), name, icon,
                      this);  // 1 TCX course point gives 1 GPX waypoint
    }
  }
}

bool CTcxProject::saveAs(const QString& fn, IGisProject& project) {
  QString _fn_ = fn;
  QFileInfo fi(_fn_);
  if (fi.suffix().toLower() != "tcx") {
    _fn_ += ".tcx";
  }

  CProjectMountLock mountLock(project);

  // safety check for existing files
  QFile file(_fn_);
  if (file.exists()) {
    file.open(QIODevice::ReadOnly);
    bool createdByQMS = false;

    // load file content to xml document
    QDomDocument xmlTcx;
    if (xmlTcx.setContent(&file, false)) {
      const QDomNodeList& tcxAuthor = xmlTcx.elementsByTagName("Author");
      if (tcxAuthor.item(0).isElement()) {
        const QDomNodeList& tcxAuthorName = tcxAuthor.item(0).toElement().elementsByTagName("Name");
        createdByQMS = tcxAuthorName.item(0).firstChild().nodeValue() == "QMapShack";
      }
    }

    if (!createdByQMS) {
      CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
      int res = QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("File exists ..."),
                                     tr("The file exists and it has not been created by QMapShack. "
                                        "If you press 'yes' all data in this file will be lost. "
                                        "Even if this file contains data and has been loaded by QMapShack, "
                                        "QMapShack might not be able to load and store all elements of this file.  "
                                        "Those elements will be lost. I recommend to use another file. "
                                        "<b>Do you really want to overwrite the file?</b>"),
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
      if (res == QMessageBox::No) {
        return false;
      }
    }
    file.close();
  }

  //  ---- start content of tcx
  QDomDocument doc;
  QDomElement tcx = doc.createElement("TrainingCenterDatabase");
  doc.appendChild(tcx);

  tcx.setAttribute("xmlns", "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");
  tcx.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
  tcx.setAttribute(
      "xsi:schemaLocation",
      "http://www.garmin.com/xmlschemas/ProfileExtension/v1 "
      "http://www.garmin.com/xmlschemas/UserProfilePowerExtensionv1.xsd "
      "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 "
      "http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd http://www.garmin.com/xmlschemas/UserProfile/v2 "
      "http://www.garmin.com/xmlschemas/UserProfileExtensionv2.xsd");

  QList<CGisItemTrk*> courseTrks, activityTrks;
  for (int i = 0; i < project.childCount(); i++)  // browse tracks in selected project
  {
    CGisItemTrk* trkItem = dynamic_cast<CGisItemTrk*>(project.child(i));
    if (nullptr == trkItem) {
      continue;  // not a track
    } else {
      if (!trkItem->isTrkTimeValid()) {
        CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
        int res =
            QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Track with invalid timestamps..."),
                                 tr("The track <b>%1</b> you have selected contains trackpoints with "
                                    "invalid timestamps. "
                                    "Device might not accept the generated TCX course file if left as is. "
                                    "<b>Do you want to apply a filter with constant speed (10 m/s) and continue?</b>")
                                     .arg(trkItem->getName()),
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (res == QMessageBox::No) {
          project.blockUpdateItems(false);
          return false;
        }

        if (res == QMessageBox::Yes) {
          project.blockUpdateItems(true);  // block is necessary as changing the start date will also change the track
                                           // position in project when project sorting order is "by date"
          trkItem->filterSpeed(10);
        }
      }

      QMessageBox courseOrActivityMsgBox;
      courseOrActivityMsgBox.setWindowTitle(tr("Activity or course?"));
      courseOrActivityMsgBox.setText(
          tr("QMapShack does not know how track <b>%1</b> should be saved. "
             "<b>Do you want to save it as a course or as an activity? </b>"
             "Remember that only waypoints close enough to the track will be saved when saving as a course. "
             "Waypoints will not be saved when saving as an activity.")
              .arg(trkItem->getName()));
      QAbstractButton* pButtonCourse = courseOrActivityMsgBox.addButton(tr("Course"), QMessageBox::AcceptRole);
      QAbstractButton* pButtonActivity = courseOrActivityMsgBox.addButton(tr("Activity"), QMessageBox::AcceptRole);
      QAbstractButton* pButtonCancel = courseOrActivityMsgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

      CTcxProject* tcx = dynamic_cast<CTcxProject*>(&project);
      if (nullptr != tcx)  // if a TCX project
      {
        if (dynamic_cast<IDevice*>(tcx->parent()) != nullptr) {
          courseTrks << trkItem;
        } else if (!tcx->trackTypes.contains(trkItem->getKey().item))  // if this is an added track
        {
          {
            CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
            courseOrActivityMsgBox.exec();
          }

          if (courseOrActivityMsgBox.clickedButton() == pButtonCourse) {
            courseTrks << trkItem;
          }
          if (courseOrActivityMsgBox.clickedButton() == pButtonActivity) {
            activityTrks << trkItem;
          }
          if (courseOrActivityMsgBox.clickedButton() == pButtonCancel) {
            project.blockUpdateItems(false);
            return false;
          }
        } else {
          if (tcx->trackTypes.value(trkItem->getKey().item) == eCourse)  // if a course
          {
            courseTrks << trkItem;
          }
          if (tcx->trackTypes.value(trkItem->getKey().item) == eActivity)  // if an activity
          {
            activityTrks << trkItem;
          }
        }
      } else  // not a TCX project, then it is necessary to ask for each track
      {
        {
          CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
          courseOrActivityMsgBox.exec();
        }

        if (courseOrActivityMsgBox.clickedButton() == pButtonCourse) {
          courseTrks << trkItem;
        }
        if (courseOrActivityMsgBox.clickedButton() == pButtonActivity) {
          activityTrks << trkItem;
        }
        if (courseOrActivityMsgBox.clickedButton() == pButtonCancel) {
          project.blockUpdateItems(false);
          return false;
        }
      }
    }
  }
  project.blockUpdateItems(false);

  QDomNode activitiesNode = doc.createElement("Activities");
  if (activityTrks.size() != 0) {
    tcx.appendChild(activitiesNode);
  }
  for (CGisItemTrk* trkToBeSaved : std::as_const(activityTrks)) {
    trkToBeSaved->saveTCXactivity(activitiesNode);
  }

  QDomNode coursesNode = doc.createElement("Courses");
  if (courseTrks.size() != 0) {
    tcx.appendChild(coursesNode);
  }
  for (CGisItemTrk* trkToBeSaved : std::as_const(courseTrks)) {
    trkToBeSaved->saveTCXcourse(coursesNode);
  }

  saveAuthor(tcx);

  bool res = true;
  QString msg;

  if (!file.open(QIODevice::WriteOnly)) {
    // copied-pasted from CGpxProject::saveAs :
    // "as saveAs() can be called from the thread that exports a database showing the
    // message box will crash the app. Therefore we test if the current thread is the
    // application's main thread. If not we forward the exception.
    //
    // Not sure if that is a good concept."
    msg = tr("Failed to create file '%1'").arg(_fn_);
    if (QThread::currentThread() == qApp->thread()) {
      CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Saving GIS data failed..."), msg,
                           QMessageBox::Abort);
    } else {
      throw msg;
    }
    res = false;
  }
  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << Qt::endl;

  out << doc.toString();
  file.close();
  if (file.error() != QFile::NoError) {
    if (QThread::currentThread() == qApp->thread()) {
      CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
      msg = tr("Failed to write file '%1'").arg(_fn_);
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Saving GIS data failed..."), msg,
                           QMessageBox::Abort);
    } else {
      throw msg;
    }
    res = false;
  }
  return res;
}

void CTcxProject::saveAuthor(QDomNode& nodeToAttachAuthor) {
  QDomDocument doc = nodeToAttachAuthor.toElement().ownerDocument();

  nodeToAttachAuthor.appendChild(doc.createElement("Author"));
  nodeToAttachAuthor.lastChild().toElement().setAttribute("xsi:type", "Application_t");

  nodeToAttachAuthor.lastChild().appendChild(doc.createElement("Name"));
  nodeToAttachAuthor.lastChild().lastChild().appendChild(doc.createTextNode("QMapShack"));
  nodeToAttachAuthor.lastChild().appendChild(doc.createElement("Build"));
  nodeToAttachAuthor.lastChild().lastChild().appendChild(doc.createElement("Version"));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().appendChild(doc.createElement("VersionMajor"));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().lastChild().appendChild(doc.createTextNode(_MKSTR(VER_MAJOR)));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().appendChild(doc.createElement("VersionMinor"));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().lastChild().appendChild(doc.createTextNode(_MKSTR(VER_MINOR)));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().appendChild(doc.createElement("BuildMajor"));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().lastChild().appendChild(doc.createTextNode(_MKSTR(VER_STEP)));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().appendChild(doc.createElement("BuildMinor"));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().lastChild().appendChild(doc.createTextNode("0"));
  nodeToAttachAuthor.lastChild().lastChild().appendChild(doc.createElement("Type"));
  nodeToAttachAuthor.lastChild().lastChild().lastChild().appendChild(doc.createTextNode("Release"));
  nodeToAttachAuthor.lastChild().appendChild(doc.createElement("LangID"));
  nodeToAttachAuthor.lastChild().lastChild().appendChild(doc.createTextNode(QLocale().bcp47Name()));
  nodeToAttachAuthor.lastChild().appendChild(doc.createElement("PartNumber"));
  nodeToAttachAuthor.lastChild().lastChild().appendChild(doc.createTextNode("000-00000-00"));  // dummy number
}
