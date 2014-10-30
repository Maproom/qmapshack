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

#include "gis/gpx/CGpxProject.h"
#include "gis/bin/CBinProject.h"
#include "gis/CGisListWks.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/CGisDraw.h"
#include "helpers/CSettings.h"



#include <QtWidgets>

CGpxProject::CGpxProject(const QString &name,  CGisListWks * parent)
    : IGisProject("", "", parent)
{
    setText(0, name);
    setIcon(0,QIcon("://icons/32x32/GisProject.png"));
    setToolTip(0, getInfo());
    valid = true;
}

CGpxProject::CGpxProject(const QString &filename, const QString& key, CGisListWks *parent)
    : IGisProject(key, filename, parent)
{
    setText(0, QFileInfo(filename).baseName());
    setIcon(0,QIcon("://icons/32x32/GisProject.png"));

    // cerate file instance
    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    // load file content to xml document
    QDomDocument xml;
    QString msg;
    int line;
    int column;
    if(!xml.setContent(&file, false, &msg, &line, &column))
    {
        file.close();
        QMessageBox::critical(0, QObject::tr("Failed to read..."), QObject::tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg), QMessageBox::Abort);
        return;
    }
    file.close();

    int N;
    xmlGpx = xml.documentElement();
    if(xmlGpx.tagName() != "gpx")
    {
        QMessageBox::critical(0, QObject::tr("Failed to read..."), QObject::tr("Not a GPX file: ") + filename, QMessageBox::Abort);
        return;
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
        new CGisItemWpt(xmlWpt, this);
    }

    const QDomElement& xmlExtension = xmlGpx.namedItem("extensions").toElement();
    const QDomNodeList& xmlAreas = xmlExtension.elementsByTagName("ql:area");
    N = xmlAreas.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlArea = xmlAreas.item(n);
        new CGisItemOvlArea(xmlArea, this);
    }

    setToolTip(0, getInfo());

    valid = true;
}

CGpxProject::~CGpxProject()
{

}

void CGpxProject::save()
{
    if(filename.isEmpty())
    {
        saveAs();
    }
    else
    {
        saveAs(filename, *this);
        markAsSaved();
    }
}

void CGpxProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString filter = "*.gpx";
    QString fn = QFileDialog::getSaveFileName(0, QObject::tr("Save GIS data to..."), path, "*.gpx;; *.qms", &filter);

    if(fn.isEmpty())
    {
        return;
    }


    if(filter == "*.gpx")
    {
        saveAs(fn, *this);

        filename = fn;
        setText(0, QFileInfo(filename).baseName());
        markAsSaved();

    }
    else if(filter == "*.qms")
    {
        CBinProject::saveAs(fn, *this);
    }
    else
    {
        return;
    }

    path = QFileInfo(fn).absolutePath();
    cfg.setValue("Paths/lastGisPath", path);
}


void CGpxProject::saveAs(const QString& fn, IGisProject& project)
{
    QString _fn_ = fn;
    QFileInfo fi(_fn_);
    if(fi.suffix() != "gpx")
    {
        _fn_ += ".gpx";
    }

    // todo save gpx
    QFile file(_fn_);

    // safety check for existing files
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
                const  QDomElement& docElem = xml.documentElement();
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
            int res = QMessageBox::warning(0,QObject::tr("File exists ...")
                ,QObject::tr("The file exists and it has not been created by QMapShack. "
                "If you press 'yes' all data in this file will be lost. "
                "Even if this file contains GPX data and has been loaded by QMapShack, "
                "QMapShack might not be able to load and store all elements of this file.  "
                "Those elements will be lost. I recommend to use another file. "
                "<b>Do you really want to overwrite the file?</b>")
                ,QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
            if(res == QMessageBox::No)
            {
                return;
            }
        }

        file.close();
    }

    QDomDocument doc;

    //  ---- start content of gpx
    QDomNode gpx = project.writeMetadata(doc);

    for(int i = 0; i < project.childCount(); i++)
    {
        CGisItemWpt * item = dynamic_cast<CGisItemWpt*>(project.child(i));
        if(item == 0)
        {
            continue;
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


    //  ---- stop  content of gpx

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0, QObject::tr("Saveing GIS data failed..."), QObject::tr("Failed to create file '%1'").arg(_fn_), QMessageBox::Abort);
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << endl;;
    out << doc.toString();
    file.close();
    if(file.error() != QFile::NoError)
    {
        QMessageBox::warning(0, QObject::tr("Saveing GIS data failed..."), QObject::tr("Failed to write file '%1'").arg(_fn_), QMessageBox::Abort);
        return;
    }
}

