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

#include "gis/CGisWidget.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/router/CRouterRoutino.h"
#include "gis/rte/router/CRouterRoutinoPathSetup.h"
#include "helpers/CSettings.h"
#include <QtWidgets>
#include <proj_api.h>
#include <routino.h>


CRouterRoutino::CRouterRoutino(QWidget *parent)
    : IRouter(true, parent)
{
    setupUi(this);

    comboProfile->addItem(tr("Foot"), "foot");
    comboProfile->addItem(tr("Horse"), "horse");
    comboProfile->addItem(tr("Wheelchair"), "wheelchair");
    comboProfile->addItem(tr("Bicycle"), "bicycle");
    comboProfile->addItem(tr("Moped"), "moped");
    comboProfile->addItem(tr("Motorcycle"), "motorcycle");
    comboProfile->addItem(tr("Motorcar"), "motorcar");
    comboProfile->addItem(tr("Goods"), "goods");

    comboMode->addItem(tr("Shortest"));
    comboMode->addItem(tr("Quickest"));

    QFile _profiles("://xml/routino/routino-profiles.xml");
    _profiles.open(QIODevice::ReadOnly);
    QTemporaryFile profiles;
    profiles.open();
    profiles.write(_profiles.readAll());
    profiles.close();

    QFile _translations("://xml/routino/routino-translations.xml");
    _translations.open(QIODevice::ReadOnly);
    QTemporaryFile translations;
    translations.open();
    translations.write(_translations.readAll());
    translations.close();

    RoutinoInit(profiles.fileName().toUtf8(), translations.fileName().toUtf8());

    connect(toolSetupPaths, SIGNAL(clicked()), this, SLOT(slotSetupPaths()));

    SETTINGS;
    dbPaths = cfg.value("Route/routino/paths", dbPaths).toStringList();
    buildDatabaseList();

    comboProfile->setCurrentIndex(cfg.value("Route/routino/profile",0).toInt());
    comboMode->setCurrentIndex(cfg.value("Route/routino/mode",0).toInt());
    comboDatabase->setCurrentIndex(cfg.value("Route/routino/database",0).toInt());

    updateHelpText();
}

CRouterRoutino::~CRouterRoutino()
{
    SETTINGS;
    cfg.setValue("Route/routino/paths", dbPaths);
    cfg.setValue("Route/routino/profile", comboProfile->currentIndex());
    cfg.setValue("Route/routino/mode", comboMode->currentIndex());
    cfg.setValue("Route/routino/database", comboDatabase->currentIndex());

    freeDatabaseList();
    RoutinoRelease();
}


void CRouterRoutino::slotSetupPaths()
{
    CRouterRoutinoPathSetup dlg(dbPaths);
    dlg.exec();

    buildDatabaseList();
    updateHelpText();
}

void CRouterRoutino::buildDatabaseList()
{
    QRegExp re("(.*)-nodes.mem");
    freeDatabaseList();

    foreach(const QString &path, dbPaths)
    {
        QDir dir(path);
        foreach(const QString &filename, dir.entryList(QStringList("*nodes.mem"), QDir::Files|QDir::Readable, QDir::Name))
        {
            QString prefix;
            if(re.exactMatch(filename))
            {
                prefix = re.cap(1);
            }
            else
            {
                continue;
            }

            H_RoutinoDataSet data = RoutinoRegisterData(dir.absolutePath().toUtf8(), prefix.toUtf8());
            if(data)
            {
                comboDatabase->addItem(prefix.replace("_", " "), quint64(data));
            }
        }
    }
}

void CRouterRoutino::freeDatabaseList()
{
    for(int i = 0; i < comboDatabase->count(); i++)
    {
        H_RoutinoDataSet data = H_RoutinoDataSet(comboDatabase->itemData(i, Qt::UserRole).toULongLong());
        RoutinoFreeData(data);
    }
    comboDatabase->clear();
}

void CRouterRoutino::updateHelpText()
{
    if(comboDatabase->count() != 0)
    {
        frameHelp->hide();
        comboDatabase->setEnabled(true);
    }
    else
    {
        frameHelp->show();
        comboDatabase->setEnabled(false);
    }
}

void CRouterRoutino::calcRoute(const IGisItem::key_t& key)
{
    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(CGisWidget::self().getItemByKey(key));
    if(rte == 0)
    {
        return;
    }

    H_RoutinoDataSet data   = H_RoutinoDataSet(comboDatabase->currentData(Qt::UserRole).toULongLong());
    if(data == 0)
    {
        return;
    }

    QString profile         = comboProfile->currentData(Qt::UserRole).toString();

    SGisLine line;
    QVector<float> lon,lat;
    rte->getPolylineFromData(line);
    foreach(const IGisLine::point_t &pt, line)
    {
        lon << pt.coord.x();
        lat << pt.coord.y();
    }

    T_RoutinoRoute * route = RoutinoCalculate(data, profile.toUtf8(), comboMode->currentIndex(), lon.data(), lat.data(), line.size());

    rte->setResult(route);

    RoutinoFreeRoute(route);
}


bool CRouterRoutino::calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords)
{
    H_RoutinoDataSet data   = H_RoutinoDataSet(comboDatabase->currentData(Qt::UserRole).toULongLong());
    if(data == 0)
    {
        return -1;
    }
    QString profile         = comboProfile->currentData(Qt::UserRole).toString();


    float lon[2];
    float lat[2];

    lon[0] = p1.x();
    lon[1] = p2.x();

    lat[0] = p1.y();
    lat[1] = p2.y();

    T_RoutinoRoute * route = RoutinoCalculate(data, profile.toUtf8(), comboMode->currentIndex(), lon, lat, 2);

    T_RoutinoRoute * next = route;
    while(next)
    {
        if(next->type != IMP_WAYPOINT)
        {
            coords << QPointF(next->lon, next->lat);
        }
        next = next->next;
    }

    RoutinoFreeRoute(route);
    return !coords.isEmpty();
}
