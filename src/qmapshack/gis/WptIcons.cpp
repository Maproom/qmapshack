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


#include "helpers/CSettings.h"
#include "setup/IAppSetup.h"
#include "WptIcons.h"

#include <QtGui>

static const char * wptDefault = "://icons/waypoints/32x32/Default.png";

static QMap<QString, icon_t> wptIcons;

void initWptIcons()
{
    wptIcons.clear();

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
    wptIcons["1stCategory"]         = icon_t("://icons/waypoints/32x32/1stCategory.png", 16, 16);
    wptIcons["2ndCategory"]         = icon_t("://icons/waypoints/32x32/2ndCategory.png", 16, 16);
    wptIcons["3rdCategory"]         = icon_t("://icons/waypoints/32x32/3rdCategory.png", 16, 16);
    wptIcons["4thCategory"]         = icon_t("://icons/waypoints/32x32/4thCategory.png", 16, 16);
    wptIcons["Danger"]              = icon_t("://icons/waypoints/32x32/Danger.png", 16, 16);
    wptIcons["FirstAid"]            = icon_t("://icons/waypoints/32x32/FirstAid.png", 16, 16);
    wptIcons["Food"]                = icon_t("://icons/waypoints/32x32/Food.png", 16, 16);
    wptIcons["Generic"]             = icon_t("://icons/waypoints/32x32/Generic.png", 16, 16);
    wptIcons["HorsCategory"]        = icon_t("://icons/waypoints/32x32/HorsCategory.png", 16, 16);
    wptIcons["Left"]                = icon_t("://icons/waypoints/32x32/Left.png", 16, 16);
    wptIcons["Right"]               = icon_t("://icons/waypoints/32x32/Right.png", 16, 16);
    wptIcons["Sprint"]              = icon_t("://icons/waypoints/32x32/Sprint.png", 16, 16);
    wptIcons["Straight"]            = icon_t("://icons/waypoints/32x32/Straight.png", 16, 16);
    wptIcons["Summit"]              = icon_t("://icons/waypoints/32x32/Summit.png", 16, 16);
    wptIcons["Valley"]              = icon_t("://icons/waypoints/32x32/Valley.png", 16, 16);
    wptIcons["Water"]               = icon_t("://icons/waypoints/32x32/Water.png", 16, 16);
    wptIcons["LeftFork"]            = icon_t("://icons/waypoints/32x32/LeftFork.png", 16, 16);
    wptIcons["RightFork"]           = icon_t("://icons/waypoints/32x32/RightFork.png", 16, 16);
    wptIcons["MiddleFork"]          = icon_t("://icons/waypoints/32x32/MiddleFork.png", 16, 16);
    wptIcons["SlightLeft"]          = icon_t("://icons/waypoints/32x32/SlightLeft.png", 16, 16);
    wptIcons["SharpLeft"]           = icon_t("://icons/waypoints/32x32/SharpLeft.png", 16, 16);
    wptIcons["SlightRight"]         = icon_t("://icons/waypoints/32x32/SlightRight.png", 16, 16);
    wptIcons["SharpRight"]          = icon_t("://icons/waypoints/32x32/SharpRight.png", 16, 16);
    wptIcons["UTurn"]               = icon_t("://icons/waypoints/32x32/UTurn.png", 16, 16);
    wptIcons["Start"]               = icon_t("://icons/waypoints/32x32/Start.png", 16, 16);
    wptIcons["End"]                 = icon_t("://icons/waypoints/32x32/End.png", 16, 16);

    setWptIconByName("Traditional Cache", "://icons/cache/32x32/traditional.png");
    setWptIconByName("Multi-cache", "://icons/cache/32x32/multi.png");
    setWptIconByName("Unknown Cache", "://icons/cache/32x32/unknown.png");
    setWptIconByName("Wherigo Cache", "://icons/cache/32x32/wherigo.png");
    setWptIconByName("Event Cache", "://icons/cache/32x32/event.png");
    setWptIconByName("Earthcache", "://icons/cache/32x32/earth.png");
    setWptIconByName("Letterbox Hybrid", "://icons/cache/32x32/letterbox.png");
    setWptIconByName("Virtual Cache", "://icons/cache/32x32/virtual.png");
    setWptIconByName("Webcam Cache", "://icons/cache/32x32/webcam.png");

    SETTINGS;
    QDir dirIcon(cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString());


    QStringList filenames = dirIcon.entryList(QString("*.bmp *.png").split(" "), QDir::Files);

    for(const QString &filename : filenames)
    {
        QFileInfo fi(filename);
        QString name = fi.completeBaseName();
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
    SETTINGS;
    QDir dirIcon(cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString());
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
