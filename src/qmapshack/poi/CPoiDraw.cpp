/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CMainWindow.h"
#include "helpers/CSettings.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiItem.h"
#include "poi/CPoiList.h"
#include "poi/CPoiPathSetup.h"

#include <QtWidgets>

QList<CPoiDraw*> CPoiDraw::pois;
QStringList CPoiDraw::poiPaths;
QStringList CPoiDraw::supportedFormats = QString("*.poi").split('|');


CPoiDraw::CPoiDraw(CCanvas *canvas)
    : IDrawContext("poi", CCanvas::eRedrawPoi, canvas)
{
    poiList = new CPoiList(canvas);
    CMainWindow::self().addPoiList(poiList, canvas->objectName());
    connect(canvas,  &CCanvas::destroyed,   poiList, &CPoiList::deleteLater);
    connect(poiList, &CPoiList::sigChanged, this,    &CPoiDraw::emitSigCanvasUpdate);

    buildPoiList();

    pois << this;
}

CPoiDraw::~CPoiDraw()
{
    pois.removeOne(this);
}


void CPoiDraw::saveConfig(QSettings& cfg)
{
    // store group context for later use
    cfgGroup = cfg.group();
    // -------------------
    QStringList keys;
    cfg.beginGroup("poi");
    saveActivePoisList(keys, cfg);
    cfg.setValue("active", keys);
    cfg.endGroup();
}

void CPoiDraw::loadConfig(QSettings& cfg)
{
    // store group context for later use
    cfgGroup = cfg.group();
    // -------------------
    cfg.beginGroup("poi");
    if(cfgGroup.isEmpty())
    {
        restoreActivePoisList(cfg.value("active", "").toStringList(), cfg);
    }
    else
    {
        restoreActivePoisList(cfg.value("active", "").toStringList());
    }

    cfg.endGroup();
}

void CPoiDraw::setProjection(const QString& proj)
{
    // --- save the active maps
    QStringList keys;
    saveActivePoisList(keys);
    // --- now set the new projection
    IDrawContext::setProjection(proj);
    // --- now build the map list from scratch. This will deactivate -> activate all maps
    //     By that everything is restored with the new projection
    buildPoiList();
    restoreActivePoisList(keys);
}

void CPoiDraw::setupPoiPath()
{
    QStringList paths = poiPaths;
    CPoiPathSetup dlg(paths);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    setupPoiPath(paths);
}

void CPoiDraw::setupPoiPath(const QString &path)
{
    QStringList paths(poiPaths);
    if(!poiPaths.contains(path))
    {
        paths << path;
    }
    setupPoiPath(paths);
}

void CPoiDraw::setupPoiPath(const QStringList &paths)
{
    poiPaths = paths;

    for(CPoiDraw * poi : pois)
    {
        QStringList keys;
        poi->saveActivePoisList(keys);
        poi->buildPoiList();
        poi->restoreActivePoisList(keys);
    }
}

void CPoiDraw::savePoiPath(QSettings &cfg)
{
    cfg.setValue("poiPaths", poiPaths);
}

void CPoiDraw::loadPoiPath(QSettings &cfg)
{
    poiPaths = cfg.value("poiPaths", poiPaths).toStringList();
}

void CPoiDraw::buildPoiList()
{
    QCryptographicHash md5(QCryptographicHash::Md5);

    QMutexLocker lock(&CPoiItem::mutexActivePois);
    poiList->clear();

    for(const QString &path : poiPaths)
    {
        QDir dir(path);
        // find available maps
        for(const QString &filename : dir.entryList(supportedFormats, QDir::Files | QDir::Readable, QDir::Name))
        {
            QFileInfo fi(filename);

            CPoiItem * item = new CPoiItem(*poiList, this);

            item->setText(0, fi.completeBaseName().replace("_", " "));
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

    poiList->sort();
    poiList->updateHelpText();
}

void CPoiDraw::saveActivePoisList(QStringList &keys, QSettings &cfg)
{
    QMutexLocker lock(&CPoiItem::mutexActivePois);

    for(int i = 0; i < poiList->count(); i++)
    {
        CPoiItem * item = poiList->item(i);
        if(item && !item->poifile.isNull())
        {
            item->saveConfig(cfg);
            keys << item->key;
        }
    }
}


void CPoiDraw::saveActivePoisList(QStringList &keys)
{
    SETTINGS;
    cfg.beginGroup(cfgGroup);
    cfg.beginGroup("poi");
    saveActivePoisList(keys, cfg);
    cfg.endGroup();
    cfg.endGroup();
}

void CPoiDraw::loadConfigForPoiItem(CPoiItem * item)
{
    if(cfgGroup.isEmpty())
    {
        return;
    }

    SETTINGS;
    cfg.beginGroup(cfgGroup);
    cfg.beginGroup("poi");
    item->loadConfig(cfg);
    cfg.endGroup();
    cfg.endGroup();
}

void CPoiDraw::restoreActivePoisList(const QStringList &keys)
{
    QMutexLocker lock(&CPoiItem::mutexActivePois);

    for(const QString &key : keys)
    {
        for(int i = 0; i < poiList->count(); i++)
        {
            CPoiItem * item = poiList->item(i);

            if(item && item->key == key)
            {
                /**
                    @Note   the item will load it's configuration upon successful activation
                            by calling loadConfigForPoiItem().
                 */
                item->activate();
                break;
            }
        }
    }

    poiList->updateHelpText();
}

void CPoiDraw::restoreActivePoisList(const QStringList& keys, QSettings &cfg)
{
    QMutexLocker lock(&CPoiItem::mutexActivePois);

    for(const QString &key : keys)
    {
        for(int i = 0; i < poiList->count(); i++)
        {
            CPoiItem * item = poiList->item(i);

            if(item && item->key == key)
            {
                if(item->activate())
                {
                    item->loadConfig(cfg);
                }
                break;
            }
        }
    }

    poiList->updateHelpText();
}


void CPoiDraw::drawt(buffer_t& currentBuffer)
{
    // iterate over all active maps and call the draw method
    CPoiItem::mutexActivePois.lock();
    if(poiList)
    {
        for(int i = 0; i < poiList->count(); i++)
        {
            CPoiItem * item = poiList->item(i);

            if(!item || item->poifile.isNull())
            {
                // as all active maps have to be at the top of the list
                // it is ok to break as soon as the first map with no
                // active files is hit.
                break;
            }

            item->poifile->draw(currentBuffer);
        }
    }
    CPoiItem::mutexActivePois.unlock();
}
