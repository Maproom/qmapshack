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

#include "CCanvas.h"
#include "CMainWindow.h"
#include "dem/CDemDraw.h"
#include "dem/CDemList.h"
#include "dem/CDemItem.h"
#include "dem/CDemPathSetup.h"
#include "units/IUnit.h"
#include "CSettings.h"

#include <QtWidgets>


QList<CDemDraw*> CDemDraw::dems;
QStringList CDemDraw::demPaths;
QStringList CDemDraw::supportedFormats = QString("*.vrt").split('|');


CDemDraw::CDemDraw(CCanvas *canvas)
    : IDrawContext(canvas)
{

    demList = new CDemList(canvas);
    CMainWindow::self().addDemList(demList, canvas->objectName());
    connect(canvas, SIGNAL(destroyed()), demList, SLOT(deleteLater()));
    connect(demList, SIGNAL(sigChanged()), this, SLOT(emitSigCanvasUpdate()));

    buildMapList();

    dems << this;
}

CDemDraw::~CDemDraw()
{
    dems.removeOne(this);
}

void CDemDraw::setupDemPath()
{
    CDemPathSetup dlg(demPaths);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    foreach(CDemDraw * dem, dems)
    {
        QStringList keys;
        dem->saveActiveMapsList(keys);
        dem->buildMapList();
        dem->restoreActiveMapsList(keys);
    }
}

void CDemDraw::saveDemPath(QSettings& cfg)
{
    cfg.setValue("demPaths", demPaths);
}

void CDemDraw::loadDemPath(QSettings& cfg)
{
    demPaths = cfg.value("demPaths", demPaths).toStringList();
}

void CDemDraw::saveConfig(QSettings& cfg)
{
    cfgGroup = cfg.group();
    QStringList keys;
    cfg.beginGroup("dem");
    saveActiveMapsList(keys, cfg);
    cfg.setValue("active", keys);
    cfg.setValue("zoomIndex", zoomIndex);
    cfg.endGroup();

}

void CDemDraw::loadConfig(QSettings& cfg)
{
    cfgGroup = cfg.group();
    cfg.beginGroup("dem");
    restoreActiveMapsList(cfg.value("active", "").toStringList());
    int idx = cfg.value("zoomIndex",zoomIndex).toInt();
    cfg.endGroup();

    zoom(idx);
}

void CDemDraw::buildMapList()
{
    QCryptographicHash md5(QCryptographicHash::Md5);

    QMutexLocker lock(&CDemItem::mutexActiveDems);
    demList->clear();

    foreach(const QString& path, demPaths)
    {
        QDir dir(path);
        // find available maps
        foreach(const QString& filename, dir.entryList(supportedFormats, QDir::Files|QDir::Readable, QDir::Name))
        {
            QFileInfo fi(filename);

            CDemItem * item = new CDemItem(*demList, this);

            item->setText(0,fi.baseName());
            item->filename = dir.absoluteFilePath(filename);
            item->updateIcon();

            QFile f(dir.absoluteFilePath(filename));
            f.open(QIODevice::ReadOnly);
            md5.reset();
            md5.addData(f.read(1024));
            item->key = md5.result().toHex();
            f.close();
        }
    }
    demList->updateHelpText();
}


void CDemDraw::saveActiveMapsList(QStringList& keys)
{
    SETTINGS;
    cfg.beginGroup(cfgGroup);
    cfg.beginGroup("dem");
    saveActiveMapsList(keys, cfg);
    cfg.endGroup();
    cfg.endGroup();
}

void CDemDraw::saveActiveMapsList(QStringList& keys, QSettings& cfg)
{
    QMutexLocker lock(&CDemItem::mutexActiveDems);

    for(int i = 0; i < demList->count(); i++)
    {
        CDemItem * item = demList->item(i);
        if(item && !item->demfile.isNull())
        {
            item->saveConfig(cfg);
            keys << item->key;
        }
    }
}

void CDemDraw::loadConfigForDemItem(CDemItem * item)
{
    SETTINGS;
    cfg.beginGroup(cfgGroup);
    cfg.beginGroup("dem");
    item->loadConfig(cfg);
    cfg.endGroup();
    cfg.endGroup();
}

void CDemDraw::restoreActiveMapsList(const QStringList& keys)
{
    QMutexLocker lock(&CDemItem::mutexActiveDems);

    foreach(const QString& key, keys)
    {
        for(int i = 0; i < demList->count(); i++)
        {
            CDemItem * item = demList->item(i);

            if(item && item->key == key)
            {
                item->activate();
                break;
            }
        }
    }

    demList->updateHelpText();
}


qreal CDemDraw::getElevation(const QPointF& pos)
{
    return NOFLOAT;
}

void CDemDraw::drawt(buffer_t& currentBuffer)
{
    msleep(500);
}
