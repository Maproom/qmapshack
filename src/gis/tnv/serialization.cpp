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

#include "CMainWindow.h"
#include "GeoMath.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

#include <QtWidgets>

struct twonav_icon_t
{
    const char * twonav;
    const char * qlgt;
};

static const twonav_icon_t TwoNavIcons[] =
{
    {"City (Capitol)","City (Capitol)"}
    ,{"City (Large)","City (Large)"}
    ,{"City (Medium)","City (Medium)"}
    ,{"City (Small)","City (Small)"}
    ,{"City (Small)","Small City"}
    ,{"Closed Box","Geocache"}
    ,{"Open Box","Geocache Found"}
    ,{"Red Flag","Flag, Red"}
    ,{"Blue Flag","Flag, Blue"}
    ,{"Green Flag","Flag, Green"}
    ,{"Red Booble","Pin, Red"}
    ,{"Blue Booble","Pin, Blue"}
    ,{"Green Booble","Pin, Green"}
    ,{"Red Cube","Block, Red"}
    ,{"Blue Cube","Block, Blue"}
    ,{"Green Cube","Block, Green"}
    ,{"Blue Diamond","Blue Diamond"}
    ,{"Green Diamond","Green Diamond"}
    ,{"Red Diamond","Red Diamond"}
    ,{"Traditional Cache","Traditional Cache"}
    ,{"Multi-cache","Multi-cache"}
    ,{"Unknown Cache","Unknown Cache"}
    ,{"Wherigo","Wherigo Cache"}
    ,{"Event Cache","Event Cache"}
    ,{"Earthcache","Earthcache"}
    ,{"Letterbox","Letterbox Hybrid"}
    ,{"Virtual Cache","Virtual Cache"}
    ,{"Webcam Cache","Webcam Cache"}
    ,{0,0}
};


static QStringList writeCompeTime( const QDateTime& t, bool isTrack)
{
    QStringList result;
    QString dateFormat;
    QString monthStr;

    if(!t.isValid())
    {
        if(isTrack)
        {
            result << "01-Jan-1970" << "00:00:00.000";
        }
        else
        {
            result << "01-Jan-1970" << "00:00:00";
        }
        return result;
    }

    QDateTime timestamp = t.toTimeSpec(Qt::UTC);

    switch(timestamp.date().month())
    {
    case 1:
        monthStr = "Jan";
        break;

    case 2:
        monthStr = "Feb";
        break;

    case 3:
        monthStr = "Mar";
        break;

    case 4:
        monthStr = "Apr";
        break;

    case 5:
        monthStr = "May";
        break;

    case 6:
        monthStr = "Jun";
        break;

    case 7:
        monthStr = "Jul";
        break;

    case 8:
        monthStr = "Aug";
        break;

    case 9:
        monthStr = "Sep";
        break;

    case 10:
        monthStr = "Oct";
        break;

    case 11:
        monthStr = "Nov";
        break;

    case 12:
        monthStr = "Dec";
        break;
    }

    if(isTrack)
    {
        dateFormat = QString("dd-'%1'-yy").arg(monthStr);
    }
    else
    {
        dateFormat = QString("dd-'%1'-yyyy").arg(monthStr);
    }

    result << timestamp.toString(dateFormat);

    if(isTrack)
    {
        result << timestamp.toString("hh:mm:ss.000");
    }
    else
    {
        result << timestamp.toString("hh:mm:ss");
    }

    return result;
}

