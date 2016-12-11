#ifndef TRACKDATA_H
#define TRACKDATA_H

#include "gis/IGisItem.h"
#include "GeoMath.h"
#include <QtWidgets>
#include <QPoint>
#include <proj_api.h>

struct SGisLine;

class CTrackData
{
public:
    struct trkpt_t : public IGisItem::wpt_t
    {
        trkpt_t()
        {
            reset();
        }

        void reset()
        {
            deltaDistance   = NOFLOAT;
            distance        = NOFLOAT;
            ascent          = NOFLOAT;
            descent         = NOFLOAT;
            elapsedSeconds  = NOFLOAT;
            elapsedSecondsMoving = NOFLOAT;
            slope1          = NOFLOAT;
            slope2          = NOFLOAT;
            speed           = NOFLOAT;
            idxVisible      = NOIDX;
        }

        enum flag_e
        {
            eHidden     = 0x00000004      ///< mark point as deleted
            ,eSubpt     = 0x00000008
                              // activity flags
            ,eActNone   = 0x00000000
            ,eActFoot   = 0x80000000
            ,eActCycle  = 0x40000000
            ,eActBike   = 0x20000000
            ,eActCar    = 0x10000000
            ,eActCable  = 0x08000000
            ,eActSwim   = 0x04000000
            ,eActShip   = 0x02000000
            ,eActAero   = 0x01000000
            ,eActSki    = 0x00800000
            ,eActMask   = 0xFF800000    ///< mask for activity flags
            ,eActMaxNum = 9             ///< maximum number of activity flags. this is defined by the mask
        };

        enum valid_e
        {
            eValidTime  = 0x00000001
            ,eValidEle  = 0x00000002
            ,eValidPos  = 0x00000004
            ,eValidMask = 0x0000FFFF
        };

        enum invalid_e
        {
            eInvalidTime  = eValidTime << 16
            ,eInvalidEle  = eValidEle  << 16
            ,eInvalidPos  = eValidPos  << 16
            ,eInvalidMask = 0xFFFF0000
        };


        inline bool isHidden() const
        {
            return hasFlag(trkpt_t::eHidden);
        }

        inline bool hasFlag(enum flag_e flag) const
        {
            return flags & flag;
        }

        inline void setFlag(enum flag_e flag)
        {
            flags |= flag;
        }

        inline void unsetFlag(enum flag_e flag)
        {
            flags &= ~flag;
        }

        inline bool isValid(valid_e flag) const
        {
            return (valid & flag) != 0;
        }

        inline bool isInvalid(invalid_e flag) const
        {
            return (valid & flag) != 0;
        }

        inline QPointF radPoint() const
        {
            return QPointF(lon * DEG_TO_RAD, lat * DEG_TO_RAD);
        }

        inline qreal distanceTo(const trkpt_t &other)
        {
            return GPS_Math_Distance(lon * DEG_TO_RAD, lat * DEG_TO_RAD, other.lon * DEG_TO_RAD, other.lat * DEG_TO_RAD);
        }

        quint32 flags = 0;
        quint32 valid = 0;
        qint32 idxTotal = NOIDX;            //< index within the complete track
        qint32 idxVisible;                  //< offset into lineSimple
        qreal deltaDistance;                //< the distance to the last point
        qreal distance;                     //< the distance from the start of the track
        qreal ascent;                       //< the ascent from the start of the track
        qreal descent;                      //< the descent from the start of the track
        qreal slope1;                       //< the slope [°] over several points close by
        qreal slope2;                       //< the slope [%] over several points close by
        qreal speed;                        //< the speed over several points close by
        qreal elapsedSeconds;               //< the seconds since the start of the track
        qreal elapsedSecondsMoving;         //< the seconds since the start of the track with moving speed
        IGisItem::key_t keyWpt;             //< the key of an attached waypoint
        QHash<QString,QVariant> extensions; //< track point extensions
    };

    struct trkseg_t
    {
        QVector<trkpt_t> pts;

        bool isEmpty() const
        {
            return pts.isEmpty();
        }
    };



    CTrackData() {}

    CTrackData(const QString &name, const CTrackData &other, qint32 rangeStart, qint32 rangeEnd);

    // -- all gpx tags - start
    QString name;
    QString cmt;
    QString desc;
    QString src;
    QList<IGisItem::link_t> links;
    quint64 number = 0;
    QString type;
    QVector<trkseg_t> segs;
    // -- all gpx tags - stop

    QString color;

    void removeEmptySegments();

    void readFrom(const SGisLine &l);
    void getPolyline(SGisLine  &l) const;
    void getPolyline(QPolygonF &l) const;

    bool isEmpty() const
    {
        return segs.isEmpty();
    }

    /**
       @brief Check if the track point at index it the first one visible
       @param idxTotal  The point's index
       @return True if it is the first one visible
     */
    bool isTrkPtFirstVisible(qint32 idxTotal) const;

    const trkpt_t* getTrkPtByCondition(std::function<bool(const trkpt_t&)> cond) const;
    trkpt_t* getTrkPtByCondition(std::function<bool(const trkpt_t&)> cond);


    /**
       @brief Try to get access Nth visible point matching the idx

       This will iterate over all segments and count the visible points. If the
       count matches idx a pointer to the track point is returned.

       @param idx The index into all visible points
       @return A null pointer of no point is found.
     */
    const trkpt_t *getTrkPtByVisibleIndex(qint32 idx) const;
    /**
       @brief Try to get access Nth point

       This will iterate over all segments. If the index matches
       a pointer to the track point is returned.

       @param idx The index into all points
       @return A null pointer of no point is found.
     */
    const trkpt_t *getTrkPtByTotalIndex(qint32 idx) const;

    /**
       @brief Check if the track point at index it the last one visible
       @param idxTotal  The point's index
       @return True if it is the last one visible
     */
    bool isTrkPtLastVisible(qint32 idxTotal) const;


    template<typename T1, typename T2>
    class iterator : public std::iterator<std::forward_iterator_tag, T2>
    {
        T1 &trk;
        int seg = 0;
        int pt  = 0;

    public:
        explicit iterator(T1 &trk, int seg, int pt) : trk(trk), seg(seg), pt(pt) {}

        iterator& operator++()
        {
            Q_ASSERT(seg < trk.segs.count());
            ++pt;

            if(this->trk.segs[seg].pts.count() <= pt) {
                pt = 0;
                ++seg;
            }

            return *this;
        }

        iterator operator++(int)
        {
            iterator prev = *this;
            ++(*this);
            return prev;
        }

        bool operator==(iterator other) const
        {
            return (&trk == &other.trk) && (seg == other.seg) && (pt == other.pt);
        }

        bool operator!=(iterator other) const
        {
            return !(*this == other);
        }

        T2& operator*()
        {
            return this->trk.segs[seg].pts[pt];
        }
    };

    iterator<CTrackData, trkpt_t> begin() { return iterator<CTrackData, trkpt_t>(*this,            0, 0); }
    iterator<CTrackData, trkpt_t> end()   { return iterator<CTrackData, trkpt_t>(*this, segs.count(), 0); }

    iterator<const CTrackData, const trkpt_t> begin() const { return iterator<const CTrackData, const trkpt_t>(*this,            0, 0); }
    iterator<const CTrackData, const trkpt_t> end()   const { return iterator<const CTrackData, const trkpt_t>(*this, segs.count(), 0); }
};

#endif /* TRACKDATA_H */
