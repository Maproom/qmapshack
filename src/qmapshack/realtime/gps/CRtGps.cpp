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

#include "helpers/CSettings.h"
#include "realtime/gps/CRtGps.h"
#include "realtime/gps/CRtGpsInfo.h"
#include "realtime/gps/CRtGpsTether.h"

#include <QtPositioning>
#include <QtWidgets>

#define GPS_TETHER "GPS Tether"

const QString CRtGps::strIcon("://icons/48x48/Gps.png");

CRtGps::CRtGps(QTreeWidget *parent)
    : IRtSource(eTypeGps, false, parent)
{
    setIcon(eColumnIcon, QIcon(strIcon));
    setText(eColumnName, "GPS");
    setCheckState(eColumnCheckBox, Qt::Checked);

    registerWithTreeWidget();
}


void CRtGps::registerWithTreeWidget()
{
    QTreeWidget * tree = treeWidget();
    if(tree != nullptr)
    {
        QTreeWidgetItem * itemInfo = new QTreeWidgetItem(this);
        itemInfo->setFlags(Qt::ItemIsEnabled|Qt::ItemNeverHasChildren);
        info = new CRtGpsInfo(*this, tree);
        tree->setItemWidget(itemInfo, eColumnWidget, info);
        emit sigChanged();
    }
}

void CRtGps::loadSettings(QSettings& cfg)
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::loadSettings(cfg);

    cfgGroup = cfg.group();

    if(info != nullptr)
    {
        info->loadSettings(cfg);
    }

    emit sigChanged();
}

void CRtGps::saveSettings(QSettings& cfg) const
{
    QMutexLocker lock(&IRtSource::mutex);

    IRtSource::saveSettings(cfg);

    if(!info.isNull())
    {
        info->saveSettings(cfg);
    }
    IRtGpsDevice * dev = dynamic_cast<IRtGpsDevice*>(device.data());
    if(dev != nullptr)
    {
        dev->saveSettings(cfg);
    }
}


QString CRtGps::getDescription() const
{
    return tr("<b>GPS</b><br/>"
              "Get position from a GPS device."
              );
}

QStringList CRtGps::getDevices() const
{
    QStringList devices = QGeoPositionInfoSource::availableSources();
    devices += GPS_TETHER;
    return devices;
}

bool CRtGps::setDevice(const QString& name)
{
    delete device;

    qDebug() << "Set GPS device:" << name;
    device = QGeoPositionInfoSource::createSource(name, this);
    if(device.isNull())
    {
        if(name == GPS_TETHER)
        {
            CRtGpsTether * dev = new CRtGpsTether(this);
            SETTINGS;
            cfg.beginGroup(cfgGroup);
            dev->loadSettings(cfg);
            cfg.endGroup();
            device = dev;
        }
    }
    if(!device.isNull())
    {
        connect(device, &QGeoPositionInfoSource::positionUpdated, this, &CRtGps::slotPositionUpdate);
    }

    emit sigChanged();

    return !device.isNull();
}

QGeoPositionInfoSource *CRtGps::getDevice() const
{
    return device;
}

void CRtGps::slotPositionUpdate(const QGeoPositionInfo &update)
{
    qDebug() << update;
}

void CRtGps::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
{
}

void CRtGps::fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt)
{
}
