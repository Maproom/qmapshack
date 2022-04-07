/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CDraw.h"
#include "helpers/CTryMutexLocker.h"
#include "poi/CPoiCategory.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiPropSetup.h"
#include "poi/IPoiFile.h"

constexpr int iconsize = 22;

QSize IPoiFile::_iconSize = {iconsize, iconsize};
QImage IPoiFile::_iconHighlight;

void IPoiFile::init()
{
    // default sizes are for iconsize 22.
    qreal sx = qreal(_iconSize.width()) * 42.0 / 22.0;
    qreal sy = qreal(_iconSize.height()) * 42.0 / 22.0;
    _iconHighlight = QImage("://cursors/poiHighlightRed.png").scaled(sx, sy, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

IPoiFile::IPoiFile(CPoiDraw* parent)
    : IDrawObject(parent)
    , poi(parent)
    , loadTimer(new QTimer(this))

{
    //Set up a timer to only start reloading and redrawing the map when the user has made his selections
    loadTimer->setSingleShot(true);
    loadTimer->setInterval(500);
    connect(loadTimer, &QTimer::timeout, poi, &CPoiDraw::emitSigCanvasUpdate);
}

IPoiFile::~IPoiFile()
{
    for(const CPoiItem* poi : loadedPois)
    {
        delete poi;
    }
}

IPoiProp* IPoiFile::getSetup()
{
    if(setup.isNull())
    {
        setup = new CPoiPropSetup(this, poi);
    }

    return setup;
}
void IPoiFile::slotCheckedStateChanged(QTreeWidgetItem* item)
{
    QMutexLocker lock(&mutex);

    CPoiCategory* categoryItem = static_cast<CPoiCategory*>(item);
    if(categoryItem == nullptr)
    {
        return;
    }

    categoryActivated[categoryItem->getId()] = categoryItem->checkState();

    loadTimer->start();
}
void IPoiFile::draw(IDrawContext::buffer_t& buf)
{
    // !!!! NOTE !!!!
    // This is running in it's own thread, not the main thread.
    // Use CPoiPOI::mutex whenever you access data shared between
    // the main thread and this thread. Do not block it more than
    // neccessary.


    // do that on a regular base while you draw the buffer
    // to abort drawing ass soon as possible if there is
    // another draw request pending
    if(poi->needsRedraw())
    {
        return;
    }

    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p, true);
    p.setOpacity(getOpacity() / 100.0);

    QPointF pp = buf.ref1;
    poi->convertRad2Px(pp);
    p.translate(-pp);
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);

    // calculate maximum viewport
    qreal xMin = qMin(buf.ref1.x(), buf.ref4.x());
    qreal xMax = qMax(buf.ref2.x(), buf.ref3.x());
    qreal yMin = qMin(buf.ref3.y(), buf.ref4.y());
    qreal yMax = qMax(buf.ref1.y(), buf.ref2.y());

    if(xMin < -180.0 * DEG_TO_RAD)
    {
        xMin = -180 * DEG_TO_RAD;
    }
    if(xMax > 180.0 * DEG_TO_RAD)
    {
        xMax = 180 * DEG_TO_RAD;
    }

    // draw POI
    QMutexLocker lock(&mutex);
    displayedPois.clear();
    QRectF freeSpaceRect (QPointF(), IPoiFile::iconSize() * 2);
    //Find POIs in view
    const QList<quint64>& keys = categoryActivated.keys();
    for(quint64 categoryID : keys)
    {
        if(categoryActivated[categoryID] != Qt::Checked)
        {
            continue;
        }
        for(int minLonM10 = qFloor(xMin * RAD_TO_DEG * 10); minLonM10 < qCeil(xMax * RAD_TO_DEG * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(yMin * RAD_TO_DEG * 10); minLatM10 < qCeil(yMax * RAD_TO_DEG * 10); minLatM10++)
            {
                if(!loadedPoisByArea.contains(categoryID) ||
                   !loadedPoisByArea[categoryID].contains(minLonM10) ||
                   !loadedPoisByArea[categoryID][minLonM10].contains(minLatM10))
                {
                    loadPOIsFromFile(categoryID, minLonM10, minLatM10);
                }
                if(poi->needsRedraw())
                {
                    return;
                }
                for(quint64 poiToDrawID : qAsConst(loadedPoisByArea)[categoryID][minLonM10][minLatM10])
                {
                    const CPoiItem* poiToDraw = loadedPois[poiToDrawID];
                    QPointF pt = poiToDraw->getPos();
                    poi->convertRad2Px(pt);

                    freeSpaceRect.moveCenter(pt);

                    bool foundIntersection = false;
                    for(poiGroup_t& poiGroup : displayedPois)
                    {
                        if(poiGroup.iconLocation.intersects(freeSpaceRect))
                        {
                            foundIntersection = true;
                            poiGroup.pois.insert(poiToDrawID);
                            break;
                        }
                    }

                    if(!foundIntersection)
                    {
                        poiGroup_t poiGroup;
                        QRectF iconRect (QPointF(), IPoiFile::iconSize());
                        iconRect.moveCenter(pt);
                        poiGroup.iconLocation = iconRect;
                        poiGroup.iconCenter = poiToDraw->getPos();
                        poiGroup.pois.insert(poiToDrawID);
                        displayedPois.append(poiGroup);
                    }
                }
            }
        }
    }

    //Draw Icons
    for(const poiGroup_t& poiGroup : qAsConst(displayedPois))
    {
        QFontMetricsF fm(CMainWindow::self().getMapFont());

        QPixmap icon;
        getPoiIcon(icon, poiGroup);
        icon = icon.scaled(IPoiFile::iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        const QRectF& iconLocation = poiGroup.iconLocation;

        p.drawRect(iconLocation.adjusted(-1, -1, 1, 1));
        p.drawPixmap(iconLocation.topLeft(), icon);
        p.save();
        if(poiGroup.pois.count() > 1)
        {
            const QString& text = QString::number(poiGroup.pois.count());

            QRectF labelRect = fm.boundingRect(text);
            labelRect.moveCenter(iconLocation.bottomRight());
            p.drawRect(labelRect.adjusted(-1, -1, 1, 1));
            CDraw::text(text, p, labelRect.toRect(), Qt::darkBlue);
        }
        else if(CMainWindow::self().isPoiText())
        {
            //Draw Name
            const QString& name = loadedPois[*poiGroup.pois.begin()]->getName();
            QRectF rect = fm.boundingRect(name);
            rect.adjust(-2, -2, 2, 2);

            bool noLocationFound = false;
            // place label on bottom
            rect.moveCenter(iconLocation.center());
            rect.moveTop(iconLocation.bottom());
            if(overlapsWithIcon(rect))
            {
                // place label on top
                rect.moveBottom(iconLocation.top());
                if(overlapsWithIcon(rect))
                {
                    // place label on right
                    rect.moveCenter(iconLocation.center());
                    rect.moveLeft(iconLocation.right());
                    if(overlapsWithIcon(rect))
                    {
                        // place label on left
                        rect.moveRight(iconLocation.left());
                        if(overlapsWithIcon(rect))
                        {
                            // failed to place label anywhere
                            noLocationFound = true;
                        }
                    }
                }
            }
            if(!noLocationFound)
            {
                CDraw::text(name, p, rect.toRect(), Qt::darkBlue);
            }
        }
        p.restore();
    }
}
bool IPoiFile::findPoiCloseBy(const QPoint& px, QSet<const CPoiItem*>& poiItems, QList<QPointF>& posPoiHighlight) const
{
    CTryMutexLocker lock(mutex);
    if(!lock.try_lock())
    {
        return false;
    }

    poiGroup_t poiGroup;
    if(getPoiGroupCloseBy(px, poiGroup))
    {
        for(quint64 key : qAsConst(poiGroup.pois))
        {
            poiItems.insert(loadedPois[key]);
        }
        posPoiHighlight.append(poiGroup.iconCenter);
        return true;
    }
    return false;
}

void IPoiFile::findPoisIn(const QRectF& degRect, QSet<const CPoiItem*>& pois, QList<QPointF>& posPoiHighlight)
{
    CTryMutexLocker lock(mutex);
    if(!lock.try_lock())
    {
        return;
    }

    //Treat highlighting and POIs seperately, as highlighting only applies to items in the current view

    //Find POIs
    QSet<uint> copiedItems; //Some Items may appear in multiple categories. We only want to copy those once.
    const QList<quint64>& keys = categoryActivated.keys();
    for(quint64 categoryID : keys)
    {
        if(categoryActivated[categoryID] != Qt::Checked)
        {
            continue;
        }
        for(int minLonM10 = qFloor(degRect.left() * 10); minLonM10 <= qFloor(degRect.right() * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(degRect.bottom() * 10); minLatM10 <= qFloor(degRect.top() * 10); minLatM10++)
            {
                //Imagine the user moves the screen in an l-shape while updating the selection rectangle. It is possible that some tiles are not laded then
                if(!loadedPoisByArea.contains(categoryID) ||
                   !loadedPoisByArea[categoryID].contains(minLonM10) ||
                   !loadedPoisByArea[categoryID][minLonM10].contains(minLatM10))
                {
                    loadPOIsFromFile(categoryID, minLonM10, minLatM10);
                }
                for(quint64 poiFoundID : qAsConst(loadedPoisByArea)[categoryID][minLonM10][minLatM10])
                {
                    const CPoiItem* poiItemFound = loadedPois[poiFoundID];
                    if(!copiedItems.contains(poiItemFound->getKey()))
                    {
                        //Maybe look through the whole code of selecting items from a map to avoid this conversion
                        if(degRect.contains(poiItemFound->getPos() * RAD_TO_DEG))
                        {
                            pois.insert(poiItemFound);
                            copiedItems.insert(poiItemFound->getKey());
                        }
                    }
                }
            }
        }
    }

    //Find Highlights
    for(const poiGroup_t& poiGroup : qAsConst(displayedPois))
    {
        if(degRect.contains(poiGroup.iconCenter * RAD_TO_DEG))
        {
            posPoiHighlight.append(poiGroup.iconCenter);
        }
    }
}

bool IPoiFile::overlapsWithIcon(const QRectF& rect) const
{
    for(const poiGroup_t& poiGroup : displayedPois)
    {
        if(poiGroup.iconLocation.intersects(rect))
        {
            return true;
        }
    }
    return false;
}

bool IPoiFile::getPoiGroupCloseBy(const QPoint& px, IPoiFile::poiGroup_t& poiItem) const
{
    for(const poiGroup_t& poiGroup : displayedPois)
    {
        if(poiGroup.iconLocation.contains(px))
        {
            poiItem = poiGroup;
            return true;
        }
    }
    return false;
}
