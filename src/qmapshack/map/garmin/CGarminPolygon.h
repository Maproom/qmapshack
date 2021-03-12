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
#ifndef CGARMINPOLYGON_H
#define CGARMINPOLYGON_H

#include "gis/proj_x.h"

#ifdef __MINGW32__
#undef LP
#endif
#include <QPolygonF>
#include <QtCore>

struct subdiv_desc_t;
struct sign_info_t;

class CGarminPolygon
{
public:
    CGarminPolygon() = default;
    virtual ~CGarminPolygon() = default;

    quint32 decode(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, bool line, const quint8 * pData, const quint8 * pEnd);
    quint32 decode2(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, bool line, const quint8 * pData, const quint8 * pEnd);

    QString getLabelText() const;

    bool hasLabel() const
    {
        return !labels.isEmpty();
    }

    quint32 type = 0;
    /// direction of line (polyline, only)
    bool direction = false;
    /// the label offset
    quint32 lbl_info = 0;
    /// true if label offset has to be used in NET section
    bool lbl_in_NET = false;
    ///
    bool hasV2Label = false;
    /// delta longitude from subdivision center
    qint16 dLng = 0;
    /// delta latitude from subdivision center
    qint16 dLat = 0;
    /** @brief the actual polyline points as [pixel]
       @note After decode() or decode2() the content will be the same as coords.
            It is up to the render object to convert it into pixel coordinates
     */
    QPolygonF pixel;
    /// the actual polyline points as longitude / latitude [rad]
    QPolygonF coords;

    quint32 id = 0;

    QStringList labels;

    static quint32 cnt;
    static qint32 maxVecSize;
private:
    void bits_per_coord(quint8 base, quint8 bfirst, quint32& bx, quint32& by, sign_info_t& signinfo, bool isVer2);
    int bits_per_coord(quint8 base, bool is_signed);
};

class CShiftReg
{
public:
    CShiftReg(const quint8* pData, quint32 n, quint32 bx, quint32 by, bool extra_bit, sign_info_t& si);

    bool get(qint32& x, qint32& y);
private:
    void fill(quint32 bits);
    /// the register to work on
    quint64 reg;
    /// the data stream to get data from
    const quint8 * pData;

    quint32 bytes;         //< bytes left in stream
    quint32 xmask;         //< bitmask x coord.
    quint32 ymask;         //< bitmask y coord.
    qint32 xsign;          //< sign bit for x value
    qint32 ysign;          //< sign bit for y value
    qint32 xsign2;         //< sign bit * 2 for x value
    qint32 ysign2;         //< sign bit * 2 for y value
    quint8 bits;           //< total bits in register
    quint8 bits_per_x;     //< bits per x coord.
    quint8 bits_per_y;     //< bits per y coord.
    quint8 bits_per_coord; //< bits per coord.

    sign_info_t& sinfo;

    bool extraBit;
};
#endif                           //CGARMINPOLYGON_H
