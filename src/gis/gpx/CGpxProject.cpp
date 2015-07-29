/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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
#include "device/CDeviceGarmin.h"
#include "device/IDevice.h"
#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CGpxProject::CGpxProject(const QString &filename, CGisListWks *parent)
    : IGisProject(eTypeGpx, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/GpxProject.png"));
    blockUpdateItems(true);
    loadGpx(filename);
    blockUpdateItems(false);
}

CGpxProject::CGpxProject(const QString &filename, IDevice * parent)
    : IGisProject(eTypeGpx, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/GpxProject.png"));
    blockUpdateItems(true);
    loadGpx(filename);
    blockUpdateItems(false);
}

CGpxProject::CGpxProject(const QString &filename, const IGisProject * project, IDevice * parent)
    : IGisProject(eTypeGpx, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/GpxProject.png"));
    *(IGisProject*)this = *project;
    blockUpdateItems(project->blockUpdateItems());

    int res     = CSelectCopyAction::eResultNone;
    const int N = project->childCount();
    for(int n = 0; n < N; n++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(project->child(n));
        if(item)
        {
            insertCopyOfItem(item, NOIDX, res);
        }
    }

    setupName(QFileInfo(filename).baseName().replace("_", " "));
    blockUpdateItems(false);
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
}

CGpxProject::~CGpxProject()
{
}

void CGpxProject::loadGpx(const QString& filename)
{
    // cerate file instance
    QFile file(filename);

    // if the file does not exist, the filename is assumed to be a name for a new project
    if(!file.exists())
    {
        IGisProject::filename.clear();
        setupName(filename);
        setToolTip(CGisListWks::eColumnName, getInfo());
        valid = true;
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(&CMainWindow::self(), QObject::tr("Failed to open..."), QObject::tr("Failed to open %1").arg(filename), QMessageBox::Abort);
        return;
    }


    // load file content to xml document
    QDomDocument xml;
    QString msg;
    int line;
    int column;
    if(!xml.setContent(&file, false, &msg, &line, &column))
    {
        file.close();
        QMessageBox::critical(&CMainWindow::self(), QObject::tr("Failed to read..."), QObject::tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg), QMessageBox::Abort);
        return;
    }
    file.close();

    int N;
    QDomElement xmlGpx = xml.documentElement();
    if(xmlGpx.tagName() != "gpx")
    {
        QMessageBox::critical(&CMainWindow::self(), QObject::tr("Failed to read..."), QObject::tr("Not a GPX file: ") + filename, QMessageBox::Abort);
        return;
    }

    const QDomElement& xmlExtension = xmlGpx.namedItem("extensions").toElement();
    if(xmlExtension.namedItem("ql:key").isElement())
    {
        key = xmlExtension.namedItem("ql:key").toElement().text();
    }

    if(xmlExtension.namedItem("ql:sorting").isElement())
    {
        sorting = sorting_e(xmlExtension.namedItem("ql:sorting").toElement().text().toInt());
    }

    if(xmlExtension.namedItem("ql:correlation").isElement())
    {
        noCorrelation = bool(xmlExtension.namedItem("ql:correlation").toElement().text().toInt() == 0);
    }

    const QDomNode& xmlMetadata = xmlGpx.namedItem("metadata");
    if(xmlMetadata.isElement())
    {
        readMetadata(xmlMetadata, metadata);
    }


    /** @note   If you change the order of the item types read you have to
                take care of the order enforced in IGisItem().
     */
    const QDomNodeList& xmlTrks = xmlGpx.elementsByTagName("trk");
    N = xmlTrks.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlTrk = xmlTrks.item(n);
        new CGisItemTrk(xmlTrk, this);
    }

    const QDomNodeList& xmlRtes = xmlGpx.elementsByTagName("rte");
    N = xmlRtes.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlRte = xmlRtes.item(n);
        new CGisItemRte(xmlRte, this);
    }

    const QDomNodeList& xmlWpts = xmlGpx.elementsByTagName("wpt");
    N = xmlWpts.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlWpt = xmlWpts.item(n);
        CGisItemWpt * wpt = new CGisItemWpt(xmlWpt, this);

        /*
            Special care for waypoints stored on Garmin devices. Images attached
            to the waypoint are stored in the file system of the device and written
            as links to the waypoint. Let the device object take care of this.
         */
        IDevice * device = dynamic_cast<IDevice*>(parent());
        if(device)
        {
            device->loadImages(*wpt);
        }
    }

    const QDomNodeList& xmlAreas = xmlExtension.elementsByTagName("ql:area");
    N = xmlAreas.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlArea = xmlAreas.item(n);
        new CGisItemOvlArea(xmlArea, this);
    }


    setupName(QFileInfo(filename).baseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
}

bool CGpxProject::save()
{
    if(filename.isEmpty())
    {
        return saveAs();
    }
    else
    {
        if(saveAs(filename, *this))
        {
            markAsSaved();
        }
    }

    return true;
}

