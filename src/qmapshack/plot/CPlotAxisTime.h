/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CPLOTAXISTIME_H
#define CPLOTAXISTIME_H

#include "plot/CPlotAxis.h"

class CPlotAxisTime : public CPlotAxis
{
    Q_OBJECT
public:
    CPlotAxisTime(QObject* parent) : CPlotAxis(parent)
    {
    }

    virtual ~CPlotAxisTime()
    {
    }

    ///calculate format for the given value
    const QString fmtsgl(qreal /*val*/) override
    {
        return strFormat;
    }
    ///calculate format for the given value
    const QString fmtdbl(qreal /*val*/) override
    {
        return strFormat;
    }

    const tic_t* ticmark( const tic_t* t ) override;
protected:
    void calc() override;

    const QString strFormat = "hh:mm:ss";
};

#endif //CPLOTAXISTIME_H

