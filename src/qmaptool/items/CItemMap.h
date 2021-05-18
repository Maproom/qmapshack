/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CITEMMAP_H
#define CITEMMAP_H

#include "helpers/CGdalFile.h"
#include "items/IItem.h"

#include <QTreeWidgetItem>

class GDALDataset;
class CItemMapLayer;

class CItemMap : public CGdalFile, public IItem, public QTreeWidgetItem
{
public:
    CItemMap(const QString& filename);
    virtual ~CItemMap();

    void setupChanged() override {}

    QPointF getScale() const;

    const QString& getHash() const
    {
        return hash;
    }

    void reload() override;

    void drawBoundingBox(QPainter& p, IDrawContext* dc);

private:
    QString hash;
};

#endif //CITEMMAP_H

