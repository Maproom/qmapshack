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

#include "gis/fit/CFitProject.h"
#include "gis/fit/CFitStream.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"
#include "gis/CGisListWks.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"
#include "CMainWindow.h"

#include <QtWidgets>
#include <helpers/CSettings.h>
#include <gis/gpx/CGpxProject.h>
#include <gis/qms/CQmsProject.h>


CFitProject::CFitProject(const QString &filename, CGisListWks *parent)
    : IGisProject(eTypeFit, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/FitProject.png"));
    blockUpdateItems(true);
    loadFit(filename);
    blockUpdateItems(false);
}


CFitProject::CFitProject(const QString &filename, IDevice *parent)
: IGisProject(eTypeFit, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/FitProject.png"));
    blockUpdateItems(true);
    loadFit(filename);
    blockUpdateItems(false);
}

void CFitProject::loadFit(const QString & filename)
{
    // create file instance
    QFile file(filename);
    qDebug() << "FIT file " << filename;
    
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
    
    CFitStream in(file);
        if(in.decodeFile())
        {
            const CFitMessage& mesg = in.firstMesgOf(MesgNumFileId);
            qDebug() << mesg.messageInfo();
            qDebug() << mesg.getFieldUIntValue(FileIdType);

            if(mesg.getFieldUIntValue(FileIdType) == FileActivity || mesg.getFieldUIntValue(FileIdType) == FileCourse)
            {
                new CGisItemTrk(in, this);
            }
            in.reset();
            while(in.nextMesgOf(MesgNumCoursePoint).getGlobalMesgNr() != GlobalMesgNrInvalid)
            {
                new CGisItemWpt(in, this);
            }
             // TODO other types
        }
        else
        {
            qWarning() << "FIT decoding error for "<< filename;
            QMessageBox::critical(&CMainWindow::self(), QObject::tr("Failed to open..."), QObject::tr("Failed to open %1").arg(filename), QMessageBox::Abort);
        }
    
    file.close();
    
    markAsSaved();
    
    setupName(QFileInfo(filename).baseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
}

CFitProject::~CFitProject()
{

}


bool CFitProject::save()
{
    // saving fit file is not supported
    return false;
}

bool CFitProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString filter = filedialogFilterGPX;
    QString fn = QFileDialog::getSaveFileName(CMainWindow::getBestWidgetForParent(), QObject::tr("Save GIS data to..."), path, filedialogSaveFilters, &filter);

    if(fn.isEmpty())
    {
        return false;
    }

    bool res = false;
    if(filter == filedialogFilterGPX)
    {
        res = CGpxProject::saveAs(fn, *this);
    }
    else if(filter == filedialogFilterQMS)
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

bool CFitProject::saveAs(const QString& fn, IGisProject& project)
{
    // not implemented
    return false;
}
