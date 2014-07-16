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
    // todo save gpx


    filename = fn;
    setText(0, QFileInfo(filename).baseName());
}
