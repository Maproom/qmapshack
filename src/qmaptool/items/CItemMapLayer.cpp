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

#include "canvas/CDrawContextProj.h"
#include "CMainWindow.h"
#include "items/CItemMap.h"
#include "items/CItemMapLayer.h"
#include "items/CItemTreeWidget.h"

#include <QtWidgets>

CItemMapLayer::CItemMapLayer(QTreeWidget* parent)
    : IItem("")
    , QTreeWidgetItem(parent)
    , vrt(QDir::temp().absoluteFilePath("QMapTool_XXXXXX.vrt"))
{
    drawContext = new CDrawContextProj(CMainWindow::self().getCanvas(), parent);

    setText(CItemTreeWidget::eColumnName, tr("Layer"));
    setIcon(CItemTreeWidget::eColumnName, QIcon("://icons/32x32/MapLayer.png"));

    // this is needed to create a filename
    vrt.open();
    vrt.close();
}


QString CItemMapLayer::getProjection() const
{
    return drawContext->getProjection();
}

bool CItemMapLayer::addMap(CItemMap* map)
{
    const QPointF& mapScale = map->getScale();

    const QTransform& trFwd = drawContext->getTrFwd();

    if(trFwd.isScaling())
    {
        if((qAbs((mapScale.x() - trFwd.m11()) / trFwd.m11()) > 0.2) || (qAbs((mapScale.y() - trFwd.m22()) / trFwd.m22()) > 0.2))
        {
            return false;
        }
    }

    addChild(map);
    updateLayer();

    return drawContext->getIsValid();
}

void CItemMapLayer::updateLayer()
{
    drawContext->unload();
    setToolTip(CItemTreeWidget::eColumnName, "");

    const int N = childCount();
    if(N == 0)
    {
        return;
    }

    QStringList args;
    args << vrt.fileName();

    for(int n = 0; n < N; n++)
    {
        CItemMap* map = dynamic_cast<CItemMap*>(child(n));
        if(map != nullptr)
        {
            args << map->getFilename();
        }
    }

    QProcess proc;
    proc.start(IAppSetup::self().getGdalbuildvrt(), args);
    proc.waitForStarted();
    proc.waitForFinished();

    drawContext->setSourceFile(vrt.fileName(), true);
    setToolTip(CItemTreeWidget::eColumnName, drawContext->getInfo());
}

bool CItemMapLayer::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    drawContext->draw(p, needsRedraw);
    return true;
}

void CItemMapLayer::drawBoundingBoxes(QPainter& p, IDrawContext* dc)
{
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        CItemMap* map = dynamic_cast<CItemMap*>(child(n));
        if(map == nullptr)
        {
            continue;
        }

        map->drawBoundingBox(p, dc);
    }
}
