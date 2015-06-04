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
#include "binout.h"
#include <stdlib.h>
#include <string.h>

#define MAXSEARCH 1

static int isInitialized    = 0;

int option_quickest  = 0;
int option_quiet    = 0;


struct T_DataSet
{
    Nodes     *OSMNodes;
    Segments  *OSMSegments;
    Ways      *OSMWays;
    Relations *OSMRelations;
};

static Results *CalculateRoute(Nodes *nodes,Segments *segments,Ways *ways,Relations *relations,Profile *profile,
                               index_t start_node,index_t prev_segment,index_t finish_node,
                               int start_waypoint,int finish_waypoint);

int RoutinoInit(const char *profiles, const char *translations)
{
    if(isInitialized)
    {
        return 0;
    }

    if(ParseXMLProfiles(profiles))
    {
        return -1;
    }

    if(ParseXMLTranslations(translations, NULL))
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

    return 0;
}

extern H_RoutinoDataSet RoutinoRegisterData(const char * dirname, const char * prefix)
{
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
    DestroyNodeList(data->OSMNodes);
    DestroySegmentList(data->OSMSegments);
    DestroyWayList(data->OSMWays);
    DestroyRelationList(data->OSMRelations);

    free(data);
}

extern int RoutinoCalculate(H_RoutinoDataSet data, const char * profilename, const float * lon, const float * lat, int nCoord)
{
    if(nCoord < 2)
    {
        return -1;
    }

    Profile * profile = GetProfile(profilename);
    if(profile == NULL)
    {
        return -1;
    }

    if(UpdateProfile(profile,data->OSMWays))
    {
        return -1;
    }

    int i;
    int res = -1;
    Results * results[NWAYPOINTS+1] = {NULL};
    index_t start_node              = NO_NODE;
    index_t finish_node             = NO_NODE;
    for(i = 0; i < nCoord; i++)
    {
        waypoint_t start_waypoint   = NO_WAYPOINT;
        waypoint_t finish_waypoint  = NO_WAYPOINT;
        distance_t distmax          = km_to_distance(MAXSEARCH);
        index_t segment             = NO_SEGMENT;
        index_t join_segment        = NO_SEGMENT;
        distance_t distmin;
        distance_t dist1;
        distance_t dist2;
        index_t node1;
        index_t node2;       

        start_node = finish_node;

        segment = FindClosestSegment(data->OSMNodes, data->OSMSegments, data->OSMWays, lat[i], lon[i], distmax, profile, &distmin, &node1, &node2, &dist1, &dist2);

        if(segment!=NO_SEGMENT)
        {
            finish_node = CreateFakes(data->OSMNodes, data->OSMSegments, i, LookupSegment(data->OSMSegments, segment, 1), node1, node2, dist1, dist2);
        }
        else
        {
            finish_node=NO_NODE;
        }

        printf("%i %i %i %i %i\n", node1, node2, finish_node, dist1, dist2);

        if(finish_node == NO_NODE)
        {
            goto RoutinoCalculate_end;
        }

        if(start_node == NO_NODE)
        {
            continue;
        }


        results[i -1] = CalculateRoute(data->OSMNodes, data->OSMSegments, data->OSMWays, data->OSMRelations, profile, start_node, join_segment, finish_node, start_waypoint, finish_waypoint);

    }

    SimplifyResult(results, nCoord-1, data->OSMNodes, data->OSMSegments, data->OSMWays, profile);

    res = 0;
RoutinoCalculate_end:

    for(int n = 0; results[n] != NULL; n++)
    {
        FreeResultsList(results[n]);
    }

    return res;
}


