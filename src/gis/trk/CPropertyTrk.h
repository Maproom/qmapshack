/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "plot/CPlot.h"
#include "plot/CPlotData.h"

#include <QObject>

class QComboBox;

class CPropertyTrk
{
public:
    virtual ~CPropertyTrk() = default;

    struct property_t
    {
        property_t() = default;
        property_t(const QString& key, const QString& name, const QIcon& icon, CPlotData::axistype_e axisType, const QString& xLabel, const QString& yLabel, qreal factor, CPlot::funcGet getX, CPlot::funcGet getY)
            : key(key)
            , name(name)
            , icon(icon)
            , axisType(axisType)
            , xLabel(xLabel)
            , yLabel(yLabel)
            , factor(factor)
            , getX(getX)
            , getY(getY)
        {
        }

        QString key;
        QString name;
        QIcon icon;
        CPlotData::axistype_e axisType = CPlotData::eAxisLinear;
        QString xLabel;
        QString yLabel;
        qreal factor = 1.0;
        CPlot::funcGet getX = nullptr;
        CPlot::funcGet getY = nullptr;
    };

    void fillComboBox(QComboBox * box) const;

    void setupPlot(CPlot * plot, int idx) const;

private:
    friend class CGisItemTrk;
    CPropertyTrk(const CGisItemTrk &trk);

    const CGisItemTrk& trk;

    QList<property_t> properties;
};

#endif //CPROPERTYTRK_H

