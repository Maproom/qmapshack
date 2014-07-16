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

#include "gis/CGisProject.h"
#include "gis/CGisListWks.h"
#include "gis/CGisItemWpt.h"
#include "gis/CGisItemTrk.h"
#include "gis/CGisItemRte.h"
#include "helpers/CSettings.h"
#include "version.h"


#include <QtWidgets>
#include <QtXml>

CGisProject::CGisProject(const QString &filename, const QString& key, CGisListWks *parent)
    : QTreeWidgetItem(parent)
    , key(key)
    , filename(filename)
    , valid(false)
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


    QList<QTreeWidgetItem*> items;


    const QDomNodeList& xmlTrks = xmlGpx.elementsByTagName("trk");
    N = xmlTrks.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlTrk = xmlTrks.item(n);
        QTreeWidgetItem * item = new CGisItemTrk(xmlTrk, this);
        items << item;
    }

    const QDomNodeList& xmlRtes = xmlGpx.elementsByTagName("rte");
    N = xmlRtes.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlRte = xmlRtes.item(n);
        QTreeWidgetItem * item = new CGisItemRte(xmlRte, this);
        items << item;
    }

    const QDomNodeList& xmlWpts = xmlGpx.elementsByTagName("wpt");
    N = xmlWpts.count();
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& xmlWpt = xmlWpts.item(n);
        QTreeWidgetItem * item = new CGisItemWpt(xmlWpt, this);
        items << item;
    }

    addChildren(items);

    valid = true;
}

CGisProject::~CGisProject()
{

}

void CGisProject::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis)
{
    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }
        item->drawItem(p, viewport, blockedAreas, gis);
    }

}

void CGisProject::drawLabel(QPainter& p, const QRectF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis)
{

    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }
        item->drawLabel(p, viewport, blockedAreas, fm, gis);
    }

}

void CGisProject::save()
{
    saveGpx(filename);
}

void CGisProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString fn = QFileDialog::getSaveFileName(0, QObject::tr("Save GIS data to..."), path, "*.gpx");

    if(fn.isEmpty())
    {
        return;
    }
    saveGpx(fn);

    path = QFileInfo(fn).absolutePath();
    cfg.setValue("Paths/lastGisPath", path);
}

void CGisProject::saveGpx(const QString& fn)
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
                if(!attr.namedItem("creator").nodeValue().startsWith("MapRoom"))
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
                ,QObject::tr("The file exists and it has not been created by MapRoom. "
                "If you press 'yes' all data in this file will be lost. "
                "Even if this file contains GPX data and has been loaded by MapRoom, "
                "MapRoom might not be able to load and store all elements of this file.  "
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
    QDomNode gpx = writeMetadata(doc);

    for(int i = 0; i < childCount(); i++)
    {
        CGisItemWpt * item = dynamic_cast<CGisItemWpt*>(child(i));
        if(item == 0)
        {
            continue;
        }
        item->save(gpx);
    }

    //  ---- stop  content of gpx

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0, QObject::tr("Save GIS data failed..."), QObject::tr("Failed to create file '%1'").arg(_fn_), QMessageBox::Abort);
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << endl;;
    out << doc.toString();
    file.close();
    if(file.error() != QFile::NoError)
    {
        QMessageBox::warning(0, QObject::tr("Save GIS data failed..."), QObject::tr("Failed to write file '%1'").arg(_fn_), QMessageBox::Abort);
        return;
    }


    filename = _fn_;
    setText(0, QFileInfo(filename).baseName());
}

const QString CGisProject::gpx_ns      = "http://www.topografix.com/GPX/1/1";
const QString CGisProject::xsi_ns      = "http://www.w3.org/2001/XMLSchema-instance";
const QString CGisProject::gpxx_ns     = "http://www.garmin.com/xmlschemas/GpxExtensions/v3";
const QString CGisProject::gpxtpx_ns   = "http://www.garmin.com/xmlschemas/TrackPointExtension/v1";
const QString CGisProject::wptx1_ns    = "http://www.garmin.com/xmlschemas/WaypointExtension/v1";
const QString CGisProject::rmc_ns      = "urn:net:trekbuddy:1.0:nmea:rmc";
const QString CGisProject::ql_ns       = "http://www.qlandkarte.org/xmlschemas/v1.1";
const QString CGisProject::gs_ns       = "http://www.groundspeak.com/cache/1/0";

QDomNode CGisProject::writeMetadata(QDomDocument& doc)
{
    QDomElement gpx = doc.createElement("gpx");
    doc.appendChild(gpx);

    gpx.setAttribute("version","1.1");
    gpx.setAttribute("creator","MapRoom " VER_STR " http://www.qlandkarte.org/");
    gpx.setAttribute("xmlns",gpx_ns);
    gpx.setAttribute("xmlns:xsi",xsi_ns);
    gpx.setAttribute("xmlns:gpxx",gpxx_ns);
    gpx.setAttribute("xmlns:gpxtpx",gpxtpx_ns);
    gpx.setAttribute("xmlns:wptx1",wptx1_ns);
    gpx.setAttribute("xmlns:rmc",rmc_ns);
    gpx.setAttribute("xmlns:ql",ql_ns);

    QString schemaLocation = QString()
        + gpx_ns    + " http://www.topografix.com/GPX/1/1/gpx.xsd "
        + gpxx_ns   + " http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd "
        + gpxtpx_ns + " http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd "
        + wptx1_ns  + " http://www.garmin.com/xmlschemas/WaypointExtensionv1.xsd"
        + ql_ns     + " http://www.qlandkarte.org/xmlschemas/v1.1/ql-extensions.xsd";

    gpx.setAttribute("xsi:schemaLocation", schemaLocation);

    QDomElement metadata = doc.createElement("metadata");
    gpx.appendChild(metadata);

    QDomElement time = doc.createElement("time");
    metadata.appendChild(time);
    QDomText _time_ = doc.createTextNode(QDateTime::currentDateTime().toUTC().toString("yyyy-MM-dd'T'hh:mm:ss'Z'"));
    time.appendChild(_time_);

    return gpx;
}