static Results *CalculateRoute(Nodes *nodes,Segments *segments,Ways *ways,Relations *relations,Profile *profile,
                               index_t start_node,index_t prev_segment,index_t finish_node,
                               int start_waypoint,int finish_waypoint)
{
    Results *complete=NULL;

    /* A special case if the first and last nodes are the same */

    if(start_node==finish_node)
    {
        index_t fake_segment;
        Result *result1,*result2;

        complete=NewResultsList(8);

        if(prev_segment==NO_SEGMENT)
        {
            double lat,lon;
            distance_t distmin,dist1,dist2;
            index_t node1,node2;

            GetLatLong(nodes,start_node,NULL,&lat,&lon);

            prev_segment=FindClosestSegment(nodes,segments,ways,lat,lon,1,profile,&distmin,&node1,&node2,&dist1,&dist2);
        }

        fake_segment=CreateFakeNullSegment(segments,start_node,prev_segment,finish_waypoint);

        result1=InsertResult(complete,start_node,prev_segment);
        result2=InsertResult(complete,finish_node,fake_segment);

        result1->next=result2;

        complete->start_node=start_node;
        complete->prev_segment=prev_segment;

        complete->finish_node=finish_node;
        complete->last_segment=fake_segment;
    }
    else
    {
        Results *begin;

        /* Calculate the beginning of the route */

        begin=FindStartRoutes(nodes,segments,ways,relations,profile,start_node,prev_segment,finish_node);

        if(begin)
        {
            /* Check if the end of the route was reached */

            if(begin->finish_node!=NO_NODE)
            {
                complete=begin;
            }
        }
        else
        {
            if(prev_segment!=NO_SEGMENT)
            {
                /* Try again but allow a U-turn at the start waypoint -
                   this solves the problem of facing a dead-end that contains no super-nodes. */

                prev_segment=NO_SEGMENT;

                begin=FindStartRoutes(nodes,segments,ways,relations,profile,start_node,prev_segment,finish_node);
            }

            if(begin)
            {
                /* Check if the end of the route was reached */

                if(begin->finish_node!=NO_NODE)
                {
                    complete=begin;
                }
            }
            else
            {
                fprintf(stderr,"Error: Cannot find initial section of route compatible with profile.\n");
                exit(EXIT_FAILURE);
            }
        }

        /* Calculate the rest of the route */

        if(!complete)
        {
            Results *middle,*end;

            /* Calculate the end of the route */

            end=FindFinishRoutes(nodes,segments,ways,relations,profile,finish_node);

            if(!end)
            {
                fprintf(stderr,"Error: Cannot find final section of route compatible with profile.\n");
                exit(EXIT_FAILURE);
            }

            /* Calculate the middle of the route */

            middle=FindMiddleRoute(nodes,segments,ways,relations,profile,begin,end);

            if(!middle && prev_segment!=NO_SEGMENT)
            {
                /* Try again but allow a U-turn at the start waypoint -
                   this solves the problem of facing a dead-end that contains some super-nodes. */

                FreeResultsList(begin);

                begin=FindStartRoutes(nodes,segments,ways,relations,profile,start_node,NO_SEGMENT,finish_node);

                if(begin)
                {
                    middle=FindMiddleRoute(nodes,segments,ways,relations,profile,begin,end);
                }
            }

            if(!middle)
            {
                fprintf(stderr,"Error: Cannot find super-route compatible with profile.\n");
                exit(EXIT_FAILURE);
            }

            complete=CombineRoutes(nodes,segments,ways,relations,profile,begin,middle,end);

            if(!complete)
            {
                fprintf(stderr,"Error: Cannot create combined route following super-route.\n");
                exit(EXIT_FAILURE);
            }

            FreeResultsList(begin);
            FreeResultsList(middle);
            FreeResultsList(end);
        }
    }

    complete->start_waypoint=start_waypoint;
    complete->finish_waypoint=finish_waypoint;

#if DEBUG
    Result *r=FindResult(complete,complete->start_node,complete->prev_segment);

    printf("The final route is:\n");

    while(r)
    {
        printf("  node=%"Pindex_t " segment=%"Pindex_t " score=%f\n",r->node,r->segment,r->score);

        r=r->next;
    }
#endif

    return complete;
}
