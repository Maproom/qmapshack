/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

#ifndef CSLFREADER_H
#define CSLFREADER_H

#include <QTreeWidgetItem>

#include <QColor>
#include <QDateTime>
#include <QDomNode>
#include <QMap>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include "gis/prj/IGisProject.h"

class CGisItemWpt;
class CSlfProject;

class CSlfReader
{
public:
    static void readFile(const QString &file, CSlfProject *proj);

private:
    CSlfReader(const QString &filename, CSlfProject *proj);

    void readMarkers (const QDomNode& xml);
    void readEntries (const QDomNode& xml);
    void readMetadata(const QDomNode& xml, IGisProject::metadata_t& metadata);

    CSlfProject* getProject();

    CSlfProject   *proj = nullptr; /// the resulting project after construction
    const QString &filename;
    QDateTime      baseTime;       /// the time all entries refer to
    QList<qreal>   laps;           /// the distances a new lap starts at (a lap is a .slf segment)
};

#endif // CSLFREADER_H

