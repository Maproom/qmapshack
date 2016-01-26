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

#ifndef CQLGTTRACK_H
#define CQLGTTRACK_H

#include "qlgt/IItem.h"
#include <QObject>

class CQlgtWpt;

class CFlags
{
public:
    CFlags(quint32 f=0)
    {
        flags = f;
        changed = true;
    }
    virtual ~CFlags()
    {
    }
    const quint32 flag() const
    {
        return flags;
    }
    void setFlags( quint32 f )
    {
        if ( flags != f )
        {
            changed = true;
        }
        flags = f;
    }
    quint32 operator  &(quint32 f) const
    {
        return flags&f;
    }
    quint32 operator |=(quint32 f)
    {
        if ( flags != (flags|f) )
        {
            changed = true;
        }
        flags|=f;
        return flags;
    }
    quint32 operator &=(quint32 f)
    {
        if ( flags != (flags&f) )
        {
            changed = true;
        }
        flags&=f;
        return flags;
    }
    quint32 operator >>(quint32 & f)
    {
        if ( flags != f )
        {
            changed = true;
        }
        flags = f;
        return flags;
    }
    const bool isChanged() const
    {
        return changed;
    }
    void setChanged(bool b)
    {
        changed = b;
    }
protected:
    /// display flags
    quint32 flags;
    bool changed;
};

QDataStream& operator >>(QDataStream& s, CFlags& flag);
QDataStream& operator <<(QDataStream& s, CFlags& flag);



class CQlgtTrack : public QObject, public IItem
{
public:
    CQlgtTrack(quint64 id, QObject * parent);
    virtual ~CQlgtTrack();


    enum type_e {eEnd,eBase,eTrkPts,eTrain,eTrkExt1,eTrkGpxExt,eTrkShdw, eTrkShdw2, eTrkPts2};

    struct pt_t
    {
        enum flag_e
        {
            eSelected  = 1       ///< selected by track info view
            ,eCursor    = 2      ///< selected by cursor
            ,eDeleted   = 4      ///< mark point as deleted
            ,eFocus     = 8      ///< mark current point of user focus
        };

        pt_t() : idx(-1), lon(WPT_NOFLOAT), lat(WPT_NOFLOAT), ele(WPT_NOFLOAT), timestamp(0), timestamp_msec(0),
            speed(WPT_NOFLOAT), avgspeed(0), delta(WPT_NOFLOAT), azimuth(WPT_NOFLOAT), distance(WPT_NOFLOAT),
            ascend(0), descend(0), heartReateBpm(-1), cadenceRpm(-1), slope(0), slope2(WPT_NOFLOAT), timeSinceStart(0),
            fix(""), sat(0), velocity(WPT_NOFLOAT), heading(WPT_NOFLOAT),
            vdop(WPT_NOFLOAT), hdop(WPT_NOFLOAT), pdop(WPT_NOFLOAT),
            _lon(WPT_NOFLOAT),_lat(WPT_NOFLOAT),_ele(WPT_NOFLOAT), _timestamp(0), _timestamp_msec(0),
            flags(0), px_valid(false), dem(WPT_NOFLOAT), editItem(NULL)
        {
        }

        bool operator==(const pt_t& pt) const
        {
            return pt.idx == idx;
        }

        /// index counter for easy QVector access
        qint32 idx;
        /// longitude [deg]
        float lon;
        /// latitude [deg]
        float lat;
        /// elevation [m]
        float ele;
        /// timestamp for track creation
        quint32 timestamp;
        quint32 timestamp_msec;

        /// secondary data: the speed between this and the previous point
        float speed;
        /// secondary data: the short term average speed
        float avgspeed;
        /// secondary data: the distance between this and the previous point
        float delta;
        /// secondary data: the azimuth to the next point
        double azimuth;
        /// secondary data: the total distance of all visible points up to this point
        float distance;
        /// secondary data: the total ascend of all visible points up to this point
        float ascend;
        /// secondary data: the total descend of all visible points up to this point
        float descend;
        /// secondary data: the heart rate in bpm
        int heartReateBpm;
        /// secondary data: cadence in rpm
        int cadenceRpm;
        /// secondary data: slope in %
        float slope;
        /// secondary data: slope in %
        float slope2;

