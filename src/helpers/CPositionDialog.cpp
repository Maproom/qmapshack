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

#include "CPositionDialog.h"
#include "GeoMath.h"

#include <QtWidgets>

QRegExp reCoord1("^\\s*([N|S]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s+([E|W|O]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s*$");

QRegExp reCoord2("^\\s*([N|S]){1}\\s*([0-9]+\\.[0-9]+)\\W*\\s+([E|W|O]){1}\\s*([0-9]+\\.[0-9]+)\\W*\\s*$");

QRegExp reCoord3("^\\s*([-0-9]+\\.[0-9]+)\\s+([-0-9]+\\.[0-9]+)\\s*$");

QRegExp reCoord4("^\\s*([N|S]){1}\\s*([0-9]+)\\W+([0-9]+)\\W+([0-9]+)\\W*([E|W|O]){1}\\W*([0-9]+)\\W+([0-9]+)\\W+([0-9]+)\\W*\\s*$");

QRegExp reCoord5("^\\s*([-0-9]+\\.[0-9]+)([N|S])\\s+([-0-9]+\\.[0-9]+)([W|E])\\s*$");


CPositionDialog::CPositionDialog(QWidget * parent, QPointF &pos)
    : QDialog(parent)
    , pos(pos)
{
    setupUi(this);
    QString str;
    GPS_Math_Deg_To_Str(pos.x(), pos.y(), str);
    lineEdit->setText(str);

    labelWarning->hide();

    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotEdit(QString)));
}

CPositionDialog::~CPositionDialog()
{
}

void CPositionDialog::accept()
{
    qreal lon, lat;
    QString str = lineEdit->text();

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
        int degLat    = reCoord4.cap(2).toInt();
        int minLat    = reCoord4.cap(3).toInt();
        int secLat    = reCoord4.cap(4).toInt();

        GPS_Math_DegMinSec_To_Deg(signLat, degLat, minLat, secLat, lat);

        bool signLon    = reCoord4.cap(5) == "W";
        int degLon    = reCoord4.cap(6).toInt();
        int minLon    = reCoord4.cap(7).toInt();
        int secLon    = reCoord4.cap(8).toInt();

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
        return;
    }

    pos.rx() = lon;
    pos.ry() = lat;

    QDialog::accept();
}

void CPositionDialog::slotEdit(const QString& str)
{
    if(reCoord1.exactMatch(str))
    {
        labelWarning->hide();
    }
    else if(reCoord2.exactMatch(str))
    {
        labelWarning->hide();
    }
    else if(reCoord3.exactMatch(str))
    {
        labelWarning->hide();
    }
    else if(reCoord4.exactMatch(str))
    {
        labelWarning->hide();
    }
    else if(reCoord5.exactMatch(str))
    {
        labelWarning->hide();
    }
    else
    {
        labelWarning->show();
    }
}
