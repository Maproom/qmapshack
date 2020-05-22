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

#include "CRouterOptimization.h"
#include <gis/rte/router/CRouterSetup.h>
#include <GeoMath.h>
#include <helpers/CProgressDialog.h>

CRouterOptimization::CRouterOptimization()
{
    routerOptions = CRouterSetup::self().getOptions();
}

int CRouterOptimization::optimize(SGisLine &line)
{
    checkRouter();
    if(!CRouterSetup::self().hasFastRouting())
    {
        return -1;
    }
    if(line.length() < 4)
    {
        return 0; //There is nothing to optimize
    }

    CProgressDialog progress(tr("Optimizing route"), 0, line.length() + 2, nullptr);

    //Optimize using air distance and known distances, since this is much faster than routing, especially brouter
    SGisLine newAirdistanceOrder;
    SGisLine oldAirdistanceOrder = line;
    qreal gain = createNextBestOrder(oldAirdistanceOrder, newAirdistanceOrder);
    while(gain < 0)
    {
        oldAirdistanceOrder = newAirdistanceOrder;
        gain = createNextBestOrder(oldAirdistanceOrder, newAirdistanceOrder);
    }
    //Do routing and calculate cost for order found
    qreal airdistanceOrderCosts = getRealRouteCosts(oldAirdistanceOrder);

    progress.setValue(1);

    //Do routing and calculate costs of the order the user supplied
    //cancel the route calculation when the cost of the airdistance order is reached
    qreal givenOrderCosts = getRealRouteCosts(line, airdistanceOrderCosts);
    if(givenOrderCosts < 0 && airdistanceOrderCosts < 0)
    {
        return -1;
    }

    //determine starting order for optimization
    qreal bestCosts = NOINT;
    if(givenOrderCosts > 0 && givenOrderCosts < airdistanceOrderCosts)
    {
        bestCosts = givenOrderCosts;
    }
    else
    {
        bestCosts = airdistanceOrderCosts;
        //the old order is the "optimal" one, since the new one didn't have a gain<0
        line = oldAirdistanceOrder;
        fillSubPts(line);
    }
    progress.setValue(2);

    SGisLine lastWorkingOrder = line;
    qreal lastWorkingOrderCosts = bestCosts;
    int numOfRestarts = 0;
    // The number of needed starting permutations is somewhat arbitrary,
    // but you'd likely need more to find the global optimum if there are more possibilities
    while(numOfRestarts < line.length())
    {
        progress.setValue(numOfRestarts + 2);
        if(progress.wasCanceled())
        {
            return -1;
        }

        SGisLine newWorkingOrder;
        qreal bestInsertionGain = createNextBestOrder(lastWorkingOrder, newWorkingOrder);

        if(bestInsertionGain < 0)
        {
            qreal newWorkingOrderCosts = getRealRouteCosts(newWorkingOrder, lastWorkingOrderCosts);

            if(newWorkingOrderCosts > 0 && newWorkingOrderCosts < lastWorkingOrderCosts)
            {
                lastWorkingOrder = newWorkingOrder;
                lastWorkingOrderCosts = newWorkingOrderCosts;

                if(newWorkingOrderCosts < bestCosts)
                {
                    bestCosts = newWorkingOrderCosts;
                    line = newWorkingOrder;

                    // Do this every time, since changes to the line are displayed and the data is available anyways.
                    // Gives a nice animation on screen if the subpoints are displayed aswell.
                    fillSubPts(line);
                }
            }
        }
        else
        {
            numOfRestarts += 1;
            //We accept any order that is produced, as we want to escape from the local optimum
            twoOptStep(lastWorkingOrder, newWorkingOrder);
            lastWorkingOrder = newWorkingOrder;
            lastWorkingOrderCosts = getRealRouteCosts(lastWorkingOrder);
        }
    }

    //Do this a last time, since it happens that the route is already optimal.
    //Return the return value as this is the last point the code may fail for some odd reason
    return fillSubPts(line);
}

