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
{
    this->searchMode=searchMode;

    const QStringList& sections = searchstring.split(",");
    for(const QString& currentSection : sections)
    {
        search_t newSearch;
        if(currentSection.simplified()=="")
        {
            continue;
        }

        QString searchTypeKeyword = "";
        for(QString& key:keywordSearchTypeMap.keys())
        {
            if(currentSection.contains(key,Qt::CaseInsensitive))
            {
                searchTypeKeyword=key;
                break;
            }
        }
        if(searchTypeKeyword == "")
        {
            //Search for what the user typed in the name or the full text
            newSearch.searchType=eSearchTypeWith;
            if(searchMode == eSearchModeText)
            {
                newSearch.property=searchKeyword_e::eSearchKeywordGeneralFullText;
            }
            else
            {
                newSearch.property=searchKeyword_e::eSearchKeywordGeneralName;
            }
            newSearch.searchValue.str1 = currentSection.simplified();
        }
        else
        {
            newSearch.searchType=keywordSearchTypeMap.value(searchTypeKeyword);
            newSearch.property=searchKeywordEnumMap.value(currentSection.section(searchTypeKeyword,0,0,QString::SectionCaseInsensitiveSeps).simplified(),eSearchKeywordNoMatch);

            QString filterValueString = currentSection.section(searchTypeKeyword,1,-1,QString::SectionCaseInsensitiveSeps).simplified();
            searchValue_t filterValue;

            QRegExp numericArguments("(\\d+)(?:\\s*)(m|km|mi|ft|ml)?(/)?(h|min|s|sec)?(" + tr("and") + ")?(\\d+)?(?:\\s*)(m|km|mi|ft|ml)?(/)?(h|min|s|sec)?",Qt::CaseInsensitive);

            if(numericArguments.indexIn(filterValueString)!=-1)
            {
                if(numericArguments.cap(1)!="") //to avoid removal of NOFLOAT
                {
                    filterValue.value1=numericArguments.cap(1).toFloat();
                }

                filterValue.str1=numericArguments.cap(2) + numericArguments.cap(4) + numericArguments.cap(5);

                if(numericArguments.cap(7)!="") //to avoid removal of NOFLOAT
                {
                    filterValue.value2=numericArguments.cap(3).toFloat();
                }

                filterValue.str2=numericArguments.cap(8) + numericArguments.cap(9) + numericArguments.cap(10);
            }
            else
            {
                filterValue.str1 = filterValueString;
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
    for(search_t search:searches)
    {
        if(searchTypeLambdaMap.contains(search.searchType))
        {
            searchValue_t itemFilterValue = *(dynamic_cast<IGisItem*>(item)->getValueByKeyword(search.property));
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
    if(search.searchValue.str1!=""&&search.searchValue.str2=="")
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

    if(search.property == eSearchKeywordNoMatch)
    {
        if(search.searchValue.str1.contains("/H")||search.searchValue.str1.contains("/S"))
        {
            search.property = eSearchKeywordRteTrkAvgSpeed;
        }
        if(search.searchValue.str1 == "KM" || search.searchValue.str1 == "MI" || search.searchValue.str1 == "ML")
        {
            search.property = eSearchKeywordRteTrkDistance;
        }
        if(search.searchValue.str1 == "M" || search.searchValue.str1 == "FT")
        {
            search.property = eSearchKeywordGeneralElevation;
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
    map.insert(tr("longer than"),eSearchTypeBigger);
    map.insert(tr("lower than"),eSearchTypeSmaller);
    map.insert(tr("higher than"),eSearchTypeBigger);
    map.insert(tr("bigger than"),eSearchTypeBigger);
    map.insert(tr("greater than"),eSearchTypeBigger);
    map.insert(tr("above"),eSearchTypeBigger);
    map.insert(tr("over"),eSearchTypeBigger);
    map.insert(tr("regex"),eSearchTypeRegEx);
    return map;
}

QMap<QString,searchKeyword_e> CSearch::searchKeywordEnumMap = CSearch::initSearchKeywordEnumMap();
QMap<QString,searchKeyword_e> CSearch::initSearchKeywordEnumMap()
{
    //Everything to upper since search comes in CAPS and we can't get value() case insensitive easily
    QMap<QString,searchKeyword_e> map;
    //General keywords
    map.insert(tr("name").toUpper(),eSearchKeywordGeneralName);
    map.insert(tr("full text").toUpper(),eSearchKeywordGeneralFullText);
    map.insert(tr("elevation").toUpper(),eSearchKeywordGeneralElevation);

    //Area keywords
    map.insert(tr("area").toUpper(),eSearchKeywordAreaArea);

    //Geocache keywords
    map.insert(tr("difficulty").toUpper(),eSearchKeywordGeocacheDifficulty);
    map.insert("D",eSearchKeywordGeocacheDifficulty);
    map.insert(tr("terrain").toUpper(),eSearchKeywordGeocacheTerrain);
    map.insert(tr("T").toUpper(),eSearchKeywordGeocacheTerrain);
    map.insert(tr("attributes").toUpper(),eSearchKeywordGeocacheAttributes);
    map.insert(tr("size").toUpper(),eSearchKeywordGeocacheSize);

    //Waypoint keywords

    //Route / track keywords
    map.insert(tr("distance").toUpper(),eSearchKeywordRteTrkDistance);
    map.insert(tr("length").toUpper(),eSearchKeywordRteTrkDistance);
    map.insert(tr("ascent").toUpper(),eSearchKeywordRteTrkAscent);
    map.insert(tr("elevation gain").toUpper(),eSearchKeywordRteTrkAscent);
    map.insert(tr("descent").toUpper(),eSearchKeywordRteTrkDescent);
    map.insert(tr("min elevation").toUpper(),eSearchKeywordRteTrkMinElevation);
    map.insert(tr("minimal elevation").toUpper(),eSearchKeywordRteTrkMinElevation);
    map.insert(tr("max elevation").toUpper(),eSearchKeywordRteTrkMaxElevation);
    map.insert(tr("maximal elevation").toUpper(),eSearchKeywordRteTrkMaxElevation);
    map.insert(tr("max speed").toUpper(),eSearchKeywordRteTrkMaxSpeed);
    map.insert(tr("maximal speed").toUpper(),eSearchKeywordRteTrkMaxSpeed);
    map.insert(tr("min speed").toUpper(),eSearchKeywordRteTrkMinSpeed);
    map.insert(tr("minimal speed").toUpper(),eSearchKeywordRteTrkMinSpeed);
    map.insert(tr("average speed").toUpper(),eSearchKeywordRteTrkAvgSpeed);

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
            return itemValue.str1 == searchValue.str1;
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
                return qMax(itemValue.value1,itemValue.value2)<searchValue.value1;
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
                return qMin(itemValue.value1,itemValue.value2)>searchValue.value1;
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
            return itemValue.toString().contains(searchValue.str1);
        }
        else
        {
            return itemValue.toString().contains(QString::number(searchValue.value1));
        }

        return false;
    });
    map.insert(eSearchTypeWithout, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if(searchValue.str1 != "")
        {
            return !itemValue.toString().contains(searchValue.str1);
        }
        else
        {
            return !itemValue.toString().contains(QString::number(searchValue.value1));
        }

        return false;
    });

    map.insert(eSearchTypeRegEx, [](const searchValue_t& itemValue, searchValue_t& searchValue){
        if( searchValue.str1 != "")
        {
            return itemValue.str1.contains(QRegExp(searchValue.str1));
        }
        return false;
    });
    return map;
}
