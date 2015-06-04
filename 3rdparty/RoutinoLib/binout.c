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
#include "types.h"
#include "profiles.h"
#include "results.h"
#include "fakes.h"
#include "nodes.h"
#include "segments.h"
#include "ways.h"
#include "translations.h"
#include "xmlparse.h"

#include <stdlib.h>

/* Constants */

#define IMP_IGNORE      -1      /*+ Ignore this point. +*/
#define IMP_UNIMPORTANT  0      /*+ An unimportant, intermediate, node. +*/
#define IMP_RB_NOT_EXIT  1      /*+ A roundabout exit that is not taken. +*/
#define IMP_JUNCT_CONT   2      /*+ An un-interesting junction where the route continues without comment. +*/
#define IMP_CHANGE       3      /*+ The highway changes type but nothing else happens. +*/
#define IMP_JUNCT_IMPORT 4      /*+ An interesting junction to be described. +*/
#define IMP_RB_ENTRY     5      /*+ The entrance to a roundabout. +*/
#define IMP_RB_EXIT      6      /*+ The exit from a roundabout. +*/
#define IMP_MINI_RB      7      /*+ The location of a mini-roundabout. +*/
#define IMP_UTURN        8      /*+ The location of a U-turn. +*/
#define IMP_WAYPOINT     9      /*+ A waypoint. +*/

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
void SimplifyResult(Results ** results, int nresults, Nodes *nodes, Segments *segments, Ways *ways, Profile *profile)
{
    char *prev_bearing=NULL,*prev_wayname=NULL;
    index_t prev_node=NO_NODE;
    distance_t cum_distance=0;
    duration_t cum_duration=0;

    int point=0;
    int segment_count=0,route_count=0;
    int point_count=0;
    int roundabout=0;



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
            speed_t seg_speed=0;
            char *waynameraw=NULL,*wayname=NULL,*next_waynameraw=NULL,*next_wayname=NULL;
            int bearing_int=0,turn_int=0,next_bearing_int=0;
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

            if(!first)
            {
                waynameraw=WayName(ways,resultwayp);
                if(!*waynameraw)
                {
                    waynameraw=translate_raw_highway[HIGHWAY(resultwayp->type)];
                }

                bearing_int=(int)BearingAngle(nodes,resultsegmentp,result->node);

                seg_speed=profile->speed[HIGHWAY(resultwayp->type)];
            }

            if(next_result && important>IMP_JUNCT_CONT)
            {
                if(!first)
                {
                    if(DISTANCE(resultsegmentp->distance)==0 || DISTANCE(next_resultsegmentp->distance)==0)
                    {
                        turn_int=0;
                    }
                    else
                    {
                        turn_int=(int)TurnAngle(nodes,resultsegmentp,next_resultsegmentp,result->node);
                    }

                    turn=translate_xml_turn[((202+turn_int)/45)%8];
                }

//                if(gpxroutefile || htmlfile)
                {
                    next_waynameraw=WayName(ways,next_resultwayp);
                    if(!*next_waynameraw)
                    {
                        next_waynameraw=translate_raw_highway[HIGHWAY(next_resultwayp->type)];
                    }

                    next_wayname=ParseXML_Encode_Safe_XML(next_waynameraw);
                }

//                if(htmlfile || gpxroutefile || textfile)
                {
                    if(!first && DISTANCE(next_resultsegmentp->distance)==0)
                    {
                        next_bearing_int=(int)BearingAngle(nodes,resultsegmentp,result->node);
                    }
                    else
                    {
                        next_bearing_int=(int)BearingAngle(nodes,next_resultsegmentp,next_result->node);
                    }

                    next_bearing=translate_xml_heading[(4+(22+next_bearing_int)/45)%8];
                }
            }

            /* Print out the important points (junctions / waypoints) */

            if(important>IMP_JUNCT_CONT)
            {
//                if(htmlfile)
//                {
//                    char *type;

//                    if(important==IMP_WAYPOINT)
//                    {
//                        type=translate_html_waypoint;
//                    }
//                    else if(important==IMP_MINI_RB)
//                    {
//                        type=translate_html_roundabout;
//                    }
//                    else
//                    {
//                        type=translate_html_junction;
//                    }

//                    if(point_count>0) /* not the first point */
//                    {
//                        /* <tr class='s'><td class='l'>Follow:<td class='r'><span class='h'>*highway name*</span> for <span class='d'>*distance* km, *time* min</span> [<span class='j'>*distance* km, *time* minutes</span>] */
//                        fprintf(htmlfile,"<tr class='s'><td class='l'>%s:<td class='r'>",translate_html_segment[0]);
//                        fprintf(htmlfile,translate_html_segment[1],
//                                (roundabout>1 ? translate_html_roundabout : prev_wayname),
//                                distance_to_km(junc_distance),duration_to_minutes(junc_duration));
//                        fprintf(htmlfile," [<span class='j'>");
//                        fprintf(htmlfile,translate_html_total[1],
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration));
//                        fprintf(htmlfile,"</span>]\n");
//                    }

