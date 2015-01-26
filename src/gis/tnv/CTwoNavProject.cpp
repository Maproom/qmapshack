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
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSettings.h"

#include <QtWidgets>



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
            QString fn = getName();
            fn = fn.remove(QRegExp("[^A-Za-z0-9_]"));
            fn = dir.absoluteFilePath(fn + ".trk");

            trk->saveTwoNav(fn);
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
    foreach(const QString &entry, entries)
    {
        QFileInfo fi(entry);

        if(fi.suffix().toLower() == "key")
        {
            key = fi.baseName();
            break;
        }
    }

    foreach(const QString &entry, entries)
    {
        QFileInfo fi(entry);

        if(fi.suffix().toLower() == "trk")
        {
            new CGisItemTrk(dir.absoluteFilePath(entry), this);
        }
        else if(fi.suffix().toLower() == "wpt")
        {
        }
    }
}




