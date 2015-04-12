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

#include "CMainWindow.h"
#include "device/IDevice.h"
#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/IGisItem.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/CDetailsPrj.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectCopyAction.h"

#include <QtWidgets>


IGisProject::IGisProject(type_e type, const QString &filename, CGisListWks *parent)
    : QTreeWidgetItem(parent)
    , type(type)
    , filename(filename)
    , valid(false)
    , sorting(eSortNone)
{
    setCheckState(CGisListWks::eColumnDecoration, Qt::Checked);
}

IGisProject::IGisProject(type_e type, const QString &filename, IDevice *parent)
    : QTreeWidgetItem(parent)
    , type(type)
    , filename(filename)
    , valid(false)
    , sorting(eSortNone)
{
    setCheckState(CGisListWks::eColumnDecoration, Qt::Checked);
}

IGisProject::~IGisProject()
{
    delete dlgDetails;
}


bool IGisProject::askBeforClose()
{
    int res = QMessageBox::Ok;
    if(isChanged())
    {
        res = QMessageBox::question(&CMainWindow::self(), QObject::tr("Save project?"), QObject::tr("The project \"%1\" was changed. Save befor closing it?").arg(getName()), QMessageBox::Save|QMessageBox::No|QMessageBox::Abort, QMessageBox::Save);
        if(res == QMessageBox::Save)
        {
            save();
        }
    }

    return res == QMessageBox::Abort;
}


bool IGisProject::isVisible() const
{
    return checkState(CGisListWks::eColumnDecoration) == Qt::Checked;
}

void IGisProject::genKey()
{
    if(key.isEmpty())
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setVersion(QDataStream::Qt_5_2);

        *this >> stream;

        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(buffer);
        key = md5.result().toHex();
    }
}

QString IGisProject::getDeviceKey() const
{
    IDevice * device = dynamic_cast<IDevice*>(parent());
    if(device)
    {
        return device->getKey();
    }

    return "";
}

QPixmap IGisProject::getIcon() const
{
    return icon(CGisListWks::eColumnIcon).pixmap(22,22);
}

bool IGisProject::isOnDevice() const
{
    IDevice * device = dynamic_cast<IDevice*>(parent());
    return device != 0;
}

bool IGisProject::isChanged() const
{
    return text(CGisListWks::eColumnDecoration) == "*";
}

void IGisProject::edit()
{
    if(dlgDetails.isNull())
    {
        dlgDetails = new CDetailsPrj(*this, 0);
        dlgDetails->setObjectName(getName());
    }

    CMainWindow::self().addWidgetToTab(dlgDetails);
}

void IGisProject::setName(const QString& str)
{
    metadata.name = str;
    setText(CGisListWks::eColumnName, str);
    setChanged();
}

void IGisProject::setKeywords(const QString& str)
{
    metadata.keywords = str;
    setChanged();
}

void IGisProject::setDescription(const QString& str)
{
    metadata.desc = str;
    setChanged();
}

void IGisProject::setLinks(const QList<IGisItem::link_t>& links)
{
    metadata.links = links;
    setChanged();
}

void IGisProject::setSorting(sorting_e s)
{
    sorting = s;
    setChanged();
}

void IGisProject::setChanged()
{
    setText(CGisListWks::eColumnDecoration,"*");
    updateItems();
}


void IGisProject::updateItems()
{
    for(int i = 0; i < childCount(); i++)
    {
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(child(i));
        if(trk)
        {
            trk->findWaypointsCloseBy();
        }
    }
}

void IGisProject::setupName(const QString &defaultName)
{
    if(metadata.name.isEmpty())
    {
        setText(CGisListWks::eColumnName, defaultName);
        metadata.name = defaultName;
    }
    else
    {
        setText(CGisListWks::eColumnName,metadata.name);
    }
}

void IGisProject::markAsSaved()
{
    setText(CGisListWks::eColumnDecoration,"");
    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }
        item->updateDecoration(IGisItem::eMarkNone, IGisItem::eMarkChanged);
    }
}

QString IGisProject::getName() const
{
    IDevice * device = dynamic_cast<IDevice*>(parent());
    if(device == 0)
    {
        return metadata.name;
    }

    return metadata.name + " @ " + device->getName();
}


QString IGisProject::getInfo() const
{
    QString str = metadata.name.isEmpty() ? text(CGisListWks::eColumnName) : metadata.name;
    str = "<div style='font-weight: bold;'>" + str + "</div>";

    if(metadata.time.isValid())
    {
        str += "<br/>\n";
        str += IUnit::datetime2string(metadata.time, false);
    }


    QString desc = IGisItem::removeHtml(metadata.desc).simplified();
    if(!desc.isEmpty())
    {
        str += "<br/>\n";

        if(desc.count() < 100)
        {
            str += desc;
        }
        else
        {
            str += desc.left(97) + "...";
        }
    }

    if(!filename.isEmpty())
    {
        str += QObject::tr("<br/>\nFilename: %1").arg(filename);
    }

    // count number of items by type
    int counter[IGisItem::eTypeMax] = {0};
    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        counter[item->type()]++;
    }
    if(counter[IGisItem::eTypeWpt])
    {
        str += "<br/>\n" + QObject::tr("Waypoints: %1").arg(counter[IGisItem::eTypeWpt]);
    }
    if(counter[IGisItem::eTypeTrk])
    {
        str += "<br/>\n" + QObject::tr("Tracks: %1").arg(counter[IGisItem::eTypeTrk]);
    }
    if(counter[IGisItem::eTypeRte])
    {
        str += "<br/>\n" + QObject::tr("Routes: %1").arg(counter[IGisItem::eTypeRte]);
    }
    if(counter[IGisItem::eTypeOvl])
    {
        str += "<br/>\n" + QObject::tr("Areas: %1").arg(counter[IGisItem::eTypeOvl]);
    }

    return str;
}

