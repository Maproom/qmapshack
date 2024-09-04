/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/gpx/CGpxProject.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "device/IDevice.h"
#include "gis/CGisListWks.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectCopyAction.h"

CGpxProject::CGpxProject(const QString& filename, CGisListWks* parent) : IGisProject(eTypeGpx, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/GpxProject.png"));
  blockUpdateItems(true);
  loadGpx(filename);
  blockUpdateItems(false);
}

CGpxProject::CGpxProject(const QString& filename, IDevice* parent) : IGisProject(eTypeGpx, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/GpxProject.png"));
  blockUpdateItems(true);
  loadGpx(filename);
  blockUpdateItems(false);
}

CGpxProject::CGpxProject(const QString& filename, const IGisProject* project, IDevice* parent)
    : IGisProject(eTypeGpx, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/GpxProject.png"));
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

  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
  blockUpdateItems(false);
  setToolTip(CGisListWks::eColumnName, getInfo());
  valid = true;
}

CGpxProject::~CGpxProject() {}

void CGpxProject::loadGpx(const QString& filename) {
  try {
    loadGpx(filename, this);
  } catch (QString& errormsg) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to load file %1...").arg(filename),
                          errormsg, QMessageBox::Abort);
    valid = false;
  }
}

void CGpxProject::loadGpx(const QString& filename, CGpxProject* project) {
  // create file instance
  QFile file(filename);

  // if the file does not exist, the filename is assumed to be a name for a new project
  if (!file.exists() || QFileInfo(filename).suffix().toLower() != "gpx") {
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
  QString msg;
  int line;
  int column;
  if (!xml.setContent(&file, false, &msg, &line, &column)) {
    file.close();
    throw tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg);
  }
  file.close();

  int N;
  QDomElement xmlGpx = xml.documentElement();
  if (xmlGpx.tagName() != "gpx") {
    throw tr("Not a GPX file: %1").arg(filename);
  }

  // Read all attributes and find any registrations for actually known extensions.
  // This is used to properly detect valid .gpx files using uncommon namespaces.
  QDomNamedNodeMap attributes = xmlGpx.attributes();
  for (int i = 0; i < attributes.size(); ++i) {
    const QString xmlns("xmlns");
    QDomAttr att = attributes.item(i).toAttr();

    if (att.name().startsWith(xmlns + ":")) {
      QString ns = att.name().mid(xmlns.length() + 1);

      if (att.value() == gpxtpx_ns) {
        CKnownExtension::initGarminTPXv1(IUnit::self(), ns);
      } else if (att.value() == gpxdata_ns) {
        CKnownExtension::initClueTrustTPXv1(IUnit::self(), ns);
      }
    }
  }

  const QDomElement& xmlExtension = xmlGpx.namedItem("extensions").toElement();
  if (xmlExtension.namedItem("ql:key").isElement()) {
    project->key = xmlExtension.namedItem("ql:key").toElement().text();
  }

  if (xmlExtension.namedItem("ql:sortingRoadbook").isElement()) {
    project->sortingRoadbook =
        sorting_roadbook_e(xmlExtension.namedItem("ql:sortingRoadbook").toElement().text().toInt());
  }

  if (xmlExtension.namedItem("ql:sortingFolder").isElement()) {
    project->sortingFolder = sorting_folder_e(xmlExtension.namedItem("ql:sortingFolder").toElement().text().toInt());
  }

  if (xmlExtension.namedItem("ql:correlation").isElement()) {
    project->noCorrelation = bool(xmlExtension.namedItem("ql:correlation").toElement().text().toInt() == 0);
  }

  if (xmlExtension.namedItem("ql:invalidDataOk").isElement()) {
    project->invalidDataOk = bool(xmlExtension.namedItem("ql:invalidDataOk").toElement().text().toInt() != 0);
  }

  const QDomNode& xmlMetadata = xmlGpx.namedItem("metadata");
  if (xmlMetadata.isElement()) {
    project->readMetadata(xmlMetadata, project->metadata);
  }

  /** @note   If you change the order of the item types read you have to
              take care of the order enforced in IGisItem().
   */
  const QDomNodeList& xmlTrks = xmlGpx.elementsByTagName("trk");
  N = xmlTrks.count();
  for (int n = 0; n < N; ++n) {
    const QDomNode& xmlTrk = xmlTrks.item(n);
    new CGisItemTrk(xmlTrk, project);
  }

  const QDomNodeList& xmlRtes = xmlGpx.elementsByTagName("rte");
  N = xmlRtes.count();
  for (int n = 0; n < N; ++n) {
    const QDomNode& xmlRte = xmlRtes.item(n);
    new CGisItemRte(xmlRte, project);
  }

  const QDomNodeList& xmlWpts = xmlGpx.elementsByTagName("wpt");
  N = xmlWpts.count();
  for (int n = 0; n < N; ++n) {
    const QDomNode& xmlWpt = xmlWpts.item(n);
    CGisItemWpt* wpt = new CGisItemWpt(xmlWpt, project);

    /*
        Special care for waypoints stored on Garmin devices. Images attached
        to the waypoint are stored in the file system of the device and written
        as links to the waypoint. Let the device object take care of this.
     */
    IDevice* device = dynamic_cast<IDevice*>(project->parent());
    if (device) {
      device->loadImages(*wpt);
    }
  }

  const QDomNodeList& xmlAreas = xmlExtension.elementsByTagName("ql:area");
  N = xmlAreas.count();
  for (int n = 0; n < N; ++n) {
    const QDomNode& xmlArea = xmlAreas.item(n);
    new CGisItemOvlArea(xmlArea, project);
  }

  project->sortItems();
  project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
  project->setToolTip(CGisListWks::eColumnName, project->getInfo());
  project->valid = true;
}

