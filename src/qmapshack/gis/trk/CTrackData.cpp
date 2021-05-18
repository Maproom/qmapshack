#include "gis/IGisLine.h"
#include "gis/trk/CTrackData.h"

const QMap<CTrackData::trkpt_t::act10_e, CTrackData::trkpt_t::act20_e> CTrackData::trkpt_t::act1to2
{
    {CTrackData::trkpt_t::eActNone, CTrackData::trkpt_t::eAct20None}
    , {CTrackData::trkpt_t::eActFoot, CTrackData::trkpt_t::eAct20Foot}
    , {CTrackData::trkpt_t::eActCycle, CTrackData::trkpt_t::eAct20Cycle}
    , {CTrackData::trkpt_t::eActBike, CTrackData::trkpt_t::eAct20Bike}
    , {CTrackData::trkpt_t::eActCar, CTrackData::trkpt_t::eAct20Car}
    , {CTrackData::trkpt_t::eActCable, CTrackData::trkpt_t::eAct20Cable}
    , {CTrackData::trkpt_t::eActSwim, CTrackData::trkpt_t::eAct20Swim}
    , {CTrackData::trkpt_t::eActShip, CTrackData::trkpt_t::eAct20Ship}
    , {CTrackData::trkpt_t::eActAero, CTrackData::trkpt_t::eAct20Aero}
    , {CTrackData::trkpt_t::eActSki, CTrackData::trkpt_t::eAct20Ski}
    , {CTrackData::trkpt_t::eActTrain, CTrackData::trkpt_t::eAct20Train}
};

const QMap<CTrackData::trkpt_t::act20_e, CTrackData::trkpt_t::act10_e> CTrackData::trkpt_t::act2to1
{
    {CTrackData::trkpt_t::eAct20None, CTrackData::trkpt_t::eActNone}
    , {CTrackData::trkpt_t::eAct20Foot, CTrackData::trkpt_t::eActFoot}
    , {CTrackData::trkpt_t::eAct20Cycle, CTrackData::trkpt_t::eActCycle}
    , {CTrackData::trkpt_t::eAct20Bike, CTrackData::trkpt_t::eActBike}
    , {CTrackData::trkpt_t::eAct20Car, CTrackData::trkpt_t::eActCar}
    , {CTrackData::trkpt_t::eAct20Cable, CTrackData::trkpt_t::eActCable}
    , {CTrackData::trkpt_t::eAct20Swim, CTrackData::trkpt_t::eActSwim}
    , {CTrackData::trkpt_t::eAct20Ship, CTrackData::trkpt_t::eActShip}
    , {CTrackData::trkpt_t::eAct20Aero, CTrackData::trkpt_t::eActAero}
    , {CTrackData::trkpt_t::eAct20Ski, CTrackData::trkpt_t::eActSki}
    , {CTrackData::trkpt_t::eAct20Train, CTrackData::trkpt_t::eActTrain}
};


CTrackData::CTrackData(const QString& name, const CTrackData& other, qint32 rangeStart, qint32 rangeEnd) : name(name)
{
    for(const trkseg_t& oseg : other.segs)
    {
        trkseg_t seg;
        for(const trkpt_t& opt : oseg.pts)
        {
            if(opt.idxTotal < rangeStart)
            {
                continue;
            }
            if(opt.idxTotal > rangeEnd)
            {
                break;
            }

            seg.pts << opt;
        }

        if(!seg.isEmpty())
        {
            segs << seg;
        }
    }

    cmt = other.cmt;
    desc = other.desc;
    src = other.src;
    links = other.links;
    number = other.number;
    type = other.type;
}

