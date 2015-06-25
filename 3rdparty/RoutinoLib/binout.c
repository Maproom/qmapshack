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
#include "nodes.h"
#include "profiles.h"
#include "results.h"
#include "segments.h"
#include "translations.h"
#include "types.h"
#include "ways.h"
#include "xmlparse.h"

#include <stdlib.h>


/* Global variables */

/*+ The option to calculate the quickest route insted of the shortest. +*/
extern int option_quickest;

/*+ The options to select the format of the output. +*/
extern int option_html,option_gpx_track,option_gpx_route,option_text,option_text_all,option_stdout;


/* Local variables */

/*+ Heuristics for determining if a junction is important. +*/
static const char junction_other_way[Highway_Count][Highway_Count]=
{  /* M, T, P, S, T, U, R, S, T, C, P, S, F = Way type of route not taken */
    {   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },/* Motorway     */
    {   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },/* Trunk        */
    {   1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },/* Primary      */
    {   1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },/* Secondary    */
    {   1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },/* Tertiary     */
    {   1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1 },/* Unclassified */
    {   1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1 },/* Residential  */
    {   1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 },/* Service      */
    {   1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1 },/* Track        */
    {   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },/* Cycleway     */
    {   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },/* Path         */
    {   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },/* Steps        */
    {   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },/* Ferry        */
};