IGisItem * IGisProject::getItemByKey(const IGisItem::key_t& key)
{
    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            return item;
        }
    }
    return 0;
}

void IGisProject::getItemByPos(const QPointF& pos, QList<IGisItem *> &items)
{
    if(!isVisible())
    {
        return;
    }

    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->isCloseTo(pos))
        {
            items << item;
        }
    }
}


bool IGisProject::delItemByKey(const IGisItem::key_t& key, QMessageBox::StandardButtons& last)
{
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            if(last != QMessageBox::YesToAll)
            {
                QString msg = QObject::tr("Are you sure you want to delete '%1' from project '%2'?").arg(item->getName()).arg(text(CGisListWks::eColumnName));
                last = QMessageBox::question(&CMainWindow::self(), QObject::tr("Delete..."), msg, QMessageBox::YesToAll|QMessageBox::Cancel|QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
                if((last == QMessageBox::No) || (last == QMessageBox::Cancel))
                {
                    // as each item in the project has to be unique, we can stop searching.
                    return false;
                }
            }
            delete item;

            /*
                Database projects are a bit different. Deleteing an item does not really
                mean the project is changed as the item is still stored in the database.
             */
            if(type != eTypeDb)
            {
                setChanged();
            }

            // as each item in the project has to be unique, we can stop searching.
            return true;
        }
    }
    return false;
}

void IGisProject::editItemByKey(const IGisItem::key_t& key)
{
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            item->edit();
        }
    }
}


void IGisProject::insertCopyOfItem(IGisItem * item, int off, int& lastResult)
{
    bool clone = false;
    IGisItem::key_t key = item->getKey();
    key.project = getKey();
    key.device  = getDeviceKey();

    IGisItem * item2 = getItemByKey(key);
    if(item2 != 0)
    {
        int result = lastResult;
        if(lastResult == CSelectCopyAction::eResultNone)
        {
            CSelectCopyAction dlg(item, item2, &CMainWindow::self());
            dlg.exec();
            result = dlg.getResult();
            if(dlg.allOthersToo())
            {
                lastResult = result;
            }
        }

        if(result == CSelectCopyAction::eResultSkip)
        {
            return;
        }
        if(result == CSelectCopyAction::eResultNone)
        {
            return;
        }
        if(result == CSelectCopyAction::eResultClone)
        {
            clone = true;
        }
        else
        {
            // replace item2 with item
            if(item != item2)
            {
                delete item2;
            }
            else
            {
                // replacing an item with itself does not make sense
                return;
            }
        }
    }

    switch(item->type())
    {
    case IGisItem::eTypeTrk:
    {
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(item);
        if(trk != 0)
        {
            CGisItemTrk * newTrk = new CGisItemTrk(*trk, this, off, clone);
            // if the track is on a device, remove hidden trackpoints
            if(isOnDevice())
            {
                newTrk->filterDelete();
            }
        }
        break;
    }

    case IGisItem::eTypeWpt:
    {
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(item);
        if(wpt != 0)
        {
            new CGisItemWpt(*wpt, this, off, clone);
        }
        break;
    }

    case IGisItem::eTypeRte:
    {
        CGisItemRte * rte = dynamic_cast<CGisItemRte*>(item);
        if(rte != 0)
        {
            new CGisItemRte(*rte, this, off, clone);
        }
        break;
    }

    case IGisItem::eTypeOvl:
    {
        CGisItemOvlArea * area = dynamic_cast<CGisItemOvlArea*>(item);
        if(area != 0)
        {
            new CGisItemOvlArea(*area, this, off, clone);
        }
        break;
    }
    }
}

void IGisProject::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis)
{
    if(!isVisible())
    {
        return;
    }

    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        item->drawItem(p, viewport, blockedAreas, gis);
    }
}

void IGisProject::drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    if(!isVisible())
    {
        return;
    }

    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }


        item->drawItem(p, viewport, gis);
    }
}

void IGisProject::drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis)
{
    if(!isVisible())
    {
        return;
    }

    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        item->drawLabel(p, viewport, blockedAreas, fm, gis);
    }
}

void IGisProject::mount()
{
    if(!isOnDevice())
    {
        return;
    }
    IDevice * device = dynamic_cast<IDevice*>(parent());
    if(device)
    {
        device->mount();
    }
}

void IGisProject::umount()
{
    if(!isOnDevice())
    {
        return;
    }
    IDevice * device = dynamic_cast<IDevice*>(parent());
    if(device)
    {
        device->umount();
    }
}

bool IGisProject::remove()
{
    mount();

    /*
       Check if parent is a device and give it a chance to take care of data.

       e.g. Garmin devices remove images attached to the project.
     */
    IDevice * device = dynamic_cast<IDevice*>(parent());
    if(device)
    {
        device->aboutToRemoveProject(this);
    }

    QFileInfo fi(filename);
    if(fi.isFile())
    {
        QFile::remove(filename);
    }
    else if(fi.isDir())
    {
        QDir(filename).removeRecursively();
    }

    umount();
    return true;
}


