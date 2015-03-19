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

#ifndef CPLOTAXIS_H
#define CPLOTAXIS_H

#include <QObject>

class QFontMetrics;

class CPlotAxis : public QObject
{
    Q_OBJECT
public:
    CPlotAxis(QObject * parent);
    virtual ~CPlotAxis();

    /// tic mark information structure
    struct tic_t
    {
        tic_t()
        {
            val=0; lbl="";
        }
        qreal val;
        QString lbl;
    };

    ///tic type
    enum tictype_e
    {
        eNoTic,                   /**< no tics are produced*/
        eTicMinMax,               /**< only min max tics are produced*/
        eTicNorm,                 /**< tics by interval*/
        eTicFull                  /**< minmax && norm*/
    };

    ///zoom in/out with a given point as static
    virtual void zoom(bool in, int point);
    ///set the desired minimum and maximum value equal to limit values
    virtual void resetZoom();
    ///add delta_pt to min and max values
    virtual void move(int delta);
    ///set the desired minimum and maximum value
    virtual void setMinMax(qreal givenMin, qreal givenMax);
    ///set the limit minimum and maximum value
    virtual void setLimits(qreal min, qreal max);
    ///set the scale factor for a given size in points
    virtual void setScale(const unsigned int pts);
    ///calculate format for the given value
    virtual const QString fmtsgl(qreal val);
    ///calculate format for the given value
    virtual const QString fmtdbl(qreal val);
    ///get the maximum width of a scale with provided fontmetrics
    virtual int getScaleWidth(const QFontMetrics& m);
    ///get a new ticmark object
    virtual const tic_t* ticmark(const tic_t * t = NULL);
    /// get the total limits and the used ones
    virtual void getLimits(qreal& limMin, qreal& limMax, qreal& useMin, qreal& useMax);

    inline int val2pt( qreal val ) const
    {
        if ( scale == 0 )
        {
            return 0;
        }
        return ( int ) ( ( val - usedMin ) * scale + 0.5 );
    }

    inline qreal pt2val( int pt ) const
    {
        if ( scale == 0 )
        {
            return 0;
        }
        return ( qreal ) ( ( (qreal)pt - 0.5 ) / scale + usedMin );
    }

    void setAutoscale(bool on)
    {
        autoscale = on;
    }

    inline tictype_e getTicType()
    {
        return ticType;
    }
    inline tictype_e setTicType(tictype_e t)
    {
        tictype_e old = ticType;
        ticType = t;
        return old;
    }

    qreal min() const
    {
        return usedMin;
    }
    qreal max() const
    {
        return usedMax;
    }

    bool isValid() const
    {
        return valid;
    }

protected:
    virtual void calc();

    ///true if axis has been initialized
    bool initialized;
    ///true if autoscaling
    bool autoscale;

    bool valid;

    ///scalefactor
    qreal scale;

    ///the actual applied min value
    qreal usedMin;
    ///the actual applied max value
    qreal usedMax;

    qreal limitMin;
    qreal limitMax;

    ///the intervall of the ticmarks
    qreal interval;

    ///start value of the tic marks
    qreal ticStart;

    /// this is set to -1 by default
    /**
        a value > 0 will override the dynamic value in getScaleWidth();
     */
    qint32 scaleWidth;

    ///the ticmark generation type
    tictype_e ticType;
    ///local copy of the last ticmark object
    tic_t tic;

    /// used by ticmark()
    bool firstTic;
    /// used by ticmark()
    bool lastTic;

    ///points of dimension
    quint32 points;
};

#endif //CPLOTAXIS_H