static QDateTime readCompeTime(QString str, bool isTrack)
{
    QDateTime timestamp;
    QRegExp re("([0-9]{2})-([A-Za-z]{3})-.*");

    if(re.exactMatch(str))
    {
        QString monthNum;
        QString monthStr = re.cap(2);

        if(monthStr.toUpper() == "JAN")
        {
            monthNum = "01";
        }
        else if(monthStr.toUpper() == "FEB")
        {
            monthNum = "02";
        }
        else if(monthStr.toUpper() == "MAR")
        {
            monthNum = "03";
        }
        else if(monthStr.toUpper() == "APR")
        {
            monthNum = "04";
        }
        else if(monthStr.toUpper() == "MAY")
        {
            monthNum = "05";
        }
        else if(monthStr.toUpper() == "JUN")
        {
            monthNum = "06";
        }
        else if(monthStr.toUpper() == "JUL")
        {
            monthNum = "07";
        }
        else if(monthStr.toUpper() == "AUG")
        {
            monthNum = "08";
        }
        else if(monthStr.toUpper() == "SEP")
        {
            monthNum = "09";
        }
        else if(monthStr.toUpper() == "OCT")
        {
            monthNum = "10";
        }
        else if(monthStr.toUpper() == "NOV")
        {
            monthNum = "11";
        }
        else if(monthStr.toUpper() == "DEC")
        {
            monthNum = "12";
        }

        str.replace(monthStr, monthNum);

        if(isTrack)
        {
            timestamp = QDateTime::fromString(str, "dd-MM-yy hh:mm:ss.zzz");
            timestamp = timestamp.addYears(100);
        }
        else
        {
            timestamp = QDateTime::fromString(str, "dd-MM-yyyy hh:mm:ss");
        }
    }

    timestamp.setTimeSpec(Qt::UTC);
    return timestamp;
}

static QString iconTwoNav2QlGt(const QString& sym)
{
    int i = 0;
    while(TwoNavIcons[i].qlgt)
    {
        if(sym == TwoNavIcons[i].twonav)
        {
            return TwoNavIcons[i].qlgt;
        }

        i++;
    }

    return sym;
}


static QString iconQlGt2TwoNav(const QString& sym)
{
    int i = 0;
    while(TwoNavIcons[i].qlgt)
    {
        if(sym == TwoNavIcons[i].qlgt)
        {
            return TwoNavIcons[i].twonav;
        }

        i++;
    }

    return sym;
}

static QString makeUniqueName(const QString& name, const QDir& dir)
{
    int cnt = 0;

    QFileInfo fi(name);
    QString tmp(name);

    while(dir.exists(tmp))
    {
        tmp = QString("%1_%2.%3").arg(fi.baseName()).arg(cnt++).arg(fi.completeSuffix());
    }

    return tmp;
}



bool CGisItemTrk::saveTwoNav(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(&CMainWindow::self(), QObject::tr("Error..."), QObject::tr("Failed to open %1.").arg(filename), QMessageBox::Abort);
        return false;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    out << "B  UTF-8" << endl;
    out << "G  WGS 84" << endl;
    out << "U  1" << endl;

    QString name = getName();
    name = name.replace(" ","_");

    QColor color = getColor();

    QStringList list;
    list << "C";
    list << QString::number(color.red());
    list << QString::number(color.green());
    list << QString::number(color.blue());
    list << "5";                 // ???
    list << "1";                 // ???
    out << list.join(" ") << endl;

    out << "s " << name << endl;
    out << "k " << getKey().item << endl;


    foreach(const CGisItemTrk::trkseg_t& seg, trk.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
        {
            list.clear();

            list << "T";
            list << "A";
            list << (trkpt.lat > 0 ? QString("%1%2N") : QString("%1%2S")).arg(trkpt.lat,0,'f').arg(QChar(186));
            list << (trkpt.lon > 0 ? QString("%1%2E") : QString("%1%2W")).arg(trkpt.lon,0,'f').arg(QChar(186));
            list << writeCompeTime(trkpt.time, true);
            list << "s";
            list << QString("%1").arg(trkpt.ele == NOINT ? 0 : trkpt.ele);
            list << "0.000000";
            list << "0.000000";
            list << "0.000000";
            list << "0";
            list << "-1000.000000";
            list << "-1.000000";
            list << "-1";
            list << "-1.000000";
            list << "-1";
            list << "-1";
            list << "-1";
            list << "-1.000000";

            out << list.join(" ") << endl;
        }
    }

    return true;
}

