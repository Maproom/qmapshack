/**********************************************************************************************
    Copyright (C) 2006, 2007 Oliver Eichler oliver.eichler@gmx.de

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
#include "CGarminPoint.h"
#include "Garmin.h"
#include "helpers/Platform.h"

#include <QtCore>

CGarminPoint::CGarminPoint()
    : type(0)
    , isLbl6(false)
    , hasSubType(false)
    , lbl_ptr(0xFFFFFFFF)
{
}


CGarminPoint::~CGarminPoint()
{
}


quint32 CGarminPoint::decode(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8 * pData)
{
    qint16 dLng, dLat;

    type        = (quint16)(*pData) << 8;

    ++pData;

    lbl_ptr     = gar_ptr_load(uint24_t, pData);
    hasSubType  = lbl_ptr & 0x00800000;
    isLbl6      = lbl_ptr & 0x00400000;
    lbl_ptr     = lbl_ptr & 0x003FFFFF;

    pData += 3;

    dLng = gar_ptr_load(int16_t, pData); pData += 2;
    dLat = gar_ptr_load(int16_t, pData); pData += 2;

    qint32 x1,y1;

    x1 = ((qint32)dLng << shift) + iCenterLon;
    y1 = ((qint32)dLat << shift) + iCenterLat;
    pos = QPointF(GARMIN_RAD(x1),GARMIN_RAD(y1));

#ifdef DEBUG_SHOW_POINTS
    qDebug() << x1 << y1 << point.u << point.v;
#endif

    if(hasSubType)
    {
        type |= *pData;
        return 9;
    }

    return 8;
}


quint32 CGarminPoint::decode2(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8 * pData, const quint8 * pEnd)
{
    qint16 dLng, dLat;
    quint32 byte_size = 6;
    quint8 subtype;

    type        = (quint16)(*pData) << 8;
    ++pData;
    subtype     = (quint16)(*pData);
    ++pData;

    type        = 0x10000 + type + (subtype & 0x1F);

    if(subtype & 0x80)
    {
        byte_size += 1;
    }

    dLng = gar_ptr_load(int16_t, pData); pData += 2;
    dLat = gar_ptr_load(int16_t, pData); pData += 2;

    qint32 x1,y1;

    x1 = ((qint32)dLng << shift) + iCenterLon;
    y1 = ((qint32)dLat << shift) + iCenterLat;
    pos = QPointF(GARMIN_RAD(x1),GARMIN_RAD(y1));

    if(subtype & 0x20)
    {
        byte_size += 3;
        lbl_ptr     = gar_ptr_load(uint24_t, pData);
        isLbl6      = lbl_ptr & 0x00400000;
        lbl_ptr    &= 0x003FFFFF;
    }

    return byte_size;
}
