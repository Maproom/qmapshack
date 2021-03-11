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

#ifndef PROJ_X_H
#define PROJ_X_H

#include <proj.h>
#include <QtCore>

#include <functional>

#define RAD_TO_DEG    57.295779513082321
#define DEG_TO_RAD   .017453292519943296

class CProj
{
    Q_DECLARE_TR_FUNCTIONS(CProj)
public:
    CProj() = default;
    virtual ~CProj();

    void init(const char *crsSrc, const char *crsTar);

    void transform(QPointF& pt, PJ_DIRECTION dir) const;
    void transform(QPolygonF& line, PJ_DIRECTION dir) const;
    bool isValid()const {return nullptr != pj;}
    bool isSrcLatLong() const {return _isSrcLatLong;}
    bool isTarLatLong() const {return _isTarLatLong;}

    QString getProjTar() const {return isValid() ? strProjTar : "";}
    QString getProjSrc() const {return isValid() ? strProjSrc : "";}

    using fErrMessage = std::function<void (const QString&)>;

    static bool validProjStr(const QString projStr, bool allowLonLatToo, fErrMessage errMessage);

private:
    void transform(qreal& lon, qreal& lat, PJ_DIRECTION dir) const;
    bool isLatLong(const QString& crs) const;

    PJ * pj = nullptr;
    bool _isSrcLatLong = false;
    bool _isTarLatLong = false;

    QString strProjSrc;
    QString strProjTar;
};




#endif //PROJ_X_H

