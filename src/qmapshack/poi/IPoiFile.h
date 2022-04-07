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

#ifndef IPOI_H
#define IPOI_H

#include "canvas/IDrawContext.h"
#include "canvas/IDrawObject.h"
#include "poi/CPoiItem.h"
#include "poi/IPoiProp.h"

#include <QPointer>

class CPoiDraw;

class IPoiFile : public IDrawObject
{
    Q_OBJECT
public:
    IPoiFile(CPoiDraw* parent);
    virtual ~IPoiFile();

    virtual void draw(IDrawContext::buffer_t& buf);

    /**
       @brief Get the POI collection's setup widget.

       As default an instance of CPoiPropSetup is used. For other setups you have
       to override this method.

       @return A pointer to the widget. Use a smart pointer to store as the widget can be destroyed at any time
     */
    virtual IPoiProp* getSetup();

    bool activated() const {return isActivated;}

    virtual void addTreeWidgetItems(QTreeWidget* widget) = 0;

    ///The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
    /// Thus the location where to draw the highlight is separately given
    virtual bool findPoiCloseBy(const QPoint& px, QSet<const CPoiItem*>& poiItems, QList<QPointF>& posPoiHighlight) const;
    ///The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
    /// Thus the location where to draw the highlight is separately given
    virtual void findPoisIn(const QRectF& degRect, QSet<const CPoiItem*>& pois, QList<QPointF>& posPoiHighlight);
    virtual bool getToolTip(const QPoint& px, QString& str) const = 0;
    // category, minLon multiplied by 10, minLat multiplied by 10. POIs are loaded in squares of degrees (should be fine enough to not hang the system)
    virtual void loadPOIsFromFile(quint64 categoryID, int minLonM10, int minLatM10) = 0;

    static void init();
    static const QSize& iconSize(){return _iconSize;}
    static const QImage& iconHighlight(){return _iconHighlight;}

public slots:
    virtual void slotCheckedStateChanged(QTreeWidgetItem* item);

protected:
    struct poiGroup_t
    {
        ///Area covered by the icon in pixels
        QRectF iconLocation;
        ///Location of the center of the icon in rad
        QPointF iconCenter;
        QSet<quint64> pois;
    };

    virtual void getPoiIcon(QPixmap& icon, const poiGroup_t& poiGroup) = 0;
    virtual void getPoiIcon(QPixmap& icon, const CPoiItem* poi, const QString& definingTag = "") = 0;
    bool overlapsWithIcon(const QRectF& rect) const;
    bool getPoiGroupCloseBy(const QPoint& px, poiGroup_t& poiItem) const;

    CPoiDraw* poi;

    /**
       @brief True if POI collection was loaded successfully
     */
    bool isActivated = false;
    QTimer* loadTimer;

    /// the setup dialog. Use getSetup() for access
    QPointer<IPoiProp> setup;

    mutable QMutex mutex {QMutex::Recursive};

    QMap<quint64, Qt::CheckState> categoryActivated;
    QMap<quint64, QString> categoryNames;
    // category, minLon multiplied by 10, minLat multiplied by 10. POIs are loaded in squares of degrees (should be fine enough to not hang the system)
    QMap<quint64, QMap<int, QMap<int, QList<quint64> > > > loadedPoisByArea;
    QMap<quint64, const CPoiItem*> loadedPois;
    QList<poiGroup_t> displayedPois;
    QRectF bbox;
private:
    static QSize _iconSize;
    static QImage _iconHighlight;
};

#endif //IPOI_H

