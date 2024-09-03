/**********************************************************************************************
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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

QMap<QString, CSearch::search_type_e> CSearch::keywordSearchTypeMap;
QMap<QString, QString> CSearch::keywordSearchExampleMap;
QMap<QString, searchProperty_e> CSearch::searchPropertyEnumMap;
QMap<searchProperty_e, QString> CSearch::searchPropertyMeaningMap;

CSearch::CSearch(QString searchstring) : searchText(searchstring) {
  // Initialisation
  searchTypeLambdaMap = initSearchTypeLambdaMap();

  if (searchstring.simplified().isEmpty()) {
    return;
  }

  // Since in the shortened search the searchType may be at the beginning and for reasons stated below
  // there must be a space before such a keyword, we prepend it here just to make sure.
  searchstring.prepend(" ");

  // Detect which comparison keyword in order to later spilt the string at this word.
  QString searchTypeKeyword;
  const QStringList& keys = keywordSearchTypeMap.keys();
  for (const QString& key : keys) {
    // Make spaces around so for example with is not detected inside without
    if (searchstring.contains(" " + key + " ", Qt::CaseInsensitive)) {
      searchTypeKeyword = key;
      break;
    }
  }

  if (searchTypeKeyword.isEmpty()) {
    // Default to search for what the user typed in the name or the full text
    search.searchType = eSearchTypeWith;
    if (searchMode == eSearchModeText) {
      search.property = searchProperty_e::eSearchPropertyGeneralFullText;
    } else {
      search.property = searchProperty_e::eSearchPropertyGeneralName;
    }
    search.searchValue.str1 = searchstring.simplified();
    syntaxError = true;
  } else {
    search.searchType = keywordSearchTypeMap.value(searchTypeKeyword);
    // Everything before the Search Type keyword is the property, i.e. "date after 2019" would result in "date"
    search.property = eSearchPropertyNoMatch;
    const QString& propertyString =
        searchstring.section(searchTypeKeyword, 0, 0, QString::SectionCaseInsensitiveSeps).simplified();
    const QStringList& keys = searchPropertyEnumMap.keys();
    for (const QString& key : keys) {
      if (propertyString.compare(key, Qt::CaseInsensitive) == 0) {
        search.property = searchPropertyEnumMap[key];
        break;
      }
    }
    // Don't raise a syntax error yet, since the improve query might find the correct property

    // Everything after the Search Type keyword is the value, i.e. "date after 2019" would result in "2019"
    const QString& filterValueString =
        searchstring.section(searchTypeKeyword, 1, -1, QString::SectionCaseInsensitiveSeps).simplified();
    const QString& filterValueStringFirstPart =
        filterValueString.section(tr("and"), 0, 0, QString::SectionCaseInsensitiveSeps).simplified();
    const QString& filterValueStringSecondPart =
        filterValueString.section(tr("and"), 1, -1, QString::SectionCaseInsensitiveSeps).simplified();
    searchValue_t filterValue;

    // Try if it is a time. Do so first, since this is the most exclusive
    const static QList<QString> timeFormats = {
        QLocale::system().timeFormat(QLocale::LongFormat), QLocale::system().timeFormat(QLocale::ShortFormat),
        QLocale::c().timeFormat(QLocale::LongFormat), QLocale::c().timeFormat(QLocale::ShortFormat)};

    for (const QString& tf : timeFormats) {
      const QTime& time1a = QLocale::system().toTime(filterValueStringFirstPart, tf);
      if (time1a.isValid()) {
        filterValue.value1 = time1a.msecsSinceStartOfDay() / 1000;
        filterValue.str1 = "SsE";
      }

      const QTime& time1b = QLocale::c().toTime(filterValueStringFirstPart, tf);
      if (time1b.isValid()) {
        filterValue.value1 = time1b.msecsSinceStartOfDay() / 1000;
        filterValue.str1 = "SsE";
      }

      if (time1a.isValid() || time1b.isValid()) {
        const QTime& time2a = QLocale::system().toTime(filterValueStringSecondPart, tf);
        if (time2a.isValid()) {
          filterValue.value2 = time2a.msecsSinceStartOfDay() / 1000;
          filterValue.str2 = "SsE";
        }

        const QTime& time2b = QLocale::c().toTime(filterValueStringSecondPart, tf);
        if (time2b.isValid()) {
          filterValue.value2 = time2b.msecsSinceStartOfDay() / 1000;
          filterValue.str2 = "SsE";
        }
        break;
      }
    }

    if (filterValue.toString().isEmpty()) {
      // Try if it is a date
      const static QList<QString> dateFormats = {
          QLocale::system().dateTimeFormat(QLocale::LongFormat), QLocale::system().dateTimeFormat(QLocale::ShortFormat),
          QLocale::c().dateTimeFormat(QLocale::LongFormat),      QLocale::c().dateTimeFormat(QLocale::ShortFormat),
          QLocale::system().dateFormat(QLocale::LongFormat),     QLocale::system().dateFormat(QLocale::ShortFormat),
          QLocale::c().dateFormat(QLocale::LongFormat),          QLocale::c().dateFormat(QLocale::ShortFormat)};

      for (const QString& df : dateFormats) {
        const QDateTime& time1a = QLocale::system().toDateTime(filterValueStringFirstPart, df);
        if (time1a.isValid()) {
          filterValue.value1 = time1a.toSecsSinceEpoch();
          filterValue.str1 = "SsE";
        }

        const QDateTime& time1b = QLocale::c().toDateTime(filterValueStringFirstPart, df);
        if (time1b.isValid()) {
          filterValue.value1 = time1b.toSecsSinceEpoch();
          filterValue.str1 = "SsE";
        }

        if (time1a.isValid() || time1b.isValid()) {
          const QDateTime& time2a = QLocale::system().toDateTime(filterValueStringSecondPart, df);
          if (time2a.isValid()) {
            filterValue.value2 = time2a.toSecsSinceEpoch();
            filterValue.str2 = "SsE";
          }

          const QDateTime& time2b = QLocale::c().toDateTime(filterValueStringSecondPart, df);
          if (time2b.isValid()) {
            filterValue.value2 = time2b.toSecsSinceEpoch();
            filterValue.str2 = "SsE";
          }
          break;
        }
      }
    }

    if (filterValue.toString().isEmpty()) {
      // Match speeds and distances after dates to have less problems with avoid sorting them out
      const static QString capNum = "(\\d+\\.?\\d*)(?![\\.\\d\\/\\:])";  // Match all numbers making sure no numbers are
                                                                         // omitted directly at the end
      const static QString capNumOpt = "(\\d+\\.?\\d*)?(?![\\.\\d\\/\\:])";
      const static QString capIgnWS = "(?:\\s*)";  // Ignore Whitespaces
      // Capture distances, speeds and simple Times that don't get caught by QDateTime.
      const static QString capUnit =
          "(m|km|mi|ft|mi|m\\/h|km\\/h|mi\\/h|ft\\/h|mi\\/h|h|min|s|м|км|м\\/ч|км\\/ч|ч|мин|с)?";
      const static QString capIgnAnd = "(?:" + tr("and") + ")?";
      // The second number, the units and the "and" are optional
      // The String has to be matched completely in order to avoid false positives thus the ^ and the $
      static const QRegularExpression numericArguments(
          "^" + capNum + capIgnWS + capUnit + capIgnWS + capIgnAnd + capIgnWS + capNumOpt + capIgnWS + capUnit + "$",
          QRegularExpression::PatternOption::CaseInsensitiveOption);
      const QRegularExpressionMatch& match = numericArguments.match(filterValueString);
      if (match.captured(0).simplified() != "") {
        if (match.captured(1) != "")  // to avoid removal of NOFLOAT
        {
          filterValue.value1 = match.captured(1).toFloat();
        }

        filterValue.str1 = match.captured(2);

        if (match.captured(3) != "")  // to avoid removal of NOFLOAT
        {
          filterValue.value2 = match.captured(3).toFloat();
        }

        filterValue.str2 = match.captured(4);
      }
    }
    if (filterValue.toString().isEmpty()) {
      filterValue.str1 = filterValueString;
    }
    search.searchValue = filterValue;
  }
  improveQuery();
  if (search.property == eSearchPropertyNoMatch) {
    syntaxError = true;
  }
}

bool CSearch::getSearchResult(IGisItem* item) {
  bool passed = true;
  if (searchTypeLambdaMap.contains(search.searchType)) {
    const searchValue_t& itemFilterValue = item->getValueByKeyword(search.property);
    passed = searchTypeLambdaMap.value(search.searchType)(itemFilterValue, search.searchValue);
    return passed;
  }
  return true;  // Empty search shouldn't hide anything
}

// itemValue is the value returned by a GisItem and thus always has the same unit.
// searchValue is what the user queried
bool CSearch::adjustUnits(const searchValue_t& itemValue, searchValue_t& searchValue) {
  bool success = true;
  if (searchValue.str1 != "" && searchValue.str1 != itemValue.str1) {
    success = IUnit::convert(searchValue.value1, searchValue.str1, itemValue.str1);
    syntaxError |= !success;
    if (!success) {
      return false;
    }
  } else {
    searchValue.str1 = itemValue.str1;
  }

  // Try to adjust unit of search to second value returned by item.
  // If this fails (for example if the search is "max speed between a and b",
  // adjust the unit of the search to the first returned unit
  if (itemValue.str2 != "") {
    if (searchValue.str2 != "" && searchValue.str2 != itemValue.str2) {
      success = IUnit::convert(searchValue.value2, searchValue.str2, itemValue.str2);
      syntaxError |= !success;
    } else {
      searchValue.str2 = itemValue.str2;
    }
  } else {
    if (searchValue.str2 != "" && searchValue.str2 != itemValue.str1) {
      success = IUnit::convert(searchValue.value2, searchValue.str2, itemValue.str1);
      syntaxError |= !success;
    }
    // Don't add a unit if there is no valid value
    else if (searchValue.value2 != NOFLOAT) {
      searchValue.str2 = itemValue.str1;
    }
  }
  return success;
}

// Make life easier for the user. The method tries to make assumption on what the user meant
void CSearch::improveQuery() {
  // If the user entered a number with a unit and another number, assume they have the same unit
  if (search.searchValue.str1 != "" && search.searchValue.str2 == "" && search.searchValue.value1 != NOFLOAT &&
      search.searchValue.value2 != NOFLOAT) {
    search.searchValue.str2 = search.searchValue.str1;
  }
  if (search.searchValue.str1 == "" && search.searchValue.str2 != "" && search.searchValue.value1 != NOFLOAT &&
      search.searchValue.value2 != NOFLOAT) {
    search.searchValue.str1 = search.searchValue.str2;
  }

  // Try to guess what property the user meant when there is no match. I.e. make "shorter than 5km" work
  if (search.property == eSearchPropertyNoMatch) {
    if (search.searchValue.str1.contains("/h", Qt::CaseInsensitive) ||
        search.searchValue.str1.contains("/s", Qt::CaseInsensitive)) {
      search.property = eSearchPropertyRteTrkAvgSpeed;
      autoDetectedProperty = true;
    } else if (search.searchValue.str1.compare("km", Qt::CaseInsensitive) == 0 ||
               search.searchValue.str1.compare("mi", Qt::CaseInsensitive) == 0) {
      search.property = eSearchPropertyRteTrkDistance;
      autoDetectedProperty = true;
    } else if (search.searchValue.str1.compare("m", Qt::CaseInsensitive) == 0 ||
               search.searchValue.str1.compare("ft", Qt::CaseInsensitive) == 0) {
      search.property = eSearchPropertyGeneralElevation;
      autoDetectedProperty = true;
    } else if (search.searchValue.str1.compare("s", Qt::CaseInsensitive) == 0 ||
               search.searchValue.str1.compare("min", Qt::CaseInsensitive) == 0 ||
               search.searchValue.str1.compare("h", Qt::CaseInsensitive) == 0) {
      search.property = eSearchPropertyRteTrkTimeMoving;
      autoDetectedProperty = true;
    } else if (search.searchValue.str1.compare("SsE", Qt::CaseInsensitive) == 0) {
      search.property = eSearchPropertyGeneralDate;
      autoDetectedProperty = true;
    }
  }

  // Searching for dates is error prone, thus some checks to make sure one searche sfor a probable value.
  if (search.property == eSearchPropertyGeneralDate) {
    if (search.searchValue.value1 != NOFLOAT) {
      // Try to catch if user only entered a year. Not done in regular detecting as it could be a speed or so.
      if (search.searchValue.str1.isEmpty()) {
        search.searchValue.value1 = QDate(search.searchValue.value1, 1, 1).startOfDay().toSecsSinceEpoch();
        search.searchValue.str1 = "SsE";
      }
      // Assume you want 2012 and not 1912 (qt defaults to 19xx)
      else if (QDateTime::fromSecsSinceEpoch(search.searchValue.value1).addYears(100) <= QDateTime::currentDateTime()) {
        search.searchValue.value1 =
            QDateTime::fromSecsSinceEpoch(search.searchValue.value1).addYears(100).toSecsSinceEpoch();
        search.searchValue.str1 = "SsE";
      }
    }
    if (search.searchValue.value2 != NOFLOAT) {
      // Try to catch if user only entered a year. Not done in regular detecting as it could be a speed or so.
      if (search.searchValue.str2.isEmpty()) {
        search.searchValue.value2 = QDate(search.searchValue.value2, 1, 1).startOfDay().toSecsSinceEpoch();
        search.searchValue.str2 = "SsE";
      }
      // Assume you want 2012 and not 1912 (qt defaults to 19xx)
      else if (QDateTime::fromSecsSinceEpoch(search.searchValue.value2).addYears(100) <= QDateTime::currentDateTime()) {
        search.searchValue.value2 =
            QDateTime::fromSecsSinceEpoch(search.searchValue.value2).addYears(100).toSecsSinceEpoch();
        search.searchValue.str2 = "SsE";
      }
    }

    // If the user searched for 'date equals' change it to 'between 0h and 24h of the day queried'
    if (search.searchType == eSearchTypeEquals) {
      search.searchType = eSearchTypeBetween;
      search.searchValue.str2 = "SsE";
      search.searchValue.value2 = search.searchValue.value1 + 24 * 60 * 60;
    }
  }
}

QMap<QString, CSearch::search_type_e> CSearch::initKeywordSearchTypeMap() {
  QMap<QString, search_type_e> map;
  map.insert(tr("with"), eSearchTypeWith);
  map.insert(tr("contains"), eSearchTypeWith);
  map.insert(tr("contain"), eSearchTypeWith);
  map.insert(tr("containing"), eSearchTypeWith);
  map.insert(tr("without"), eSearchTypeWithout);
  map.insert(tr("shorter than"), eSearchTypeSmaller);
  map.insert(tr("smaller than"), eSearchTypeSmaller);
  map.insert(tr("under"), eSearchTypeSmaller);
  map.insert(tr("lower than"), eSearchTypeSmaller);
  map.insert(tr("earlier than"), eSearchTypeSmaller);
  map.insert(tr("before"), eSearchTypeSmaller);
  map.insert(tr("less than"), eSearchTypeSmaller);
  map.insert("<", eSearchTypeSmaller);
  map.insert(tr("longer than"), eSearchTypeBigger);
  map.insert(tr("higher than"), eSearchTypeBigger);
  map.insert(tr("bigger than"), eSearchTypeBigger);
  map.insert(tr("greater than"), eSearchTypeBigger);
  map.insert(tr("above"), eSearchTypeBigger);
  map.insert(tr("over"), eSearchTypeBigger);
  map.insert(tr("after"), eSearchTypeBigger);
  map.insert(tr("later than"), eSearchTypeBigger);
  map.insert(">", eSearchTypeBigger);
  map.insert(tr("regex"), eSearchTypeRegEx);
  map.insert("=", eSearchTypeEquals);
  map.insert(tr("equals"), eSearchTypeEquals);
  map.insert(tr("is"), eSearchTypeEquals);
  map.insert(tr("between"), eSearchTypeBetween);
  return map;
}

QMap<QString, QString> CSearch::initKeywordSearchExampleMap() {
  QMap<QString, QString> map;
  map.insert(tr("with"), tr("example: attributes with dog"));
  map.insert(tr("contains"), tr("example: name contains bike"));
  map.insert(tr("contain"), tr("example: keywords contain bike"));
  map.insert(tr("containing"), tr("example: name containing bike"));
  map.insert(tr("without"), tr("example: name without water"));
  map.insert(tr("shorter than"), tr("example: shorter than 5km"));
  map.insert(tr("smaller than"), tr("example: area smaller than 5m²"));
  map.insert(tr("under"), tr("example: elevation under 1000ft"));
  map.insert(tr("lower than"), tr("example: lower than 500m"));
  map.insert(tr("earlier than"), tr("example: date earlier than 2015"));
  map.insert(tr("before"), tr("example: date before 10.05.2017"));  // Localisation of date in example!
  map.insert(tr("less than"), tr("example: ascent less than 500m"));
  map.insert("<", tr("example: D < 3"));
  map.insert(tr("longer than"), tr("example: distance longer than 20mi"));
  map.insert(tr("higher than"), tr("example: terrain higher than 2"));
  map.insert(tr("bigger than"), tr("example: area bigger than 50m²"));
  map.insert(tr("greater than"), tr("example: descent greater than 3000ft"));
  map.insert(tr("above"), tr("example: above 50m"));
  map.insert(tr("over"), tr("example: elevation over 400m"));
  map.insert(tr("after"), tr("example: date after 2013"));
  map.insert(tr("later than"), tr("example: date later than 2015"));
  map.insert(">", tr("example: T > 4"));
  map.insert(tr("regex"), tr("example: size regex (regular|large)"));
  map.insert("=", tr("example: size = micro"));
  map.insert(tr("equals"), tr("example: activity equals bike"));
  map.insert(tr("is"), tr("example: status is available"));
  map.insert(tr("between"), tr("example: length between 20km and 20mi"));
  return map;
}

QMap<QString, searchProperty_e> CSearch::initSearchPropertyEnumMap() {
  QMap<QString, searchProperty_e> map;
  // General keywords
  map.insert(tr("name"), eSearchPropertyGeneralName);
  map.insert(tr("full text"), eSearchPropertyGeneralFullText);
  map.insert(tr("elevation"), eSearchPropertyGeneralElevation);
  map.insert(tr("date"), eSearchPropertyGeneralDate);
  map.insert(tr("comment"), eSearchPropertyGeneralComment);
  map.insert(tr("description"), eSearchPropertyGeneralDescription);
  map.insert(tr("rating"), eSearchPropertyGeneralRating);
  map.insert(tr("keywords"), eSearchPropertyGeneralKeywords);
  map.insert(tr("type"), eSearchPropertyGeneralType);

  // Area keywords
  map.insert(tr("area"), eSearchPropertyAreaArea);

  // Geocache keywords
  map.insert(tr("difficulty"), eSearchPropertyGeocacheDifficulty);
  map.insert("D", eSearchPropertyGeocacheDifficulty);
  map.insert(tr("terrain"), eSearchPropertyGeocacheTerrain);
  map.insert(tr("T"), eSearchPropertyGeocacheTerrain);
  map.insert(tr("positive attributes"), eSearchPropertyGeocachePositiveAttributes);
  map.insert(tr("non-negated attributes"), eSearchPropertyGeocachePositiveAttributes);
  map.insert(tr("negated attributes"), eSearchPropertyGeocacheNegatedAttributes);
  map.insert(tr("size"), eSearchPropertyGeocacheSize);
  map.insert(tr("GCCode"), eSearchPropertyGeocacheGCCode);
  map.insert(tr("GCName"), eSearchPropertyGeocacheGCName);
  map.insert(tr("status"), eSearchPropertyGeocacheStatus);
  map.insert(tr("GCType"), eSearchPropertyGeocacheGCType);
  map.insert(tr("logged by"), eSearchPropertyGeocacheLoggedBy);
  map.insert(tr("latest log date"), eSearchPropertyGeocacheLastLogDate);
  map.insert(tr("latest log type"), eSearchPropertyGeocacheLastLogType);
  map.insert(tr("latest log by"), eSearchPropertyGeocacheLastLogBy);
  map.insert(tr("GCOwner"), eSearchPropertyGeocacheGCOwner);

  // Waypoint keywords

  // Route / track keywords
  map.insert(tr("distance"), eSearchPropertyRteTrkDistance);
  map.insert(tr("length"), eSearchPropertyRteTrkDistance);
  map.insert(tr("ascent"), eSearchPropertyRteTrkAscent);
  map.insert(tr("elevation gain"), eSearchPropertyRteTrkAscent);
  map.insert(tr("descent"), eSearchPropertyRteTrkDescent);
  map.insert(tr("min elevation"), eSearchPropertyRteTrkMinElevation);
  map.insert(tr("minimal elevation"), eSearchPropertyRteTrkMinElevation);
  map.insert(tr("max elevation"), eSearchPropertyRteTrkMaxElevation);
  map.insert(tr("maximal elevation"), eSearchPropertyRteTrkMaxElevation);
  map.insert(tr("max speed"), eSearchPropertyRteTrkMaxSpeed);
  map.insert(tr("maximal speed"), eSearchPropertyRteTrkMaxSpeed);
  map.insert(tr("min speed"), eSearchPropertyRteTrkMinSpeed);
  map.insert(tr("minimal speed"), eSearchPropertyRteTrkMinSpeed);
  map.insert(tr("average speed"), eSearchPropertyRteTrkAvgSpeed);
  map.insert(tr("activity"), eSearchPropertyRteTrkActivity);
  map.insert(tr("total time"), eSearchPropertyRteTrkTotalTime);
  map.insert(tr("duration"), eSearchPropertyRteTrkTotalTime);
  map.insert(tr("time moving"), eSearchPropertyRteTrkTimeMoving);

  return map;
}

QMap<searchProperty_e, QString> CSearch::initSearchPropertyMeaningMap() {
  QMap<searchProperty_e, QString> map;
  // General keywords
  map.insert(eSearchPropertyGeneralName, tr("searches the name of the item. For Geocaches this is \"Name - GCCode\"."));
  map.insert(eSearchPropertyGeneralFullText, tr("searches the full text"));
  map.insert(eSearchPropertyGeneralElevation,
             tr("searches the elevation. For items consisting of multiple points the minimum and the maximum is used"));
  map.insert(eSearchPropertyGeneralDate, tr("searches the Date"));
  map.insert(eSearchPropertyGeneralComment, tr("searches the Comment"));
  map.insert(eSearchPropertyGeneralDescription, tr("searches the Description"));
  map.insert(eSearchPropertyGeneralKeywords, tr("searches the Keywords"));
  map.insert(eSearchPropertyGeneralRating, tr("compares the Rating"));
  map.insert(eSearchPropertyGeneralType, tr("searches the type of the GisItem (Waypoint, Track, Route, Area)"));

  // Area keywords
  map.insert(eSearchPropertyAreaArea, tr("searches the area"));

  // Geocache keywords
  map.insert(eSearchPropertyGeocacheDifficulty, tr("searches the difficulty rating of a geocache"));
  map.insert(eSearchPropertyGeocacheTerrain, tr("searches the terrain rating of a geocache"));
  map.insert(eSearchPropertyGeocachePositiveAttributes,
             tr("searches the translated meanings of the non-negated attributes (Those not crossed out)"));
  map.insert(eSearchPropertyGeocacheNegatedAttributes,
             tr("searches the translated meanings of the negated attributes (Those crossed out)"));
  map.insert(eSearchPropertyGeocacheSize, tr("searches the size of a geocache. (micro, small, regular, large)"));
  map.insert(eSearchPropertyGeocacheGCCode, tr("searches the GCCode of a geocache."));
  map.insert(eSearchPropertyGeocacheGCName, tr("searches the Name of a geocache."));
  map.insert(eSearchPropertyGeocacheStatus,
             tr("searches the status of a geocache. (available, not available, archived)"));
  map.insert(eSearchPropertyGeocacheGCType, tr("searches the type of a geocache. (traditional, unknown, virtual...)"));
  map.insert(eSearchPropertyGeocacheLoggedBy, tr("searches the available logs for a username"));
  map.insert(eSearchPropertyGeocacheLastLogDate, tr("searches the date of the latest log"));
  map.insert(eSearchPropertyGeocacheLastLogType,
             tr("searches the type of the latest log (Found It, Didn't find it, Owner Maintenance, Write Note...)"));
  map.insert(eSearchPropertyGeocacheLastLogBy, tr("searches the username of the latest log"));
  map.insert(eSearchPropertyGeocacheGCOwner, tr("searches the username of the geocache owner"));

  // Waypoint keywords

  // Route / track keywords
  map.insert(eSearchPropertyRteTrkDistance, tr("searches the distance covered by a route or track"));
  map.insert(eSearchPropertyRteTrkAscent, tr("searches the total ascent in a route or track"));
  map.insert(eSearchPropertyRteTrkDescent, tr("searches the total descent in a route or track"));
  map.insert(eSearchPropertyRteTrkMinElevation, tr("searches the minimal elevation in a route or track"));
  map.insert(eSearchPropertyRteTrkMaxElevation, tr("searches the maximal elevation in a route or track"));
  map.insert(eSearchPropertyRteTrkMaxSpeed, tr("searches the maximal speed in a track"));
  map.insert(eSearchPropertyRteTrkMinSpeed, tr("searches the minimal speed in a track"));
  map.insert(eSearchPropertyRteTrkAvgSpeed, tr("searches the average speed in a track"));
  map.insert(eSearchPropertyRteTrkActivity, tr("searches the activity of a route or track"));
  map.insert(eSearchPropertyRteTrkTotalTime, tr("searches the total time spent on a route or track"));
  map.insert(eSearchPropertyRteTrkTimeMoving, tr("searches the time spent moving on a track"));

  return map;
}

QMap<CSearch::search_type_e, CSearch::fSearch> CSearch::initSearchTypeLambdaMap() {
  QMap<CSearch::search_type_e, CSearch::fSearch> map;
  map.insert(eSearchTypeEquals, [](const searchValue_t& itemValue, searchValue_t& searchValue) {
    return itemValue.toString().compare(searchValue.toString(), CSearch::caseSensitivity) == 0;
  });
  map.insert(eSearchTypeSmaller, [this](const searchValue_t& itemValue, searchValue_t& searchValue) {
    if (itemValue.value1 != NOFLOAT) {
      if (searchValue.value1 != NOFLOAT) {
        bool adjustSuccess = adjustUnits(itemValue, searchValue);
        if (adjustSuccess == false) {
          return false;
        }
        if (itemValue.value2 == NOFLOAT) {
          return itemValue.value1 < searchValue.value1;
        } else {
          return qMax(itemValue.value1, itemValue.value2) < searchValue.value1;
        }
      } else {
        syntaxError = true;
      }
    }
    return false;
  });
  map.insert(eSearchTypeBigger, [this](const searchValue_t& itemValue, searchValue_t& searchValue) {
    if (itemValue.value1 != NOFLOAT) {
      if (searchValue.value1 != NOFLOAT) {
        bool adjustSuccess = adjustUnits(itemValue, searchValue);
        if (adjustSuccess == false) {
          return false;
        }
        if (itemValue.value2 == NOFLOAT) {
          return itemValue.value1 > searchValue.value1;
        } else {
          return qMin(itemValue.value1, itemValue.value2) > searchValue.value1;
        }
      } else {
        syntaxError = true;
      }
    }
    return false;
  });

  map.insert(eSearchTypeBetween, [this](const searchValue_t& itemValue, searchValue_t& searchValue) {
    if (itemValue.value1 != NOFLOAT) {
      if (searchValue.value1 != NOFLOAT && searchValue.value2 != NOFLOAT) {
        bool adjustSuccess = adjustUnits(itemValue, searchValue);
        if (adjustSuccess == false) {
          return false;
        }
        if (itemValue.value2 == NOFLOAT) {
          return itemValue.value1 < qMax(searchValue.value1, searchValue.value2) &&
                 itemValue.value1 > qMin(searchValue.value1, searchValue.value2);
        } else {
          return qMax(itemValue.value1, itemValue.value2) < qMax(searchValue.value1, searchValue.value2) &&
                 qMin(itemValue.value1, itemValue.value2) > qMin(searchValue.value1, searchValue.value2);
        }
      } else {
        syntaxError = true;
      }
    }
    return false;
  });
  map.insert(eSearchTypeWith, [](const searchValue_t& itemValue, searchValue_t& searchValue) {
    return itemValue.toString().contains(searchValue.toString(), CSearch::caseSensitivity);
  });
  map.insert(eSearchTypeWithout, [](const searchValue_t& itemValue, searchValue_t& searchValue) {
    if (itemValue.toString().isEmpty()) {
      return false;
    } else {
      return !itemValue.toString().contains(searchValue.toString(), CSearch::caseSensitivity);
    }
  });
  map.insert(eSearchTypeRegEx, [](const searchValue_t& itemValue, searchValue_t& searchValue) {
    if (CSearch::caseSensitivity == Qt::CaseInsensitive)  // There is no option to make regex caseinsensitive
    {
      const QRegularExpression re(searchValue.toString().toLower());
      return itemValue.toString().toLower().contains(re);
    } else {
      const QRegularExpression re(searchValue.toString());
      return itemValue.toString().contains(re);
    }
  });
  return map;
}
