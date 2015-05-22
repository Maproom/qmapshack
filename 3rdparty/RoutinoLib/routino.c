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

static int isInitialized = 0;

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

