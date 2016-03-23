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
            ,eAxisTime
        };

        property_t() = default;
        property_t(const QString& key, const QString& name, const QString& unit, const QIcon& icon, axistype_e axisType, const QString& yLabel, qreal factor, fTrkPtGetVal getX, fTrkPtGetVal getY)
            : key(key)
            , name(name)
            , unit(unit)
            , icon(icon)
            , axisType(axisType)
            , yLabel(yLabel)
            , factor(factor)
            , getX(getX)
            , getY(getY)
        {
        }

        QString key;
        QString name;
        QString unit;
        QIcon icon;
        axistype_e axisType = eAxisDistance;
        QString yLabel;
        qreal min = NOFLOAT;
        qreal max = NOFLOAT;
        qreal factor = 1.0;
        fTrkPtGetVal getX = nullptr;
        fTrkPtGetVal getY = nullptr;
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

