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
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/IGisItem.h"
#include "gis/db/CDBProject.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CProjWpt.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSelectProjectDialog.h"
#include "helpers/CSettings.h"
#include "device/IDevice.h"

#include <QtWidgets>
#include <QtXml>

CGisWidget * CGisWidget::pSelf = 0;

CGisWidget::CGisWidget(QMenu *menuProject, QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);

    treeWks->setExternalMenu(menuProject);

    SETTINGS;
    treeWks->header()->restoreState(cfg.value("Workspace/treeWks/state", treeWks->header()->saveState()).toByteArray());
    treeDB->header()->restoreState(cfg.value("Workspace/treeDB/state", treeDB->header()->saveState()).toByteArray());

    connect(treeWks, SIGNAL(sigChanged()), SIGNAL(sigChanged()));
    connect(treeDB, SIGNAL(sigChanged()), SLOT(slotHelpText()));

    slotHelpText();
}

CGisWidget::~CGisWidget()
{
    SETTINGS;
    cfg.setValue("Workspace/treeWks/state", treeWks->header()->saveState());
    cfg.setValue("Workspace/treeDB/state", treeDB->header()->saveState());
}

void CGisWidget::postEventForWks(QEvent * event)
{
    QCoreApplication::postEvent(treeWks, event);
}

void CGisWidget::postEventForDb(QEvent * event)
{
    QCoreApplication::postEvent(treeDB, event);
}


void CGisWidget::loadGisProject(const QString& filename)
{
    // add project to workspace
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QMutexLocker lock(&IGisItem::mutexItems);
    IGisProject * item = 0;
    QString suffix = QFileInfo(filename).suffix().toLower();
    if(suffix == "gpx")
    {
        item = new CGpxProject(filename, treeWks);
    }
    else if(suffix == "qms")
    {
        item = new CQmsProject(filename, treeWks);
    }

    if(item && !item->isValid())
    {
        delete item;
        item = 0;
    }

    // skip if project is already loaded
    if(item && treeWks->hasProject(item))
    {
        delete item;
        item = 0;
    }


    QApplication::restoreOverrideCursor();

    emit sigChanged();
}

void CGisWidget::slotHelpText()
{
    if(treeDB->topLevelItemCount() == 0)
    {
        frameHelp->show();
    }
    else
    {
        frameHelp->hide();
    }
}

void CGisWidget::slotSaveAll()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        IGisProject * item = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
        if(item == 0)
        {
            continue;
        }
        item->save();
    }

    QApplication::restoreOverrideCursor();
}

IGisProject * CGisWidget::selectProject()
{
    QString key, name;
    CSelectProjectDialog::type_e type;

    CSelectProjectDialog dlg(key, name, type, treeWks);
    dlg.exec();

    IGisProject * project = 0;
    if(!key.isEmpty())
    {
        QMutexLocker lock(&IGisItem::mutexItems);
        for(int i = 0; i < treeWks->topLevelItemCount(); i++)
        {
            project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
            if(project == 0)
            {
                continue;
            }
            if(key == project->getKey())
            {
                break;
            }
        }
    }
    else if(!name.isEmpty())
    {
        QMutexLocker lock(&IGisItem::mutexItems);
        if(type == CSelectProjectDialog::eTypeGpx)
        {
            project = new CGpxProject(name, treeWks);
        }
        else if (type == CSelectProjectDialog::eTypeQms)
        {
            project = new CQmsProject(name, treeWks);
        }
    }

    return project;
}

void CGisWidget::getItemsByPos(const QPointF& pos, QList<IGisItem*>& items)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }
        project->getItemByPos(pos, items);
    }
}

IGisItem * CGisWidget::getItemByKey(const IGisItem::key_t& key)
{
    IGisItem * item = 0;
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }

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

    return item;
}

void CGisWidget::delItemByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    QMessageBox::StandardButtons last = QMessageBox::NoButton;
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }

        if(project->delItemByKey(key, last))
        {
            // update database tree if that is a database project
            CDBProject * dbp = dynamic_cast<CDBProject*>(project);
            if(dbp)
            {
                dbp->postStatus();
            }
        }

        if(last == QMessageBox::Cancel)
        {
            break;
        }
    }


    emit sigChanged();
}

void CGisWidget::editItemByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
        if(project == 0)
        {
            continue;
        }
        project->editItemByKey(key);
    }


    emit sigChanged();
}

void CGisWidget::copyItemByKey(const IGisItem::key_t &key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    IGisItem * item = getItemByKey(key);
    if(item == 0)
    {
        return;
    }

    IGisProject * project = selectProject();
    if(project == 0)
    {
        return;
    }

    int lastResult = CSelectCopyAction::eResultNone;
    project->insertCopyOfItem(item, -1, lastResult);


    emit sigChanged();
}

void CGisWidget::projWptByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
    if(wpt != 0)
    {
        CProjWpt dlg(*wpt, 0);
        dlg.exec();
    }


    emit sigChanged();
}

void CGisWidget::moveWptByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
    if(wpt != 0)
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas != 0)
        {
            canvas->setMouseMoveWpt(*wpt);
        }
    }
}

void CGisWidget::focusTrkByKey(bool yes, const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(trk != 0)
    {
        trk->gainUserFocus(yes);
    }


    emit sigChanged();
}

void CGisWidget::cutTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(trk != 0 && trk->cut())
    {
        int res = QMessageBox::question(this, tr("Cut Track..."), tr("Do you want to delete the original track?"), QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
        if(res == QMessageBox::Ok)
        {
            delete trk;
        }
    }

    emit sigChanged();
}

void CGisWidget::reverseTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(trk)
    {
        trk->reverse();
    }

    emit sigChanged();
}

void CGisWidget::combineTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(trk)
    {
        trk->combine();
    }

    emit sigChanged();
}

void CGisWidget::editTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(trk != 0)
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas != 0)
        {
            canvas->setMouseEditTrk(*trk);
        }
    }
}

void CGisWidget::rangeTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(trk != 0)
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas != 0)
        {
            canvas->setMouseRangeTrk(*trk);
        }
    }
}

void CGisWidget::editAreaByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemOvlArea * area = dynamic_cast<CGisItemOvlArea*>(getItemByKey(key));
    if(area != 0)
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas != 0)
        {
            canvas->setMouseEditArea(*area);
        }
    }
}

void CGisWidget::draw(QPainter& p, const QPolygonF& viewport, CGisDraw * gis)
{
    QFontMetricsF fm(CMainWindow::self().getMapFont());
    QList<QRectF> blockedAreas;

    QMutexLocker lock(&IGisItem::mutexItems);
    // draw mandatory stuff first
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        QTreeWidgetItem * item = treeWks->topLevelItem(i);

        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->drawItem(p, viewport, blockedAreas, gis);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(device)
        {
            device->drawItem(p, viewport, blockedAreas, gis);
            continue;
        }
    }

    // draw optional labels second
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        QTreeWidgetItem * item = treeWks->topLevelItem(i);

        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->drawLabel(p, viewport, blockedAreas, fm, gis);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(device)
        {
            device->drawLabel(p, viewport, blockedAreas, fm, gis);
            continue;
        }
    }
}

void CGisWidget::fastDraw(QPainter& p, const QRectF& viewport, CGisDraw *gis)
{
    /*
        Mutex locking will make map moving very slow if there are many GIS items
        visible. Remove it for now. But I am not sure if that is a good idea.
     */
    //QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = treeWks->topLevelItem(i);

        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->drawItem(p, viewport, gis);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(device)
        {
            device->drawItem(p, viewport, gis);
            continue;
        }
    }
}
