/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

**********************************************************************************************/
#include "CMainWindow.h"
#include "gis/GeoMath.h"
#include "gis/proj_x.h"
#include "units/CUnitImperial.h"
#include "units/CUnitMetric.h"
#include "units/CUnitNautic.h"

#include <QtWidgets>
const IUnit * IUnit::m_self = nullptr;

const QPointF NOPOINTF(NOFLOAT, NOFLOAT);
const QPoint NOPOINT (NOINT, NOINT);

IUnit::tz_mode_e IUnit::timeZoneMode = IUnit::eTZUtc;
IUnit::coord_format_e IUnit::coordFormat = IUnit::eCoordFormat1;
QByteArray IUnit::timeZone = "UTC";
bool IUnit::useShortFormat = false;

const char * IUnit::tblTimezone[] =
{
    "Africa/Abidjan",
    "Africa/Accra",
    "Africa/Addis_Ababa",
    "Africa/Algiers",
    "Africa/Asmara",
    "Africa/Bamako",
    "Africa/Bangui",
    "Africa/Banjul",
    "Africa/Bissau",
    "Africa/Blantyre",
    "Africa/Brazzaville",
    "Africa/Bujumbura",
    "Africa/Cairo",
    "Africa/Casablanca",
    "Africa/Conakry",
    "Africa/Dakar",
    "Africa/Dar_es_Salaam",
    "Africa/Djibouti",
    "Africa/Douala",
    "Africa/El_Aaiun",
    "Africa/Freetown",
    "Africa/Gaborone",
    "Africa/Harare",
    "Africa/Johannesburg",
    "Africa/Kampala",
    "Africa/Khartoum",
    "Africa/Kigali",
    "Africa/Kinshasa",
    "Africa/Lagos",
    "Africa/Libreville",
    "Africa/Lome",
    "Africa/Luanda",
    "Africa/Lubumbashi",
    "Africa/Lusaka",
    "Africa/Malabo",
    "Africa/Maputo",
    "Africa/Maseru",
    "Africa/Mbabane",
    "Africa/Mogadishu",
    "Africa/Monrovia",
    "Africa/Nairobi",
    "Africa/Ndjamena",
    "Africa/Niamey",
    "Africa/Nouakchott",
    "Africa/Ouagadougou",
    "Africa/Porto-Novo",
    "Africa/Sao_Tome",
    "Africa/Tripoli",
    "Africa/Tunis",
    "Africa/Windhoek",
    "America/Adak",
    "America/Anguilla",
    "America/Antigua",
    "America/Araguaina",
    "America/Argentina/Buenos_Aires",
    "America/Argentina/Catamarca",
    "America/Argentina/Cordoba",
    "America/Argentina/Jujuy",
    "America/Argentina/La_Rioja",
    "America/Argentina/Mendoza",
    "America/Argentina/Rio_Gallegos",
    "America/Argentina/San_Juan",
    "America/Argentina/San_Luis",
    "America/Argentina/Tucuman",
    "America/Argentina/Ushuaia",
    "America/Aruba",
    "America/Asuncion",
    "America/Atikokan",
    "America/Bahia",
    "America/Barbados",
    "America/Belem",
    "America/Belize",
    "America/Blanc-Sablon",
    "America/Boa_Vista",
    "America/Bogota",
    "America/Boise",
    "America/Cambridge_Bay",
    "America/Campo_Grande",
    "America/Cancun",
    "America/Caracas",
    "America/Cayenne",
    "America/Cayman",
    "America/Chicago",
    "America/Chihuahua",
    "America/Coral_Harbour",
    "America/Costa_Rica",
    "America/Cuiaba",
    "America/Curacao",
    "America/Dawson",
    "America/Dawson_Creek",
    "America/Denver",
    "America/Dominica",
    "America/Edmonton",
    "America/Eirunepe",
    "America/El_Salvador",
    "America/Fortaleza",
    "America/Glace_Bay",
    "America/Goose_Bay",
    "America/Grand_Turk",
    "America/Grenada",
    "America/Guadeloupe",
    "America/Guatemala",
    "America/Guayaquil",
    "America/Guyana",
    "America/Halifax",
    "America/Havana",
    "America/Hermosillo",
    "America/Indiana/Indianapolis",
    "America/Indiana/Knox",
    "America/Indiana/Marengo",
    "America/Indiana/Petersburg",
    "America/Indiana/Vevay",
    "America/Indiana/Vincennes",
    "America/Indiana/Winamac",
    "America/Inuvik",
    "America/Iqaluit",
    "America/Jamaica",
    "America/Juneau",
    "America/Kentucky/Louisville",
    "America/Kentucky/Monticello",
    "America/La_Paz",
    "America/Lima",
    "America/Los_Angeles",
    "America/Maceio",
    "America/Managua",
    "America/Manaus",
    "America/Marigot",
    "America/Martinique",
    "America/Mazatlan",
    "America/Menominee",
    "America/Merida",
    "America/Mexico_City",
    "America/Miquelon",
    "America/Moncton",
    "America/Monterrey",
    "America/Montevideo",
    "America/Montreal",
    "America/Montserrat",
    "America/Nassau",
    "America/New_York",
    "America/Nipigon",
    "America/Noronha",
    "America/North_Dakota/Center",
    "America/North_Dakota/Salem",
    "America/Panama",
    "America/Pangnirtung",
    "America/Paramaribo",
    "America/Phoenix",
    "America/Port-au-Prince",
    "America/Port_of_Spain",
    "America/Porto_Velho",
    "America/Puerto_Rico",
    "America/Rainy_River",
    "America/Rankin_Inlet",
    "America/Recife",
    "America/Regina",
    "America/Resolute",
    "America/Rio_Branco",
    "America/Santarem",
    "America/Santiago",
    "America/Santo_Domingo",
    "America/Sao_Paulo",
    "America/St_Barthelemy",
    "America/St_Johns",
    "America/St_Kitts",
    "America/St_Lucia",
    "America/St_Thomas",
    "America/St_Vincent",
    "America/Tegucigalpa",
    "America/Thunder_Bay",
    "America/Tijuana",
    "America/Toronto",
    "America/Tortola",
    "America/Vancouver",
    "America/Whitehorse",
    "America/Winnipeg",
    "America/Yellowknife",
    "Ameriica/Swift_Current",
    "Arctic/Longyearbyen",
    "Asia/Aden",
    "Asia/Almaty",
    "Asia/Amman",
    "Asia/Anadyr",
    "Asia/Aqtau",
    "Asia/Aqtobe",
    "Asia/Ashgabat",
    "Asia/Baghdad",
    "Asia/Bahrain",
    "Asia/Baku",
    "Asia/Bangkok",
    "Asia/Beirut",
    "Asia/Bishkek",
    "Asia/Brunei",
    "Asia/Choibalsan",
    "Asia/Chongqing",
    "Asia/Colombo",
    "Asia/Damascus",
    "Asia/Dhaka",
    "Asia/Dili",
    "Asia/Dubai",
    "Asia/Dushanbe",
    "Asia/Gaza",
    "Asia/Harbin",
    "Asia/Ho_Chi_Minh",
    "Asia/Hong_Kong",
    "Asia/Hovd",
    "Asia/Irkutsk",
    "Asia/Jakarta",
    "Asia/Jayapura",
    "Asia/Jerusalem",
    "Asia/Kabul",
    "Asia/Kamchatka",
    "Asia/Karachi",
    "Asia/Kashgar",
    "Asia/Katmandu",
    "Asia/Kolkata",
    "Asia/Krasnoyarsk",
    "Asia/Kuala_Lumpur",
    "Asia/Kuching",
    "Asia/Kuwait",
    "Asia/Macau",
    "Asia/Magadan",
    "Asia/Makassar",
    "Asia/Manila",
    "Asia/Muscat",
    "Asia/Nicosia",
    "Asia/Novosibirsk",
    "Asia/Omsk",
    "Asia/Oral",
    "Asia/Phnom_Penh",
    "Asia/Pontianak",
    "Asia/Pyongyang",
    "Asia/Qatar",
    "Asia/Qyzylorda",
    "Asia/Rangoon",
    "Asia/Riyadh",
    "Asia/Sakhalin",
    "Asia/Samarkand",
    "Asia/Seoul",
    "Asia/Shanghai",
    "Asia/Singapore",
    "Asia/Taipei",
    "Asia/Tashkent",
    "Asia/Tbilisi",
    "Asia/Tehran",
    "Asia/Thimphu",
    "Asia/Tokyo",
    "Asia/Ulaanbaatar",
    "Asia/Urumqi",
    "Asia/Vientiane",
    "Asia/Vladivostok",
    "Asia/Yakutsk",
    "Asia/Yekaterinburg",
    "Asia/Yerevan",
    "Atlantic/Azores",
    "Atlantic/Bermuda",
    "Atlantic/Canary",
    "Atlantic/Cape_Verde",
    "Atlantic/Faroe",
    "Atlantic/Madeira",
    "Atlantic/Reykjavik",
    "Atlantic/South_Georgia",
    "Atlantic/St_Helena",
    "Atlantic/Stanley",
    "Australia/Adelaide",
    "Australia/Brisbane",
    "Australia/Broken_Hill",
    "Australia/Currie",
    "Australia/Darwin",
    "Australia/Eucla",
    "Australia/Hobart",
    "Australia/Lindeman",
    "Australia/Lord_Howe",
    "Australia/Melbourne",
    "Australia/Perth",
    "Australia/Sydney",
    "Europe/Amsterdam",
    "Europe/Andorra",
    "Europe/Athens",
    "Europe/Belgrade",
    "Europe/Berlin",
    "Europe/Bratislava",
    "Europe/Brussels",
    "Europe/Bucharest",
    "Europe/Budapest",
    "Europe/Chisinau",
    "Europe/Copenhagen",
    "Europe/Dublin",
    "Europe/Gibraltar",
    "Europe/Guernsey",
    "Europe/Helsinki",
    "Europe/Isle_of_Man",
    "Europe/Istanbul",
    "Europe/Jersey",
    "Europe/Kaliningrad",
    "Europe/Kiev",
    "Europe/Lisbon",
    "Europe/Ljubljana",
    "Europe/London",
    "Europe/Luxembourg",
    "Europe/Madrid",
    "Europe/Malta",
    "Europe/Marienhamn",
    "Europe/Minsk",
    "Europe/Monaco",
    "Europe/Moscow",
    "Europe/Oslo",
    "Europe/Paris",
    "Europe/Podgorica",
    "Europe/Prague",
    "Europe/Riga",
    "Europe/Rome",
    "Europe/Samara",
    "Europe/San_Marino",
    "Europe/Sarajevo",
    "Europe/Simferopol",
    "Europe/Skopje",
    "Europe/Sofia",
    "Europe/Stockholm",
    "Europe/Tallinn",
    "Europe/Tirane",
    "Europe/Uzhgorod",
    "Europe/Vaduz",
    "Europe/Vatican",
    "Europe/Vienna",
    "Europe/Vilnius",
    "Europe/Volgograd",
    "Europe/Warsaw",
    "Europe/Zagreb",
    "Europe/Zaporozhye",
    "Europe/Zurich",
    "Indian/Antananarivo",
    "Indian/Chagos",
    "Indian/Christmas",
    "Indian/Cocos",
    "Indian/Comoro",
    "Indian/Kerguelen",
    "Indian/Mahe",
    "Indian/Maldives",
    "Indian/Mauritius",
    "Indian/Mayotte",
    "Indian/Reunion",
    "Pacific/Apia",
    "Pacific/Auckland",
    "Pacific/Chatham",
    "Pacific/Easter",
    "Pacific/Efate",
    "Pacific/Enderbury",
    "Pacific/Fakaofo",
    "Pacific/Fiji",
    "Pacific/Funafuti",
    "Pacific/Galapagos",
    "Pacific/Gambier",
    "Pacific/Guadalcanal",
    "Pacific/Guam",
    "Pacific/Honolulu",
    "Pacific/Johnston",
    "Pacific/Kiritimati",
    "Pacific/Kosrae",
    "Pacific/Kwajalein",
    "Pacific/Majuro",
    "Pacific/Marquesas",
    "Pacific/Midway",
    "Pacific/Nauru",
    "Pacific/Niue",
    "Pacific/Norfolk",
    "Pacific/Noumea",
    "Pacific/Pago_Pago",
    "Pacific/Palau",
    "Pacific/Pitcairn",
    "Pacific/Ponape",
    "Pacific/Port_Moresby",
    "Pacific/Rarotonga",
    "Pacific/Saipan",
    "Pacific/Tahiti",
    "Pacific/Tarawa",
    "Pacific/Tongatapu",
    "Pacific/Truk",
    "Pacific/Wake",
    "Pacific/Wallis",
    0
};

