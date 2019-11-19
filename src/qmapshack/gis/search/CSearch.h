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
    QString toString(bool lowerCase = false) const
    {
        QString str;
        if(value1 != NOFLOAT)
        {
            str.append(QString::number(value1));
        }
        str.append(str1);
        if(value2 != NOFLOAT)
        {
            str.append(QString::number(value2));
        }
        str.append(str2);

        if(lowerCase)
        {
            return str.toLower();
        }
        else
        {
            return str;
        }
    }
};

enum searchProperty_e: unsigned int
{
    eSearchPropertyNoMatch,

    //General keywords
    eSearchPropertyGeneralName,
    eSearchPropertyGeneralFullText,
    eSearchPropertyGeneralElevation,
    eSearchPropertyGeneralDate,
    eSearchPropertyGeneralComment,
    eSearchPropertyGeneralDescription,

    //Area keywords
    eSearchPropertyAreaArea,

    //Geocache keywords
    eSearchPropertyGeocacheDifficulty,
    eSearchPropertyGeocacheTerrain,
    eSearchPropertyGeocachePositiveAttributes,
    eSearchPropertyGeocacheNegatedAttributes,
    eSearchPropertyGeocacheSize,
    eSearchPropertyGeocacheGCCode,
    eSearchPropertyGeocacheGCName,

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
    eSearchPropertyRteTrkActivity,
    eSearchPropertyRteTrkTotalTime,
    eSearchPropertyRteTrkTimeMoving
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

    CSearch(QString searchstring);

    search_type_e getSearchType(QString keyword)
    {
        return keywordSearchTypeMap.value(keyword, eSearchTypeNone);
    }

    bool getSyntaxError()
    {
        return syntaxError;
    }

    bool isAutodetectedProperty()
    {
        return autoDetectedProperty;
    }

    const QString& getSearchText()
    {
        return searchText;
    }

    static QStringList getSearchTypeKeywords()
    {
        return keywordSearchTypeMap.keys();
    }

    static QString getSearchTypeExample(QString searchType)
    {
        return keywordSearchExampleMap.value(searchType, tr("No information available"));
    }

    static QStringList getSearchPropertyKeywords()
    {
        return searchPropertyEnumMap.keys();
    }

    static QString getSearchPropertyMeaning(QString property)
    {
        return searchPropertyMeaningMap.value(searchPropertyEnumMap.value(property), tr("No information available"));
    }

    bool getSearchResult(IGisItem * item);

    static Qt::CaseSensitivity getCaseSensitivity()
    {
        return caseSensitivity;
    }
    static void setCaseSensitivity(const Qt::CaseSensitivity &value)
    {
        caseSensitivity=value;
    }

    static search_mode_e getSearchMode()
    {
        return searchMode;
    }
    static void setSearchMode(const search_mode_e &value)
    {
        searchMode = value;
    }

    static void init()
    {
        searchPropertyMeaningMap = initSearchPropertyMeaningMap();
        searchPropertyEnumMap = initSearchPropertyEnumMap();
        keywordSearchExampleMap = initKeywordSearchExampleMap();
        keywordSearchTypeMap = initKeywordSearchTypeMap();
    }

private:

    static Qt::CaseSensitivity caseSensitivity;
    static search_mode_e searchMode;

    bool adjustUnits(const searchValue_t &itemValue, searchValue_t &searchValue);
    void improveQuery();

    search_t search;
    QString searchText;
    bool syntaxError = false;
    bool autoDetectedProperty = false;

    static QMap<QString, search_type_e> keywordSearchTypeMap;
    static QMap<QString, search_type_e> initKeywordSearchTypeMap();

    static QMap<QString, QString> keywordSearchExampleMap;
    static QMap<QString, QString> initKeywordSearchExampleMap();

    static QMap<QString, searchProperty_e> searchPropertyEnumMap;
    static QMap<QString, searchProperty_e> initSearchPropertyEnumMap();

    static QMap<searchProperty_e, QString> searchPropertyMeaningMap;
    static QMap<searchProperty_e, QString> initSearchPropertyMeaningMap();

    //First is itemValue, second is searchValue, which is non const to adjust units
    using fSearch = std::function<bool (const searchValue_t&, searchValue_t&)>;
    QMap<search_type_e, fSearch > searchTypeLambdaMap;
    QMap<search_type_e, fSearch > initSearchTypeLambdaMap();
};

#endif // CSEARCH_H
