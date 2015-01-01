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

#include "gis/qms/CQmsProject.h"
#include "gis/gpx/CGpxProject.h"
#include "helpers/CSettings.h"
#include "gis/CGisListWks.h"


#include <QtWidgets>


CQmsProject::CQmsProject(const QString &filename, CGisListWks *parent)
    : IGisProject(eTypeQms, filename, parent)
{   
    setIcon(CGisListWks::eColumnName,QIcon("://icons/32x32/QmsProject.png"));

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
        QMessageBox::critical(0, QObject::tr("Failed to open..."), QObject::tr("Failed to open %1").arg(filename), QMessageBox::Abort);
        return;
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);
    *this << in;    
    file.close();

    markAsSaved();

    setupName(QFileInfo(filename).baseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
}

CQmsProject::~CQmsProject()
{

}


bool CQmsProject::save()
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

bool CQmsProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString filter = "*.qms";
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
        filename = fn;
        metadata.name.clear();
        setupName(QFileInfo(filename).baseName().replace("_", " "));

        res = saveAs(fn, *this);
        if(res)
        {
            markAsSaved();
        }
    }
    else
    {
        return false;
    }

    path = QFileInfo(fn).absolutePath();
    cfg.setValue("Paths/lastGisPath", path);

    return res;
}

bool CQmsProject::saveAs(const QString& fn, IGisProject& project)
{
    QString _fn_ = fn;
    QFileInfo fi(_fn_);
    if(fi.suffix() != "qms")
    {
        _fn_ += ".qms";
    }

    // todo save qms
    QFile file(_fn_);

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(0, QObject::tr("Failed to open..."), QObject::tr("Failed to open %1").arg(_fn_), QMessageBox::Abort);
        return false;
    }
    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    QString tmp = project.getFilename();
    project.setFilename(_fn_);

    project.IGisProject::operator>>(out);

    project.setFilename(tmp);

    file.close();
}

