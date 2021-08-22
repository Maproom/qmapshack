/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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
    Q_DECLARE_TR_FUNCTIONS(CGisItemOvlArea)
public:
    CGisItemOvlArea(const SGisLine& line, const QString& name, IGisProject* project, int idx);
    CGisItemOvlArea(const CGisItemOvlArea& parentArea, IGisProject* project, int idx, bool clone);
    CGisItemOvlArea(const QDomNode& xml, IGisProject* project);
    CGisItemOvlArea(const history_t& hist, const QString& dbHash, IGisProject* project);
    CGisItemOvlArea(quint64 id, QSqlDatabase& db, IGisProject* project);
    CGisItemOvlArea(const IQlgtOverlay& ovl, IGisProject* project = nullptr);
    virtual ~CGisItemOvlArea();

    IGisItem* createClone() override;

    QDataStream& operator<<(QDataStream& stream) override;
    QDataStream& operator>>(QDataStream& stream) const override;

    const QString& getName() const override
    {
        return area.name.isEmpty() ? noName : area.name;
    }

    int getColorIdx() const
    {
        return colorIdx;
    }
    QString getInfo(quint32 feature) const override;
    void getPolylineFromData(SGisLine& l) const override;
    void getPolylineDegFromData(QPolygonF& polygon) const override;

    const QString& getComment() const override
    {
        return area.cmt;
    }
    const QString& getDescription() const override
    {
        return area.desc;
    }
    const QList<link_t>& getLinks() const override
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

    QDateTime getTimestamp() const override
    {
        return QDateTime();
    }

    void setName(const QString& str);
    void setColor(int idx);
    void setDataFromPolyline(const SGisLine& l) override;
    void setWidth(qint32 w);
    void setStyle(qint32 s);
    void setOpacity(bool yes);
    void setComment(const QString& str)       override;
    void setDescription(const QString& str)   override;
    void setLinks(const QList<link_t>& links) override;

    void save(QDomNode& gpx, bool strictGpx11) override;
    void edit() override;

    using IGisItem::drawItem;
    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) override;
    void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw* gis) override;
    void drawHighlight(QPainter& p) override;

    IScrOpt* getScreenOptions(const QPoint& origin, IMouse* mouse) override;
    QPointF getPointCloseBy(const QPoint& screenPos) override;
    bool isCloseTo(const QPointF& pos) override;
    bool isWithin(const QRectF& area, selflags_t flags) override;

    void gainUserFocus(bool yes) override;

    const searchValue_t getValueByKeyword(searchProperty_e keyword) override;

    struct width_t
    {
        int width;
        QString string;
    };

    const width_t lineWidths[OVL_N_WIDTHS] =
    {
        {3, tr("thin")}
        , {5, tr("normal")}
        , {9, tr("wide")}
        , {13, tr("strong")}
    };

    const Qt::BrushStyle brushStyles[OVL_N_STYLES] =
    {
        Qt::NoBrush
        , Qt::HorPattern
        , Qt::VerPattern
        , Qt::CrossPattern
        , Qt::BDiagPattern
        , Qt::FDiagPattern
        , Qt::DiagCrossPattern
        , Qt::SolidPattern
    };

protected:
    void setSymbol() override;


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
    void readAreaDataFromGisLine(const SGisLine& line);
    void deriveSecondaryData();
    QPointF getPolygonCentroid(const QPolygonF& polygon);

    area_t area;


    static key_t keyUserFocus;

    QPen penForeground {Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
    QPen penBackground {Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};

    /// the track line color
    QColor color;
    /// the trackpoint bullet icon
    QPixmap bullet;
    /// the track line color by index
    unsigned colorIdx = 0;

    QPolygonF polygonArea;

    QPointer<CScrOptOvlArea>  scrOpt;

    using fSearch = std::function<const searchValue_t (CGisItemOvlArea*)>;
    static QMap<searchProperty_e, fSearch > keywordLambdaMap;
    static QMap<searchProperty_e, fSearch > initKeywordLambdaMap();
};

#endif //CGISITEMOVLAREA_H

