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
        search_t newFilter;
        if(currentSection.simplified()=="")
        {
            continue;
        }

        QString keyword = "";
        for(QString& key:keywordSearchTypeMap.keys())
        {
            if(currentSection.contains(key,Qt::CaseInsensitive))
            {
                keyword=key;
                break;
            }
        }
        if(keyword == "")
        {
            newFilter.searchType=eSearchTypeWith;
            if(searchMode == eSearchModeText)
            {
                newFilter.property=searchKeyword_e::eSearchKeywordGeneralFullText;
            }
            else
            {
                newFilter.property=searchKeyword_e::eSearchKeywordGeneralName;
            }
            searchValue_t filterValue;
            filterValue.str1 = currentSection.simplified();
            newFilter.searchValue=filterValue;
        }
        else
        {
            newFilter.searchType=keywordSearchTypeMap.value(keyword);
            newFilter.property=searchKeywordEnumMap.value(currentSection.section(keyword,0,0,QString::SectionCaseInsensitiveSeps).simplified());

            QString filterValueString = currentSection.section(keyword,1,-1,QString::SectionCaseInsensitiveSeps).simplified();
            searchValue_t filterValue;

            QRegExp argumentCapture("(\\d+)(?:\\s*)(m|km|mi|ft)?(\\d+)?(?:\\s*)(m|km|mi|ft)?",Qt::CaseInsensitive);

            if(argumentCapture.indexIn(filterValueString)!=-1)
            {
                filterValue.str2=argumentCapture.cap(4);

                if(argumentCapture.cap(3)!="") //to avoid removal of NOFLOAT
                {
                    filterValue.value2=argumentCapture.cap(3).toFloat();
                }

                filterValue.str1=argumentCapture.cap(2);

                if(argumentCapture.cap(1)!="") //to avoid removal of NOFLOAT
                {
                    filterValue.value1=argumentCapture.cap(1).toFloat();
                    if(filterValue.str1!=""&&filterValue.str2=="")
                    {
                        //Assume they have the same unit
                        filterValue.str2=filterValue.str1;
                    }
                }
            }
            else
            {
                filterValue.str1 = filterValueString;
            }

            newFilter.searchValue=filterValue;
        }
        searches.append(newFilter);
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

QMap<QString,CSearch::search_type_e> CSearch::keywordSearchTypeMap = CSearch::initKeywordSearchTypeMap();
QMap<QString,CSearch::search_type_e> CSearch::initKeywordSearchTypeMap()
{
    QMap<QString,search_type_e> map;
    map.insert(tr("with"),eSearchTypeWith);
    map.insert(tr("without"),eSearchTypeWithout);
    map.insert(tr("shorter than"),eSearchTypeSmaller);
    map.insert(tr("longer than"),eSearchTypeBigger);
    map.insert(tr("lower than"),eSearchTypeSmaller);
    map.insert(tr("higher than"),eSearchTypeBigger);
    map.insert(tr("bigger than"),eSearchTypeBigger);
    map.insert(tr("greater than"),eSearchTypeBigger);
    map.insert(tr("above"),eSearchTypeBigger);
    map.insert(tr("over"),eSearchTypeBigger);
    map.insert(tr("smaller than"),eSearchTypeSmaller);
    map.insert(tr("under"),eSearchTypeSmaller);
    return map;
}

QMap<QString,searchKeyword_e> CSearch::searchKeywordEnumMap = CSearch::initSearchKeywordEnumMap();
QMap<QString,searchKeyword_e> CSearch::initSearchKeywordEnumMap()
{
    //Everything to upper since search comes in CAPS and we can't get value() case insensitive easily
    QMap<QString,searchKeyword_e> map;
    //General keywords
    map.insert(tr("name").toUpper(),searchKeyword_e::eSearchKeywordGeneralName);

    //Area keywords
    map.insert(tr("area").toUpper(),searchKeyword_e::eSearchKeywordAreaArea);

    //Geocache keywords
    map.insert(tr("difficulty").toUpper(),searchKeyword_e::eSearchKeywordGeocacheDifficulty);
    map.insert("D",searchKeyword_e::eSearchKeywordGeocacheDifficulty);

    //Waypoint keywords
    map.insert(tr("elevation").toUpper(),searchKeyword_e::eSearchKeywordWptElevation);

    //Route / track keywords
    map.insert(tr("distance").toUpper(),searchKeyword_e::eSearchKeywordRteTrkDistance);

    return map;
}

QMap<CSearch::search_type_e, CSearch::fSearch> CSearch::searchTypeLambdaMap = CSearch::initSearchTypeLambdaMap();
QMap<CSearch::search_type_e, CSearch::fSearch> CSearch::initSearchTypeLambdaMap()
{
    QMap<CSearch::search_type_e, CSearch::fSearch> map;
    map.insert(eSearchTypeEquals, [](const searchValue_t& itemValue, const searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT)
        {
            //TODO unit conversion
            /*
             * if(filterValue.str1 != "")
               {
               }else
             */
            return itemValue.value1 == searchValue.value1;
        }
        else if(searchValue.str1 != "")
        {
            return itemValue.str1 == searchValue.str1;
        }
        return false;
    });
    map.insert(eSearchTypeSmaller, [](const searchValue_t& itemValue, const searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT && itemValue.value1 != NOFLOAT)
        {
            //TODO unit conversion
            /*
             * if(filterValue.str1 != "")
               {
               }else
             */
            return itemValue.value1 < searchValue.value1;
        }
        return false;
    });
    map.insert(eSearchTypeBigger, [](const searchValue_t& itemValue, const searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT && itemValue.value1 != NOFLOAT)
        {
            //TODO unit conversion
            /*
             * if(filterValue.str1 != "")
               {
               }else
             */
            return itemValue.value1 > searchValue.value1;
        }
        return false;
    });

    map.insert(eSearchTypeBetween, [](const searchValue_t& itemValue, const searchValue_t& searchValue){
        if(searchValue.value1 != NOFLOAT && itemValue.value1 != NOFLOAT && searchValue.value2 != NOFLOAT && itemValue.value2 != NOFLOAT)
        {
            //TODO unit conversion
            /*
             * if(filterValue.str1 != "")
               {
               }else
             */
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
    map.insert(eSearchTypeWith, [](const searchValue_t& itemValue, const searchValue_t& searchValue){
        if(searchValue.str1 != "" && searchValue.str1 != "")
        {
            return itemValue.str1.contains(searchValue.str1);
        }
        else
        {
            return itemValue.str1.contains(QString::number(searchValue.value1));
        }
        return false;
    });
    map.insert(eSearchTypeWithout, [](const searchValue_t& itemValue, const searchValue_t& searchValue){
        if(searchValue.str1 != "" && searchValue.str1 != "")
        {
            return !itemValue.str1.contains(searchValue.str1);
        }
        else
        {
            return !itemValue.str1.contains(QString::number(searchValue.value1));
        }
        return false;
    });
    return map;
}
