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

#include "plot/CPlotAxisTime.h"

#include <QtWidgets>

CPlotAxisTime::CPlotAxisTime(QObject * parent)
: CPlotAxis(parent)
{

}


CPlotAxisTime::~CPlotAxisTime()
{

}


void CPlotAxisTime::calc()
{

    int dSec    = usedMax - usedMin;
    ticStart    = usedMin;
    strFormat   = "hh:mm:ss";

    if(dSec < 20)
    {
        interval = 1;
        ticStart = usedMin;
    }
    else if(dSec < 100)
    {
        interval = 5;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 200)
    {
        interval = 10;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 600)
    {
        interval = 30;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 1200)
    {
        interval = 60;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 6000)
    {
        interval = 600;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 12000)
    {
        interval = 600;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 36000)
    {
        interval = 1800;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 72000)
    {
        interval = 3600;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else if(dSec < 216000)
    {
        interval = 10800;
        ticStart = qCeil(usedMin / interval) * interval;
    }
    else
    {
        qDebug() << "ouch";
    }

    if ( autoscale )
    {
        usedMin = qFloor( usedMin / interval ) * interval;
        usedMax = qCeil( usedMax / interval ) * interval;
    }
    else
    {
        usedMin = usedMin;
        usedMax = usedMax;
    }

    int t1 = ( int )( usedMin / interval + 0.5);
    ticStart = interval * t1;
    if ( ticStart < usedMin )
    {
        ticStart += interval;
    }

}


const CPlotAxis::tic_t* CPlotAxisTime::ticmark( const tic_t * t )
{
    const tic_t * _tic_ = CPlotAxis::ticmark(t);
    if(_tic_)
    {
        QDateTime time = QDateTime::fromTime_t(tic.val);
        time.setTimeSpec(Qt::LocalTime);
        tic.lbl = time.toString(strFormat);
    }

    return _tic_;
}
