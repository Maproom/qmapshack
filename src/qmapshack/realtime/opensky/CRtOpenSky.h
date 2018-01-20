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

#ifndef CRTOPENSKY_H
#define CRTOPENSKY_H

#include "realtime/IRtSource.h"

class CRtOpenSky : public IRtSource
{
    Q_DECLARE_TR_FUNCTIONS(CRtOpenSky)
public:
    CRtOpenSky(QTreeWidget * parent);
    virtual ~CRtOpenSky() = default;

    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) const override;
    QString getDescription() const override;
    void loadSettings(QSettings& cfg) override;
    void saveSettings(QSettings& cfg) const override;


    static const QString strIcon;
};

#endif //CRTOPENSKY_H

