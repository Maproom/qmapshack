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

#ifndef CGISITEMOVLAREA_H
#define CGISITEMOVLAREA_H

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"

#include <QPen>
#include <QPointer>

class IGisProject;
class CScrOptOvlArea;
class IQlgtOverlay;

#define OVL_N_WIDTHS 4
#define OVL_N_STYLES 8

class CGisItemOvlArea : public IGisItem, public IGisLine
{
public:
    CGisItemOvlArea(const SGisLine& line, const QString &name, IGisProject * project, int idx);
    CGisItemOvlArea(const CGisItemOvlArea &parentArea, IGisProject * project, int idx, bool clone);
    CGisItemOvlArea(const QDomNode &xml, IGisProject *project);
    CGisItemOvlArea(const history_t& hist, IGisProject * project);
    CGisItemOvlArea(quint64 id, QSqlDatabase& db, IGisProject * project);
    CGisItemOvlArea(const IQlgtOverlay& ovl);
    virtual ~CGisItemOvlArea();

    IGisItem * createClone() override;

    virtual QDataStream& operator<<(QDataStream& stream) override;
    virtual QDataStream& operator>>(QDataStream& stream) const override;

    virtual const QString& getName() const override
    {
        return area.name.isEmpty() ? noName : area.name;
    }

    int getColorIdx() const
    {
        return colorIdx;
    }
    virtual QString getInfo(bool allowEdit = false) const override;
    virtual void getPolylineFromData(SGisLine& l) override;
    virtual const QString& getComment() const override
    {
        return area.cmt;
    }
    virtual const QString& getDescription() const override
    {
        return area.desc;
    }
    virtual const QList<link_t>& getLinks() const override
    {
        return area.links;
    }
    qint32 getWidth() const
    {
        return area.width;
    }
    qint32 getStyle() const
    {
        return area.style;
    }
    bool getOpacity() const
    {
        return area.opacity;
    }

    void setName(const QString& str);
    void setColor(size_t idx);
    virtual void setDataFromPolyline(const SGisLine& l) override;
    void setWidth(qint32 w);
    void setStyle(qint32 s);
    void setOpacity(bool yes);
    virtual void setComment(const QString& str)       override;
    virtual void setDescription(const QString& str)   override;
    virtual void setLinks(const QList<link_t>& links) override;

    virtual void save(QDomNode& gpx) override;
    virtual void edit() override;

    virtual void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis) override;
    virtual void drawLabel(QPainter& p, const QPolygonF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis) override;
    virtual void drawHighlight(QPainter& p) override;

    virtual IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse) override;
    virtual QPointF getPointCloseBy(const QPoint& screenPos) override;
    virtual bool isCloseTo(const QPointF& pos) override;

    virtual void gainUserFocus(bool yes) override;

    struct width_t
    {
        int width;
        QString string;
    };

    static const width_t lineWidths[OVL_N_WIDTHS];
    static const Qt::BrushStyle brushStyles[OVL_N_STYLES];
protected:
    virtual void setSymbol() override;


public:

    struct pt_t : public wpt_t
    {
    };

    struct area_t
    {
        // -- all gpx tags - start
        QString name;
        QString cmt;
        QString desc;
        QString src;
        QList<link_t> links;
        quint64 number = 0;
        QString type;
        QVector<pt_t> pts;
        QString color;
        qint32 width = 5;
        qint32 style = Qt::BDiagPattern;
        bool opacity = false;

        // secondary data;
        qreal area;
    };

    const area_t& getAreaData() const
    {
        return area;
    }

private:
    void readArea(const QDomNode& xml, area_t& area);
    void setColor(const QColor& c);
    void setIcon(const QString& c);
    void readAreaDataFromGisLine(const SGisLine &line);
    void deriveSecondaryData();
    QPointF getPolygonCentroid(const QPolygonF& polygon);

    area_t area;


    static key_t keyUserFocus;

    QPen penForeground {Qt::blue,  3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
    QPen penBackground {Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};

    /// the track line color
    QColor color;
    /// the trackpoint bullet icon
    QPixmap bullet;
    /// the track line color by index
    unsigned colorIdx = 0;

    QPolygonF polygonArea;

    QPointer<CScrOptOvlArea>  scrOpt;
};

#endif //CGISITEMOVLAREA_H