bool CGisItemTrk::readTwoNav(const QString& filename)
{
    QString line("start");

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Failed to open %1.").arg(filename),QMessageBox::Abort,QMessageBox::Abort);
        return false;
    }
    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("UTF-8"));

    trkseg_t seg;

    while(!line.isEmpty())
    {
        line = in.readLine();
        switch(line[0].toLatin1())
        {
        case 'B':
        {
            QString name        = line.mid(1).simplified();
            QTextCodec * codec  = QTextCodec::codecForName(name.toLatin1());
            if(codec)
            {
                in.setCodec(codec);
            }
            break;
        }

        case 'G':
        {
            QString name  = line.mid(1).simplified();
            if(name != "WGS 84")
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Only support lon/lat WGS 84 format."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }
            break;
        }

        case 'U':
        {
            QString name  = line.mid(1).simplified();
            if(name != "1")
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Only support lon/lat WGS 84 format."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }
            break;
        }

        case 'C':
        {
            QStringList values = line.split(' ', QString::SkipEmptyParts);
            QColor c(values[1].toInt(),values[2].toInt(),values[3].toInt());
            setColor(c);
            break;
        }

        case 'T':
        {
            trkpt_t pt;
            QStringList values = line.split(' ', QString::SkipEmptyParts);

            if(values.size() < 8)
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Failed to read data."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }

            QString lat = values[2].replace(QChar(186),"");
            QString lon = values[3].replace(QChar(186),"");
            GPS_Math_Str_To_Deg(lat + " " + lon, pt.lon, pt.lat);

            pt.time = readCompeTime(values[4] + " " + values[5], true);
            pt.ele = values[7].toFloat();

            seg.pts << pt;
            break;
        }

        case 's':
        {
            trk.name = line.mid(1).simplified();
            trk.name = trk.name.replace("_", " ");
            break;
        }

        case 'k':
        {
            key.item = line.mid(1).simplified();
            break;
        }
        }
    }
    file.close();

    trk.segs << seg;

    if(trk.name.isEmpty())
    {
        QFileInfo fi(filename);
        trk.name = fi.baseName();
    }

    deriveSecondaryData();

    return true;
}


void CGisItemWpt::saveTwoNav(QTextStream& out, const QDir& dir)
{
    QString name = getName();
    name = name.replace(" ","_");

    QString comment = getComment();
    comment = removeHtml(comment);
    if(comment.isEmpty())
    {
        comment = getDescription();
        comment = removeHtml(comment);
    }
    comment = comment.replace("\n","%0A%0D");

    QStringList list;
    list << "W";
    list << name;
    list << "A";
    list << (wpt.lat > 0 ? QString("%1%2N") : QString("%1%2S")).arg(wpt.lat,0,'f').arg(QChar(186));
    list << (wpt.lon > 0 ? QString("%1%2E") : QString("%1%2W")).arg(wpt.lon,0,'f').arg(QChar(186));
    list << writeCompeTime(wpt.time, false);
    list << QString("%1").arg(wpt.ele == NOINT ? 0 : wpt.ele);

    out << list.join(" ") << " ";
    out << comment << endl;

    list.clear();
    list << iconQlGt2TwoNav(getIconName());
    list << "0";                 //test position
    list << "-1.0";
    list << "0";
    list << QString("%1").arg(QColor(Qt::darkBlue).value());
    list << "1";
    list << "37";                // 1 Name 2 Beschreibung 4 Symbol 8 Hhe 16 URL 32 Radius
    list << "";                  //wpt->link;
    list << QString("%1").arg(proximity == NOFLOAT ? 0 : proximity,0,'f');
    list << getKey().item;

    out << "w ";
    out << list.join(",");
    out << endl;

    foreach(const image_t &img, images)
    {
        QString fn = img.info;
        if(fn.isEmpty())
        {
            fn = QString("picture.png");
        }

        QFileInfo fi(fn);

        if(!(fi.completeSuffix().toLower() == "png"))
        {
            fn = fi.baseName() + ".png";
        }

        fn = makeUniqueName(fn, dir);
        img.pixmap.save(dir.absoluteFilePath(fn));
        out << "a " << ".\\" << fn << endl;
    }

    if(isGeocache())
    {
        // write geocache data
        QDomDocument doc;
        QDomElement gpxCache = doc.createElement("groundspeak:cache");
        writeGcExt(gpxCache);
        doc.appendChild(gpxCache);

        out << "e" << endl;
        out << doc.toString();
        out << "ee" << endl;
    }
}

