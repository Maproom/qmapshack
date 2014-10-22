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
#ifndef CGARMINPOLYGON_H
#define CGARMINPOLYGON_H

#include <proj_api.h>
#ifdef __MINGW32__
#undef LP
#endif
#include <QtCore>
#include <QPolygonF>

struct subdiv_desc_t;
struct sign_info_t;

class CGarminPolygon
{
    public:
        CGarminPolygon();
        virtual ~CGarminPolygon();

        quint32 decode(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, bool line, const quint8 * pData, const quint8 * pEnd);
        quint32 decode2(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, bool line, const quint8 * pData, const quint8 * pEnd);

        quint32 type;
        /// direction of line (polyline, only)
        bool direction;
        /// the label offset
        quint32 lbl_info;
        /// true if label offset has to be used in NET section
        bool lbl_in_NET;
        ///
        bool hasV2Label;
        /// delta longitude from subdivision center
        qint16 dLng;
        /// delta latitude from subdivision center
        qint16 dLat;
        /// the actual polyline points as longitude / latitude [rad], will become pixel later
        QPolygonF poly;
        /// the actual polyline points as longitude / latitude [rad]
        QPolygonF coords;

        quint32 id;

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
        /// bytes left in stream
        quint32 bytes;
        /// bitmask x coord.
        quint32 xmask;
        /// bitmask y coord.
        quint32 ymask;
        /// sign bit for x value
        qint32 xsign;
        /// sign bit for y value
        qint32 ysign;
        /// sign bit * 2 for x value
        qint32 xsign2;
        /// sign bit * 2 for y value
        qint32 ysign2;
        /// total bits in register
        quint8 bits;
        /// used bits of first byte;
        quint8 bits_of_byte;
        /// bits per x coord.
        quint8 bits_per_x;
        /// bits per y   coord.
        quint8 bits_per_y;
        /// bits per coord.
        quint8 bits_per_coord;

        sign_info_t& sinfo;

        bool extraBit;
};
#endif                           //CGARMINPOLYGON_H
