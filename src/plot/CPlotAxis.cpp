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

#include "plot/CPlotAxis.h"

#include <QtWidgets>

inline qreal qLog10(qreal x)
{
    return( qLn(x)/qLn(10));
}

CPlotAxis::CPlotAxis( QObject * parent )
    : QObject( parent )
    , initialized( false )
    , autoscale( false )
    , scale( 1.0 )
    , usedMin( 0.0 )
    , usedMax( 0.0 )
    , limitMin( 0.0 )
    , limitMax( 0.0 )
    , interval( 0.0 )
    , ticStart( 0 )
    , scaleWidth( 0 )
    , ticType( eTicNorm )
    , firstTic( false )
    , lastTic( false )
    , points(0)
{
}

CPlotAxis::~CPlotAxis()
{
}

void CPlotAxis::setLimits(qreal min, qreal max)
{
    limitMin = min;
    limitMax = max;
}


void CPlotAxis::setMinMax( qreal givenMin, qreal givenMax )
{
    qreal tmp;

    if ( givenMin == givenMax )
    {
        if ( givenMin != 0.0 )
        {
            givenMin -= givenMin / 10.0;
            givenMax += givenMax / 10.0;
        }
        else
        {
            givenMin -= 0.1;
            givenMax += 0.1;
        }
    }

    if ( givenMin > givenMax )
    {
        tmp = givenMax;
        givenMax = givenMin;
        givenMin = tmp;
    }

    usedMin = givenMin;
    usedMax = givenMax;

    calc();

    initialized = true;
}


