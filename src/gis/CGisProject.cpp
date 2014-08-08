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
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/CGisDraw.h"
#include "helpers/CSettings.h"
#include "version.h"


#include <QtWidgets>
#include <QtXml>

const QString CGisProject::gpx_ns      = "http://www.topografix.com/GPX/1/1";
const QString CGisProject::xsi_ns      = "http://www.w3.org/2001/XMLSchema-instance";
const QString CGisProject::gpxx_ns     = "http://www.garmin.com/xmlschemas/GpxExtensions/v3";
const QString CGisProject::gpxtpx_ns   = "http://www.garmin.com/xmlschemas/TrackPointExtension/v1";
const QString CGisProject::wptx1_ns    = "http://www.garmin.com/xmlschemas/WaypointExtension/v1";
const QString CGisProject::rmc_ns      = "urn:net:trekbuddy:1.0:nmea:rmc";
const QString CGisProject::ql_ns       = "http://www.qlandkarte.org/xmlschemas/v1.1";
const QString CGisProject::gs_ns       = "http://www.groundspeak.com/cache/1/0";


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

    const QDomNode& xmlMetadata = xmlGpx.namedItem("metadata");
    if(xmlMetadata.isElement())
    {
        readMetadata(xmlMetadata, metadata);
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
    setToolTip(0, getInfo());

    valid = true;
}

CGisProject::~CGisProject()
{

}

QString CGisProject::getInfo()
{
    QString str = metadata.name.isEmpty() ? text(0) : metadata.name;

    if(metadata.time.isValid())
    {
        str += "\n";
        str += IUnit::datetime2string(metadata.time);
    }


    if(!metadata.desc.isEmpty())
    {
        str += "\n";

        if(metadata.desc.count() < 200)
        {
            str += metadata.desc;
        }
        else
        {
            str += metadata.desc.left(197) + "...";
        }
    }


    return str;
}

void CGisProject::readMetadata(const QDomNode& xml, metadata_t& metadata)
{

    IGisItem::readXml(xml,"name", metadata.name);
    IGisItem::readXml(xml,"desc", metadata.desc);

    const QDomNode& xmlAuthor = xml.namedItem("author");
    if(xmlAuthor.isElement())
    {
        IGisItem::readXml(xml,"name", metadata.author.name);

        const QDomNode& xmlEmail = xmlAuthor.namedItem("email");
        if(xmlEmail.isElement())
        {
            const QDomNamedNodeMap& attr = xmlEmail.attributes();
            metadata.author.id = attr.namedItem("id").nodeValue();
            metadata.author.domain = attr.namedItem("domain").nodeValue();
        }

        const QDomNode& xmlLink = xmlAuthor.namedItem("link");
        if(xmlLink.isElement())
        {
            metadata.author.link.uri.setUrl(xmlLink.attributes().namedItem("href").nodeValue());
            IGisItem::readXml(xmlLink, "text", metadata.author.link.text);
            IGisItem::readXml(xmlLink, "type", metadata.author.link.type);
        }
    }

    const QDomNode& xmlCopyright = xml.namedItem("copyright");
    if(xmlCopyright.isElement())
    {
        metadata.copyright.author = xmlCopyright.attributes().namedItem("author").nodeValue();
        IGisItem::readXml(xmlCopyright, "year", metadata.copyright.year);
        IGisItem::readXml(xmlCopyright, "license", metadata.copyright.license);
    }

    IGisItem::readXml(xml,"link", metadata.links);
    IGisItem::readXml(xml,"time", metadata.time);
    IGisItem::readXml(xml,"keywords", metadata.keywords);

    const QDomNode& xmlBounds = xml.namedItem("bounds");
    if(xmlBounds.isElement())
    {
        const QDomNamedNodeMap& attr = xmlBounds.attributes();
        metadata.bounds.setLeft(attr.namedItem("minlon").nodeValue().toDouble());
        metadata.bounds.setTop(attr.namedItem("maxlat").nodeValue().toDouble());
        metadata.bounds.setRight(attr.namedItem("maxlon").nodeValue().toDouble());
        metadata.bounds.setBottom(attr.namedItem("minlat").nodeValue().toDouble());
    }
}