const int N_TIMEZONES = sizeof(IUnit::tblTimezone) / sizeof(const char*);

const QRegExp IUnit::reCoord1("^\\s*([N|S]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s+([E|W|O]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s*$");

const QRegExp IUnit::reCoord2("^\\s*([N|S]){1}\\s*([0-9]+\\.[0-9]+)\\W*\\s+([E|W|O]){1}\\s*([0-9]+\\.[0-9]+)\\W*\\s*$");

const QRegExp IUnit::reCoord3("^\\s*([-0-9]+\\.[0-9]+)\\s+([-0-9]+\\.[0-9]+)\\s*$");

const QRegExp IUnit::reCoord4("^\\s*([N|S]){1}\\s*([0-9]+)\\W+([0-9]+)\\W+([0-9]+\\.[0-9]+)\\W*([E|W|O]){1}\\W*([0-9]+)\\W+([0-9]+)\\W+([0-9]+\\.[0-9]+)\\W*\\s*$");

const QRegExp IUnit::reCoord5("^\\s*([-0-9]+\\.[0-9]+)([N|S])\\s+([-0-9]+\\.[0-9]+)([W|E])\\s*$");

IUnit::IUnit(const type_e &type, const QString& baseunit, const qreal basefactor, const QString& speedunit, const qreal speedfactor, QObject * parent)
    : QObject(parent)
    , type(type)
    , baseunit(baseunit)
    , basefactor(basefactor)
    , speedunit(speedunit)
    , speedfactor(speedfactor)
{
    //there can be only one...
    if(nullptr != m_self)
    {
        delete m_self;
    }
    m_self = this;
}


