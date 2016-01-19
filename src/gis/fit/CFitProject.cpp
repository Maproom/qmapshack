/**********************************************************************************************
    Copyright (C) 2015 Ivo Kronenberg

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
#include "gis/fit/CFitProject.h"
#include "gis/fit/CFitStream.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"

#include <QtWidgets>


CFitProject::CFitProject(const QString &filename, CGisListWks *parent)
    : IGisProject(eTypeFit, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/FitProject.png"));
    blockUpdateItems(true);
    try
    {
        loadFit(filename);
    }
    catch(QString &errormsg)
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                              QObject::tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
        valid = false;
    }
    blockUpdateItems(false);
}

CFitProject::CFitProject(const QString &filename, IDevice *parent)
    : IGisProject(eTypeFit, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/FitProject.png"));
    blockUpdateItems(true);
    try
    {
        loadFit(filename);
    }
    catch(QString &errormsg)
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                              QObject::tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
        valid = false;
    }
    blockUpdateItems(false);
}

void CFitProject::loadFit(const QString & filename)
{
    // create file instance
    QFile file(filename);
    qDebug() << "reading FIT file" << filename;

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
        throw QObject::tr("Failed to open FIT file %1.").arg(filename);
    }

    CFitStream in(file);
    try
    {
        in.decodeFile();
    }
    catch(QString& errormsg)
    {
        file.close();
        throw errormsg;
    }
    file.close();

    const CFitMessage& mesg = in.firstMesgOf(eMesgNumFileId);
    if(mesg.getFieldValue(eFileIdType).toUInt() == eFileActivity || mesg.getFieldValue(eFileIdType).toUInt() == eFileCourse)
    {
        new CGisItemTrk(in, this);
    }
    // fit does not have routes
    // new CGisItemRte(in, this);

    in.reset();
    while(in.nextMesgOf(eMesgNumCoursePoint).isValid())
    {
        new CGisItemWpt(in, this);
    }
    // ql:area is not directly available in FIT (could be calculated)

    markAsSaved();

    setupName(QFileInfo(filename).baseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
}

CFitProject::~CFitProject()
{
}

