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

#include "CSearch.h"

Qt::CaseSensitivity CSearch::caseSensitivity = Qt::CaseInsensitive;
CSearch::search_mode_e CSearch::searchMode = CSearch::eSearchModeText;

CSearch::CSearch(QString searchstring)
{
    if(searchstring.simplified().isEmpty())
    {
        return;
    }

    //Detect which comparison keyword in order to later spilt the string at this word.
    QString searchTypeKeyword;
    for(const QString& key:keywordSearchTypeMap.keys())
    {
        if(searchstring.contains(key,Qt::CaseInsensitive))
        {
            searchTypeKeyword=key;
            break;
        }
    }

    search_t newSearch;
    if(searchTypeKeyword.isEmpty())
    {
        //Default to search for what the user typed in the name or the full text
        newSearch.searchType=eSearchTypeWith;
        if(searchMode == eSearchModeText)
        {
            newSearch.property=searchProperty_e::eSearchPropertyGeneralFullText;
        }
        else
        {
            newSearch.property=searchProperty_e::eSearchPropertyGeneralName;
        }
        newSearch.searchValue.str1 = searchstring.simplified();
    }
    else
    {
        newSearch.searchType=keywordSearchTypeMap.value(searchTypeKeyword);
        //Everything before the Search Type keyword is the property, i.e. "date after 2019" would result in "date"
        newSearch.property=searchPropertyEnumMap.value(searchstring.section(searchTypeKeyword,0,0,QString::SectionCaseInsensitiveSeps).simplified(),eSearchPropertyNoMatch);
        //Everything after the Search Type keyword is the value, i.e. "date after 2019" would result in "2019"
        QString filterValueString = searchstring.section(searchTypeKeyword,1,-1,QString::SectionCaseInsensitiveSeps).simplified();
        searchValue_t filterValue;

        //Try if it is a time. Do so first, since this is the most exclusive
        const static QList<QString> timeFormats = {
            QLocale::system().timeFormat(QLocale::LongFormat),
            QLocale::system().timeFormat(QLocale::ShortFormat),
            QLocale::c().timeFormat(QLocale::LongFormat),
            QLocale::c().timeFormat(QLocale::ShortFormat)
        };

        for(const QString& tf:timeFormats)
        {
            QTime time1 = QLocale::system().toTime(filterValueString.section(tr("and"),0,0,QString::SectionCaseInsensitiveSeps).simplified(),tf);
            if(time1.isValid())
            {
                filterValue.value1=time1.msecsSinceStartOfDay()/1000;
                filterValue.str1="S";
                QTime time2 = QLocale::system().toTime(filterValueString.section(tr("and"),1,0,QString::SectionCaseInsensitiveSeps).simplified(),tf);
                if(time1.isValid())
                {
                    filterValue.value2=time2.msecsSinceStartOfDay()/1000;
                    filterValue.str2="S";
                }
                break;
            }
        }

        if(filterValue.toString().isEmpty())
        {
            //Try if it is a date
            const static QList<QString> dateFormats = {
                QLocale::system().dateTimeFormat(QLocale::LongFormat),
                QLocale::system().dateTimeFormat(QLocale::ShortFormat),
                QLocale::c().dateTimeFormat(QLocale::LongFormat),
                QLocale::c().dateTimeFormat(QLocale::ShortFormat),
                QLocale::system().dateFormat(QLocale::LongFormat),
                QLocale::system().dateFormat(QLocale::ShortFormat),
                QLocale::c().dateFormat(QLocale::LongFormat),
                QLocale::c().dateFormat(QLocale::ShortFormat)
            };

            for(const QString& df:dateFormats)
            {
                QDateTime time1 = QLocale::system().toDateTime(filterValueString.section(tr("and"),0,0,QString::SectionCaseInsensitiveSeps).simplified(),df);
                if(time1.isValid())
                {
                    filterValue.value1=time1.toSecsSinceEpoch();
                    filterValue.str1="SsE";
                    QDateTime time2 = QLocale::system().toDateTime(filterValueString.section(tr("and"),1,0,QString::SectionCaseInsensitiveSeps).simplified(),df);
                    if(time2.isValid())
                    {
                        filterValue.value2=time2.toSecsSinceEpoch();
                        filterValue.str2="SsE";
                    }
                    break;
                }
            }
        }

        if(filterValue.toString().isEmpty())
        {
            //Match speeds and distances after dates to have less problems with avoid sorting them out
            const static QString capNum = "(\\d+\\.?\\d*)(?![\\.\\d\\/\\:])";    //Match all numbers making sure no numbers are omitted directly at the end
            const static QString capNumOpt = "(\\d+\\.?\\d*)?(?![\\.\\d\\/\\:])";
            const static QString capIgnWS = "(?:\\s*)";     //Ignore Whitespaces
            //Capture distances, speeds and simple Times that don't get caught by QDateTime.
            const static QString capUnit = "(m|km|mi|ft|ml|m\\/h|km\\/h|mi\\/h|ft\\/h|ml\\/h|h|min|s)?";
            const static QString capIgnAnd =  "(?:" + tr("and") + ")?";
            //The second number, the units and the "and" are optional
            //The String has to be matched completely in order to avoid false positives thus the ^ and the $
            QRegExp numericArguments("^" + capNum + capIgnWS + capUnit  + capIgnWS + capIgnAnd + capIgnWS + capNumOpt + capIgnWS + capUnit + "$",Qt::CaseInsensitive);
            numericArguments.indexIn(filterValueString);
            if(numericArguments.cap(0).simplified() != "")
            {
                if(numericArguments.cap(1) != "")     //to avoid removal of NOFLOAT
                {
                    filterValue.value1=numericArguments.cap(1).toFloat();
                }

                filterValue.str1=numericArguments.cap(2);

                if(numericArguments.cap(3) != "")     //to avoid removal of NOFLOAT
                {
                    filterValue.value2=numericArguments.cap(3).toFloat();
                }

                filterValue.str2=numericArguments.cap(4);
            }
        }
        if(filterValue.toString().isEmpty())
        {
            filterValue.str1 = filterValueString.section(tr("and"),0,0,QString::SectionCaseInsensitiveSeps).simplified();
            filterValue.str2 = filterValueString.section(tr("and"),1,0,QString::SectionCaseInsensitiveSeps).simplified();
        }
        newSearch.searchValue=filterValue;
    }
    improveQuery(newSearch);
    searches.append(newSearch);
}

