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

CSearch::CSearch(QString searchstring, CSearch::search_mode_e filterMode)
{
    this->searchMode=filterMode;

    for(unsigned int currentSectionPos = 0; true; currentSectionPos++)
    {
        search_t newFilter;

        QString currentSection = searchstring.section(',',currentSectionPos, currentSectionPos,QString::SectionSkipEmpty);
        if(currentSection=="")
        {
            break;
        }

        currentSection=currentSection.simplified(); //removes leading and trailing whitespaces and double ones
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
            newFilter.property="Text";
            searchValue_t filterValue;
            filterValue.str1 = currentSection;
            newFilter.searchValue=filterValue;
        }
        else
        {
            newFilter.searchType=keywordSearchTypeMap.value(keyword);
            newFilter.property=currentSection.section(keyword,0,0,QString::SectionCaseInsensitiveSeps).simplified();


            QString filterValueString = currentSection.section(keyword,1,-1,QString::SectionCaseInsensitiveSeps).simplified();
            searchValue_t filterValue;

            int indexOfFirstValueBegin = filterValueString.indexOf(QRegExp("[0-9]"));
            if(indexOfFirstValueBegin>=0)
            {
                int indexOfFirstValueEnd = filterValueString.indexOf(QRegExp("[A-Z ]"),indexOfFirstValueBegin);
                if(indexOfFirstValueEnd<0)
                {
                    filterValue.value1 = filterValueString.midRef(indexOfFirstValueBegin,filterValueString.length()-indexOfFirstValueBegin).toFloat();
                }
                else
                {
                    filterValue.value1 = filterValueString.midRef(indexOfFirstValueBegin,indexOfFirstValueEnd-indexOfFirstValueBegin).toFloat();

                    int indexOfFirstUnitEnd = filterValueString.indexOf(QRegExp("[ ]"),indexOfFirstValueEnd+1);
                    if(indexOfFirstUnitEnd>0)
                    {
                        filterValue.str1 = filterValueString.mid(indexOfFirstValueEnd,indexOfFirstUnitEnd-indexOfFirstValueEnd).simplified();
                    }
                    else
                    {
                        filterValue.str1 = filterValueString.mid(indexOfFirstValueEnd,filterValueString.length()-indexOfFirstValueEnd).simplified();
                    }

                    int indexOfSecondValueBegin = filterValueString.indexOf(QRegExp("[0-9]"),indexOfFirstUnitEnd);
                    if(indexOfSecondValueBegin>0)
                    {
                        int indexOfSecondValueEnd = filterValueString.indexOf(QRegExp("[A-Z ]"));
                        if(indexOfSecondValueEnd<0)
                        {
                            filterValue.value2 = filterValueString.midRef(indexOfFirstValueBegin,filterValueString.length()-indexOfSecondValueBegin).toFloat();
                        }
                        else
                        {
                            filterValue.value2 = filterValueString.midRef(indexOfFirstValueBegin,indexOfSecondValueEnd-indexOfSecondValueBegin).toFloat();

                            int indexOfSecondUnitEnd = filterValueString.indexOf(QRegExp("[ ]"),indexOfFirstValueEnd+1);
                            if(indexOfSecondUnitEnd>0)
                            {
                                filterValue.str2 = filterValueString.mid(indexOfFirstValueEnd,indexOfSecondUnitEnd-indexOfSecondValueEnd).simplified();
                            }
                            else
                            {
                                filterValue.str2 = filterValueString.mid(indexOfFirstValueEnd,filterValueString.length()-indexOfSecondValueEnd).simplified();
                            }
                        }
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
    for(search_t filter:searches)
    {
        if(searchTypeLambdaMap.contains(filter.searchType))
        {
            searchValue_t itemFilterValue = dynamic_cast<IGisItem*>(item)->getValueByKeyword(filter.property);
            passed = searchTypeLambdaMap.value(filter.searchType)(itemFilterValue,filter.searchValue);
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


QMap<CSearch::search_type_e,std::function<bool(searchValue_t, searchValue_t)> > CSearch::searchTypeLambdaMap = CSearch::initSearchTypeLambdaMap();
QMap<CSearch::search_type_e,std::function<bool(searchValue_t, searchValue_t)> > CSearch::initSearchTypeLambdaMap()
{
    QMap<CSearch::search_type_e,std::function<bool(searchValue_t, searchValue_t)> > map;
    map.insert(eSearchTypeEquals, [](searchValue_t itemValue, searchValue_t filterValue){
        if(filterValue.value1 != NOFLOAT)
        {
            //TODO unit conversion
            /*
             * if(filterValue.str1 != "")
               {
               }else
             */
            if(itemValue.value1 == filterValue.value1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(filterValue.str1 != "")
        {
            if(itemValue.str1 == filterValue.str1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    });
    map.insert(eSearchTypeSmaller, [](searchValue_t itemValue, searchValue_t filterValue){
        if(filterValue.value1 != NOFLOAT)
        {
            //TODO unit conversion
            /*
             * if(filterValue.str1 != "")
               {
               }else
             */
            if(itemValue.value1 < filterValue.value1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    });
    return map;
}
