/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#ifndef CPOIPROJECT_H
#define CPOIPROJECT_H


#include "gis/prj/IGisProject.h"

#include <QObject>

class CPoiProject : public QObject, public IGisProject
{
    Q_OBJECT
public:
    CPoiProject(const QString &filename, CGisListWks *parent);
    void updateVisiblePois();
    void openConfigDialog();

    bool skipSave() const override
    {
        return true;
    }
private:
    QList<int> selectedCategories;
    QString connectionName;

    enum SqlColumn_e
    {
        eSqlColumnID,
        eSqlColumnMaxLat,
        eSqlColumnMaxLon,
        eSqlColumnMinLat,
        eSqlColumnMinLon,
        eSqlColumnData,
        eSqlColumnCategory
    };
};

#endif // CPOIPROJECT_H
