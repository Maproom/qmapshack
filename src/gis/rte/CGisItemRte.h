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
#include <routino.h>

#include <QPen>

class QDomNode;
class IGisProject;
class CQlgtRoute;

class CGisItemRte : public IGisItem, public IGisLine
{
public:
    struct subpt_t
    {
        subpt_t() : lon(NOFLOAT), lat(NOFLOAT), time(0,0)
        {
        }

        enum type_e
        {
            eTypeNone
            , eTypeJunct
        };

        qreal lon;
        qreal lat;
        quint8 type;

        qreal turn;
        qreal bearing;
        QStringList names;

        qreal distance;
        QTime time;
    };

    struct rtept_t : public wpt_t
    {
        QPixmap icon;
        QPointF focus;
        QVector<subpt_t> subpts;
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
    CGisItemRte(const SGisLine& l, const QString &name, IGisProject *project, int idx);
    virtual ~CGisItemRte();

    QDataStream& operator<<(QDataStream& stream);
    QDataStream& operator>>(QDataStream& stream);

    const QString& getName() const
    {
        return rte.name;
    }
    QString getInfo(bool allowEdit = false) const;
    IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
    QPointF getPointCloseBy(const QPoint& screenPos);
    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
    void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
    void drawHighlight(QPainter& p);
    void save(QDomNode& gpx);
    bool isCloseTo(const QPointF& pos);
    void gainUserFocus(bool yes);

    void setDataFromPolyline(const SGisLine& l);

    void getPolylineFromData(SGisLine &l);

    const QString& getComment() const
    {
        return rte.cmt;
    }
    const QString& getDescription() const
    {
        return rte.desc;
    }
    const QList<link_t>& getLinks() const
    {
        return rte.links;
    }

    void setComment(const QString& str);
    void setDescription(const QString& str);
    void setLinks(const QList<link_t>& links);

    void calc();

    void reset();

    void setResult(T_RoutinoRoute * route, const QString &options);

private:
    void deriveSecondaryData();
    void setSymbol();
    void readRte(const QDomNode& xml, rte_t& rte);
    void readRouteDataFromGisLine(const SGisLine &l);

    static key_t keyUserFocus;

    static const QPen penBackground;
    QPen penForeground;

    rte_t rte;
    QPolygonF line;

    QString     lastRoutedWith;
    QDateTime   lastRoutedTime;
    quint32     lastRoutedCalcTime;

    qreal totalDistance;
    QTime totalTime;
    quint32 totalDays;
};

#endif //CGISITEMRTE_H

