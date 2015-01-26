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

#include "gis/CGisListWks.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/tnv/CTwoNavProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"
#include "helpers/CSelectCopyAction.h"

#include <QtWidgets>

QStringList writeCompeTime( const QDateTime& t, bool isTrack)
{
    QStringList result;
    QString dateFormat;
    QString monthStr;

    QDateTime timestamp = t.toTimeSpec(Qt::UTC);

    switch(timestamp.date().month())
    {
        case 1:
            monthStr = "Jan";
            break;
        case 2:
            monthStr = "Feb";
            break;
        case 3:
            monthStr = "Mar";
            break;
        case 4:
            monthStr = "Apr";
            break;
        case 5:
            monthStr = "May";
            break;
        case 6:
            monthStr = "Jun";
            break;
        case 7:
            monthStr = "Jul";
            break;
        case 8:
            monthStr = "Aug";
            break;
        case 9:
            monthStr = "Sep";
            break;
        case 10:
            monthStr = "Oct";
            break;
        case 11:
            monthStr = "Nov";
            break;
        case 12:
            monthStr = "Dec";
            break;
    }

    if(isTrack)
    {
        dateFormat = QString("dd-'%1'-yy").arg(monthStr);
    }
    else
    {
        dateFormat = QString("dd-'%1'-yyyy").arg(monthStr);
    }

    result << timestamp.toString(dateFormat);

    if(isTrack)
    {
        result << timestamp.toString("hh:mm:ss.000");
    }
    else
    {
        result << timestamp.toString("hh:mm:ss");
    }

    return result;
}


CTwoNavProject::CTwoNavProject(const QString &filename, IDevice * parent)
    : IGisProject(eTypeTwoNav, filename, parent)
{
    setIcon(CGisListWks::eColumnName,QIcon("://icons/32x32/2NavProject.png"));

    load(filename);

    setupName(QFileInfo(filename).baseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
}

CTwoNavProject::CTwoNavProject(const QString &filename, const IGisProject * project, IDevice * parent)
    : IGisProject(eTypeTwoNav, filename, parent)
{
    setIcon(CGisListWks::eColumnName,QIcon("://icons/32x32/2NavProject.png"));
    *(IGisProject*)this = *project;

    const int N = project->childCount();
    for(int n = 0; n < N; n++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(project->child(n));
        if(item)
        {
            int res = CSelectCopyAction::eResultNone;
            insertCopyOfItem(item, -1, res);
        }
    }


    setupName(QFileInfo(filename).baseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
}

CTwoNavProject::~CTwoNavProject()
{
}

bool CTwoNavProject::save()
{
    bool res = true;
    mount();
    QDir().mkpath(filename);
    QDir dir(filename);

    QFile fileKey(dir.absoluteFilePath(QString("%1.key").arg(getKey())));
    fileKey.open(QIODevice::WriteOnly);
    fileKey.close();

    QList<CGisItemWpt*> wpts;
    QList<CGisItemWpt*> geocaches;
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        QTreeWidgetItem * item = child(n);
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(item);
        if(trk)
        {
            saveTrk(trk, dir);
        }
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(item);
        if(wpt)
        {
            if(wpt->isGeocache())
            {
                geocaches << wpt;
            }
            else
            {
                wpts << wpt;
            }
        }
    }

    if(!wpts.isEmpty())
    {
        saveWpts(wpts, dir);
    }
    if(!geocaches.isEmpty())
    {
        saveGeoCaches(geocaches, dir);
    }

    umount();
    return res;
}

bool CTwoNavProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString filter = "*.gpx";
    QString fn = QFileDialog::getSaveFileName(0, QObject::tr("Save GIS data to..."), path, "*.gpx;; *.qms", &filter);

    if(fn.isEmpty())
    {
        return false;
    }


    bool res = false;
    if(filter == "*.gpx")
    {
        res = CGpxProject::saveAs(fn, *this);
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

void CTwoNavProject::saveTrk(CGisItemTrk * trk, const QDir &dir)
{
    QString filename = trk->getName();
    filename = filename.remove(QRegExp("[^A-Za-z0-9_]"));
    filename = dir.absoluteFilePath(filename + ".trk");

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    out << "B  UTF-8" << endl;
    out << "G  WGS 84" << endl;
    out << "U  1" << endl;

    QString name = trk->getName();
    name = name.replace(" ","_");

    QColor color = trk->getColor();

    QStringList list;
    list << "C";
    list << QString::number(color.red());
    list << QString::number(color.green());
    list << QString::number(color.blue());
    list << "5";                 // ???
    list << "1";                 // ???
    out << list.join(" ") << endl;

    out << "s " << name << endl;
    out << "k " << trk->getKey().item << endl;

    const CGisItemTrk::trk_t& tTrk = trk->getTrackData();
    foreach(const CGisItemTrk::trkseg_t& seg, tTrk.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
        {
            list.clear();

            list << "T";
            list << "A";
            list << (trkpt.lat > 0 ? QString("%1%2N") : QString("%1%2S")).arg(trkpt.lat,0,'f').arg(QChar(0272));
            list << (trkpt.lon > 0 ? QString("%1%2E") : QString("%1%2W")).arg(trkpt.lon,0,'f').arg(QChar(0272));
            list << writeCompeTime(trkpt.time, true);
            list << "s";
            list << QString("%1").arg(trkpt.ele == NOINT ? 0 : trkpt.ele);
            list << "0.000000";
            list << "0.000000";
            list << "0.000000";
            list << "0";
            list << "-1000.000000";
            list << "-1.000000";
            list << "-1";
            list << "-1.000000";
            list << "-1";
            list << "-1";
            list << "-1";
            list << "-1.000000";

            out << list.join(" ") << endl;
        }
    }

}

void CTwoNavProject::saveWpts(QList<CGisItemWpt*>& wpts, const QDir& dir)
{

}

void CTwoNavProject::saveGeoCaches(QList<CGisItemWpt*>& wpts, const QDir& dir)
{

}


void CTwoNavProject::load(const QString& filename)
{
    QDir dir(filename);

    QStringList entries = dir.entryList(QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files);
    foreach(const QString& entry, entries)
    {
        QFileInfo fi(entry);

        if(fi.suffix() == "key")
        {
            key = fi.baseName();
        }
    }


}