bool CSearch::getSearchResult(IGisItem *item)
{
    bool passed = true;
    //No const search_t& to avoid multiple unit conversions
    for(search_t& search:searches)
    {
        if(searchTypeLambdaMap.contains(search.searchType))
        {
            const searchValue_t& itemFilterValue = item->getValueByKeyword(search.property);
            passed = searchTypeLambdaMap.value(search.searchType)(itemFilterValue,search.searchValue);
            if(!passed)
            {
                return false;
            }
        }
    }
    return true;
}

void CSearch::adjustUnits(const searchValue_t& itemValue, searchValue_t& searchValue)
{
    if(searchValue.str1 != "" && searchValue.str1 != itemValue.str1)
    {
        IUnit::convert(searchValue.value1,searchValue.str1,itemValue.str1);
    }
    else
    {
        searchValue.str1 = itemValue.str1;
    }

    if(searchValue.str2 != "" && searchValue.str2 != itemValue.str2)
    {
        IUnit::convert(searchValue.value2,searchValue.str2,itemValue.str2);
    }
    else
    {
        searchValue.str2 = itemValue.str2;
    }
}

//Make life easier for the user. The method tries to make assumption on what the user meant
void CSearch::improveQuery(search_t &search)
{
    //If the user entered a number with a unit and another number, assume they have the same unit
    if(search.searchValue.str1 != "" && search.searchValue.str2 == "" && search.searchValue.value1 != NOFLOAT && search.searchValue.value2 != NOFLOAT)
    {
        search.searchValue.str2=search.searchValue.str1;
    }

    //Adjust abbreviations for miles
    if(search.searchValue.value1 != NOFLOAT && search.searchValue.str1.compare("MI",Qt::CaseInsensitive) == 0)
    {
        search.searchValue.str1 = "ML"; //ml is used in CUnit
    }
    if(search.searchValue.value2 != NOFLOAT && search.searchValue.str2.compare("MI",Qt::CaseInsensitive) == 0)
    {
        search.searchValue.str2 = "ML"; //ml is used in CUnit
    }

    //Try to guess what property the user meant when there is no match. I.e. make "shorter than 5km" work
    if(search.property == eSearchPropertyNoMatch)
    {
        if(search.searchValue.str1.contains("/H",Qt::CaseInsensitive) ||
           search.searchValue.str1.contains("/S",Qt::CaseInsensitive))
        {
            search.property = eSearchPropertyRteTrkAvgSpeed;
        }
        else if(search.searchValue.str1.compare("KM",Qt::CaseInsensitive) == 0 ||
                search.searchValue.str1.compare("ML",Qt::CaseInsensitive) == 0)
        {
            search.property = eSearchPropertyRteTrkDistance;
        }
        else if(search.searchValue.str1.compare("M",Qt::CaseInsensitive) == 0 ||
                search.searchValue.str1.compare("FT",Qt::CaseInsensitive) == 0)
        {
            search.property = eSearchPropertyGeneralElevation;
        }
        else if(search.searchValue.str1.compare("S",Qt::CaseInsensitive) == 0 ||
                search.searchValue.str1.compare("MIN",Qt::CaseInsensitive) == 0 ||
                search.searchValue.str1.compare("H",Qt::CaseInsensitive) == 0)
        {
            search.property = eSearchPropertyRteTrkTimeMoving;
        }
        else if(search.searchValue.str1.compare("SsE",Qt::CaseInsensitive) == 0)
        {
            search.property = eSearchPropertyGeneralDate;
        }
    }

    //Searching for dates is error prone, thus some checks to make sure one searche sfor a probable value.
    if(search.property == eSearchPropertyGeneralDate)
    {
        if(search.searchValue.value1 != NOFLOAT)
        {
            //Try to catch if user only entered a year. Not done in regular detecting as it could be a speed or so.
            if(search.searchValue.value1 <= QDateTime::currentDateTime().date().year() && search.searchValue.value1 >= 1970)
            {
                search.searchValue.value1=QDateTime(QDate(search.searchValue.value1,1,1)).toSecsSinceEpoch();
            }
            //Assume you want 2012 and not 1912 (qt defaults to 19xx)
            else if(QDateTime::fromSecsSinceEpoch(search.searchValue.value1).addYears(100) <=  QDateTime::currentDateTime())
            {
                search.searchValue.value1=QDateTime::fromSecsSinceEpoch(search.searchValue.value1).addYears(100).toSecsSinceEpoch();
            }
        }
        if(search.searchValue.value1 != NOFLOAT)
        {
            //Try to catch if user only entered a year. Not done in regular detecting as it could be a speed or so.
            if(search.searchValue.value2 <= QDateTime::currentDateTime().date().year() && search.searchValue.value2 >= 1970)
            {
                search.searchValue.value2=QDateTime(QDate(search.searchValue.value2,0,0)).toSecsSinceEpoch();
            }
            //Assume you want 2012 and not 1912 (qt defaults to 19xx)
            if(QDateTime::fromSecsSinceEpoch(search.searchValue.value2).addYears(100) <=  QDateTime::currentDateTime())
            {
                search.searchValue.value2=QDateTime::fromSecsSinceEpoch(search.searchValue.value2).addYears(100).toSecsSinceEpoch();
            }
        }
    }
}

