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

#include "GeoMath.h"
#include "gis/trk/CGisItemTrk.h"

#include <QtWidgets>

static QStringList writeCompeTime( const QDateTime& t, bool isTrack)
{
    QStringList result;
    QString dateFormat;
    QString monthStr;

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

void CGisItemTrk::saveTwoNav(const QDir& dir)
{
    QString filename = getName();
    filename = filename.remove(QRegExp("[^A-Za-z0-9_]"));
    filename = dir.absoluteFilePath(filename + ".trk");

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
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

}

void CGisItemTrk::readTowNav(const QString& filename)
{
    QString line("start");

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
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
                QMessageBox::information(0,QObject::tr("Error..."), QObject::tr("Only support lon/lat WGS 84 format."),QMessageBox::Abort,QMessageBox::Abort);
                return;
            }
            break;
        }

        case 'U':
        {
            QString name  = line.mid(1).simplified();
            if(name != "1")
            {
                QMessageBox::information(0,QObject::tr("Error..."), QObject::tr("Only support lon/lat WGS 84 format."),QMessageBox::Abort,QMessageBox::Abort);
                return;
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

    trk.segs << seg;

    if(trk.name.isEmpty())
    {
        QFileInfo fi(filename);
        trk.name = fi.baseName();
    }

    deriveSecondaryData();
}