QDomNode CGisProject::writeMetadata(QDomDocument& doc)
{
    QDomElement gpx = doc.createElement("gpx");
    doc.appendChild(gpx);

    gpx.setAttribute("version","1.1");
    gpx.setAttribute("creator","QMapShack " VER_STR " http://www.qlandkarte.org/");
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

    QDomElement xmlMetadata = doc.createElement("metadata");
    gpx.appendChild(xmlMetadata);

    IGisItem::writeXml(xmlMetadata,"name", metadata.name);
    IGisItem::writeXml(xmlMetadata,"desc", metadata.desc);

    if(!metadata.author.name.isEmpty())
    {
        QDomElement xmlAuthor = doc.createElement("author");
        xmlMetadata.appendChild(xmlAuthor);

        IGisItem::writeXml(xmlAuthor,"name", metadata.author.name);

        if(!metadata.author.id.isEmpty() && !metadata.author.domain.isEmpty())
        {
            QDomElement xmlEmail = doc.createElement("email");
            xmlAuthor.appendChild(xmlEmail);
            xmlEmail.setAttribute("id", metadata.author.id);
            xmlEmail.setAttribute("domain", metadata.author.domain);
        }

        if(metadata.author.link.uri.isValid())
        {
            QDomElement xmlLink = doc.createElement("link");
            xmlAuthor.appendChild(xmlLink);

            xmlLink.setAttribute("href", metadata.author.link.uri.toString());
            IGisItem::writeXml(xmlLink, "text", metadata.author.link.text);
            IGisItem::writeXml(xmlLink, "type", metadata.author.link.type);

        }
    }

    if(!metadata.copyright.author.isEmpty())
    {
        QDomElement xmlCopyright = doc.createElement("copyright");
        xmlMetadata.appendChild(xmlCopyright);

        xmlCopyright.setAttribute("author", metadata.copyright.author);
        IGisItem::writeXml(xmlCopyright, "year", metadata.copyright.year);
        IGisItem::writeXml(xmlCopyright, "license", metadata.copyright.license);

    }
    IGisItem::writeXml(xmlMetadata, "link", metadata.links);
    IGisItem::writeXml(xmlMetadata, "time", metadata.time);
    IGisItem::writeXml(xmlMetadata, "keywords", metadata.keywords);

    if(metadata.bounds.isValid())
    {
        QDomElement xmlBounds = doc.createElement("bounds");
        xmlMetadata.appendChild(xmlBounds);

        xmlBounds.setAttribute("minlat", metadata.bounds.bottom());
        xmlBounds.setAttribute("minlon", metadata.bounds.left());
        xmlBounds.setAttribute("maxlat", metadata.bounds.top());
        xmlBounds.setAttribute("maxlon", metadata.bounds.right());

    }

    return gpx;
}


void CGisProject::getItemByPos(const QPointF& pos, QList<IGisItem *> &items)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->isCloseTo(pos))
        {
            items << item;
        }
    }
}

IGisItem * CGisProject::getItemByKey(const QString& key)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            return item;
        }
    }
    return 0;
}

void CGisProject::delItemByKey(const QString& key)
{
    QList<QTreeWidgetItem*> items;
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            QString msg = QObject::tr("Are you sure you want to delete '%1' from project '%2'?").arg(item->getName()).arg(text(0));
            QMessageBox::StandardButtons res = QMessageBox::question(0, QObject::tr("Delete..."), msg, QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
            if(res != QMessageBox::Ok)
            {
                continue;
            }

            items << takeChild(i-1);
        }
    }
    qDeleteAll(items);
}

void CGisProject::editItemByKey(const QString& key)
{
    QList<QTreeWidgetItem*> items;
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            item->edit();
        }
    }
}

void CGisProject::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, CGisDraw * gis)
{
    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(seenKeys.contains(item->getKey()))
        {
            continue;
        }
        seenKeys << item->getKey();

        item->drawItem(p, viewport, blockedAreas, gis);
    }

}

void CGisProject::drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, const QFontMetricsF& fm, CGisDraw * gis)
{

    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(seenKeys.contains(item->getKey()))
        {
            continue;
        }
        seenKeys << item->getKey();

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
    for(int i = 0; i < childCount(); i++)
    {
        CGisItemRte * item = dynamic_cast<CGisItemRte*>(child(i));
        if(item == 0)
        {
            continue;
        }
        item->save(gpx);
    }
    for(int i = 0; i < childCount(); i++)
    {
        CGisItemTrk * item = dynamic_cast<CGisItemTrk*>(child(i));
        if(item == 0)
        {
            continue;
        }
        item->save(gpx);
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


    filename = _fn_;
    setText(0, QFileInfo(filename).baseName());
}

