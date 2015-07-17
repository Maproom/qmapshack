/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "binout.h"
#include "fakes.h"
#include "files.h"
#include "functions.h"
#include "nodes.h"
#include "profiles.h"
#include "relations.h"
#include "results.h"
#include "routino.h"
#include "segments.h"
#include "translations.h"
#include "ways.h"
#include <stdlib.h>
#include <string.h>

#define MAXSEARCH 1

static int isInitialized    = 0;
int option_quickest         = 0;
int option_quiet            = 0;


struct T_DataSet
{
    Nodes     *OSMNodes;
    Segments  *OSMSegments;
    Ways      *OSMWays;
    Relations *OSMRelations;
};


int RoutinoInit(const char *profiles, const char *translations)
{
    if(isInitialized)
    {
        return 0;
    }

    if(ParseXMLProfiles(profiles, NULL, 1))
    {
        return -1;
    }

    if(ParseXMLTranslations(translations, NULL, 1))
    {
        return -1;
    }

    isInitialized = 1;
    return 0;
}

int RoutinoRelease()
{
    if(!isInitialized)
    {
        return 0;
    }

    ///@todo free memory from profiles and translations

    isInitialized = 0;
    return 0;
}

extern H_RoutinoDataSet RoutinoRegisterData(const char * dirname, const char * prefix)
{
    if(!isInitialized)
    {
        return 0;
    }

    char * filename;
    Nodes     *OSMNodes;
    Segments  *OSMSegments;
    Ways      *OSMWays;
    Relations *OSMRelations;

    filename = FileName(dirname,prefix,"nodes.mem");
    OSMNodes = LoadNodeList(filename);
    free(filename);
    if(OSMNodes == NULL)
    {
        goto RegisterDataErrorNodes;
    }

    filename = FileName(dirname,prefix,"segments.mem");
    OSMSegments = LoadSegmentList(filename);
    free(filename);
    if(OSMSegments == NULL)
    {
        goto RegisterDataErrorSegments;
    }

    filename = FileName(dirname,prefix,"ways.mem");
    OSMWays = LoadWayList(filename);
    free(filename);
    if(OSMWays == NULL)
    {
        goto RegisterDataErrorWays;
    }

    filename = FileName(dirname,prefix,"relations.mem");
    OSMRelations = LoadRelationList(filename);
    free(filename);
    if(OSMNodes == NULL)
    {
        goto RegisterDataErrorRelations;
    }

    H_RoutinoDataSet dataset = malloc(sizeof(struct T_DataSet));
    dataset->OSMNodes       = OSMNodes;
    dataset->OSMSegments    = OSMSegments;
    dataset->OSMWays        = OSMWays;
    dataset->OSMRelations   = OSMRelations;
    return dataset;

RegisterDataErrorRelations:
    DestroyWayList(OSMWays);
RegisterDataErrorWays:
    DestroySegmentList(OSMSegments);
RegisterDataErrorSegments:
    DestroyNodeList(OSMNodes);
RegisterDataErrorNodes:
    return 0;
}

extern void RoutinoFreeData(H_RoutinoDataSet data)
{
    if(data)
    {
        DestroyNodeList(data->OSMNodes);
        DestroySegmentList(data->OSMSegments);
        DestroyWayList(data->OSMWays);
        DestroyRelationList(data->OSMRelations);
        free(data);
    }
}

extern T_RoutinoRoute * RoutinoCalculate(H_RoutinoDataSet data, const char * profilename, int quickest, const float * lon, const float * lat, int nCoord)
{
    Profile profile;

    if(!isInitialized)
    {
        return 0;
    }

    if(nCoord < 2)
    {
        return 0;
    }

    Profile * pProfile = GetProfile(profilename);
    if(pProfile == NULL)
    {
        return 0;
    }

    profile = *pProfile;

    if(UpdateProfile(&profile,data->OSMWays))
    {
        return 0;
    }

    option_quickest = quickest;

    int i;
    T_RoutinoRoute * route         = 0;
    int nResults                    = 0;    
    Results * results[NWAYPOINTS+1] = {NULL};
    index_t start_node              = NO_NODE;
    index_t finish_node             = NO_NODE;
    waypoint_t start_waypoint       = NO_WAYPOINT;
    waypoint_t finish_waypoint      = NO_WAYPOINT;
    for(i = 1; i <= nCoord; i++)
    {
        distance_t distmax          = 300;//km_to_distance(MAXSEARCH);
        index_t segment             = NO_SEGMENT;
        index_t join_segment        = NO_SEGMENT;
        distance_t distmin;
        distance_t dist1;
        distance_t dist2;
        index_t node1;
        index_t node2;

        start_node = finish_node;
        start_waypoint=finish_waypoint;

        segment = FindClosestSegment(data->OSMNodes, data->OSMSegments, data->OSMWays, lat[i-1], lon[i-1], distmax, &profile, &distmin, &node1, &node2, &dist1, &dist2);

        if(segment!=NO_SEGMENT)
        {
            finish_node = CreateFakes(data->OSMNodes, data->OSMSegments, i, LookupSegment(data->OSMSegments, segment, 1), node1, node2, dist1, dist2);
        }
        else
        {
            finish_node=NO_NODE;
        }


        if(finish_node == NO_NODE)
        {
            goto RoutinoCalculate_end;
        }


        finish_waypoint = i;

        if(start_node == NO_NODE)
        {
            continue;
        }


        results[nResults] = CalculateRoute(data->OSMNodes, data->OSMSegments, data->OSMWays, data->OSMRelations, &profile, start_node, join_segment, finish_node, start_waypoint, finish_waypoint);

        if(results[nResults] == 0)
        {
            goto RoutinoCalculate_end;
        }

        join_segment = results[nResults]->last_segment;

        nResults++;
    }

    route = SimplifyResult(results, nResults, data->OSMNodes, data->OSMSegments, data->OSMWays, &profile);

RoutinoCalculate_end:

    DeleteFakeNodes();
    for(int n = 0; results[n] != NULL; n++)
    {
        FreeResultsList(results[n]);
    }

    return route;
}

extern void RoutinoFreeRoute(T_RoutinoRoute * route)
{
    while(route)
    {
        T_RoutinoRoute * next = route->next;
        free(route);
        route = next;
    }
}

// ---------------------- stop API ------------------------------

