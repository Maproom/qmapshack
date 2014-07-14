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

#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "dem/IDem.h"
#include "dem/CDemDraw.h"
#include "dem/CDemList.h"
#include "dem/CDemItem.h"
#include "dem/CDemPathSetup.h"
#include "units/IUnit.h"
#include "helpers/CSettings.h"

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

void CDemDraw::setProjection(const QString& proj)
{
    // --- save the active maps
    QStringList keys;
    saveActiveMapsList(keys);
    // --- neow set the new projection
    IDrawContext::setProjection(proj);
    // --- now build the map list from scratch. This will deactivate -> activate all maps
    //     By that everything is restored with the new projection
    buildMapList();
    restoreActiveMapsList(keys);
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
    // store group context for later use
    cfgGroup = cfg.group();
    // -------------------
    QStringList keys;
    cfg.beginGroup("dem");
    saveActiveMapsList(keys, cfg);
    cfg.setValue("active", keys);
    cfg.endGroup();

}

void CDemDraw::loadConfig(QSettings& cfg)
{
    // store group context for later use
    cfgGroup = cfg.group();
    // -------------------
    cfg.beginGroup("dem");
    restoreActiveMapsList(cfg.value("active", "").toStringList());
    cfg.endGroup();
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

            item->setText(0,fi.baseName().replace("_", " "));
            item->filename = dir.absoluteFilePath(filename);
            item->updateIcon();

            // calculate MD5 hash from the file's first 1024 bytes
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
                /**
                    @Note   the item will load it's configuration uppon successful activation
                            by calling loadConfigForDemItem().
                */
                item->activate();
                break;
            }
        }
    }

    demList->updateHelpText();
}


qreal CDemDraw::getElevation(const QPointF& pos)
{
    qreal ele = NOFLOAT;
    if(CDemItem::mutexActiveDems.tryLock())
    {
        if(demList)
        {
            for(int i = 0; i < demList->count(); i++)
            {
                CDemItem * item = demList->item(i);

                if(!item || item->demfile.isNull())
                {
                    // as all active maps have to be at the top of the list
                    // it is ok to break ass soon as the first map with no
                    // active files is hit.
                    break;
                }

                ele = item->demfile->getElevation(pos);
                if(ele != NOFLOAT)
                {
                    break;
                }

            }
        }
        CDemItem::mutexActiveDems.unlock();
    }
    return ele;
}

void CDemDraw::drawt(buffer_t& currentBuffer)
{
    // iterate over all active maps and call the draw method
    CDemItem::mutexActiveDems.lock();
    if(demList)
    {
        for(int i = 0; i < demList->count(); i++)
        {
            CDemItem * item = demList->item(i);

            if(!item || item->demfile.isNull())
            {
                // as all active maps have to be at the top of the list
                // it is ok to break ass soon as the first map with no
                // active files is hit.
                break;
            }

            item->demfile->draw(currentBuffer);

        }
    }
    CDemItem::mutexActiveDems.unlock();

}
