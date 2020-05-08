/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CPROPERTYTRK_H
#define CPROPERTYTRK_H

#include "gis/trk/CGisItemTrk.h"
#include "plot/CPlotData.h"

#include <QObject>

class QComboBox;
class CPlot;
class CLimit;

class CPropertyTrk
{
    Q_DECLARE_TR_FUNCTIONS(CPropertyTrk)
public:
    virtual ~CPropertyTrk() = default;

    struct property_t
    {
        enum axistype_e
        {
            eAxisDistance
            , eAxisTime
        };

        property_t() = default;
        property_t(const QString& key,
                   const QString& nameShortText,
                   const QString& nameLongText,
                   const QIcon& icon,
                   axistype_e axisType,
                   fTrkPtGetVal getX,
                   const QString& unit,
                   const QString& yLabel,
                   qreal factor,
                   fTrkPtGetVal getY)
            : key(key)
            , nameShortText(nameShortText)
            , nameLongText(nameLongText)
            , icon(icon)
            , axisType(axisType)
            , getX(getX)
            , unit(unit)
            , yLabel(yLabel)
            , factor(factor)
            , getY(getY)
        {
        }

        // key/extension source of the property
        QString key;
        // long and short name of the property
        QString nameShortText;
        QString nameLongText;
        // symbol representing the property
        QIcon icon;

        // the x-axis type to be used in plots
        axistype_e axisType = eAxisDistance;
        // access function to the x-values
        fTrkPtGetVal getX = nullptr;

        // the unit of the y-axis
        QString unit;
        // the y-axis label
        QString yLabel;
        // a scaling factor for the y-values (e.g. m/h -> km/h)
        qreal factor = 1.0;
        // access function to the y-values
        fTrkPtGetVal getY = nullptr;
        // lower limit of the y-axis
        qreal min = NOFLOAT;
        // upper limit of the y-axis
        qreal max = NOFLOAT;
    };

    void fillComboBox(QComboBox * box) const;

    void setupData();
    void setupPlot(CPlot * plot, const QString &source) const;
    const property_t &propBySource(const QString& source) const;

private:
    friend class CGisItemTrk;
    CPropertyTrk(CGisItemTrk &trk);

    CGisItemTrk& trk;

    QList<property_t> properties;
};

#endif //CPROPERTYTRK_H