void IUnit::setUnitType(type_e t, QObject * parent)
{
    switch(t)
    {
    case eTypeMetric:
        new CUnitMetric(parent);
        break;

    case eTypeImperial:
        new CUnitImperial(parent);
        break;

    case eTypeNautic:
        new CUnitNautic(parent);
        break;
    }

    QSettings cfg;
    cfg.setValue("Units/type", t);
}

void IUnit::meter2speed(qreal meter, QString& val, QString& unit) const
{
    val.sprintf("%2.2f", meter * speedfactor);
    unit = speedunit;
}

void IUnit::seconds2time(quint32 ttime, QString& val, QString& unit) const
{
    QTime time(0, 0, 0);
    quint32 days  = ttime / 86400;

    time = time.addSecs(ttime);

    if(days)
    {
        val = QString("%1:").arg(days) + time.toString("HH:mm:ss");
        unit = "d";
    }
    else
    {
        val = time.toString("HH:mm:ss");
        unit = "h";
    }
}

bool IUnit::parseTimestamp(const QString &time, QDateTime &datetime)
{
    int tzoffset;
    datetime = parseTimestamp(time, tzoffset);

    return datetime.isValid();
}


QDateTime IUnit::parseTimestamp(const QString &timetext, int& tzoffset)
{
    const QRegExp tzRE("[-+]\\d\\d:\\d\\d$");
    int i;

    tzoffset = 0;

    QString format = "yyyy-MM-dd'T'hh:mm:ss";

    i = timetext.indexOf(".");
    if (i != NOIDX)
    {
        if(timetext[i + 1] == '0')
        {
            format += ".zzz";
        }
        else
        {
            format += ".z";
        }
    }

    // trailing "Z" explicitly declares the timestamp to be UTC
    if (timetext.indexOf("Z") != NOIDX)
    {
        format += "'Z'";
    }
    else if ((i = tzRE.indexIn(timetext)) != NOIDX)
    {
        // trailing timezone offset [-+]HH:MM present
        // This does not match the original intentions of the GPX
        // file format but appears to be found occasionally in
        // the wild.  Try our best parsing it.

        // add the literal string to the format so fromString()
        // will succeed
        format += "'";
        format += timetext.right(6);
        format += "'";

        // calculate the offset
        int offsetHours(timetext.mid(i + 1, 2).toUInt());
        int offsetMinutes(timetext.mid(i + 4, 2).toUInt());
        if (timetext[i] == '-')
        {
            tzoffset = -(60 * offsetHours + offsetMinutes);
        }
        else
        {
            tzoffset = 60 * offsetHours + offsetMinutes;
        }
        tzoffset *= 60;          // seconds
    }

    QDateTime datetime = QDateTime::fromString(timetext, format);
    datetime.setOffsetFromUtc(tzoffset);

    return datetime;
}