QMap<QString,CSearch::search_type_e> CSearch::keywordSearchTypeMap = CSearch::initKeywordSearchTypeMap();
QMap<QString,CSearch::search_type_e> CSearch::initKeywordSearchTypeMap()
{
    QMap<QString,search_type_e> map;
    map.insert(tr("with"),eSearchTypeWith);
    map.insert(tr("contains"),eSearchTypeWith);
    map.insert(tr("without"),eSearchTypeWithout);
    map.insert(tr("shorter than"),eSearchTypeSmaller);
    map.insert(tr("smaller than"),eSearchTypeSmaller);
    map.insert(tr("under"),eSearchTypeSmaller);
    map.insert(tr("lower than"),eSearchTypeSmaller);
    map.insert(tr("earlier than"),eSearchTypeSmaller);
    map.insert(tr("before"),eSearchTypeSmaller);
    map.insert(tr("less than"),eSearchTypeSmaller);
    map.insert("<",eSearchTypeSmaller);
    map.insert(tr("longer than"),eSearchTypeBigger);
    map.insert(tr("higher than"),eSearchTypeBigger);
    map.insert(tr("bigger than"),eSearchTypeBigger);
    map.insert(tr("greater than"),eSearchTypeBigger);
    map.insert(tr("above"),eSearchTypeBigger);
    map.insert(tr("over"),eSearchTypeBigger);
    map.insert(tr("after"),eSearchTypeBigger);
    map.insert(tr("later than"),eSearchTypeBigger);
    map.insert(">",eSearchTypeBigger);
    map.insert(tr("regex"),eSearchTypeRegEx);
    map.insert("=",eSearchTypeEquals);
    map.insert(tr("equals"),eSearchTypeEquals);
    map.insert(tr("between"),eSearchTypeBetween);
    return map;
}

