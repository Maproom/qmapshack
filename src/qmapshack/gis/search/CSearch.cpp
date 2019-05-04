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

CSearch::CSearch(QString searchstring, CSearch::search_mode_e searchMode)
    : searchMode(searchMode)
{
    const QStringList& sections = searchstring.split(",");
    for(const QString& currentSection : sections)
    {
        if(currentSection.simplified().isEmpty())
        {
            continue;
        }

        QString searchTypeKeyword;
        for(QString& key:keywordSearchTypeMap.keys())
        {
            if(currentSection.contains(key,Qt::CaseInsensitive))
            {
                searchTypeKeyword=key;
                break;
            }
        }
        search_t newSearch;
        if(searchTypeKeyword.isEmpty())
        {
            //Search for what the user typed in the name or the full text
            newSearch.searchType=eSearchTypeWith;
            if(searchMode == eSearchModeText)
            {
                newSearch.property=searchProperty_e::eSearchPropertyGeneralFullText;
            }
            else
            {
                newSearch.property=searchProperty_e::eSearchPropertyGeneralName;
            }
            newSearch.searchValue.str1 = currentSection.simplified();
        }
        else
        {
            newSearch.searchType=keywordSearchTypeMap.value(searchTypeKeyword);
            newSearch.property=searchPropertyEnumMap.value(currentSection.section(searchTypeKeyword,0,0,QString::SectionCaseInsensitiveSeps).simplified(),eSearchPropertyNoMatch);

            QString filterValueString = currentSection.section(searchTypeKeyword,1,-1,QString::SectionCaseInsensitiveSeps).simplified();
            searchValue_t filterValue;

            //Try if it is a time
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
                //Match speeds and distances
                const static QString capNum = "(?:[^\\.\\d\\/\\:])(\\d+\\.?\\d*)(?![\\.\\d\\/\\:])";
                const static QString capNumOpt = "(?:[^\\.\\d\\/\\:])(\\d+\\.?\\d*)?(?![\\.\\d\\/\\:])";
                const static QString capIgnWS = "(?:\\s*)"; //Ignore Whitespaces
                //Capture only distances and speeds. Times get handled by QDateTime. QT does not support lookbehind
                const static QString capUnit = "(m|km|mi|ft|ml|m\\/h|km\\/h|mi\\/h|ft\\/h|ml\\/h|h|min|s)?";
                const static QString capIgnAnd =  "(?:" + tr("and") + ")?";
                //The second number, the units and the "and" are optional
                QRegExp numericArguments(capNum + capIgnWS + capUnit  + capIgnWS + capIgnAnd + capIgnWS + capNumOpt + capIgnWS + capUnit,Qt::CaseInsensitive);
                //Prepend whitespace to make sure regex works, as there is no lookbehind in qt
                numericArguments.indexIn(filterValueString.prepend(' ').append(' '));
                if(numericArguments.cap(0).simplified() != "")
                {
                    if(numericArguments.cap(1) != "") //to avoid removal of NOFLOAT
                    {
                        filterValue.value1=numericArguments.cap(1).toFloat();
                    }

                    filterValue.str1=numericArguments.cap(2);

                    if(numericArguments.cap(3) != "") //to avoid removal of NOFLOAT
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
}

bool CSearch::getSearchResult(IGisItem *item)
{
    bool passed = true;
    //No const search_t& to avoid multiple unit conversions
    for(search_t& search:searches)
    {
        if(searchTypeLambdaMap.contains(search.searchType))
        {
            searchValue_t itemFilterValue = *((item)->getValueByKeyword(search.property));
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

void CSearch::improveQuery(search_t &search)
{
    if(search.searchValue.str1 != "" && search.searchValue.str2 == "" && search.searchValue.value1 != NOFLOAT)
    {
        //Assume they have the same unit
        search.searchValue.str2=search.searchValue.str1;
    }

    if(search.searchValue.str1 == "MI")
    {
        search.searchValue.str1 = "ML"; //ml is used in CUnit
    }

    if(search.searchValue.str2 == "MI")
    {
        search.searchValue.str2 = "ML"; //ml is used in CUnit
    }

    if(search.property == eSearchPropertyNoMatch)
    {
        if(search.searchValue.str1.contains("/H") || search.searchValue.str1.contains("/S"))
        {
            search.property = eSearchPropertyRteTrkAvgSpeed;
        }
        else if(search.searchValue.str1 == "KM" || search.searchValue.str1 == "MI" || search.searchValue.str1 == "ML")
        {
            search.property = eSearchPropertyRteTrkDistance;
        }
        else if(search.searchValue.str1 == "M" || search.searchValue.str1 == "FT")
        {
            search.property = eSearchPropertyGeneralElevation;
        }
        else if(search.searchValue.str1 == "S")
        {
            search.property = eSearchPropertyRteTrkTimeMoving;
        }
        else if(search.searchValue.str1 == "SsE")
        {
            search.property = eSearchPropertyGeneralDate;
        }
    }

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
    return map;
}

QMap<QString,searchProperty_e> CSearch::searchPropertyEnumMap = CSearch::initSearchPropertyEnumMap();
QMap<QString,searchProperty_e> CSearch::initSearchPropertyEnumMap()
{
    //Everything to upper since search comes in CAPS and we can't get value() case insensitive easily
    QMap<QString,searchProperty_e> map;
    //General keywords
    map.insert(tr("name").toUpper(),eSearchPropertyGeneralName);
    map.insert(tr("full text").toUpper(),eSearchPropertyGeneralFullText);
    map.insert(tr("elevation").toUpper(),eSearchPropertyGeneralElevation);
    map.insert(tr("date").toUpper(),eSearchPropertyGeneralDate);
    map.insert(tr("comment").toUpper(),eSearchPropertyGeneralComment);
    map.insert(tr("description").toUpper(),eSearchPropertyGeneralDescription);

    //Area keywords
    map.insert(tr("area").toUpper(),eSearchPropertyAreaArea);

    //Geocache keywords
    map.insert(tr("difficulty").toUpper(),eSearchPropertyGeocacheDifficulty);
    map.insert("D",eSearchPropertyGeocacheDifficulty);
    map.insert(tr("terrain").toUpper(),eSearchPropertyGeocacheTerrain);
    map.insert(tr("T").toUpper(),eSearchPropertyGeocacheTerrain);
    map.insert(tr("attributes").toUpper(),eSearchPropertyGeocacheAttributes);
    map.insert(tr("size").toUpper(),eSearchPropertyGeocacheSize);

    //Waypoint keywords

    //Route / track keywords
    map.insert(tr("distance").toUpper(),eSearchPropertyRteTrkDistance);
    map.insert(tr("length").toUpper(),eSearchPropertyRteTrkDistance);
    map.insert(tr("ascent").toUpper(),eSearchPropertyRteTrkAscent);
    map.insert(tr("elevation gain").toUpper(),eSearchPropertyRteTrkAscent);
    map.insert(tr("descent").toUpper(),eSearchPropertyRteTrkDescent);
    map.insert(tr("min elevation").toUpper(),eSearchPropertyRteTrkMinElevation);
    map.insert(tr("minimal elevation").toUpper(),eSearchPropertyRteTrkMinElevation);
    map.insert(tr("max elevation").toUpper(),eSearchPropertyRteTrkMaxElevation);
    map.insert(tr("maximal elevation").toUpper(),eSearchPropertyRteTrkMaxElevation);
    map.insert(tr("max speed").toUpper(),eSearchPropertyRteTrkMaxSpeed);
    map.insert(tr("maximal speed").toUpper(),eSearchPropertyRteTrkMaxSpeed);
    map.insert(tr("min speed").toUpper(),eSearchPropertyRteTrkMinSpeed);
    map.insert(tr("minimal speed").toUpper(),eSearchPropertyRteTrkMinSpeed);
    map.insert(tr("average speed").toUpper(),eSearchPropertyRteTrkAvgSpeed);
    map.insert(tr("activity").toUpper(),eSearchPropertyRteTrkActivity);
    map.insert(tr("total time").toUpper(),eSearchPropertyRteTrkTotalTime);
    map.insert(tr("duration").toUpper(),eSearchPropertyRteTrkTotalTime);
    map.insert(tr("time moving").toUpper(),eSearchPropertyRteTrkTimeMoving);

    return map;
}

QMap<CSearch::search_type_e, CSearch::fSearch> CSearch::searchTypeLambdaMap = CSearch::initSearchTypeLambdaMap();
QMap<CSearch::search_type_e, CSearch::fSearch> CSearch::initSearchTypeLambdaMap()
{
    QMap<CSearch::search_type_e, CSearch::fSearch> map;
    map.insert(eSearchTypeEquals, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT)
        {
            adjustUnits(itemValue, searchValue);
            return itemValue.value1 == searchValue.value1;
        }
        else if(searchValue.str1 != "")
        {
            return itemValue.str1.toUpper() == searchValue.str1.toUpper();
        }
        return false;
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
        if(searchValue.str1 != "")
        {
            return itemValue.toString().contains(searchValue.str1,Qt::CaseInsensitive);
        }
        else
        {
            return itemValue.toString().contains(QString::number(searchValue.value1),Qt::CaseInsensitive);
        }

        return false;
    });
    map.insert(eSearchTypeWithout, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if(searchValue.str1 != "")
        {
            return !itemValue.toString().contains(searchValue.str1,Qt::CaseInsensitive);
        }
        else
        {
            return !itemValue.toString().contains(QString::number(searchValue.value1),Qt::CaseInsensitive);
        }

        return false;
    });

    map.insert(eSearchTypeRegEx, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if( searchValue.str1 != "")
        {
            //toUpper() since searches are handled in upper case
            return itemValue.str1.toUpper().contains(QRegExp(searchValue.str1));
        }
        return false;
    });
    return map;
}