//                    /* <tr class='c'><td class='l'>*N*:<td class='r'>*latitude* *longitude* */
//                    fprintf(htmlfile,"<tr class='c'><td class='l'>%d:<td class='r'>%.6f %.6f\n",
//                            point_count+1,
//                            radians_to_degrees(latitude),radians_to_degrees(longitude));

//                    if(point_count==0) /* first point */
//                    {
//                        /* <tr class='n'><td class='l'>Start:<td class='r'>At <span class='w'>Waypoint</span>, head <span class='b'>*heading*</span> */
//                        fprintf(htmlfile,"<tr class='n'><td class='l'>%s:<td class='r'>",translate_html_start[0]);
//                        fprintf(htmlfile,translate_html_start[1],
//                                translate_html_waypoint,
//                                next_bearing);
//                        fprintf(htmlfile,"\n");
//                    }
//                    else if(next_result) /* middle point */
//                    {
//                        if(roundabout>1 && important!=IMP_WAYPOINT)
//                        {
//                            /* <tr class='n'><td class='l'>At:<td class='r'>Roundabout, take <span class='t'>the *Nth* exit</span> heading <span class='b'>*heading*</span> */
//                            fprintf(htmlfile,"<tr class='n'><td class='l'>%s:<td class='r'>",translate_html_rbnode[0]);
//                            fprintf(htmlfile,translate_html_rbnode[1],
//                                    translate_html_roundabout,
//                                    translate_xml_ordinal[roundabout-2],
//                                    next_bearing);
//                            fprintf(htmlfile,"\n");
//                        }
//                        else
//                        {
//                            /* <tr class='n'><td class='l'>At:<td class='r'>Junction, go <span class='t'>*direction*</span> heading <span class='b'>*heading*</span> */
//                            fprintf(htmlfile,"<tr class='n'><td class='l'>%s:<td class='r'>",translate_html_node[0]);
//                            fprintf(htmlfile,translate_html_node[1],
//                                    type,
//                                    turn,
//                                    next_bearing);
//                            fprintf(htmlfile,"\n");
//                        }
//                    }
//                    else     /* end point */
//                    {
//                        /* <tr class='n'><td class='l'>Stop:<td class='r'>At <span class='w'>Waypoint</span> */
//                        fprintf(htmlfile,"<tr class='n'><td class='l'>%s:<td class='r'>",translate_html_stop[0]);
//                        fprintf(htmlfile,translate_html_stop[1],
//                                translate_html_waypoint);
//                        fprintf(htmlfile,"\n");

//                        /* <tr class='t'><td class='l'>Total:<td class='r'><span class='j'>*distance* km, *time* minutes</span> */
//                        fprintf(htmlfile,"<tr class='t'><td class='l'>%s:<td class='r'><span class='j'>",translate_html_total[0]);
//                        fprintf(htmlfile,translate_html_total[1],
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration));
//                        fprintf(htmlfile,"</span>\n");
//                    }
//                }

//                if(gpxroutefile)
//                {
//                    if(point_count>0) /* not first point */
//                    {
//                        fprintf(gpxroutefile,"<desc>");
//                        fprintf(gpxroutefile,translate_gpx_step,
//                                prev_bearing,
//                                prev_wayname,
//                                distance_to_km(junc_distance),duration_to_minutes(junc_duration));
//                        fprintf(gpxroutefile,"</desc></rtept>\n");
//                    }

//                    if(point_count==0) /* first point */
//                    {
//                        fprintf(gpxroutefile,"<rtept lat=\"%.6f\" lon=\"%.6f\"><name>%s</name>\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                translate_gpx_start);
//                    }
//                    else if(!next_result) /* end point */
//                    {
//                        fprintf(gpxroutefile,"<rtept lat=\"%.6f\" lon=\"%.6f\"><name>%s</name>\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                translate_gpx_finish);
//                        fprintf(gpxroutefile,"<desc>");
//                        fprintf(gpxroutefile,translate_gpx_final,
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration));
//                        fprintf(gpxroutefile,"</desc></rtept>\n");
//                    }
//                    else     /* middle point */
//                    {
//                        if(important==IMP_WAYPOINT)
//                        {
//                            fprintf(gpxroutefile,"<rtept lat=\"%.6f\" lon=\"%.6f\"><name>%s%d</name>\n",
//                                    radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                    translate_gpx_inter,++segment_count);
//                        }
//                        else
//                        {
//                            fprintf(gpxroutefile,"<rtept lat=\"%.6f\" lon=\"%.6f\"><name>%s%03d</name>\n",
//                                    radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                    translate_gpx_trip,++route_count);
//                        }
//                    }
//                }