void CTrackData::removeEmptySegments()
{
    QVector<trkseg_t>::iterator it = segs.begin();
    while(it != segs.end())
    {
        if(it->pts.isEmpty())
        {
            it = segs.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CTrackData::readFrom(const SGisLine& l)
{
    segs.clear();
    segs.resize(1);
    trkseg_t& seg = segs.first();

    for(int i = 0; i < l.size(); i++)
    {
        seg.pts << trkpt_t();

        trkpt_t& trkpt = seg.pts.last();
        const IGisLine::point_t& pt = l[i];

        trkpt.lon = pt.coord.x() * RAD_TO_DEG;
        trkpt.lat = pt.coord.y() * RAD_TO_DEG;
        trkpt.ele = pt.ele;

        for(int n = 0; n < pt.subpts.size(); n++)
        {
            seg.pts << trkpt_t();

            trkpt_t& trkpt = seg.pts.last();
            const IGisLine::subpt_t& sub = pt.subpts[n];

            trkpt.lon = sub.coord.x() * RAD_TO_DEG;
            trkpt.lat = sub.coord.y() * RAD_TO_DEG;
            trkpt.ele = sub.ele;
            trkpt.setFlag(trkpt_t::eFlagSubpt);
        }
    }
}

void CTrackData::readFrom(const QVector<CTrackData::trkpt_t>& pts)
{
    segs.clear();
    segs.resize(1);
    trkseg_t& seg = segs.first();
    seg.pts = pts;
}

void CTrackData::getPolyline(SGisLine& l) const
{
    l.clear();
    for(const trkpt_t& pt : *this)
    {
        if(!pt.isHidden())
        {
            if(pt.hasFlag(trkpt_t::eFlagSubpt))
            {
                l.last().subpts << IGisLine::subpt_t(pt.radPoint());
            }
            else
            {
                l << IGisLine::point_t(pt.radPoint());
            }
        }
    }
}

void CTrackData::getPolyline(QPolygonF& l) const
{
    l.clear();
    for(const trkpt_t& pt : *this)
    {
        if(!pt.isHidden())
        {
            l << pt.radPoint();
        }
    }
}

void CTrackData::getPolylineDeg(QPolygonF& l) const
{
    l.clear();
    for(const trkpt_t& pt : *this)
    {
        if(!pt.isHidden())
        {
            l << QPointF(pt.lon, pt.lat);
        }
    }
}

bool CTrackData::isTrkPtFirstVisible(qint32 idxTotal) const
{
    for(const trkpt_t& pt : *this)
    {
        if((pt.idxTotal < idxTotal))
        {
            if(!pt.isHidden())
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    return true;
}

const CTrackData::trkpt_t* CTrackData::getTrkPtByVisibleIndex(qint32 idx) const
{
    if(idx == NOIDX)
    {
        return nullptr;
    }

    auto condition = [idx](const trkpt_t& pt) { return pt.idxVisible == idx;  };
    return getTrkPtByCondition(condition);
}

const CTrackData::trkpt_t* CTrackData::getTrkPtByTotalIndex(qint32 idx) const
{
    for(const trkseg_t& seg : segs)
    {
        if(seg.isEmpty() || idx < seg.pts.first().idxTotal || idx > seg.pts.last().idxTotal)
        {
            continue;
        }

        return &seg.pts[idx - seg.pts.first().idxTotal];
    }

    return nullptr;
}

CTrackData::trkpt_t* CTrackData::getTrkPtByTotalIndex(qint32 idx)
{
    for(trkseg_t& seg : segs)
    {
        if(seg.isEmpty() || idx < seg.pts.first().idxTotal || idx > seg.pts.last().idxTotal)
        {
            continue;
        }

        return &seg.pts[idx - seg.pts.first().idxTotal];
    }

    return nullptr;
}

bool CTrackData::isTrkPtLastVisible(qint32 idxTotal) const
{
    auto condition = [idxTotal](const trkpt_t& pt) { return (pt.idxTotal > idxTotal) && !pt.isHidden();  };
    return nullptr == getTrkPtByCondition(condition);
}

const CTrackData::trkpt_t* CTrackData::getTrkPtByCondition(std::function<bool(const CTrackData::trkpt_t&)> cond) const
{
    for(const trkpt_t& pt : *this)
    {
        if(cond(pt))
        {
            return &pt;
        }
    }
    return nullptr;
}

CTrackData::trkpt_t* CTrackData::getTrkPtByCondition(std::function<bool(const CTrackData::trkpt_t&)> cond)
{
    for(trkpt_t& pt : *this)
    {
        if(cond(pt))
        {
            return &pt;
        }
    }
    return nullptr;
}

bool CTrackData::setTrkPtDesc(int idxTotal, const QString& desc)
{
    trkpt_t* trkpt = getTrkPtByTotalIndex(idxTotal);

    if((trkpt != nullptr) && !trkpt->isHidden() && (trkpt->desc != desc))
    {
        trkpt->desc = desc;
        return true;
    }
    return false;
}

bool CTrackData::delTrkPtDesc(const QList<int>& idxTotal)
{
    bool result = false;

    for(int idx : idxTotal)
    {
        trkpt_t* trkpt = getTrkPtByTotalIndex(idx);
        if(trkpt != nullptr)
        {
            trkpt->desc.clear();
            result = true;
        }
    }

    return result;
}