        quint32 timeSinceStart;

        // extended data 1
        QString fix;
        qint32 sat;
        float altitude = 0;          ///< [m] Altitude, Meters, above mean sea level
        float height = 0;            ///< [m] Height of geoid (mean sea level) above WGS84 ellipsoid
        float velocity;          ///< [m/s] Ground speed, meters per hour
        float heading;           ///< [] Track angle in degrees True
        float magnetic = 0;          ///< [] Magnetic Variation
        float vdop;              ///< Vertical dilution of precision (VDOP)
        float hdop;              ///< Horizontal dilution of precision (HDOP)
        float pdop;              ///< PDOP (dilution of precision)
        float x = 0;                 ///< [m] cartesian gps coordinate
        float y = 0;                 ///< [m] cartesian gps coordinate
        float z = 0;                 ///< [m] cartesian gps coordinate
        float vx = 0;                ///< [m/s] velocity
        float vy = 0;                ///< [m/s] velocity
        float vz = 0;                ///< [m/s] velocity

#ifdef GPX_EXTENSIONS
        CGpxExtPt gpx_exts;
#endif

        // track shadow data (copy of original data)
        /// longitude [deg]
        float _lon;
        /// latitude [deg]
        float _lat;
        /// elevation [m]
        float _ele;
        quint32 _timestamp;
        quint32 _timestamp_msec;

        /// display flags
        CFlags flags;
        /// the current location in pixel
        QPoint px;
        bool px_valid;

        float dem;

        /// QTreeWidgetItem
        QPointer<QObject> editItem;

        QColor color;
    };


    struct wpt_t
    {
        wpt_t() : wpt(0), d(1e25f), x(0), y(0)
        {
        }
        CQlgtWpt * wpt;
        double d;
        double x;
        double y;
        pt_t trkpt;
    };

    CQlgtTrack& operator<<(const pt_t& pt);

    /// a track URL
    QString url;
    /// the track line color
    QColor color;

    QPixmap bullet;
    /// the track line color by index
    unsigned colorIdx = 0;
    /// the track points
    QList<pt_t> track;

    /// set true to draw track highlighted
    bool highlight = 0;

    /// total time covered by all track points
    double totalTime = 0;
    /// total time moving
    double totalTimeMoving = 0;
    /// total distance of track [m]
    double totalDistance = 0;

    /// total ascend in [m]
    double totalAscend = 0;
    /// total descend in [m]
    double totalDescend = 0;

    /// the Qt polyline for faster processing
    QPolygon polyline;
    /// the color attached to each point in polyline (only used in multicolor mode)
    QVector<QColor> polylineColor;

    float avgspeed0 = 0;
    float avgspeed1 = 0;

    pt_t ptMaxEle;
    pt_t ptMinEle;
    pt_t ptMaxSpeed;
    pt_t ptMinSpeed;

    bool traineeData = 0;
    bool ext1Data = 0;
    bool firstTime = 0;
    bool m_hide = 0;

    quint32 doScaleWpt2Track = 0;


    quint32 visiblePointCount = 0;

    quint32 cntMedianFilterApplied = 0;

    QList<wpt_t> waypoints;

    bool replaceOrigData = 0;

    enum state_select_e {eNoSel, e1stSel, e2ndSel};
    state_select_e stateSelect = eNoSel;

    QString timezone;

    enum multi_color_item_e
    {
        eMultiColorNone
        , eMultiColorSlope
        , eMultiColorEle
        , eMultiColorSpeed
        , eMultiColorMax
    };

    quint32 useMultiColor = 0;
    qint32 idMultiColor = 0;

    bool hasExt1Data() const
    {
        return ext1Data;
    }
    void setExt1Data()
    {
        ext1Data = true;
    }

    bool hasShadow1;
    bool hasShadow2;
};

QDataStream& operator >>(QDataStream& s, CQlgtTrack& trk);
QDataStream& operator <<(QDataStream& s, CQlgtTrack& trk);


#endif //CQLGTTRACK_H

