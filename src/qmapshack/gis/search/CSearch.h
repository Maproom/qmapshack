/**********************************************************************************************
    Copyright (C) 2019 Henri Hornburg hrnbg@t-online.de

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

#ifndef CSEARCH_H
#define CSEARCH_H

#include <functional>
#include <QList>
#include <QString>

#include "gis/IGisItem.h"
#include "units/IUnit.h"

class IGisItem;

struct searchValue_t //Outside of CSearch to avoid problem of nested type
{
    qreal value1 = NOFLOAT;
    qreal value2 = NOFLOAT;
    QString str1 = "";
    QString str2 = "";
    QString toString() const
    {
        QString str;
        str.append(str1);
        if(value1 != NOFLOAT)
        {
            str.append(QString::number(value1));
        }
        str.append(str2);
        if(value2 != NOFLOAT)
        {
            str.append(QString::number(value2));
        }
        return str;
    }
};

enum searchProperty_e: unsigned int
{
    eSearchPropertyNoMatch,

    //General keywords
    eSearchPropertyGeneralName,
    eSearchPropertyGeneralFullText,
    eSearchPropertyGeneralElevation,

    //Area keywords
    eSearchPropertyAreaArea,

    //Geocache keywords
    eSearchPropertyGeocacheDifficulty,
    eSearchPropertyGeocacheTerrain,
    eSearchPropertyGeocacheAttributes,
    eSearchPropertyGeocacheSize,

    //Waypoint keywords


    //Route / track keywords
    eSearchPropertyRteTrkDistance,
    eSearchPropertyRteTrkAscent,
    eSearchPropertyRteTrkDescent,
    eSearchPropertyRteTrkMinElevation,
    eSearchPropertyRteTrkMaxElevation,
    eSearchPropertyRteTrkMaxSpeed,
    eSearchPropertyRteTrkMinSpeed,
    eSearchPropertyRteTrkAvgSpeed,
};

class CSearch
{
    Q_DECLARE_TR_FUNCTIONS(CSearch)
public:

    enum search_type_e
    {
        eSearchTypeNone,
        eSearchTypeWith,
        eSearchTypeWithout,
        eSearchTypeSmaller,
        eSearchTypeBigger,
        eSearchTypeBetween,
        eSearchTypeEquals,
        eSearchTypeRegEx
    };

    enum search_mode_e
    {
        eSearchModeName,
        eSearchModeText
    };

    struct search_t
    {
        searchProperty_e property;
        search_type_e searchType;
        searchValue_t searchValue;
    };

    CSearch(QString searchstring, search_mode_e searchMode);

    search_type_e getSearchType(QString keyword)
    {
        return keywordSearchTypeMap.value(keyword,eSearchTypeNone);
    }

    static QStringList getSearchTypeKeywords()
    {
        return keywordSearchTypeMap.keys();
    }

    static QStringList getSearchPropertyKeywords()
    {
        return searchPropertyEnumMap.keys();
    }

    bool getSearchResult(IGisItem * item);

private:

    static void adjustUnits(const searchValue_t &itemValue, searchValue_t &searchValue);
    static void improveQuery(search_t& search);

    search_mode_e searchMode;
    QList<search_t> searches;

    static QMap<QString,search_type_e> keywordSearchTypeMap;
    static QMap<QString,search_type_e> initKeywordSearchTypeMap();

    static QMap<QString,searchProperty_e> searchPropertyEnumMap;
    static QMap<QString,searchProperty_e> initSearchPropertyEnumMap();

//First is itemValue, escond is searchValue, which is non const to adjust units
    using fSearch = std::function<bool(const searchValue_t&, searchValue_t&)>;
    static QMap<search_type_e,fSearch > searchTypeLambdaMap;
    static QMap<search_type_e,fSearch > initSearchTypeLambdaMap();
};

#endif // CSEARCH_H