qreal CRouterOptimization::createNextBestOrder(const SGisLine &oldOrder, SGisLine &newOrder)
{
    qreal bestInsertionGain = 0;
    int bestBaseIndex = -1;
    int bestInsertedItemIndex = -1;

    // lastWorkingOrder.length()-2, since we can't use the last two items as base
    for(int baseIndex = 0; baseIndex < oldOrder.length() - 2; baseIndex++)
    {
        // Keep start and end fixed
        for(int insertedItemIndex = 1; insertedItemIndex < oldOrder.length() - 1; insertedItemIndex++)
        {
            if(baseIndex == insertedItemIndex || baseIndex == insertedItemIndex - 1)
            {
                continue;
            }

            qreal insertionGain = bestKnownDistance(oldOrder[baseIndex], oldOrder[insertedItemIndex])
                                  + bestKnownDistance(oldOrder[insertedItemIndex], oldOrder[baseIndex + 1])
                                  - bestKnownDistance(oldOrder[baseIndex], oldOrder[baseIndex + 1])
                                  + bestKnownDistance(oldOrder[insertedItemIndex - 1], oldOrder[insertedItemIndex + 1])
                                  - bestKnownDistance(oldOrder[insertedItemIndex - 1], oldOrder[insertedItemIndex])
                                  - bestKnownDistance(oldOrder[insertedItemIndex], oldOrder[insertedItemIndex + 1]);

            if(insertionGain < bestInsertionGain)
            {
                bestBaseIndex = baseIndex;
                bestInsertionGain = insertionGain;
                bestInsertedItemIndex = insertedItemIndex;
            }
        }
    }

    newOrder = SGisLine(oldOrder);
    if(bestBaseIndex >= 0 && bestInsertedItemIndex >= 0)
    {
        // If the index of the inserted item was smaller than that of the base item,
        // moving it will cause the index of the base to decrease. Thus, we don't add 1 to place it after the base
        if(bestInsertedItemIndex < bestBaseIndex)
        {
            newOrder.move(bestInsertedItemIndex, bestBaseIndex);
        }
        else
        {
            newOrder.move(bestInsertedItemIndex, bestBaseIndex + 1);
        }
    }
    return bestInsertionGain;
}

qreal CRouterOptimization::twoOptStep(const SGisLine &oldOrder, SGisLine &newOrder)
{
    //NOINT and not 0, since we also want to take orders that don't seem to be improving the situation
    qreal bestTwoOptGain = NOINT;
    //Begin and End of the section that is inverted
    int bestBeginIndex = -1;
    int bestEndIndex = -1;

    // lastWorkingOrder.length()-2, since the end of the inverted section can't be the end of the line
    for(int beginIndex = 1; beginIndex < oldOrder.length() - 2; beginIndex++)
    {
        for(int endIndex = beginIndex + 1; endIndex < oldOrder.length() - 1; endIndex++)
        {
            qreal oldRangeCosts = bestKnownDistance(oldOrder[beginIndex - 1], oldOrder[beginIndex])
                                  + bestKnownDistance(oldOrder[endIndex], oldOrder[endIndex + 1]);

            qreal newRangeCosts = bestKnownDistance(oldOrder[beginIndex - 1], oldOrder[endIndex])
                                  + bestKnownDistance(oldOrder[beginIndex], oldOrder[endIndex + 1]);
            for(int i = beginIndex; i < endIndex; i++)
            {
                oldRangeCosts += bestKnownDistance(oldOrder[i], oldOrder[i + 1]);
                newRangeCosts += bestKnownDistance(oldOrder[i + 1], oldOrder[i]);
            }

            if(newRangeCosts - oldRangeCosts < bestTwoOptGain)
            {
                bestBeginIndex = beginIndex;
                bestEndIndex = endIndex;
                bestTwoOptGain = newRangeCosts - oldRangeCosts;
            }
        }
    }

    newOrder = SGisLine(oldOrder);
    if(bestEndIndex >= 0 && bestBeginIndex >= 0)
    {
        std::reverse(newOrder.begin() + bestBeginIndex, newOrder.begin() + bestEndIndex);
    }
    return bestTwoOptGain;
}

