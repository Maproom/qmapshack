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

#include "CBinProject.h"

#include <QtWidgets>

CBinProject::CBinProject(const QString &filename, const QString &key, CGisListWks *parent)
    : IGisProject(key, filename, parent)
{
    setText(0, QFileInfo(filename).baseName());
    setIcon(0,QIcon("://icons/32x32/GisProject.png"));

    QFile file(filename);
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

    setToolTip(0, getInfo());
    valid = true;
}

CBinProject::~CBinProject()
{

}

void CBinProject::saveAs(const QString& filename, IGisProject& project)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(0, QObject::tr("Failed to open..."), QObject::tr("Failed to open %1").arg(filename), QMessageBox::Abort);
        return;
    }
    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);   

    project >> out;

    file.close();

}

void CBinProject::save()
{

}

void CBinProject::saveAs()
{

}