QMap<QString,searchProperty_e> CSearch::searchPropertyEnumMap = CSearch::initSearchPropertyEnumMap();
QMap<QString,searchProperty_e> CSearch::initSearchPropertyEnumMap()
{
    QMap<QString,searchProperty_e> map;
    //General keywords
    map.insert(tr("name"),eSearchPropertyGeneralName);
    map.insert(tr("full text"),eSearchPropertyGeneralFullText);
    map.insert(tr("elevation"),eSearchPropertyGeneralElevation);
    map.insert(tr("date"),eSearchPropertyGeneralDate);
    map.insert(tr("comment"),eSearchPropertyGeneralComment);
    map.insert(tr("description"),eSearchPropertyGeneralDescription);

    //Area keywords
    map.insert(tr("area"),eSearchPropertyAreaArea);

    //Geocache keywords
    map.insert(tr("difficulty"),eSearchPropertyGeocacheDifficulty);
    map.insert("D",eSearchPropertyGeocacheDifficulty);
    map.insert(tr("terrain"),eSearchPropertyGeocacheTerrain);
    map.insert(tr("T"),eSearchPropertyGeocacheTerrain);
    map.insert(tr("attributes"),eSearchPropertyGeocacheAttributes);
    map.insert(tr("size"),eSearchPropertyGeocacheSize);
    map.insert(tr("GCCode"),eSearchPropertyGeocacheGCCode);

    //Waypoint keywords

    //Route / track keywords
    map.insert(tr("distance"),eSearchPropertyRteTrkDistance);
    map.insert(tr("length"),eSearchPropertyRteTrkDistance);
    map.insert(tr("ascent"),eSearchPropertyRteTrkAscent);
    map.insert(tr("elevation gain"),eSearchPropertyRteTrkAscent);
    map.insert(tr("descent"),eSearchPropertyRteTrkDescent);
    map.insert(tr("min elevation"),eSearchPropertyRteTrkMinElevation);
    map.insert(tr("minimal elevation"),eSearchPropertyRteTrkMinElevation);
    map.insert(tr("max elevation"),eSearchPropertyRteTrkMaxElevation);
    map.insert(tr("maximal elevation"),eSearchPropertyRteTrkMaxElevation);
    map.insert(tr("max speed"),eSearchPropertyRteTrkMaxSpeed);
    map.insert(tr("maximal speed"),eSearchPropertyRteTrkMaxSpeed);
    map.insert(tr("min speed"),eSearchPropertyRteTrkMinSpeed);
    map.insert(tr("minimal speed"),eSearchPropertyRteTrkMinSpeed);
    map.insert(tr("average speed"),eSearchPropertyRteTrkAvgSpeed);
    map.insert(tr("activity"),eSearchPropertyRteTrkActivity);
    map.insert(tr("total time"),eSearchPropertyRteTrkTotalTime);
    map.insert(tr("duration"),eSearchPropertyRteTrkTotalTime);
    map.insert(tr("time moving"),eSearchPropertyRteTrkTimeMoving);

    return map;
}