bool CGpxProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();
    path += "/" + getName() + ".gpx";

    QString filter = "*.gpx";
    QString fn = QFileDialog::getSaveFileName(&CMainWindow::self(), QObject::tr("Save GIS data to..."), path, "*.gpx;; *.qms", &filter);

    if(fn.isEmpty())
    {
        return false;
    }


    bool res = false;
    if(filter == "*.gpx")
    {
        filename = fn;
        metadata.name.clear();
        setupName(QFileInfo(filename).baseName().replace("_", " "));

        res = saveAs(fn, *this);
        if(res)
        {
            markAsSaved();
        }
    }
    else if(filter == "*.qms")
    {
        res = CQmsProject::saveAs(fn, *this);
    }
    else
    {
        return false;
    }

    path = QFileInfo(fn).absolutePath();
    cfg.setValue("Paths/lastGisPath", path);
    return res;
}


bool CGpxProject::saveAs(const QString& fn, IGisProject& project)
{
    QString _fn_ = fn;
    QFileInfo fi(_fn_);
    if(fi.suffix().toLower() != "gpx")
    {
        _fn_ += ".gpx";
    }

    project.mount();

    // safety check for existing files
    QFile file(_fn_);
    if(file.exists())
    {
        file.open(QIODevice::ReadOnly);
        try
        {
            // load file content to xml document
            QDomDocument xml;
            QString msg;
            int line;
            int column;
            if(xml.setContent(&file, false, &msg, &line, &column))
            {
                const QDomElement& docElem = xml.documentElement();
                const QDomNamedNodeMap& attr = docElem.attributes();
                if(!attr.namedItem("creator").nodeValue().startsWith("QMapShack"))
                {
                    throw 0;
                }
            }
            else
            {
                throw 0;
            }
        }
        catch(int)
        {
            int res = QMessageBox::warning(&CMainWindow::self(),QObject::tr("File exists ...")
                                           ,QObject::tr("The file exists and it has not been created by QMapShack. "
                                                        "If you press 'yes' all data in this file will be lost. "
                                                        "Even if this file contains GPX data and has been loaded by QMapShack, "
                                                        "QMapShack might not be able to load and store all elements of this file.  "
                                                        "Those elements will be lost. I recommend to use another file. "
                                                        "<b>Do you really want to overwrite the file?</b>")
                                           ,QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
            if(res == QMessageBox::No)
            {
                project.umount();
                return false;
            }
        }

        file.close();
    }


    //  ---- start content of gpx
    QDomDocument doc;
    QDomNode gpx = project.writeMetadata(doc);

    IDevice * device = dynamic_cast<IDevice*>(project.parent());
    if(device)
    {
        device->startSavingProject(&project);
    }

    for(int i = 0; i < project.childCount(); i++)
    {
        CGisItemWpt * item = dynamic_cast<CGisItemWpt*>(project.child(i));
        if(item == 0)
        {
            continue;
        }

        /*
            Special care for waypoints stored on Garmin devices. Images attached
            to the waypoint are stored in the file system of the device and written
            as links to the waypoint. Let the device object take care of this.
         */
        if(device)
        {
            device->saveImages(*item);
        }

        item->save(gpx);
    }
    for(int i = 0; i < project.childCount(); i++)
    {
        CGisItemRte * item = dynamic_cast<CGisItemRte*>(project.child(i));
        if(item == 0)
        {
            continue;
        }
        item->save(gpx);
    }
    for(int i = 0; i < project.childCount(); i++)
    {
        CGisItemTrk * item = dynamic_cast<CGisItemTrk*>(project.child(i));
        if(item == 0)
        {
            continue;
        }
        item->save(gpx);
    }

    QDomElement xmlExt = doc.createElement("extensions");
    gpx.appendChild(xmlExt);
    for(int i = 0; i < project.childCount(); i++)
    {
        CGisItemOvlArea * item = dynamic_cast<CGisItemOvlArea*>(project.child(i));
        if(item == 0)
        {
            continue;
        }
        item->save(xmlExt);
    }

    if(!project.getKey().isEmpty())
    {
        QDomElement elem = xmlExt.ownerDocument().createElement("ql:key");
        xmlExt.appendChild(elem);
        QDomText text = xmlExt.ownerDocument().createTextNode(project.getKey());
        elem.appendChild(text);
    }

    {
        QDomElement elem = xmlExt.ownerDocument().createElement("ql:sorting");
        xmlExt.appendChild(elem);
        QDomText text = xmlExt.ownerDocument().createTextNode(QString::number(project.getSorting()));
        elem.appendChild(text);
    }

    {
        QDomElement elem = xmlExt.ownerDocument().createElement("ql:correlation");
        xmlExt.appendChild(elem);
        QDomText text = xmlExt.ownerDocument().createTextNode(QString::number(project.doCorrelation()));
        elem.appendChild(text);
    }

    //  ---- stop  content of gpx

    bool res = true;
    try
    {
        if(!file.open(QIODevice::WriteOnly))
        {
            throw QObject::tr("Failed to create file '%1'").arg(_fn_);
        }
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << endl;;
        out << doc.toString();
        file.close();
        if(file.error() != QFile::NoError)
        {
            throw QObject::tr("Failed to write file '%1'").arg(_fn_);
        }
    }
    catch(const QString& msg)
    {
        QMessageBox::warning(&CMainWindow::self(), QObject::tr("Saveing GIS data failed..."), msg, QMessageBox::Abort);
        res = false;
    }
    project.umount();
    return res;
}