/*++++++++++++++++++++++++++++++++++++++
   Print the optimum route between two nodes.

   Results **results The set of results to print (consecutive in array even if not consecutive waypoints).

   int nresults The number of results in the list.

   Nodes *nodes The set of nodes to use.

   Segments *segments The set of segments to use.

   Ways *ways The set of ways to use.

   Profile *profile The profile containing the transport type, speeds and allowed highways.
++++++++++++++++++++++++++++++++++++++*/
T_RoutinoRoute * SimplifyResult(Results ** results, int nresults, Nodes *nodes, Segments *segments, Ways *ways, Profile *profile)
{
    FILE *textfile = stdout;

    index_t prev_node=NO_NODE;
    distance_t cum_distance=0;
    duration_t cum_duration=0;

    int point=0;
    int point_count=0;
    int roundabout=0;

    T_RoutinoRoute * route_first   = 0;
    T_RoutinoRoute * route_next    = 0;


    /* Loop through all the sections of the route and print them */

    do
    {
        int first=1;
        int next_point=point;
        distance_t junc_distance=0;
        duration_t junc_duration=0;
        Result *result;

#if DEBUG
        printf("Route section %d - waypoint %d to waypoint %d\n",point,results[point]->start_waypoint,results[point]->finish_waypoint);
        printf("  start_node=%"Pindex_t " prev_segment=%"Pindex_t "\n",results[point]->start_node,results[point]->prev_segment);
        printf("  finish_node=%"Pindex_t " last_segment=%"Pindex_t "\n",results[point]->finish_node,results[point]->last_segment);

        Result *r=FindResult(results[point],results[point]->start_node,results[point]->prev_segment);

        while(r)
        {
            printf("    node=%"Pindex_t " segment=%"Pindex_t " score=%f\n",r->node,r->segment,r->score);

            r=r->next;
        }
#endif

        result=FindResult(results[point],results[point]->start_node,results[point]->prev_segment);

        /* Loop through all the points within a section of the route and print them */

        do
        {
            double latitude,longitude;
            Node *resultnodep=NULL;
            index_t realsegment=NO_SEGMENT,next_realsegment=NO_SEGMENT;
            Segment *resultsegmentp=NULL,*next_resultsegmentp=NULL;
            Way *resultwayp=NULL,*next_resultwayp=NULL;
            Result *next_result;
            int important=IMP_UNIMPORTANT;

            distance_t seg_distance=0;
            duration_t seg_duration=0;
            char *waynameraw=NULL,*wayname=NULL,*next_waynameraw=NULL;
            int turn_int=0, next_bearing_int=0;
            char *turn=NULL,*next_bearing=NULL;

            /* Calculate the information about this point */

            if(IsFakeNode(result->node))
            {
                GetFakeLatLong(result->node,&latitude,&longitude);
            }
            else
            {
                resultnodep=LookupNode(nodes,result->node,6);

                GetLatLong(nodes,result->node,resultnodep,&latitude,&longitude);
            }

            /* Calculate the next result */

            next_result=result->next;

            if(!next_result)
            {
                next_point++;

                if(next_point<nresults)
                {
                    next_result=FindResult(results[next_point],results[next_point]->start_node,results[next_point]->prev_segment);
                    next_result=next_result->next;
                }
            }

            /* Calculate the information about this segment */

            if(!first)          /* not first point of a section of the route */
            {
                if(IsFakeSegment(result->segment))
                {
                    resultsegmentp=LookupFakeSegment(result->segment);
                    realsegment=IndexRealSegment(result->segment);
                }
                else
                {
                    resultsegmentp=LookupSegment(segments,result->segment,2);
                    realsegment=result->segment;
                }

                resultwayp=LookupWay(ways,resultsegmentp->way,1);

                seg_distance+=DISTANCE(resultsegmentp->distance);
                seg_duration+=Duration(resultsegmentp,resultwayp,profile);

                /* Calculate the cumulative distance/duration */

                junc_distance+=seg_distance;
                junc_duration+=seg_duration;
                cum_distance+=seg_distance;
                cum_duration+=seg_duration;
            }

            /* Calculate the information about the next segment */

            if(next_result)
            {
                if(IsFakeSegment(next_result->segment))
                {
                    next_resultsegmentp=LookupFakeSegment(next_result->segment);
                    next_realsegment=IndexRealSegment(next_result->segment);
                }
                else
                {
                    next_resultsegmentp=LookupSegment(segments,next_result->segment,1);
                    next_realsegment=next_result->segment;
                }
            }

            /* Decide if this is a roundabout */

            if(next_result)
            {
                next_resultwayp=LookupWay(ways,next_resultsegmentp->way,2);

                if(next_resultwayp->type&Highway_Roundabout)
                {
                    if(roundabout==0)
                    {
                        roundabout++;
                        important=IMP_RB_ENTRY;
                    }
                    else
                    {
                        Segment *segmentp;

                        if(resultnodep)
                        {
                            segmentp=FirstSegment(segments,resultnodep,3);
                        }
                        else
                        {
                            segmentp=FirstFakeSegment(result->node);
                        }

                        do
                        {
                            index_t othernode=OtherNode(segmentp,result->node);
                            index_t thissegment;

                            if(IsFakeNode(result->node))
                            {
                                thissegment=IndexFakeSegment(segmentp);
                            }
                            else
                            {
                                thissegment=IndexSegment(segments,segmentp);
                            }

                            if(othernode!=prev_node && othernode!=next_result->node &&
                               thissegment!=realsegment && IsNormalSegment(segmentp))
                            {
                                int canexit=1;

                                if(profile->oneway && IsOnewayTo(segmentp,result->node))
                                {
                                    if(profile->allow!=Transports_Bicycle)
                                    {
                                        canexit=0;
                                    }
                                    else
                                    {
                                        Way *wayp=LookupWay(ways,segmentp->way,3);

                                        if(!(wayp->type&Highway_CycleBothWays))
                                        {
                                            canexit=0;
                                        }
                                    }
                                }

                                if(canexit)
                                {
                                    Way *wayp=LookupWay(ways,segmentp->way,3);

                                    if(!(wayp->type&Highway_Roundabout))
                                    {
                                        roundabout++;
                                        important=IMP_RB_NOT_EXIT;
                                    }
                                }
                            }

                            if(resultnodep)
                            {
                                segmentp=NextSegment(segments,segmentp,result->node);
                            }
                            else
                            {
                                segmentp=NextFakeSegment(segmentp,result->node);
                            }
                        }
                        while(segmentp);
                    }
                }
                else
                if(roundabout)
                {
                    roundabout++;
                    important=IMP_RB_EXIT;
                }
            }

            /* Decide if this is an important junction */

            if(point_count==0) /* first point overall = Waypoint */
            {
                important=IMP_WAYPOINT;
            }
            else if(result->next==NULL) /* Waypoint */
            {
                important=IMP_WAYPOINT;
            }
            else if(first)      /* first point of a section of the route */
            {
                important=IMP_IGNORE;
            }
            else if(roundabout) /* roundabout */
            {
                ;
            }
            else if(realsegment==next_realsegment) /* U-turn */
            {
                important=IMP_UTURN;
            }
            else if(resultnodep && (resultnodep->flags&NODE_MINIRNDBT))
            {
                important=IMP_MINI_RB; /* mini-roundabout */
            }
            else
            {
                Segment *segmentp=FirstSegment(segments,resultnodep,3);

                do
                {
                    index_t seg=IndexSegment(segments,segmentp);

                    if(seg!=realsegment && IsNormalSegment(segmentp))
                    {
                        int cango=1;

                        if(profile->oneway && IsOnewayTo(segmentp,result->node))
                        {
                            if(profile->allow!=Transports_Bicycle)
                            {
                                cango=0;
                            }
                            else
                            {
                                Way *wayp=LookupWay(ways,segmentp->way,3);

                                if(!(wayp->type&Highway_CycleBothWays))
                                {
                                    cango=0;
                                }
                            }
                        }

                        if(cango)
                        {
                            Way *wayp=LookupWay(ways,segmentp->way,3);

                            if(seg==next_realsegment) /* the next segment that we follow */
                            {
                                if(HIGHWAY(wayp->type)!=HIGHWAY(resultwayp->type))
                                {
                                    if(important<IMP_CHANGE)
                                    {
                                        important=IMP_CHANGE;
                                    }
                                }
                            }
                            else /* a segment that we don't follow */
                            {
                                if(junction_other_way[HIGHWAY(resultwayp->type)-1][HIGHWAY(wayp->type)-1])
                                {
                                    if(important<IMP_JUNCT_IMPORT)
                                    {
                                        important=IMP_JUNCT_IMPORT;
                                    }
                                }

                                if(important<IMP_JUNCT_CONT)
                                {
                                    important=IMP_JUNCT_CONT;
                                }
                            }
                        }
                    }

                    segmentp=NextSegment(segments,segmentp,result->node);
                }
                while(segmentp);
            }

            /* Calculate the strings to be used */

            if(next_result && important>IMP_JUNCT_CONT)
            {

                next_waynameraw=WayName(ways,next_resultwayp);
                if(!*next_waynameraw)
                {
                   //next_waynameraw=translate_raw_highway[HIGHWAY(next_resultwayp->type)];
                }

                if(!first && textfile)
                {
                    if(DISTANCE(resultsegmentp->distance)==0 || DISTANCE(next_resultsegmentp->distance)==0)
                    {
                        turn_int=0;
                    }
                    else
                    {
                        turn_int=(int)TurnAngle(nodes,resultsegmentp,next_resultsegmentp,result->node);
                    }

                    //turn=translate_xml_turn[((202+turn_int)/45)%8];
                }


                if(textfile)
                {
                    if(!first && DISTANCE(next_resultsegmentp->distance)==0)
                    {
                        next_bearing_int=(int)BearingAngle(nodes,resultsegmentp,result->node);
                    }
                    else
                    {
                        next_bearing_int=(int)BearingAngle(nodes,next_resultsegmentp,next_result->node);
                    }

                    //next_bearing=translate_xml_heading[(4+(22+next_bearing_int)/45)%8];
                }
            }

            /* Print out the important points (junctions / waypoints) */
            if(important>IMP_IGNORE)
            {
                if(textfile)
                {
//                    char *type;

//                    if(important==IMP_WAYPOINT)
//                    {
//                        type="Waypt";
//                    }
//                    else
//                    {
//                        type="Junct";
//                    }

                    if(point_count==0) /* first point */
                    {
//                        fprintf(textfile,"%10.6f\t%11.6f\t%6.3f km\t%4.1f min\t%5.1f km\t%4.0f min\t%s\t\t %+d\t%s\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                0.0,0.0,0.0,0.0,
//                                type,
//                                ((22+next_bearing_int)/45+4)%8-4,
//                                next_waynameraw);

                        route_first = malloc(sizeof(T_RoutinoRoute));
                        route_first->lat = (float)latitude;
                        route_first->lon = (float)longitude;
                        route_first->dist = 0;
                        route_first->time = 0;
                        route_first->type = important;
                        route_first->turn = -1;
                        route_first->bearing = next_bearing_int;
                        route_first->string = next_waynameraw;

                        route_next = route_first;

                    }
                    else if(!next_result) /* end point */
                    {
//                        fprintf(textfile,"%10.6f\t%11.6f\t%6.3f km\t%4.1f min\t%5.1f km\t%4.0f min\t%s\t\t\t\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                distance_to_km(junc_distance),duration_to_minutes(junc_duration),
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration),
//                                type);

                        T_RoutinoRoute * temp = malloc(sizeof(T_RoutinoRoute));

                        temp->lat = (float)latitude;
                        temp->lon = (float)longitude;
                        temp->dist = (float)cum_distance;
                        temp->time = (float)cum_duration;
                        temp->type = important;
                        temp->turn = turn_int;
                        temp->bearing = next_bearing_int;
                        temp->string = next_waynameraw;

                        route_next->next = temp;
                        route_next = temp;
                        route_next->next = 0;
                    }
                    else        /* middle point */
                    {
//                        fprintf(textfile,"%10.6f\t%11.6f\t%6.3f km\t%4.1f min\t%5.1f km\t%4.0f min\t%s\t %+d\t %+d\t%s\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                distance_to_km(junc_distance),duration_to_minutes(junc_duration),
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration),
//                                type,
//                                (22+turn_int)/45,
//                                ((22+next_bearing_int)/45+4)%8-4,
//                                next_waynameraw);

                        T_RoutinoRoute * temp = malloc(sizeof(T_RoutinoRoute));

                        temp->lat = (float)latitude;
                        temp->lon = (float)longitude;
                        temp->dist = (float)cum_distance;
                        temp->time = (float)cum_duration;
                        temp->type = important;
                        temp->turn = turn_int;
                        temp->bearing = next_bearing_int;
                        temp->string = next_waynameraw;

                        route_next->next = temp;
                        route_next = temp;
                    }
                }

                junc_distance=0;
                junc_duration=0;

                if(roundabout>1)
                {
                    roundabout=0;
                }
            }

            /* Print out all of the results */

            if(wayname && wayname!=waynameraw)
            {
                free(wayname);
            }

            result=next_result;

            if(important>IMP_JUNCT_CONT)
            {
                point_count++;
            }

            first=0;
        }
        while(point==next_point);

        /* Print the end of the segment */

        point=next_point;

        if(result)
        {
            prev_node=result->node;
        }
        else
        {
            prev_node=NO_NODE;
        }
    }
    while(point<nresults);


    return route_first;
}
