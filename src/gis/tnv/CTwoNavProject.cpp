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
#include "gis/CGisListWks.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/tnv/CTwoNavProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSettings.h"

#include <QtWidgets>
#include <proj_api.h>



CTwoNavProject::CTwoNavProject(const QString &filename, IDevice * parent)
    : IGisProject(eTypeTwoNav, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/2NavProject.png"));

    load(filename);

    setupName(QFileInfo(filename).baseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    updateItems();
    valid = true;
}

CTwoNavProject::CTwoNavProject(const QString &filename, const IGisProject * project, IDevice * parent)
    : IGisProject(eTypeTwoNav, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/2NavProject.png"));
    *(IGisProject*)this = *project;

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
    setToolTip(CGisListWks::eColumnName, getInfo());
    updateItems();
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

    try
    {
        QFile fileKey(dir.absoluteFilePath(QString("%1.key").arg(getKey())));
        if(!fileKey.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(&CMainWindow::self(), QObject::tr("Error..."), QObject::tr("Failed to open %1.").arg(fileKey.fileName()), QMessageBox::Abort);
            throw -1;
        }
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
                QString fn = trk->getName();
                fn = fn.remove(QRegExp("[^A-Za-z0-9_]"));
                fn = dir.absoluteFilePath(fn + ".trk");

                if(!trk->saveTwoNav(fn))
                {
                    throw -1;
                }
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
            if(!saveWpts(wpts, dir.absoluteFilePath("waypoints.wpt"), dir))
            {
                throw -1;
            }
        }

        if(!geocaches.isEmpty())
        {
            if(!saveWpts(geocaches, dir.absoluteFilePath("geocaches.wpt"), dir))
            {
                throw -1;
            }
        }
    }
    catch(int)
    {
        res = false;
    }

    if(res)
    {
        markAsSaved();
    }
    umount();
    return res;
}

bool CTwoNavProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString filter = "*.gpx";
    QString fn = QFileDialog::getSaveFileName(&CMainWindow::self(), QObject::tr("Save GIS data to..."), path, "*.gpx;; *.qms", &filter);

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


bool CTwoNavProject::saveWpts(QList<CGisItemWpt*>& wpts, const QString& filename, const QDir& dir)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(&CMainWindow::self(), QObject::tr("Error..."), QObject::tr("Failed to open %1.").arg(filename), QMessageBox::Abort);
        return false;
    }
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    qreal north = -90.0;
    qreal south = 90.0;
    qreal west = 180.0;
    qreal east = -180.0;

    foreach(CGisItemWpt * wpt, wpts)
    {
        QPointF pt = wpt->getPosition();

        if(north < pt.y())
        {
            north = pt.y();
        }
        if(south > pt.y())
        {
            south = pt.y();
        }
        if(west >  pt.x())
        {
            west  = pt.x();
        }
        if(east <  pt.x())
        {
            east  = pt.x();
        }
    }

    out << bom;
    out << "B  UTF-8" << endl;
    out << "G  WGS 84" << endl;
    out << "U  1" << endl;
    out << "z " << west << ", " << south << ", " << east << ", " << north << endl;

    foreach(CGisItemWpt * wpt, wpts)
    {
        wpt->saveTwoNav(out, dir);
    }

    file.close();

    return true;
}



bool CTwoNavProject::load(const QString& filename)
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
            try
            {
                new CGisItemTrk(dir.absoluteFilePath(entry), this);
            }
            catch(int)
            {
                return false;
            }
        }
        else if(fi.suffix().toLower() == "wpt")
        {
            if(!loadWpts(dir.absoluteFilePath(entry), dir))
            {
                return false;
            }
        }
    }

    return true;
}


