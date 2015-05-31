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

#include <stdlib.h>
#include <string.h>
#include "routino.h"
#include "profiles.h"
#include "translations.h"
#include "files.h"
#include "nodes.h"
#include "segments.h"
#include "ways.h"
#include "relations.h"

#define MAXSEARCH 1

static int isInitialized = 0;

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

    for(int i = 0; i < nCoord; i++)
    {
        distance_t distmax  = km_to_distance(MAXSEARCH);
        index_t segment     = NO_SEGMENT;
        distance_t distmin;
        distance_t dist1;
        distance_t dist2;
        index_t node1;
        index_t node2;
        index_t finish_node = NO_NODE;


        segment = FindClosestSegment(data->OSMNodes, data->OSMSegments, data->OSMWays, lat[i], lon[i], distmax, profile, &distmin, &node1, &node2, &dist1, &dist2);

        if(segment!=NO_SEGMENT)
        {
           finish_node = CreateFakes(data->OSMNodes, data->OSMSegments, i, LookupSegment(data->OSMSegments, segment, 1), node1, node2, dist1, dist2);
        }
        else
        {
           finish_node=NO_NODE;
        }


    }

    return 0;
}
