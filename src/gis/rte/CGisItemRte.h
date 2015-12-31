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

#include "gis/fit/CFitStream.h"
#include <QPen>
#include <QPointer>

class QDomNode;
class IGisProject;
class CQlgtRoute;
class CScrOptRte;

class CGisItemRte : public IGisItem, public IGisLine
{
public:

    enum focusmode_e
    {
        eFocusMouseMove
        ,eFocusMouseClick
    };

    struct subpt_t
    {
        enum type_e
        {
            eTypeNone
            , eTypeWpt
            , eTypeJunct
        };

        qreal lon = NOFLOAT;
        qreal lat = NOFLOAT;
        quint8 type = eTypeNone;

        qreal turn = NOFLOAT;
        qreal bearing = NOFLOAT;
        QStringList streets;

        QString instruction;
        qreal distance = 0;
        quint32 time = 0;
    };

    struct rtept_t : public wpt_t
    {
        rtept_t()
        {
            fakeSubpt.type = subpt_t::eTypeWpt;
        }

        void updateIcon();

        QPixmap icon;
        QPointF focus;

        subpt_t fakeSubpt;
        QVector<subpt_t> subpts;
    };

    struct rte_t
    {
        // -- all gpx tags - start
        QString name;
        QString cmt;
        QString desc;
        QString src;
        QList<link_t> links;
        quint64 number = 0;
        QString type;
        QVector<rtept_t> pts;
        // -- all gpx tags - stop
    };

    CGisItemRte(const QDomNode &xml, IGisProject *parent);
    CGisItemRte(const CGisItemRte& parentRte, IGisProject *project, int idx, bool clone);
    CGisItemRte(const history_t& hist, IGisProject * project);
    CGisItemRte(quint64 id, QSqlDatabase& db, IGisProject * project);
    CGisItemRte(const CQlgtRoute& rte1);
    CGisItemRte(const SGisLine& l, const QString &name, IGisProject *project, int idx);
    CGisItemRte(CFitStream& stream, IGisProject * project);
    virtual ~CGisItemRte();

    IGisItem * createClone() override;

    QDataStream& operator<<(QDataStream& stream);
    QDataStream& operator>>(QDataStream& stream) const;

    const QString& getName() const
    {
        return rte.name.isEmpty() ? noName : rte.name;
    }

    QString getInfo(bool allowEdit = false) const;
    IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
    QPointF getPointCloseBy(const QPoint& screenPos);
    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
    void drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis);
    void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
    void drawHighlight(QPainter& p);
    void save(QDomNode& gpx);
    bool isCloseTo(const QPointF& pos);
    /**
       @brief Switch user focus on and off.

       If the focus is switched on any other route having the focus will loose it.

       @param yes   set true to gain focus.
     */
    void gainUserFocus(bool yes);
    /**
       @brief Make sure the route has lost focus.

       If the route has the focus, keyUserFocus will be reset. In all other cases nothing will be done.

     */
    void looseUserFocus();
    /**
       @brief Check for user focus

       @return True if the route has user focus
     */
    bool hasUserFocus() const
    {
        return key == keyUserFocus;
    }

    /**
       @brief Get the key of the current track with user focus

       @return If no route has the focus an empty string is returned
     */
    static const key_t& getKeyUserFocus()
    {
        return keyUserFocus;
    }

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

    const rte_t& getRoute() const
    {
        return rte;
    }

    void setName(const QString& str);
    void setComment(const QString& str);
    void setDescription(const QString& str);
    void setLinks(const QList<link_t>& links);

    void calc();

    void reset();

    void edit();

    QPointF setMouseFocusByPoint(const QPoint& pt, focusmode_e fmode, const QString &owner);

    void setResult(Routino_Output * route, const QString &options);
    void setResult(const QDomDocument& xml, const QString &options);

    bool isCalculated();

private:
    void deriveSecondaryData();
    void setSymbol();
    void readRte(const QDomNode& xml, rte_t& rte);
    void readRteFromFit(CFitStream &stream);
    void readRouteDataFromGisLine(const SGisLine &l);
    const subpt_t * getSubPtByIndex(quint32 idx);

    static key_t keyUserFocus;

    static const QPen penBackground;
    QPen penForeground {Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
    QPen penForegroundFocus {Qt::magenta, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};

    rte_t rte;
    QPolygonF line;

    QString lastRoutedWith;
    QDateTime lastRoutedTime;

    qreal totalDistance = NOFLOAT;
    quint32 totalTime = 0;

    const subpt_t * mouseMoveFocus = 0;

    QPointer<CScrOptRte>  scrOpt;
};

#endif //CGISITEMRTE_H

