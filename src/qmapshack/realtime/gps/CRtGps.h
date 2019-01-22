/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CRTGPS_H
#define CRTGPS_H

#include "realtime/IRtSource.h"

#include <QPointer>

class CRtGpsInfo;
class QGeoPositionInfoSource;
class QGeoPositionInfo;

class CRtGps : public IRtSource
{
    Q_OBJECT
public:
    CRtGps(QTreeWidget * parent);
    virtual ~CRtGps() = default;

    void registerWithTreeWidget() override;

    QString getDescription() const override;
    void loadSettings(QSettings& cfg) override;
    void saveSettings(QSettings& cfg) const override;

    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) override;

    void fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt) override;

    static const QString strIcon;

private:   
    QPointer<CRtGpsInfo> info;
};

#endif //CRTGPS_H

