/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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


#include "WptIcons.h"
#include "helpers/CAppSetup.h"

#include <QtGui>

static const char * wptDefault = "://icons/waypoints/32x32/Default.png";

static QMap<QString, icon_t> wptIcons;

void initWptIcons()
{
    wptIcons["Default"]             = icon_t(wptDefault, 16, 16);
    wptIcons["City (Capitol)"]      = icon_t("://icons/waypoints/32x32/CityCapitol.png", 16, 16);
    wptIcons["City (Large)"]        = icon_t("://icons/waypoints/32x32/CityLarge.png", 16, 16);
    wptIcons["City (Medium)"]       = icon_t("://icons/waypoints/32x32/CityMedium.png", 16, 16);
    wptIcons["City (Small)"]        = icon_t("://icons/waypoints/32x32/CitySmall.png", 16, 16);
//    wptIcons["Small City"]          = ":/icons/wpt/small_city15x15.png";
//    wptIcons["Geocache"]            = ":/icons/wpt/geocache15x15.png";
//    wptIcons["Geocache Found"]      = ":/icons/wpt/geocache_fnd15x15.png";
    wptIcons["Residence"]           = icon_t("://icons/waypoints/32x32/Residence.png", 16, 16);
    wptIcons["Flag, Red"]           = icon_t("://icons/waypoints/32x32/FlagRed.png", 0, 32);
    wptIcons["Flag, Blue"]          = icon_t("://icons/waypoints/32x32/FlagBlue.png", 0, 32);
    wptIcons["Flag, Green"]         = icon_t("://icons/waypoints/32x32/FlagGreen.png", 0, 32);
    wptIcons["Pin, Red"]            = icon_t("://icons/waypoints/32x32/PinRed.png", 0, 32);
    wptIcons["Pin, Blue"]           = icon_t("://icons/waypoints/32x32/PinBlue.png", 0, 32);
    wptIcons["Pin, Green"]          = icon_t("://icons/waypoints/32x32/PinGreen.png", 0, 32);
    wptIcons["Block, Red"]          = icon_t("://icons/waypoints/32x32/BoxRed.png", 16, 16);
    wptIcons["Block, Blue"]         = icon_t("://icons/waypoints/32x32/BoxBlue.png", 16, 16);
    wptIcons["Block, Green"]        = icon_t("://icons/waypoints/32x32/BoxGreen.png", 16, 16);
    wptIcons["Blue Diamond"]        = icon_t("://icons/waypoints/32x32/DiamondBlue.png", 16, 16);
    wptIcons["Green Diamond"]       = icon_t("://icons/waypoints/32x32/DiamondGreen.png", 16, 16);
    wptIcons["Red Diamond"]         = icon_t("://icons/waypoints/32x32/DiamondRed.png", 16, 16);
    wptIcons["Parking Area"]        = icon_t("://icons/cache/32x32/parking.png", 16, 16);
    wptIcons["Trailhead"]           = icon_t("://icons/cache/32x32/trailhead.png", 16, 16);
    wptIcons["Waypoint"]            = icon_t("://icons/waypoints/32x32/Waypoint.png", 16, 16);


    setWptIconByName("Traditional Cache", "://icons/cache/32x32/traditional.png");
    setWptIconByName("Multi-cache", "://icons/cache/32x32/multi.png");
    setWptIconByName("Unknown Cache", "://icons/cache/32x32/unknown.png");
    setWptIconByName("Wherigo Cache", "://icons/cache/32x32/wherigo.png");
    setWptIconByName("Event Cache", "://icons/cache/32x32/event.png");
    setWptIconByName("Earthcache", "://icons/cache/32x32/earth.png");
    setWptIconByName("Letterbox Hybrid", "://icons/cache/32x32/letterbox.png");
    setWptIconByName("Virtual Cache", "://icons/cache/32x32/virtual.png");
    setWptIconByName("Webcam Cache", "://icons/cache/32x32/webcam.png");
    
    QDir dirIcon = CAppSetup::getPlattformInstance()->configDir("WaypointIcons");
    
    QString filename;
    QStringList filenames = dirIcon.entryList(QDir::Files);

    foreach(filename, filenames)
    {
        QFileInfo fi(filename);
        QString name = fi.baseName();
        setWptIconByName(name, dirIcon.filePath(filename));
    }
}

const QMap<QString, icon_t>& getWptIcons()
{
    return wptIcons;
}


void setWptIconByName(const QString& name, const QString& filename)
{
    QPixmap icon(filename);
    wptIcons[name] = icon_t(filename, icon.width()>>1, icon.height()>>1);
}


void setWptIconByName(const QString& name, const QPixmap& icon)
{
    QDir dirIcon = CAppSetup::getPlattformInstance()->configDir("WaypointIcons");
    QString filename = dirIcon.filePath(name + ".png");

    icon.save(filename);
    wptIcons[name] = icon_t(filename, icon.width()>>1, icon.height()>>1);
}

QPixmap loadIcon(const QString& path)
{
    QFileInfo finfo(path);
    if(finfo.completeSuffix() != "bmp")
    {
        return QPixmap(path);
    }
    else
    {
        QImage img = QPixmap(path).toImage().convertToFormat(QImage::Format_Indexed8);
        img.setColor(0,qRgba(0,0,0,0));
        return QPixmap::fromImage(img);
    }

    return QPixmap();
}

QPixmap getWptIconByName(const QString& name, QPointF &focus, QString * src)
{
    QPixmap icon;
    QString path;

    if(wptIcons.contains(name))
    {
        focus   = wptIcons[name].focus;
        path    = wptIcons[name].path;
    }
    else
    {
        focus   = wptIcons["Default"].focus;
        path    = wptIcons["Default"].path;
    }

    if(path.isEmpty())
    {
        path = wptDefault;
    }

    if(src)
    {
        *src = path;
    }

    icon = loadIcon(path);

    // Limit icon size to 22 pixel max.
    if(icon.width() > 22 || icon.height() > 22)
    {
        qreal s;
        if(icon.width() > icon.height())
        {
            s = 22.0/icon.width();
        }
        else
        {
            s = 22.0/icon.height();
        }

        focus = focus * s;
        icon  = icon.scaled(icon.size()*s,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }

    return icon;
}