void CPlotAxis::calc()
{
    qreal tmpAbs = ( usedMax - usedMin ) < 0 ? -( usedMax - usedMin ) : ( usedMax - usedMin );
    qreal tmp = qLog10( tmpAbs / 10.0 );

    qreal exponent = ( int ) tmp;
    qreal residue = tmp - exponent;

    if ( residue < 0 && residue <= qLog10( 0.1 ) )
    {
        residue = qLog10( 0.1 );
    }
    else if ( residue > qLog10( 0.1 ) && residue <= qLog10( 0.2 ) )
    {
        residue = qLog10( 0.2 );
    }
    else if ( residue > qLog10( 0.2 ) && residue <= qLog10( 0.5 ) )
    {
        residue = qLog10( 0.5 );
    }
    else if ( residue > qLog10( 0.5 ) && residue <= qLog10( 1.0 ) )
    {
        residue = qLog10( 1.0 );
    }
    else if ( residue > qLog10( 1.0 ) && residue <= qLog10( 2.0 ) )
    {
        residue = qLog10( 2.0 );
    }
    else if ( residue > qLog10( 2.0 ) && residue <= qLog10( 5.0 ) )
    {
        residue = qLog10( 5.0 );
    }
    else if ( residue > qLog10( 5.0 ) && residue <= qLog10( 10. ) )
    {
        residue = qLog10( 10. );
    }

    interval = exponent + residue;
    interval = qPow( 10, interval );

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


const QString CPlotAxis::fmtsgl( qreal val )
{
    static QString f;
    qreal tmp;
    qreal exponent;
    qreal residue;

    if ( val != 0 )
    {
        if ( val < 0 )
        {
            val = -val;
        }
        tmp = qLog10( val );
        exponent = ( int ) tmp;
        residue = tmp - exponent;
    }
    else
    {
        exponent = 0;
        residue = 0;
    }

    if ( abs( ( int ) exponent ) > 5 )
    {
        f = "%1.2e";
    }
    else
    {
        if ( exponent >= 0 )
        {
            f = "%" + QString( "%1" ).arg( ( int ) ( exponent + 1 ) );
            if ( ( exponent == 0 ) && ( residue < 0 ) )
            {
                f += ".1f";
            }
            else
            {
                f += ".0f";
            }
        }
        else
        {
            f = "%1." + QString( "%1" ).arg( ( int ) ( -exponent + 1 ) ) + "f";
        }
    }

    return(f);
}


/**
   Generates a sprintf style format string for a given value.
   <pre>
   0.001   -> "%1.4f"
   0.01    -> "%1.3f"
   0.1     -> "%1.2f"
   1       -> "%1.1f"
   10      -> "%2.1f"
   >10000 scientific notation "%1.3e"
   </pre>

   @param val value to calculate the string on

   @return a zero terminated format string

 */
const QString CPlotAxis::fmtdbl( qreal val )
{
    static QString f;
    qreal tmp;
    qreal exponent;
    qreal residue;

    if ( val != 0 )
    {
        if ( val < 0 )
        {
            val = -val;
        }
        tmp = qLog10( val );
        exponent = ( int ) tmp;
        residue = tmp - exponent;
    }
    else
    {
        exponent = 0;
        residue = 0;
    }

    if ( abs( ( int ) exponent ) > 5 )
    {
        f = "%1.3e";
    }
    else
    {
        if ( exponent >= 0 )
        {
            f = "%" + QString( "%1" ).arg( ( int ) ( exponent + 1 ) );
            if ( ( exponent == 0 ) && ( residue < 0 ) )
            {
                f += ".2f";
            }
            else
            {
                f += ".1f";
            }
        }
        else
        {
            f = "%1." + QString( "%1" ).arg( ( int ) ( -exponent + 2 ) ) + "f";
        }
    }
    return(f);
}


int CPlotAxis::getScaleWidth( const QFontMetrics& m )
{
    if ( scaleWidth > 0 )
    {
        return( scaleWidth * m.width( " " ));
    }

    int width = 0;
    int tmp;
    QString format_single_prec = fmtsgl( interval );

    const tic_t * t = ticmark();
    while ( t )
    {
        tmp = m.width( QString().sprintf( format_single_prec.toLatin1().data(), t->val ) );
        if ( tmp > width )
        {
            width = tmp;
        }
        t = ticmark( t );
    }
    return(width);
}


void CPlotAxis::getLimits(qreal& limMin, qreal& limMax, qreal& useMin, qreal& useMax)
{
    limMin = limitMin;
    limMax = limitMax;
    useMin = usedMin;
    useMax = usedMax;
}


const CPlotAxis::tic_t* CPlotAxis::ticmark( const tic_t * t )
{
    QString format_single_prec = fmtsgl( interval );

    switch ( ticType )
    {
    case eNoTic:
        return( 0);
        break;

    case eTicMinMax:
        if ( t == NULL )
        {
            tic.val = usedMin;
            firstTic = true;
        }
        else if ( firstTic == true )
        {
            tic.val = usedMax;
            firstTic = false;
        }
        else
        {
            return( 0);
        }
        break;

    case eTicNorm:
        if ( interval == 0 )
        {
            //qWarning() << "CPlotAxis::ticmark() mode 'norm': interval == 0";
            return( 0);
        }
        if ( t == NULL )
        {
            tic.val = ticStart;
        }
        else
        {
            tic.val += interval;
            if ( ( tic.val - usedMax ) > interval / 20 )
            {
                return( 0);
            }
        }
        break;

    case eTicFull:
        if ( t == NULL )
        {
            tic.val = usedMin;
            firstTic = true;
        }
        else if ( firstTic == true )
        {
            tic.val = ticStart;
            firstTic = false;
        }
        else if ( lastTic == true )
        {
            lastTic = false;
            return( 0);
        }
        else
        {
            tic.val += interval;
            if ( ( tic.val - usedMax ) > interval / 20 )
            {
                tic.val = usedMax;
                lastTic = true;
            }
        }
        break;
    }

    tic.lbl.sprintf( format_single_prec.toLatin1(), tic.val );

    return(&tic);
}


void CPlotAxis::setScale( const unsigned int pts )
{
    //if ( !initialized )
    //qWarning( "you try to set the scale before defining the min & max value. not very sensible." );
    points = pts;
    scale = pts / ( usedMax - usedMin );
}


void CPlotAxis::resetZoom()
{
    setMinMax(limitMin, limitMax);
}


void CPlotAxis::zoom(bool in, int point)
{
    qreal min, p, d, factor;
    if (in)
    {
        factor = 1/1.1;
    }
    else
    {
        factor = 1.1;
    }

    p = pt2val(point);
    min = (p - usedMin) * (1 - factor) + usedMin;
    d = min - usedMin * factor;

    setMinMax(min, usedMax * factor + d);
    move(0);
}


void CPlotAxis::move(int delta_pt)
{
    qreal delta_val = pt2val(delta_pt) - pt2val(0);
    bool f = !(usedMax - usedMin < limitMax - limitMin);
    if (f ^ (usedMin + delta_val < limitMin))
    {
        delta_val = (limitMin - usedMin);
    }
    if (f ^ (usedMax + delta_val > limitMax))
    {
        delta_val = (limitMax - usedMax);
    }
    setMinMax(usedMin + delta_val, usedMax + delta_val);
}
