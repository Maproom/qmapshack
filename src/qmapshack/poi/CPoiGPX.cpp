/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>
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

#include "gis/Poi.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "poi/CPoiCategory.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiGPX.h"
#include "poi/CPoiIconCategory.h"
#include "poi/IPoi.h"

#include <QtWidgets>

CPoiGPX::CPoiGPX(const QString &filename, CPoiDraw *parent)
    : IPoi(parent)
    , filename(filename)
    , loadTimer(new QTimer(this))
{
    //Set up a timer to only start reloading and redrawing the map when the user has made his selections
    loadTimer->setSingleShot(true);
    loadTimer->setInterval(500);
    connect(loadTimer, &QTimer::timeout, poi, &CPoiDraw::emitSigCanvasUpdate);

    //Read waypoints from file
    // create file instance
    QFile file(filename);

    // load file content to xml document
    QDomDocument xml;
    QString msg;
    int line;
    int column;
    if(!xml.setContent(&file, false, &msg, &line, &column))
    {
        file.close();
        throw tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg);
    }
    file.close();

    QDomElement xmlGpx = xml.documentElement();
    if(xmlGpx.tagName() != "gpx")
    {
        throw tr("Not a GPX file: %1").arg(filename);
    }

    const QDomNodeList& xmlWpts = xmlGpx.elementsByTagName("wpt");
    for(int n = 0; n < xmlWpts.count(); ++n)
    {
        const QDomNode& xmlWpt = xmlWpts.item(n);
        QSharedPointer<CGisItemWpt> wpt = QSharedPointer<CGisItemWpt>(new CGisItemWpt(xmlWpt, nullptr));
        loadedPoisByArea[wpt->getIconName()][floor(wpt->getPosition().x() * 10)][floor(wpt->getPosition().y() * 10)].append(wpt);
    }

    // Set true if the file could be open and loaded successfully
    // If not set true the system will take care to destroy this object
    isActivated = true;
}


