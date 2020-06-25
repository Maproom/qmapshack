/**********************************************************************************************
    Copyright (C) 2020 Henri Hornburg <hrnbg@t-online.de>

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

#ifndef CROUTEROPTIMIZATION_H
#define CROUTEROPTIMIZATION_H
#include <gis/IGisLine.h>
#include <QCoreApplication>
#include <QMap>
#include <QPolygonF>

class CRouterOptimization
{
    Q_DECLARE_TR_FUNCTIONS(CRouterOptimization)
public:
    CRouterOptimization();
    int optimize(SGisLine & line);

private:

    struct routing_cache_item_t
    {
        QPolygonF route;
        qreal costs;
    };

    /// returns value by which the costs were changed
    qreal createNextBestOrder(const SGisLine& oldOrder, SGisLine& newOrder);
    qreal twoOptStep(const SGisLine &oldOrder, SGisLine &newOrder);

    qreal getRealRouteCosts(const SGisLine& line, qreal costCutoff = -1);
    qreal bestKnownDistance(const IGisLine::point_t &start, const IGisLine::point_t &end);
    const routing_cache_item_t *getRoute(const QPointF& from, const QPointF& to);
    int fillSubPts(SGisLine& line);
    /// checks if router settings were changed and if yes, discards the routingCache
    void checkRouter();

    QMap<QString, QMap<QString, routing_cache_item_t> > routingCache;
    qreal minAirToCostFactor = -1;
    qreal totalAirToCosts = 0;
    qreal totalNumOfRoutes = 0;
    QString routerOptions = "";
};

#endif // CROUTEROPTIMIZATION_H
