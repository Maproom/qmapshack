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

CProj::CProj(const char * crsSrc, const char * crsTar)
{
    init(crsSrc, crsTar);
}

CProj::~CProj()
{
    if(nullptr != pj)
    {
        proj_destroy(pj);
    }
}


void CProj::init(const char *crsSrc, const char *crsTar)
{
    strProjSrc = crsSrc;
    strProjTar = crsTar;

    if(!strProjSrc.contains("EPSG") && !strProjSrc.contains("+type=crs"))
    {
        strProjSrc += " +type=crs";
    }
    if(!strProjTar.contains("EPSG") && !strProjTar.contains("+type=crs"))
    {
        strProjTar += " +type=crs";
    }

    if(nullptr != pj)
    {
        proj_destroy(pj);
    }

    pj = proj_create_crs_to_crs (PJ_DEFAULT_CTX, strProjSrc.toLatin1(), strProjTar.toLatin1(), NULL);
    if (nullptr == pj)
    {
        return;
    }
    PJ* P_for_GIS = proj_normalize_for_visualization(PJ_DEFAULT_CTX, pj);
    proj_destroy(pj);
    pj = P_for_GIS;

    {
        PJ * pj = proj_create(PJ_DEFAULT_CTX, strProjTar.toLatin1());
        PJ_TYPE type = proj_get_type(pj);
        proj_destroy(pj);

        _isLatLong = PJ_TYPE_GEOGRAPHIC_2D_CRS == type;
    }
}

void CProj::transform(QPointF& pt, PJ_DIRECTION dir) const
{
    if(proj_degree_input(pj, dir))
    {
        pt *= RAD_TO_DEG;
    }

    transform(pt.rx(), pt.ry(), dir);

    if(proj_degree_output(pj, dir))
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

    PJ_COORD c = proj_coord(lon, lat, 0, 0);
    c = proj_trans(pj, dir, c);
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