qreal CRouterOptimization::getRealRouteCosts(const SGisLine &line, qreal costCutoff)
{
    qreal costs = 0;
    for(int i = 0; i < line.length() - 1; i++)
    {
        const routing_cache_item_t* route = getRoute(line[i].coord, line[i + 1].coord);
        if(route == nullptr)
        {
            return -1;
        }
        costs += route->costs;

        if(costCutoff > 0 && costs > costCutoff)
        {
            return -1;
        }
    }
    return costs;
}

qreal CRouterOptimization::bestKnownDistance(const IGisLine::point_t& start, const IGisLine::point_t& end)
{
    //10 digits after the decimal point in exponential format should be by far enough
    QString start_key = QString::number(start.coord.x(), 'e', 10) + QString::number(start.coord.y(), 'e', 10);
    QString end_key = QString::number(end.coord.x(), 'e', 10) + QString::number(end.coord.y(), 'e', 10);

    if(!routingCache.contains(start_key))
    {
        routingCache[start_key] = {};
    }

    if(routingCache[start_key].contains(end_key))
    {
        return routingCache[start_key][end_key].costs;
    }
    else
    {
        //Multiply it with the average of the minimum occuring factor and the average factor
        // to get a reasonable compromise of optimization speed and optimality of results
        if(totalNumOfRoutes > 0 && minAirToCostFactor > 0)
        {
            return GPS_Math_DistanceQuick(start.coord.x(), start.coord.y(),
                                          end.coord.x(), end.coord.y())
                   * (totalAirToCosts / totalNumOfRoutes + minAirToCostFactor) / 2;
        }
        else
        {
            return GPS_Math_DistanceQuick(start.coord.x(), start.coord.y(),
                                          end.coord.x(), end.coord.y());
        }
    }
}

const CRouterOptimization::routing_cache_item_t* CRouterOptimization::getRoute(const QPointF& start, const QPointF& end)
{
    QString start_key = QString::number(start.x(), 'e', 10) + QString::number(start.y(), 'e', 10);
    QString end_key = QString::number(end.x(), 'e', 10) + QString::number(end.y(), 'e', 10);

    if(!routingCache.contains(start_key))
    {
        routingCache[start_key] = {};
    }

    if(!routingCache[start_key].contains(end_key))
    {
        routing_cache_item_t cacheItem;
        int response = CRouterSetup::self().calcRoute(start, end, cacheItem.route, &cacheItem.costs);
        if(response < 0)
        {
            return nullptr;
        }
        routingCache[start_key][end_key] = cacheItem;

        qreal airToCostFactor = cacheItem.costs / GPS_Math_DistanceQuick(start.x(), start.y(), end.x(), end.y());
        if( airToCostFactor < minAirToCostFactor || minAirToCostFactor < 0)
        {
            minAirToCostFactor = airToCostFactor;
        }
        totalAirToCosts += airToCostFactor;
        totalNumOfRoutes++;
    }
    return &routingCache[start_key][end_key];
}

int CRouterOptimization::fillSubPts(SGisLine &line)
{
    for(int i = 0; i < line.length() - 1; i++)
    {
        line[i].subpts.clear();
        const routing_cache_item_t* route = getRoute(line[i].coord, line[i + 1].coord);
        if(route == nullptr)
        {
            return -1;
        }
        for(const QPointF& point : route->route)
        {
            line[i].subpts << IGisLine::subpt_t(point);
        }
    }
    return 0;
}

void CRouterOptimization::checkRouter()
{
    const QString& options = CRouterSetup::self().getOptions();
    if(routerOptions != options)
    {
        routingCache.clear();
        routerOptions = options;
    }
}
