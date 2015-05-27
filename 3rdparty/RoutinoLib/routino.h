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
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct T_DataSet * H_RoutinoDataSet;

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