QMap<searchProperty_e,QString> CSearch::searchPropertyMeaningMap = CSearch::initSearchPropertyMeaningMap();
QMap<searchProperty_e,QString> CSearch::initSearchPropertyMeaningMap()
{
    QMap<searchProperty_e,QString> map;
    //General keywords
    map.insert(eSearchPropertyGeneralName, tr("searches the name"));
    map.insert(eSearchPropertyGeneralFullText, tr("searches the full text"));
    map.insert(eSearchPropertyGeneralElevation, tr("searches the elevation. For items consisting of multiple points the minimum and the maximum is used"));
    map.insert(eSearchPropertyGeneralDate, tr("searches the Date"));
    map.insert(eSearchPropertyGeneralComment, tr("searches the Comment"));
    map.insert(eSearchPropertyGeneralDescription, tr("searches the Description"));

    //Area keywords
    map.insert(eSearchPropertyAreaArea, tr("searches the area"));

    //Geocache keywords
    map.insert(eSearchPropertyGeocacheDifficulty, tr("searches the difficulty rating of a geocache"));
    map.insert(eSearchPropertyGeocacheTerrain, tr("searches the terrain rating of a geocache"));
    map.insert(eSearchPropertyGeocacheAttributes, tr("searches the translated meanings of the attributes"));
    map.insert(eSearchPropertyGeocacheSize, tr("searches the size of a geocache. (micro, small, regular, large)"));
    map.insert(eSearchPropertyGeocacheGCCode, tr("searches the GCCode of a geocache."));

    //Waypoint keywords

    //Route / track keywords
    map.insert(eSearchPropertyRteTrkDistance, tr("searches the distance covered by a route or track"));
    map.insert(eSearchPropertyRteTrkAscent, tr("searches the total ascent in a route or track"));
    map.insert(eSearchPropertyRteTrkDescent, tr("searches the total descent in a route or track"));
    map.insert(eSearchPropertyRteTrkMinElevation, tr("searches the minimal elevation in a route or track"));
    map.insert(eSearchPropertyRteTrkMaxElevation, tr("searches the maximal elevation in a route or track"));
    map.insert(eSearchPropertyRteTrkMaxSpeed, tr("searches the maximal speed in a route or track"));
    map.insert(eSearchPropertyRteTrkMinSpeed, tr("searches the minimal speed in a route or track"));
    map.insert(eSearchPropertyRteTrkAvgSpeed, tr("searches the average speed in a route or track"));
    map.insert(eSearchPropertyRteTrkActivity, tr("searches the activity of a route or track"));
    map.insert(eSearchPropertyRteTrkTotalTime, tr("searches the total time spent on a route or track"));
    map.insert(eSearchPropertyRteTrkTimeMoving, tr("searches the time spent moving on a route or track"));

    return map;
}

QMap<CSearch::search_type_e, CSearch::fSearch> CSearch::searchTypeLambdaMap = CSearch::initSearchTypeLambdaMap();
QMap<CSearch::search_type_e, CSearch::fSearch> CSearch::initSearchTypeLambdaMap()
{
    QMap<CSearch::search_type_e, CSearch::fSearch> map;
    map.insert(eSearchTypeEquals, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        return itemValue.toString() == searchValue.toString();
    });
    map.insert(eSearchTypeSmaller, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT && itemValue.value1 != NOFLOAT)
        {
            adjustUnits(itemValue, searchValue);
            if(itemValue.value2 == NOFLOAT)
            {
                return itemValue.value1 < searchValue.value1;
            }
            else
            {
                return qMax(itemValue.value1,itemValue.value2) < searchValue.value1;
            }
        }
        return false;
    });
    map.insert(eSearchTypeBigger, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT && itemValue.value1 != NOFLOAT)
        {
            adjustUnits(itemValue, searchValue);
            if(itemValue.value2 == NOFLOAT)
            {
                return itemValue.value1 > searchValue.value1;
            }
            else
            {
                return qMin(itemValue.value1,itemValue.value2) > searchValue.value1;
            }
        }
        return false;
    });

    map.insert(eSearchTypeBetween, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT && itemValue.value1 != NOFLOAT && searchValue.value2 != NOFLOAT && itemValue.value2 != NOFLOAT)
        {
            adjustUnits(itemValue, searchValue);
            if(itemValue.value2 == NOFLOAT)
            {
                return itemValue.value1 < qMax(searchValue.value1,searchValue.value2) && itemValue.value1 > qMin(searchValue.value1,searchValue.value2);
            }
            else
            {
                return qMax(itemValue.value1,itemValue.value2) < qMax(searchValue.value1,searchValue.value2) && qMin(itemValue.value1,itemValue.value2) > qMin(searchValue.value1,searchValue.value2);
            }
        }
        return false;
    });
    map.insert(eSearchTypeWith, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        return itemValue.toString().contains(searchValue.toString(), CSearch::caseSensitivity);
    });
    map.insert(eSearchTypeWithout, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        return !itemValue.toString().contains(searchValue.toString(), CSearch::caseSensitivity);
    });
    map.insert(eSearchTypeRegEx, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if(CSearch::caseSensitivity == Qt::CaseInsensitive)//There is no option to make regex caseinsensitive
        {
            return itemValue.toString().toLower().contains(QRegExp(searchValue.toString().toLower()));
        }
        else
        {
            return itemValue.toString().contains(QRegExp(searchValue.toString()));
        }
    });
    return map;
}