bool CTwoNavProject::loadWpts(const QString& filename, const QDir& dir)
{
    wpt_t wpt;
    QString line("start");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Failed to open %1.").arg(filename),QMessageBox::Abort,QMessageBox::Abort);
        return false;
    }
    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("UTF-8"));

    while(!line.isEmpty())
    {
        line = in.readLine();

        switch(line[0].toLatin1())
        {
        case 'B':
        {
            QString name        = line.mid(1).simplified();
            QTextCodec * codec  = QTextCodec::codecForName(name.toLatin1());
            if(codec)
            {
                in.setCodec(codec);
            }
            break;
        }

        case 'G':
        {
            QString name  = line.mid(1).simplified();
            if(name != "WGS 84")
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Only support lon/lat WGS 84 format."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }
            break;
        }

        case 'U':
        {
            QString name  = line.mid(1).simplified();
            if(name != "1")
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Only support lon/lat WGS 84 format."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }
            break;
        }

        case 'W':
        {
            if(wpt.valid)
            {
                new CGisItemWpt(wpt, this);
            }

            wpt = wpt_t();
            QStringList values = line.split(' ', QString::SkipEmptyParts);

            if(values.size() < 8)
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Failed to read data."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }

            wpt.name = values[1];

            QString lat = values[3].replace(QChar(186),"");
            QString lon = values[4].replace(QChar(186),"");
            GPS_Math_Str_To_Deg(lat + " " + lon, wpt.lon, wpt.lat);

            wpt.time = readCompeTime(values[5] + " " + values[6], false);
            wpt.ele  = values[7].toFloat();

            if(values.size() > 7)
            {
                QStringList list = values.mid(8);
                wpt.comment = list.join(" ");
            }

            break;
        }

        case 'w':
        {
            QStringList values = line.mid(1).simplified().split(',', QString::KeepEmptyParts);

            if(values.size() < 10)
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Failed to read data."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }


            wpt.symbol  = iconTwoNav2QlGt(values[0]);

            wpt.url     = values[7];
            wpt.prox    = values[8].toFloat();
            wpt.key     = values[9];

            if(wpt.prox == 0)
            {
                wpt.prox = NOFLOAT;
            }
            if(wpt.ele == 0)
            {
                wpt.ele = NOINT;
            }
            if(wpt.key == "0")
            {
                wpt.key.clear();
            }

            wpt.name = wpt.name.replace("_", " ");

            wpt.valid = true;
            break;
        }

        case 'e':
        {
            QString str;

            while(!in.atEnd())
            {
                line = in.readLine();
                if(line == "ee")
                {
                    break;
                }

                str += line;
            }


            QString errorMsg;
            int errorLine = 0;
            int errorColumn = 0;
            wpt.gpx.setContent(str, &errorMsg, &errorLine, &errorColumn);
            break;
        }

        case 'a':
        {
            img_t img;
            QStringList values = line.mid(1).simplified().split(',', QString::KeepEmptyParts);
            if(values.size() < 1)
            {
                QMessageBox::information(&CMainWindow::self(),QObject::tr("Error..."), QObject::tr("Failed to read data."),QMessageBox::Abort,QMessageBox::Abort);
                return false;
            }

            QString fn = values[0].simplified();

#ifndef WIN32
            fn = fn.replace("\\","/");
#endif
            QFileInfo fi(dir.absoluteFilePath(fn));
            img.image.load(dir.absoluteFilePath(fn));
            if(!img.image.isNull())
            {
                img.filename    = fi.fileName();
                img.info        = fi.baseName();
                wpt.images << img;
            }

            break;
        }
        }
    }

    if(wpt.valid)
    {
        new CGisItemWpt(wpt, this);
    }

    return true;
}

void CGisItemWpt::readTwoNav(const CTwoNavProject::wpt_t &tnvWpt)
{
    wpt.lon     = tnvWpt.lon;
    wpt.lat     = tnvWpt.lat;
    wpt.ele     = tnvWpt.ele;
    proximity   = tnvWpt.prox;
    wpt.time    = tnvWpt.time;
    wpt.name    = tnvWpt.name;
    wpt.cmt     = tnvWpt.comment;
    wpt.sym     = tnvWpt.symbol;
    key.item    = tnvWpt.key;

    foreach(const CTwoNavProject::img_t& img, tnvWpt.images)
    {
        CGisItemWpt::image_t image;
        image.fileName  = img.filename;
        image.info      = img.info;
        image.pixmap    = img.image;
        images << image;
    }

    const QDomNode& xmlCache = tnvWpt.gpx.namedItem("groundspeak:cache");
    if(!xmlCache.isNull())
    {
        readGcExt(xmlCache);
    }


    setIcon();
}
