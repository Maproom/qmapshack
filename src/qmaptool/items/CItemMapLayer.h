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

#ifndef CITEMMAPLAYER_H
#define CITEMMAPLAYER_H

#include "items/IItem.h"

#include <QCoreApplication>
#include <QTemporaryFile>
#include <QTreeWidgetItem>

class CItemMap;
class CDrawContextProj;

class CItemMapLayer : public IItem, public QTreeWidgetItem
{
    Q_OBJECT
public:
    CItemMapLayer(QTreeWidget * parent);
    virtual ~CItemMapLayer() = default;

    bool addMap(CItemMap * map);

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) override;

    void drawBoundingBoxes(QPainter& p, IDrawContext *dc);

    void setupChanged() override {}

    const QString& getProjection() const;

protected:
    friend bool sortByScale(QTreeWidgetItem * item1, QTreeWidgetItem * item2);

    void updateLayer();

    QTemporaryFile vrt;
};

#endif //CITEMMAPLAYER_H

