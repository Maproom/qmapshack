/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "realtime/CRtDraw.h"
#include "realtime/gpstether/CRtGpsTether.h"
#include "realtime/gpstether/CRtGpsTetherInfo.h"
#include "units/IUnit.h"

#include <QtWidgets>

#define GPS_TETHER "GPS Tether"

const QString CRtGpsTether::strIcon("://icons/48x48/Gps.png");

CRtGpsTether::CRtGpsTether(QTreeWidget *parent)
    : IRtSource(eTypeGpsTether, false, parent)
{
    setIcon(eColumnIcon, QIcon(strIcon));
    setText(eColumnName, "GPS TCP/IP");
    setCheckState(eColumnCheckBox, Qt::Checked);

    registerWithTreeWidget();
}


void CRtGpsTether::registerWithTreeWidget()
{
    QTreeWidget * tree = treeWidget();
    if(tree != nullptr)
    {
        QTreeWidgetItem * itemInfo = new QTreeWidgetItem(this);
        itemInfo->setFlags(Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
        info = new CRtGpsTetherInfo(*this, tree);
        connect(info, &CRtGpsTetherInfo::sigChanged, this, &CRtGpsTether::sigChanged);

        tree->setItemWidget(itemInfo, eColumnWidget, info);
        emit sigChanged();
    }
}

void CRtGpsTether::loadSettings(QSettings& cfg)
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::loadSettings(cfg);

    if(info != nullptr)
    {
        info->loadSettings(cfg);
    }

    emit sigChanged();
}

void CRtGpsTether::saveSettings(QSettings& cfg) const
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::saveSettings(cfg);

    if(!info.isNull())
    {
        info->saveSettings(cfg);
    }
}


QString CRtGpsTether::getDescription() const
{
    return tr("<b>GPS Tether</b><br/>"
              "Get position via NMEA over TCP/IP."
              );
}

void CRtGpsTether::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
{
    if(info.isNull())
    {
        return;
    }

    info->draw(p, viewport, blockedAreas, rt);

    if(checkState(eColumnCheckBox) != Qt::Checked)
    {
        return;
    }

    QPointF pos = info->getPosition();
    if(pos == NOPOINTF)
    {
        return;
    }

    pos *= DEG_TO_RAD;
    rt->convertRad2Px(pos);

    p.setBrush(CDraw::brushBackSemiBlue);
    p.setPen(CDraw::penBorderBlue);
    p.drawEllipse(pos, 10, 10);
    p.drawEllipse(pos, 2, 2);

    qreal heading = info->getHeading();
    if(heading == NOFLOAT)
    {
        return;
    }

    QImage arrow = CDraw::createBasicArrow(CDraw::brushBackSemiBlue, 5);
    p.save();
    p.translate(pos);
    p.rotate(heading - 90);

    p.setPen(Qt::black);
    p.drawImage(-arrow.rect().center() + QPoint(2, 2), arrow);
    p.restore();
}

void CRtGpsTether::fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt)
{
}
