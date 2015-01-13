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

#ifndef CGISITEMRTE_H
#define CGISITEMRTE_H

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"

#include <QPen>

class QDomNode;
class IGisProject;
class CQlgtRoute;

class CGisItemRte : public IGisItem, public IGisLine
{
public:
    struct rtept_t : public wpt_t
    {
        QPixmap icon;
        QPointF focus;
    };

    struct rte_t
    {
        rte_t() : number(0)
        {
        }
        // -- all gpx tags - start
        QString name;
        QString cmt;
        QString desc;
        QString src;
        QList<link_t> links;
        quint64 number;
        QString type;
        QVector<rtept_t> pts;
        // -- all gpx tags - stop
        QMap<QString, QVariant> extensions;
    };

    CGisItemRte(const QDomNode &xml, IGisProject *parent);
    CGisItemRte(const CGisItemRte& parentRte, IGisProject *project, int idx, bool clone);
    CGisItemRte(const history_t& hist, IGisProject * project);
    CGisItemRte(quint64 id, QSqlDatabase& db, IGisProject * project);
    CGisItemRte(const CQlgtRoute& rte1);
    virtual ~CGisItemRte();

    QDataStream& operator<<(QDataStream& stream);
    QDataStream& operator>>(QDataStream& stream);

    const QString& getName() const
    {
        return( rte.name);
    }
    QString getInfo() const;
    IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
    QPointF getPointCloseBy(const QPoint& screenPos);
    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
    void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
    void drawHighlight(QPainter& p);
    void save(QDomNode& gpx);
    bool isCloseTo(const QPointF& pos);
    void gainUserFocus(bool yes);

    void setDataFromPolyline(const QPolygonF& line)
    {
    }
    void getPolylineFromData(QPolygonF& line)
    {
    }

    const QString& getComment() const
    {
        return( rte.cmt);
    }
    const QString& getDescription() const
    {
        return( rte.desc);
    }
    const QList<link_t>& getLinks() const
    {
        return( rte.links);
    }

    void setComment(const QString& str);
    void setDescription(const QString& str);
    void setLinks(const QList<link_t>& links);

private:
    void deriveSecondaryData();
    void setSymbol();
    void readRte(const QDomNode& xml, rte_t& rte);

    static key_t keyUserFocus;



    static const QPen penBackground;
    QPen penForeground;

    rte_t rte;
    QPolygonF line;
};

#endif //CGISITEMRTE_H