void CPoiGPX::draw(IDrawContext::buffer_t& buf)
{
    // !!!! NOTE !!!!
    // This is running in it's own thread, not the main thread.
    // Use CPoiGPX::mutex whenever you access data shared between
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
    if(xMax >  180.0 * DEG_TO_RAD)
    {
        xMax =  180 * DEG_TO_RAD;
    }

    // draw POI
    mutex.lock(); //Does this have to be so broad?
    displayedPois.clear();
    QRectF freeSpaceRect (QPointF(), IPoi::iconSize() * 2);
    //Find POIs in view
    for(const QString& category : categoryActivated.keys(Qt::Checked))
    {
        for(int minLonM10 = qFloor(xMin * RAD_TO_DEG * 10); minLonM10 < qCeil(xMax * RAD_TO_DEG * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(yMin * RAD_TO_DEG * 10); minLatM10 < qCeil(yMax * RAD_TO_DEG * 10); minLatM10++)
            {
                if(poi->needsRedraw())
                {
                    mutex.unlock();
                    return;
                }
                for(QSharedPointer<CGisItemWpt> poiToDraw : loadedPoisByArea[category][minLonM10][minLatM10])
                {
                    QPointF pt = poiToDraw->getPosition() * DEG_TO_RAD;
                    poi->convertRad2Px(pt);

                    freeSpaceRect.moveCenter(pt);

                    bool foundIntersection = false;
                    for(poiGroup_t& poiGroup: displayedPois)
                    {
                        if(poiGroup.iconLocation.intersects(freeSpaceRect))
                        {
                            foundIntersection = true;
                            poiGroup.pois.insert(poiToDraw);
                            break;
                        }
                    }

                    if(!foundIntersection)
                    {
                        poiGroup_t poiGroup;
                        QRectF iconRect (QPointF(), IPoi::iconSize());
                        iconRect.moveCenter(pt);
                        poiGroup.iconLocation = iconRect;
                        poiGroup.iconCenter = poiToDraw->getPosition() * DEG_TO_RAD;
                        poiGroup.pois.insert(poiToDraw);
                        displayedPois.append(poiGroup);
                    }
                }
            }
        }
    }

    //Draw Icons
    for(const poiGroup_t& poiGroup: displayedPois)
    {
        QFontMetricsF fm(CMainWindow::self().getMapFont());

        QPixmap icon;
        getPoiIcon(icon, poiGroup);
        icon = icon.scaled(IPoi::iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

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
            const QString& name = (*poiGroup.pois.begin())->getName();
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
    mutex.unlock();
}

bool CPoiGPX::findPoiCloseBy(const QPoint& px, QSet<poi_t>& poiItems, QList<QPointF>& posPoiHighlight) const
{
    poiGroup_t poiGroup;
    if(getPoiGroupCloseBy(px, poiGroup))
    {
        for(QSharedPointer<CGisItemWpt> poi : poiGroup.pois)
        {
            poiItems.insert(gisItemWptToPoi(poi));
        }
        posPoiHighlight.append(poiGroup.iconCenter);
        return true;
    }
    return false;
}

void CPoiGPX::findPoisIn(const QRectF &degRect, QSet<poi_t> &pois, QList<QPointF> &posPoiHighlight)
{
    //Treat highlighting and POIs seperately, as highlighting only applies to items in the current view

    //Find POIs
    for(const QString& category : categoryActivated.keys(Qt::Checked))
    {
        for(int minLonM10 = qFloor(degRect.left() * 10); minLonM10 <= qFloor(degRect.right() * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(degRect.bottom() * 10); minLatM10 <= qFloor(degRect.top() * 10); minLatM10++)
            {
                for(QSharedPointer<CGisItemWpt> poiItemFound : loadedPoisByArea[category][minLonM10][minLatM10])
                {
                    //Maybe look through the whole code of selecting items from a map to avoid this conversion
                    if(degRect.contains(poiItemFound->getPosition()))
                    {
                        pois.insert(gisItemWptToPoi(poiItemFound));
                    }
                }
            }
        }
    }

    //Find Highlights
    for(const poiGroup_t& poiGroup:displayedPois)
    {
        if(degRect.contains(poiGroup.iconCenter * RAD_TO_DEG))
        {
            posPoiHighlight.append(poiGroup.iconCenter);
        }
    }
}

bool CPoiGPX::getToolTip(const QPoint &px, QString &str) const
{
    poiGroup_t poiGroup;
    bool success = getPoiGroupCloseBy(px, poiGroup);
    if(success)
    {
        if(poiGroup.pois.count() == 1)
        {
            str = (*poiGroup.pois.begin())->getInfo(CGisItemWpt::eFeatureShowFullText);
        }
        else
        {
            str += "<i>" + tr("Zoom in to see more details.") + "</i>";
            if(poiGroup.pois.count() <= 10)
            {
                str += "<br>\n" + tr("POIs at this point:");
                for(QSharedPointer<CGisItemWpt> poi : poiGroup.pois)
                {
                    str += "<br>\n<b>" + poi->getName() + "</b>";
                }
            }
        }
    }
    return success;
}

void CPoiGPX::addTreeWidgetItems(QTreeWidget* widget)
{
    for(const QString& name: loadedPoisByArea.keys())
    {
        new CPoiCategory(name, name, widget);
    }
}

void CPoiGPX::slotCheckedStateChanged(QTreeWidgetItem * item)
{
    CPoiCategory* categoryItem = static_cast<CPoiCategory*>(item);
    if(categoryItem == nullptr)
    {
        return;
    }

    categoryActivated[categoryItem->getId().toString()] = categoryItem->checkState();

    loadTimer->start();
}

void CPoiGPX::getPoiIcon(QPixmap &icon, const CPoiGPX::poiGroup_t &poiGroup)
{
    if(poiGroup.pois.count() > 1)
    {
        const QString& firstPoiIconName = (*poiGroup.pois.begin())->getIconName();
        for(QSharedPointer<CGisItemWpt> poi:poiGroup.pois)
        {
            if(firstPoiIconName != poi->getIconName())
            {
                icon = QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png");
                return;
            }
        }
    }
    icon = (*poiGroup.pois.begin())->getIcon();
}

bool CPoiGPX::overlapsWithIcon(const QRectF &rect) const
{
    for(const poiGroup_t& poiGroup: displayedPois)
    {
        if(poiGroup.iconLocation.intersects(rect))
        {
            return true;
        }
    }
    return false;
}

bool CPoiGPX::getPoiGroupCloseBy(const QPoint &px, CPoiGPX::poiGroup_t &poiItem) const
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

poi_t CPoiGPX::gisItemWptToPoi(QSharedPointer<CGisItemWpt> wpt) const
{
    poi_t poi;
    poi.name = wpt->getName();
    poi.pos = wpt->getPosition();
    poi.desc = wpt->getDescription();
    poi.icon = wpt->getIconName();
    poi.gisItem = wpt;
    return poi;
}