bool CGpxProject::saveAs(const QString& fn, IGisProject& project, bool strictGpx11) {
  QString _fn_ = fn;
  QFileInfo fi(_fn_);
  if (fi.suffix().toLower() != "gpx") {
    _fn_ += ".gpx";
  }

  CProjectMountLock mountLock(project);

  // safety check for existing files
  QFile file(_fn_);
  if (file.exists()) {
    file.open(QIODevice::ReadOnly);
    bool createdByQMS = false;

    // load file content to xml document
    QDomDocument xml;
    if (xml.setContent(&file, false)) {
      const QDomElement& docElem = xml.documentElement();
      const QDomNamedNodeMap& attr = docElem.attributes();
      createdByQMS = attr.namedItem("creator").nodeValue().startsWith("QMapShack");
    }

    if (!createdByQMS) {
      int res = QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("File exists ..."),
                                     tr("The file exists and it has not been created by QMapShack. "
                                        "If you press 'yes' all data in this file will be lost. "
                                        "Even if this file contains GPX data and has been loaded by QMapShack, "
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

  //  ---- start content of gpx
  QDomDocument doc;
  QDomNode gpx = project.writeMetadata(doc, strictGpx11);

  IDevice* device = dynamic_cast<IDevice*>(project.parent());
  if (device) {
    device->startSavingProject(&project);
  }

  for (int i = 0; i < project.childCount(); i++) {
    CGisItemWpt* item = dynamic_cast<CGisItemWpt*>(project.child(i));
    if (nullptr == item) {
      continue;
    }

    /*
        Special care for waypoints stored on Garmin devices. Images attached
        to the waypoint are stored in the file system of the device and written
        as links to the waypoint. Let the device object take care of this.
     */
    if (device) {
      device->saveImages(*item);
    }

    item->save(gpx, strictGpx11);
  }
  for (int i = 0; i < project.childCount(); i++) {
    CGisItemRte* item = dynamic_cast<CGisItemRte*>(project.child(i));
    if (nullptr == item) {
      continue;
    }
    item->save(gpx, strictGpx11);
  }
  for (int i = 0; i < project.childCount(); i++) {
    CGisItemTrk* item = dynamic_cast<CGisItemTrk*>(project.child(i));
    if (nullptr == item) {
      continue;
    }
    item->save(gpx, strictGpx11);
  }

  if (!strictGpx11) {
    QDomElement xmlExt = doc.createElement("extensions");
    gpx.appendChild(xmlExt);
    for (int i = 0; i < project.childCount(); i++) {
      CGisItemOvlArea* item = dynamic_cast<CGisItemOvlArea*>(project.child(i));
      if (nullptr == item) {
        continue;
      }
      item->save(xmlExt, strictGpx11);
    }

    if (!project.getKey().isEmpty()) {
      QDomElement elem = xmlExt.ownerDocument().createElement("ql:key");
      xmlExt.appendChild(elem);
      QDomText text = xmlExt.ownerDocument().createTextNode(project.getKey());
      elem.appendChild(text);
    }

    {
      QDomElement elem = xmlExt.ownerDocument().createElement("ql:sortingRoadbook");
      xmlExt.appendChild(elem);
      QDomText text = xmlExt.ownerDocument().createTextNode(QString::number(project.getSortingRoadbook()));
      elem.appendChild(text);
    }

    {
      QDomElement elem = xmlExt.ownerDocument().createElement("ql:sortingFolder");
      xmlExt.appendChild(elem);
      QDomText text = xmlExt.ownerDocument().createTextNode(QString::number(project.getSortingFolder()));
      elem.appendChild(text);
    }

    {
      QDomElement elem = xmlExt.ownerDocument().createElement("ql:correlation");
      xmlExt.appendChild(elem);
      QDomText text = xmlExt.ownerDocument().createTextNode(QString::number(project.doCorrelation()));
      elem.appendChild(text);
    }

    {
      QDomElement elem = xmlExt.ownerDocument().createElement("ql:invalidDataOk");
      xmlExt.appendChild(elem);
      QDomText text = xmlExt.ownerDocument().createTextNode(QString::number(project.getInvalidDataOk()));
      elem.appendChild(text);
    }
  }
  //  ---- stop  content of gpx

  bool res = true;
  try {
    if (!file.open(QIODevice::WriteOnly)) {
      throw tr("Failed to create file '%1'").arg(_fn_);
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << Qt::endl;

    out << doc.toString();
    file.close();
    if (file.error() != QFile::NoError) {
      throw tr("Failed to write file '%1'").arg(_fn_);
    }
  } catch (const QString& msg) {
    // as saveAs() can be called from the thread that exports a database showing the
    // message box will crash the app. Therefore we test if the current thread is the
    // application's main thread. If not we forward the exception.
    //
    // Not sure if that is a good concept.
    if (QThread::currentThread() == qApp->thread()) {
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Saving GIS data failed..."), msg,
                           QMessageBox::Abort);
    } else {
      throw msg;
    }
    res = false;
  }
  return res;
}
