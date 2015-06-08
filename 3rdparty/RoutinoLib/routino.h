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

#ifndef ROUTINO_H
#define ROUTINO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

typedef struct T_DataSet * H_RoutinoDataSet;

typedef struct _T_RoutinoRoute
{
    struct _T_RoutinoRoute * next;
    float lon;
    float lat;
    float dist;
    float time;
    uint32_t type;
    uint32_t turn;
    uint32_t bearing;

}T_RoutinoRoute;

/**
   @brief Initialize Routino library.

   This has to be called once on program startup.

   @param profiles      a string pointer to the XML file defining the profiles
   @param translations  a string pointer to the XML file defining the translation tables
   @return Returns 0 on success and -1 on failure
 */
extern int RoutinoInit(const char * profiles, const char *translations);

/**
   @brief RoutinoRegisterData
   @param dirname
   @param prefix
   @return
 */
extern H_RoutinoDataSet RoutinoRegisterData(const char *dirname, const char * prefix);


extern T_RoutinoRoute * RoutinoCalculate(H_RoutinoDataSet data, const char * profilename, const float * lon, const float * lat, int nCoord);

extern void RoutinoFreeRoute(T_RoutinoRoute * route);

/**
   @brief RoutinoFreeData
   @param data
 */
extern void RoutinoFreeData(H_RoutinoDataSet data);

/**
   @brief Release Routino library

   This has to be called once when releasing the library.

   @return Return 0.
 */
extern int RoutinoRelease();

#ifdef __cplusplus
}
#endif
#endif //ROUTINO_H

