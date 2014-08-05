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
#include "gis/CGisProject.h"
#include "gis/IGisItem.h"
#include "gis/CGisDraw.h"
#include "CMainWindow.h"

#include <QtWidgets>
#include <QtXml>

CGisWidget * CGisWidget::pSelf = 0;

CGisWidget::CGisWidget(QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);

    connect(treeWks, SIGNAL(sigChanged()), SIGNAL(sigChanged()));
}

CGisWidget::~CGisWidget()
{

}

void CGisWidget::loadGpx(const QString& filename)
{
    // cerate file instance
    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    // create md5 hash
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(file.readAll());
    file.close();
    QString key = md5.result().toHex();

    // skip if project is already loaded
    if(treeWks->hasProject(key))
    {
        return;
    }

    // add project to workspace
    QApplication::setOverrideCursor(Qt::WaitCursor);
    IGisItem::mutexItems.lock();
    CGisProject * item = new CGisProject(filename, key, treeWks);
    if(!item->isValid())
    {
        delete item;
    }
    IGisItem::mutexItems.unlock();    
    QApplication::restoreOverrideCursor();

    emit sigChanged();
}

void CGisWidget::slotSaveAll()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    IGisItem::mutexItems.lock();
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        CGisProject * item = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(item == 0)
        {
            continue;
        }
        item->save();
    }
    IGisItem::mutexItems.unlock();
    QApplication::restoreOverrideCursor();
}


void CGisWidget::getItemsByPos(const QPointF& pos, QList<IGisItem*>& items)
{
    IGisItem::mutexItems.lock();
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }
        project->getItemByPos(pos, items);
    }
    IGisItem::mutexItems.unlock();
}

IGisItem * CGisWidget::getItemByKey(const QString& key)
{
    IGisItem * item = 0;
    IGisItem::mutexItems.lock();
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }
        item = project->getItemByKey(key);
        if(item != 0)
        {
            break;
        }
    }
    IGisItem::mutexItems.unlock();
    return item;
}

void CGisWidget::delItemByKey(const QString& key)
{
    IGisItem::mutexItems.lock();
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }
        project->delItemByKey(key);
        if(project->childCount() == 0)
        {
            delete project;
        }
    }

    IGisItem::mutexItems.unlock();

    emit sigChanged();
}

void CGisWidget::editItemByKey(const QString& key)
{
    IGisItem::mutexItems.lock();
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }
        project->editItemByKey(key);
    }

    IGisItem::mutexItems.unlock();

    emit sigChanged();
}

void CGisWidget::draw(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    QFontMetricsF fm(CMainWindow::self().getMapFont());
    QList<QRectF> blockedAreas;
    QSet<QString> seenKeys;

    IGisItem::mutexItems.lock();
    // draw mandatory stuff first
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        CGisProject * item = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(item == 0)
        {
            continue;
        }
        item->drawItem(p, viewport, blockedAreas, seenKeys, gis);
    }

    // reset seen keys as lables will build the list a second time
    seenKeys.clear();

    // draw optional labels second    
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        CGisProject * item = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(item == 0)
        {
            continue;
        }
        item->drawLabel(p, viewport, blockedAreas, seenKeys, fm, gis);
    }
    IGisItem::mutexItems.unlock();
}