QString IUnit::datetime2string(const QDateTime& time, bool shortDate, const QPointF& pos)
{
    QTimeZone tz;

    tz_mode_e tmpMode = (pos != NOPOINTF) ? timeZoneMode : eTZLocal;

    switch(tmpMode)
    {
    case eTZUtc:
        tz = QTimeZone("UTC");
        break;

    case eTZLocal:
        tz = QTimeZone(QTimeZone::systemTimeZoneId());
        break;

    case eTZAuto:
        tz = QTimeZone(pos2timezone(pos));
        break;

    case eTZSelected:
        tz = QTimeZone(timeZone);
        break;
    }

    QDateTime tmp = time.toTimeZone(tz);
    return tmp.toString((shortDate | useShortFormat) ? Qt::ISODate : Qt::SystemLocaleLongDate);
}

QByteArray IUnit::pos2timezone(const QPointF& pos)
{
    static QImage imgTimezone = QPixmap(":/pics/timezones.png").toImage();

    int x = qRound(2048.0 / 360.0 * (180.0 + pos.x() * RAD_TO_DEG));
    int y = qRound(1024.0 / 180.0 * (90.0  - pos.y() * RAD_TO_DEG));

    QRgb rgb = imgTimezone.pixel(x, y);

    if(qRed(rgb) == 0 && qGreen(rgb) == 0)
    {
        return "UTC";
    }

    int tz = ((qRed(rgb) & 248) << 1) + ((qGreen(rgb) >> 4) & 15);
    if(tz >= N_TIMEZONES)
    {
        return 0;
    }

    return tblTimezone[tz];
}

