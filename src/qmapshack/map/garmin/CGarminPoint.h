/**********************************************************************************************
    Copyright (C) 2006-2007 Oliver Eichler <oliver.eichler@gmx.de>

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

   Garmin and MapSource are registered trademarks or trademarks of Garmin Ltd.
   or one of its subsidiaries.

   This source is based on John Mechalas documentation "Garmin IMG File Format" found
   at sourceforge. The missing bits and error were rectified by the source code of
   Konstantin Galichsky (kg@geopainting.com), http://www.geopainting.com

**********************************************************************************************/
#ifndef CGARMINPOINT_H
#define CGARMINPOINT_H

#include "CGarminTyp.h"
#include "poi/CPoiItem.h"

#include <QPointF>
#include <QStringList>
#include <QtGlobal>

class CGarminTile;
class CMapDraw;

class CGarminPoint : public CPoiItem
{
public:
    [[deprecated("Only to enable use of CGarminPoint in QVector and similar until we upgrade to Qt >= 5.13. DO NOT USE!")]]
    CGarminPoint(){}
    CGarminPoint(const QMap<quint32, CGarminTyp::point_property>* pointProperties, const qint8* selectedLanguage);
    virtual ~CGarminPoint() = default;

    QString getName() const override;
    QString getDesc() const override {return getLabelText();}

    quint32 decode(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData);
    quint32 decode2(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData, const quint8* pEnd);

    QString getLabelText() const;

    bool hasLabel() const
    {
        return !labels.isEmpty();
    }

    quint32 type = 0;
    bool isLbl6 = false;
    bool hasSubType = false;

    QPointF getPxPos(CMapDraw* map) const;

    QStringList labels;

    quint32 lbl_ptr = 0xFFFFFFFF;
private:
    const QMap<quint32, CGarminTyp::point_property>* pointProperties;
    const qint8* selectedLanguage;
};
#endif                           //CGARMINPOINT_H