//                if(textfile)
//                {
//                    char *type;

//                    if(important==IMP_WAYPOINT)
//                    {
//                        type="Waypt";
//                    }
//                    else
//                    {
//                        type="Junct";
//                    }

//                    if(point_count==0) /* first point */
//                    {
//                        fprintf(textfile,"%10.6f\t%11.6f\t%6.3f km\t%4.1f min\t%5.1f km\t%4.0f min\t%s\t\t %+d\t%s\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                0.0,0.0,0.0,0.0,
//                                type,
//                                ((22+next_bearing_int)/45+4)%8-4,
//                                next_waynameraw);
//                    }
//                    else if(!next_result) /* end point */
//                    {
//                        fprintf(textfile,"%10.6f\t%11.6f\t%6.3f km\t%4.1f min\t%5.1f km\t%4.0f min\t%s\t\t\t\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                distance_to_km(junc_distance),duration_to_minutes(junc_duration),
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration),
//                                type);
//                    }
//                    else        /* middle point */
//                    {
//                        fprintf(textfile,"%10.6f\t%11.6f\t%6.3f km\t%4.1f min\t%5.1f km\t%4.0f min\t%s\t %+d\t %+d\t%s\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                distance_to_km(junc_distance),duration_to_minutes(junc_duration),
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration),
//                                type,
//                                (22+turn_int)/45,
//                                ((22+next_bearing_int)/45+4)%8-4,
//                                next_waynameraw);
//                    }
//                }

                junc_distance=0;
                junc_duration=0;

//                if(htmlfile || gpxroutefile)
                {
                    if(prev_wayname)
                    {
                        free(prev_wayname);
                    }

                    if(next_wayname)
                    {
                        prev_wayname=strcpy((char*)malloc(strlen(next_wayname)+1),next_wayname);
                    }
                    else
                    {
                        prev_wayname=NULL;
                    }

                    if(next_wayname && next_wayname!=next_waynameraw)
                    {
                        free(next_wayname);
                    }
                }

//                if(gpxroutefile)
                {
                    prev_bearing=next_bearing;
                }

                if(roundabout>1)
                {
                    roundabout=0;
                }
            }

            /* Print out all of the results */

//            if(gpxtrackfile)
//            {
//                fprintf(gpxtrackfile,"<trkpt lat=\"%.6f\" lon=\"%.6f\"/>\n",
//                        radians_to_degrees(latitude),radians_to_degrees(longitude));
//            }

            if(important>IMP_IGNORE)
            {
//                if(textallfile)
//                {
//                    char *type;

//                    if(important==IMP_WAYPOINT)
//                    {
//                        type="Waypt";
//                    }
//                    else if(important==IMP_UTURN)
//                    {
//                        type="U-turn";
//                    }
//                    else if(important==IMP_MINI_RB)
//                    {
//                        type="Mini-RB";
//                    }
//                    else if(important==IMP_CHANGE)
//                    {
//                        type="Change";
//                    }
//                    else if(important==IMP_JUNCT_CONT || important==IMP_RB_NOT_EXIT)
//                    {
//                        type="Junct-";
//                    }
//                    else if(important==IMP_UNIMPORTANT)
//                    {
//                        type="Inter";
//                    }
//                    else
//                    {
//                        type="Junct";
//                    }

//                    if(point_count==0) /* first point */
//                    {
//                        fprintf(textallfile,"%10.6f\t%11.6f\t%8d%c\t%s\t%5.3f\t%5.2f\t%5.2f\t%5.1f\t\t\t\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                IsFakeNode(result->node) ? (NODE_FAKE-result->node) : result->node,
//                                (resultnodep && IsSuperNode(resultnodep)) ? '*' : ' ',type,
//                                0.0,0.0,0.0,0.0);
//                    }
//                    else        /* not the first point */
//                    {
//                        fprintf(textallfile,"%10.6f\t%11.6f\t%8d%c\t%s\t%5.3f\t%5.2f\t%5.2f\t%5.1f\t%3d\t%4d\t%s\n",
//                                radians_to_degrees(latitude),radians_to_degrees(longitude),
//                                IsFakeNode(result->node) ? (NODE_FAKE-result->node) : result->node,
//                                (resultnodep && IsSuperNode(resultnodep)) ? '*' : ' ',type,
//                                distance_to_km(seg_distance),duration_to_minutes(seg_duration),
//                                distance_to_km(cum_distance),duration_to_minutes(cum_duration),
//                                speed_to_kph(seg_speed),
//                                bearing_int,
//                                waynameraw);
//                    }
//                }
            }

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


}