bool IUnit::degToStr(const qreal& x, const qreal& y, QString& str)
{
    if(x < -180 || 180 < x)
    {
        return false;
    }

    if(y < -90 || 90 < y)
    {
        return false;
    }

    switch(coordFormat)
    {
    case eCoordFormat1:
    {
        qint32 degN, degE;
        qreal minN, minE;

        bool signLat = GPS_Math_Deg_To_DegMin(y, &degN, &minN);
        bool signLon = GPS_Math_Deg_To_DegMin(x, &degE, &minE);

        const QString &lat = signLat ? "S" : "N";
        const QString &lng = signLon ? "W" : "E";
        str.sprintf("%s%02d° %06.3f %s%03d° %06.3f", lat.toUtf8().data(), qAbs(degN), minN, lng.toUtf8().data(), qAbs(degE), minE);
        break;
    }

    case eCoordFormat2:
    {
        const QString &lat = (y < 0) ? "S" : "N";
        const QString &lng = (x < 0) ? "W" : "E";
        str.sprintf("%s%02.6f° %s%03.6f°", lat.toUtf8().data(), qAbs(y), lng.toUtf8().data(), qAbs(x));
        break;
    }

    case eCoordFormat3:
    {
        qint32 degN, degE;
        qreal minN, minE;

        bool signLat = GPS_Math_Deg_To_DegMin(y, &degN, &minN);
        bool signLon = GPS_Math_Deg_To_DegMin(x, &degE, &minE);

        qreal secN = (minN - qFloor(minN)) * 60;
        qreal secE = (minE - qFloor(minE)) * 60;

        const QString &lat = signLat ? "S" : "N";
        const QString &lng = signLon ? "W" : "E";
        str.sprintf("%s%02d° %02d' %02.2f'' %s%03d° %02d' %02.2f''", lat.toUtf8().data(), qAbs(degN), qFloor(minN), secN, lng.toUtf8().data(), qAbs(degE), qFloor(minE), secE);
        break;
    }
    }

    return true;
}

