/**********************************************************************************************
    Copyright (C) 2021 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/proj_x.h"
#include <QDebug>
#include <QPolygonF>

CProj::CProj(const QString& crsSrc, const QString& crsTar)
{
    init(crsSrc.toLatin1(), crsTar.toLatin1());
}

CProj::~CProj()
{
    if(nullptr != _pj)
    {
        proj_destroy(_pj);
    }

    if(nullptr != _ctx)
    {
        proj_context_destroy(_ctx);
    }
}


void CProj::init(const char *crsSrc, const char *crsTar)
{
    _strProjSrc = crsSrc;
    _strProjTar = crsTar;

    _strProjSrc = _strProjSrc.remove("+towgs84=0,0,0,0,0,0,0,0");
    _strProjTar = _strProjTar.remove("+towgs84=0,0,0,0,0,0,0,0");

    if(!_strProjSrc.contains("EPSG") && !_strProjSrc.contains("+type=crs"))
    {
        _strProjSrc += " +type=crs";
    }
    if(!_strProjTar.contains("EPSG") && !_strProjTar.contains("+type=crs"))
    {
        _strProjTar += " +type=crs";
    }

    if(nullptr != _pj)
    {
        proj_destroy(_pj);
        _pj = nullptr;
    }

    if(nullptr != _ctx)
    {
        proj_context_destroy(_ctx);
        _ctx = nullptr;
    }


    _ctx = proj_context_create();
    if(nullptr == _ctx)
    {
        qWarning() << "Failed to create projection constex:";
        return;
    }
    _pj = proj_create_crs_to_crs(_ctx, _strProjSrc.toLatin1(), _strProjTar.toLatin1(), NULL);
    if(nullptr != _pj)
    {
        PJ* P_for_GIS = proj_normalize_for_visualization(_ctx, _pj);
        proj_destroy(_pj);
        _pj = P_for_GIS;
    }

    _isSrcLatLong = _isLatLong(_strProjSrc);
    _isTarLatLong = _isLatLong(_strProjTar);

    if (nullptr == _pj)
    {
        qWarning() << "Failed to create projection:" << _strProjSrc << "->" << _strProjTar;
        return;
    }

    qDebug() << "Create projection:" << _strProjSrc << "->" << _strProjTar;
}

bool CProj::_isLatLong(const QString &crs) const
{
    PJ * p = proj_create(_ctx, crs.toLatin1());
    PJ_TYPE type = proj_get_type(p);
    proj_destroy(p);

    return PJ_TYPE_GEOGRAPHIC_2D_CRS == type;
}

void CProj::transform(QPolygonF& line, PJ_DIRECTION dir) const
{
    if(!isValid())
    {
        return;
    }

    qreal factorPre = proj_degree_input(_pj, dir) ? RAD_TO_DEG : 1.0;
    qreal factorPost = proj_degree_output(_pj, dir) ? DEG_TO_RAD : 1.0;

    for(QPointF& pt : line)
    {
        pt *= factorPre;
        _transform(pt.rx(), pt.ry(), dir);
        pt *= factorPost;
    }
}

void CProj::transform(QPointF& pt, PJ_DIRECTION dir) const
{
    if(!isValid())
    {
        return;
    }

    if(proj_degree_input(_pj, dir))
    {
        pt *= RAD_TO_DEG;
    }

    _transform(pt.rx(), pt.ry(), dir);

    if(proj_degree_output(_pj, dir))
    {
        pt *= DEG_TO_RAD;
    }
}

void CProj::transform(qreal& lon, qreal& lat, PJ_DIRECTION dir) const
{
    if(!isValid())
    {
        return;
    }

    if(proj_degree_input(_pj, dir))
    {
        lon *= RAD_TO_DEG;
        lat *= RAD_TO_DEG;
    }

    _transform(lon, lat, dir);

    if(proj_degree_output(_pj, dir))
    {
        lon *= DEG_TO_RAD;
        lat *= DEG_TO_RAD;
    }
}

void CProj::_transform(qreal& lon, qreal& lat, PJ_DIRECTION dir) const
{
    PJ_COORD c = proj_coord(lon, lat, 0, 0);
    c = proj_trans(_pj, dir, c);
    lon = c.uv.u;
    lat = c.uv.v;
}

bool CProj::validProjStr(const QString projStr, bool allowLonLatToo, fErrMessage errMessage)
{
    bool res = false;

    PJ * projCheck = proj_create(PJ_DEFAULT_CTX, projStr.toLatin1());
    if (!projCheck)
    {
        if(errMessage)
        {
            errMessage(tr("The value\n'%1'\nis not a valid coordinate system definition:\n%2").arg(projStr).arg(proj_errno_string(proj_context_errno(PJ_DEFAULT_CTX))));
        }
    }
    else
    {
        PJ_TYPE type = proj_get_type(projCheck);
        proj_destroy(projCheck);

        if(!allowLonLatToo && (PJ_TYPE_GEOGRAPHIC_2D_CRS == type))
        {
            if(errMessage)
            {
                errMessage(tr("Lat/Lon projection is not allowed in this case."));
            }
        }
        else
        {
            res = true;
        }
    }

    return res;
}
