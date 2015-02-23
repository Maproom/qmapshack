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
#include "gis/CGisListWks.h"
#include "gis/prj/IGisProject.h"
#include "helpers/CSelectCopyAction.h"

#ifdef Q_OS_LINUX
#include <QtDBus>
#endif

IDevice::IDevice(const QString &path, const QString &key, QTreeWidget *parent)
    : QTreeWidgetItem(parent)
    , dir(path)
    , key(key)
{
    setIcon(CGisListWks::eColumnName, QIcon("://icons/32x32/Device.png"));
}

IDevice::~IDevice()
{
}

void IDevice::mount(const QString& path)
{
#ifdef Q_OS_LINUX
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",path,"org.freedesktop.UDisks2.Filesystem","Mount");
    QVariantMap args;
    args.insert("options", "sync");
    message << args;
    QDBusConnection::systemBus().call(message);
#endif
}

void IDevice::umount(const QString &path)
{
#ifdef Q_OS_LINUX
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",path,"org.freedesktop.UDisks2.Filesystem","Unmount");
    QVariantMap args;
    message << args;
    QDBusConnection::systemBus().call(message);
#endif
}


QString IDevice::getName() const
{
    return text(CGisListWks::eColumnName);
}

void IDevice::getItemByPos(const QPointF& pos, QList<IGisItem *> &items)
{
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(child(n));
        if(project)
        {
            project->getItemByPos(pos, items);
        }
    }
}

IGisItem * IDevice::getItemByKey(const IGisItem::key_t& key)
{
    IGisItem * item = 0;
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(child(n));
        if(project)
        {
            if(project->getKey() != key.project)
            {
                continue;
            }

            item = project->getItemByKey(key);
            if(item != 0)
            {
                break;
            }
        }
    }
    return item;
}

IGisProject * IDevice::getProjectByKey(const QString& key)
{
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(child(n));
        if(project)
        {
            if(project->getKey() != key)
            {
                continue;
            }

            return project;
        }
    }
    return 0;
}

void IDevice::editItemByKey(const IGisItem::key_t& key)
{
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(child(n));
        if(project)
        {
            project->editItemByKey(key);
        }
    }
}


void IDevice::insertCopyOfProject(IGisProject * project, int& lastResult)
{
    IGisProject * project2 = getProjectByKey(project->getKey());
    if(project2)
    {
        int result = lastResult;
        if(lastResult == CSelectCopyAction::eResultNone)
        {
            CSelectCopyAction dlg(project, project2, &CMainWindow::self());
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

        if(project2->remove())
        {
            delete project2;
        }
        else
        {
            return;
        }
    }

    insertCopyOfProject(project);
}

void IDevice::updateProject(IGisProject * project)
{
    IGisProject * project2 = getProjectByKey(project->getKey());
    if(project2)
    {
        if(project2->remove())
        {
            delete project2;
        }
        else
        {
            return;
        }

        insertCopyOfProject(project);
    }
}

void IDevice::drawItem(QPainter& p, const QPolygonF &viewport, QList<QRectF>& blockedAreas, CGisDraw * gis)
{
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(child(n));
        if(project)
        {
            project->drawItem(p, viewport, blockedAreas, gis);
        }
    }
}

void IDevice::drawLabel(QPainter& p, const QPolygonF &viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis)
{
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(child(n));
        if(project)
        {
            project->drawLabel(p, viewport, blockedAreas, fm, gis);
        }
    }
}

void IDevice::drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(child(n));
        if(project)
        {
            project->drawItem(p, viewport, gis);
        }
    }
}