bool IUnit::strToDeg(const QString& str, qreal& lon, qreal& lat)
{
    if(reCoord2.exactMatch(str))
    {
        bool signLat    = reCoord2.cap(1) == "S";
        qreal absLat    = reCoord2.cap(2).toDouble();
        lat = signLat ? -absLat : absLat;

        bool signLon    = reCoord2.cap(3) == "W";
        qreal absLon    = reCoord2.cap(4).toDouble();
        lon = signLon ? -absLon : absLon;
    }
    else if(reCoord1.exactMatch(str))
    {
        bool signLat    = reCoord1.cap(1) == "S";
        int degLat      = reCoord1.cap(2).toInt();
        qreal minLat    = reCoord1.cap(3).toDouble();

        GPS_Math_DegMin_To_Deg(signLat, degLat, minLat, lat);

        bool signLon    = reCoord1.cap(4) == "W";
        int degLon      = reCoord1.cap(5).toInt();
        qreal minLon    = reCoord1.cap(6).toDouble();

        GPS_Math_DegMin_To_Deg(signLon, degLon, minLon, lon);
    }
    else if(reCoord3.exactMatch(str))
    {
        lat             = reCoord3.cap(1).toDouble();
        lon             = reCoord3.cap(2).toDouble();
    }
    else if(reCoord4.exactMatch(str))
    {
        bool signLat    = reCoord4.cap(1) == "S";
        int degLat      = reCoord4.cap(2).toInt();
        int minLat      = reCoord4.cap(3).toInt();
        qreal secLat    = reCoord4.cap(4).toFloat();

        GPS_Math_DegMinSec_To_Deg(signLat, degLat, minLat, secLat, lat);

        bool signLon    = reCoord4.cap(5) == "W";
        int degLon      = reCoord4.cap(6).toInt();
        int minLon      = reCoord4.cap(7).toInt();
        qreal secLon    = reCoord4.cap(8).toFloat();

        GPS_Math_DegMinSec_To_Deg(signLon, degLon, minLon, secLon, lon);
    }
    else if(reCoord5.exactMatch(str))
    {
        bool signLon    = reCoord4.cap(4) == "W";
        bool signLat    = reCoord4.cap(2) == "S";
        lat             = reCoord5.cap(1).toDouble();
        lon             = reCoord5.cap(3).toDouble();

        if(signLon)
        {
            lon = -lon;
        }
        if(signLat)
        {
            lat = -lat;
        }
    }
    else
    {
        QMessageBox::warning(&CMainWindow::self(), tr("Error"), tr("Bad position format. Must be: \"[N|S] ddd mm.sss [W|E] ddd mm.sss\" or \"[N|S] ddd.ddd [W|E] ddd.ddd\""), QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }

    if(fabs(lon) > 180.0 || fabs(lat) > 90.0)
    {
        QMessageBox::warning(&CMainWindow::self(), tr("Error"), tr("Position values out of bounds. "), QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }

    return true;
}

bool IUnit::isValidCoordString(const QString& str)
{
    if(reCoord1.exactMatch(str))
    {
        return true;
    }
    else if(reCoord2.exactMatch(str))
    {
        return true;
    }
    else if(reCoord3.exactMatch(str))
    {
        return true;
    }
    else if(reCoord4.exactMatch(str))
    {
        return true;
    }
    else if(reCoord5.exactMatch(str))
    {
        return true;
    }
    return false;
}
