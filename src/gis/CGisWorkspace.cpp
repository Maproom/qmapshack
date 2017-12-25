/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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
#include "gis/CGisDatabase.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/CSetupFilter.h"
#include "gis/IGisItem.h"
#include "gis/db/CDBProject.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/CSetupFolder.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CCreateRouteFromWpt.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/router/IRouter.h"
#include "gis/trk/CCombineTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CProjWpt.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSelectProjectDialog.h"
#include "helpers/CSettings.h"

#include <QtWidgets>
#include <QtXml>

CGisWorkspace * CGisWorkspace::pSelf = nullptr;

CGisWorkspace::CGisWorkspace(QMenu *menuProject, QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);

    lineFilter->addAction(actionClearFilter,QLineEdit::TrailingPosition);
    lineFilter->addAction(actionSetupFilter, QLineEdit::LeadingPosition);

    treeWks->setExternalMenu(menuProject);

    SETTINGS;
    treeWks->header()->restoreState(cfg.value("Workspace/treeWks/state", treeWks->header()->saveState()).toByteArray());
    IGisProject::filterMode = IGisProject::filter_mode_e(cfg.value("Workspace/projects/filterMode", IGisProject::filterMode).toInt());

    connect(treeWks, &CGisListWks::sigChanged, this, &CGisWorkspace::sigChanged);
    connect(sliderOpacity, &QSlider::valueChanged, this, &CGisWorkspace::slotSetGisLayerOpacity);
    connect(lineFilter, &QLineEdit::textChanged, this, &CGisWorkspace::slotFilter);
    connect(actionSetupFilter, &QAction::triggered, this, &CGisWorkspace::slotSetupFilter);
    connect(treeWks, &CGisListWks::itemPressed, this, &CGisWorkspace::slotWksItemPressed);
    connect(treeWks, &CGisListWks::itemSelectionChanged, this, &CGisWorkspace::slotWksItemSelectionChanged);
    connect(treeWks, &CGisListWks::sigItemDeleted, this, &CGisWorkspace::slotWksItemSelectionChanged);

    // [Issue #265] Delay the loading of the workspace to make sure the complete IUnit system
    //              is up and running.
    QTimer::singleShot(500, treeWks, SLOT(slotLoadWorkspace()));
}

CGisWorkspace::~CGisWorkspace()
{
    SETTINGS;
    cfg.setValue("Workspace/treeWks/state", treeWks->header()->saveState());
    cfg.setValue("Workspace/projects/filterMode", IGisProject::filterMode);
    /*
        Explicitly delete workspace here, as database projects use
        CGisWorkspace upon destruction to signal the database their destruction.

     */
    delete treeWks;
}

void CGisWorkspace::setOpacity(qreal val)
{
    sliderOpacity->setValue(val * 100);
}

void CGisWorkspace::postEventForWks(QEvent * event)
{
    QCoreApplication::postEvent(treeWks, event);
}

void CGisWorkspace::loadGisProject(const QString& filename)
{
    // add project to workspace
    CCanvas::setOverrideCursor(Qt::WaitCursor, "loadGisProject");
    treeWks->blockSignals(true);

    QMutexLocker lock(&IGisItem::mutexItems);

    IGisProject * item = IGisProject::create(filename, treeWks);
    // skip if project is already loaded
    if(item && treeWks->hasProject(item))
    {
        QMessageBox::information(this, tr("Load project..."), tr("The project \"%1\" is already in the workspace.").arg(item->getName()), QMessageBox::Abort);

        delete item;
        item = nullptr;
    }

    treeWks->blockSignals(false);
    CCanvas::restoreOverrideCursor("loadGisProject");

    emit sigChanged();
}


void CGisWorkspace::slotSetGisLayerOpacity(int val)
{
    CCanvas::gisLayerOpacity = qreal(val)/100;
    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas != nullptr)
    {
        canvas->update();
    }
}

void CGisWorkspace::applyFilter()
{
    slotFilter(lineFilter->text());
}

void CGisWorkspace::slotFilter(const QString& str)
{
    actionClearFilter->setIcon(str.isEmpty() ? QIcon("://icons/32x32/Filter.png") : QIcon("://icons/32x32/Cancel.png"));

    CCanvas::setOverrideCursor(Qt::WaitCursor, "slotFilter");
    QMutexLocker lock(&IGisItem::mutexItems);

    const int N = treeWks->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IGisProject * item = dynamic_cast<IGisProject*>(treeWks->topLevelItem(n));
        if(item == nullptr)
        {
            continue;
        }

        item->filter(str.toUpper());
        item->setExpanded(!str.isEmpty());
    }

    CCanvas::restoreOverrideCursor("slotFilter");

    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CGisWorkspace::slotSetupFilter()
{
    CSetupFilter * setupFilter = new CSetupFilter(this);
    setupFilter->adjustSize();
    setupFilter->move(lineFilter->geometry().topLeft());
    setupFilter->show();
}

void CGisWorkspace::slotSaveAll()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "slotSaveAll");
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        IGisProject * item = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
        if(nullptr == item)
        {
            continue;
        }
        if(item->canSave())
        {
            item->save();
        }
        else
        {
            item->saveAs();
        }
    }

    CCanvas::restoreOverrideCursor("slotSaveAll");
}


void CGisWorkspace::slotWksItemSelectionChanged()
{
    slotWksItemPressed(treeWks->currentItem());
}

void CGisWorkspace::slotWksItemPressed(QTreeWidgetItem * i)
{
    IGisItem * item     = dynamic_cast<IGisItem*>(i);
    if(item != nullptr)
    {
        keyWksSelection = item->getKey();
        for(CCanvas * canvas : CMainWindow::self().getCanvas())
        {
            canvas->reportStatus("WksSelection", tr("<b>Item Selection: </b>Item selected from workspace list. Click on the map to switch back to normal mouse selection behavior."));
        }
    }
    else
    {
        slotWksItemSelectionReset();
    }
}

void CGisWorkspace::slotWksItemSelectionReset()
{
    keyWksSelection.clear();
    for(CCanvas * canvas : CMainWindow::self().getCanvas())
    {
        canvas->reportStatus("WksSelection", "");
    }
}


IGisProject * CGisWorkspace::selectProject()
{
    QString key, name;
    IGisProject::type_e type = IGisProject::eTypeQms;

    CSelectProjectDialog dlg(key, name, type, treeWks);
    if(dlg.exec() == QDialog::Rejected)
    {
        return nullptr;
    }

    IGisProject *project = nullptr;
    if(!key.isEmpty())
    {
        QMutexLocker lock(&IGisItem::mutexItems);
        for(int i = 0; i < treeWks->topLevelItemCount(); i++)
        {
            project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
            if(nullptr == project)
            {
                continue;
            }
            if(key == project->getKey())
            {
                break;
            }
        }
    }
    else if(type == IGisProject::eTypeDb)
    {
        quint64 idParent;
        QString db;
        QString host;
        IDBFolder::type_e type;

        CSelectDBFolder dlg1(idParent, db, host, this);
        if(dlg1.exec() == QDialog::Rejected)
        {
            return nullptr;
        }

        CSetupFolder dlg2(type, name, false, this);
        if(dlg2.exec() == QDialog::Rejected)
        {
            return nullptr;
        }

        QMutexLocker lock(&IGisItem::mutexItems);
        CEvtW2DCreate evt(name, type, idParent, db, host);
        CGisDatabase::self().sendEventForDb(&evt);

        if(evt.idChild)
        {
            CDBProject * p = nullptr;
            while(nullptr == p)
            {
                QApplication::processEvents(QEventLoop::WaitForMoreEvents|QEventLoop::ExcludeUserInputEvents, 100);
                p = dynamic_cast<CDBProject*>(treeWks->getProjectById(evt.idChild, db));
            }
            /*
               Creating a project usually does initiate an info request. However as the project isn't in the workspace
               the moment we create it, the request will fail. That is why we send the info now.
             */
            p->postStatus(false);
            project = p;
        }
    }
    else if(!name.isEmpty())
    {
        QMutexLocker lock(&IGisItem::mutexItems);
        if(type == IGisProject::eTypeGpx)
        {
            project = new CGpxProject(name, treeWks);
        }
        else if (type == IGisProject::eTypeQms)
        {
            project = new CQmsProject(name, treeWks);
        }
    }

    return project;
}

void CGisWorkspace::getItemsByPos(const QPointF& pos, QList<IGisItem*>& items)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = treeWks->topLevelItem(i);
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->getItemsByPos(pos, items);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(device)
        {
            device->getItemsByPos(pos, items);
            continue;
        }
    }

    /*
        If there is an item selected by the workspace limit
        the list of items to this item. But only if the item
        is part of the items close to position.
     */
    if(!keyWksSelection.item.isEmpty() && !items.isEmpty())
    {
        IGisItem * item = getItemByKey(keyWksSelection);
        if(item && items.contains(item))
        {
            items.clear();
            items << item;
        }
        else
        {
            items.clear();
        }
    }
}

void CGisWorkspace::getItemsByKeys(const QList<IGisItem::key_t>& keys, QList<IGisItem*>& items)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = treeWks->topLevelItem(i);
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->getItemsByKeys(keys, items);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(device)
        {
            device->getItemsByKeys(keys, items);
            continue;
        }
    }
}

void CGisWorkspace::getItemsByArea(const QRectF& area, IGisItem::selflags_t flags, QList<IGisItem *> &items)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = treeWks->topLevelItem(i);
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->getItemsByArea(area, flags, items);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(device)
        {
            device->getItemsByArea(area, flags, items);
            continue;
        }
    }
}

void CGisWorkspace::getNogoAreas(QVector<IRouter::circle_t> &areas)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = treeWks->topLevelItem(i);
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->getNogoAreas(areas);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(device)
        {
            device->getNogoAreas(areas);
            continue;
        }
    }
}

void CGisWorkspace::mouseMove(const QPointF& pos)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = treeWks->topLevelItem(i);
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project)
        {
            project->mouseMove(pos);
            continue;
        }
    }
}

IGisItem * CGisWorkspace::getItemByKey(const IGisItem::key_t& key)
{
    IGisItem *item = nullptr;
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item1 = treeWks->topLevelItem(i);
        IGisProject * project = dynamic_cast<IGisProject*>(item1);
        if(project)
        {
            if(project->getKey() != key.project)
            {
                continue;
            }

            item = project->getItemByKey(key);
            if(nullptr != item)
            {
                break;
            }

            continue;
        }

        IDevice * device = dynamic_cast<IDevice*>(item1);
        if(device)
        {
            if(device->getKey() != key.device)
            {
                continue;
            }

            item = device->getItemByKey(key);
            if(nullptr != item)
            {
                break;
            }
        }
    }

    return item;
}

void CGisWorkspace::delItemByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    QMessageBox::StandardButtons last = QMessageBox::NoButton;
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
        if(nullptr == project)
        {
            continue;
        }

        if(project->delItemByKey(key, last))
        {
            // update database tree if that is a database project
            CDBProject * dbp = dynamic_cast<CDBProject*>(project);
            if(dbp)
            {
                dbp->postStatus(true);
            }
        }

        if(last == QMessageBox::Cancel)
        {
            break;
        }
    }


    emit sigChanged();
}

void CGisWorkspace::delItemsByKey(const QList<IGisItem::key_t> &keys)
{
    QMessageBox::StandardButtons last   = QMessageBox::NoButton;

    QSet<CDBProject*>   projects;
    QSet<IGisProject*>  projectsAll;

    for(const IGisItem::key_t key : keys)
    {
        IGisItem * gisItem = getItemByKey(key);
        if(nullptr != gisItem)
        {
            bool yes = false;
            IGisProject *project = dynamic_cast<IGisProject*>(gisItem->parent());
            if(nullptr != project)
            {
                project->blockUpdateItems(true);
                yes = project->delItemByKey(gisItem->getKey(), last);


                /*
                    collect database projects to update their counterpart in
                    the database view, after all operations are done.
                 */
                if(yes && project->getType() == IGisProject::eTypeDb)
                {
                    projects << dynamic_cast<CDBProject*>(project);
                }

                /*
                    Collect all projects to unblock update later on.
                 */
                projectsAll << project;
            }

            if(last == QMessageBox::Cancel)
            {
                break;
            }
        }
    }

    // make all database projects that are changed to post their new status
    // this will update the database view.
    for(CDBProject * project : projects)
    {
        project->postStatus(true);
    }
    // unblock update for all projects seen
    for(IGisProject * project : projectsAll)
    {
        project->blockUpdateItems(false);
    }

    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CGisWorkspace::editItemByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = treeWks->topLevelItem(i);
        IGisProject *project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            project->editItemByKey(key);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(nullptr != device)
        {
            device->editItemByKey(key);
            continue;
        }
    }

    emit sigChanged();
}

void CGisWorkspace::copyItemByKey(const IGisItem::key_t &key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    IGisItem *item = getItemByKey(key);
    if(nullptr == item)
    {
        return;
    }

    IGisProject *project = selectProject();
    if(nullptr == project)
    {
        return;
    }

    int lastResult = CSelectCopyAction::eResultNone;
    project->insertCopyOfItem(item, NOIDX, lastResult);


    emit sigChanged();
}

void CGisWorkspace::copyItemsByKey(const QList<IGisItem::key_t> &keys)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    IGisProject * project = selectProject();
    if(nullptr == project)
    {
        return;
    }

    int lastResult = CSelectCopyAction::eResultNone;

    project->blockUpdateItems(true);
    int cnt = 1;
    PROGRESS_SETUP(tr("Copy items..."), 0, keys.count(), this);
    for(const IGisItem::key_t& key : keys)
    {
        PROGRESS(cnt++, break);
        IGisItem * gisItem = getItemByKey(key);
        if(nullptr != gisItem)
        {
            project->insertCopyOfItem(gisItem, NOIDX, lastResult);
        }
    }
    project->blockUpdateItems(false);

    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CGisWorkspace::changeWptSymByKey(const QList<IGisItem::key_t>& keys, const QString& sym)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    PROGRESS_SETUP(tr("Change waypoint symbols."), 0, keys.count(), this);
    int cnt = 0;
    for(const IGisItem::key_t& key : keys)
    {
        PROGRESS(cnt++, break);
        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
        if(nullptr != wpt)
        {
            wpt->setIcon(sym);
        }
    }

    emit sigChanged();
}


void CGisWorkspace::projWptByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
    if(nullptr != wpt)
    {
        CProjWpt dlg(*wpt, 0);
        dlg.exec();
    }


    emit sigChanged();
}

void CGisWorkspace::moveWptByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
    if(nullptr != wpt)
    {
        if(!wpt->setReadOnlyMode(false))
        {
            return;
        }

        CCanvas *canvas = CMainWindow::self().getVisibleCanvas();
        if(nullptr != canvas)
        {
            canvas->setMouseMoveWpt(*wpt);
        }
    }
}

void CGisWorkspace::toggleWptBubble(const IGisItem::key_t &key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
    if(nullptr != wpt)
    {
        wpt->toggleBubble();
    }
}

void CGisWorkspace::deleteWptRadius(const IGisItem::key_t &key)
{
    IGisItem * item = getItemByKey(key);
    if(nullptr != item)
    {
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt *>(item);
        wpt->setProximity(NOFLOAT);
    }
}

void CGisWorkspace::toggleWptNogoArea(const IGisItem::key_t &key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
    if(nullptr != wpt)
    {
        wpt->toggleNogoArea();
    }
}

void CGisWorkspace::editWptRadius(const IGisItem::key_t &key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(getItemByKey(key));
    if(nullptr != wpt)
    {
        if(!wpt->setReadOnlyMode(false))
        {
            return;
        }

        CCanvas *canvas = CMainWindow::self().getVisibleCanvas();
        if(nullptr != canvas)
        {
            canvas->setMouseRadiusWpt(*wpt);
        }
    }
}

void CGisWorkspace::addWptByPos(QPointF pt, const QString& label, const QString& desc) const
{
    QString name = label;
    QString icon;
    if(!CGisItemWpt::getNewWptData(pt, icon, name))
    {
        return;
    }

    IGisProject * project = CGisWorkspace::self().selectProject();
    if(nullptr == project)
    {
        return;
    }

    QMutexLocker lock(&IGisItem::mutexItems);
    CGisItemWpt * wpt = new CGisItemWpt(pt, name, icon, project);
    if(!desc.isEmpty())
    {
        wpt->setDescription(desc);
    }
    wpt->edit();
}

void CGisWorkspace::focusTrkByKey(bool yes, const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(nullptr != trk)
    {
        trk->gainUserFocus(yes);
    }

    emit sigChanged();
}

void CGisWorkspace::focusRteByKey(bool yes, const IGisItem::key_t &key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(getItemByKey(key));
    if(nullptr != rte)
    {
        rte->gainUserFocus(yes);
    }

    emit sigChanged();
}

void CGisWorkspace::convertRouteToTrack(const IGisItem::key_t &key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(getItemByKey(key));
    if(nullptr != rte)
    {
        rte->toTrack();
    }

    emit sigChanged();
}

void CGisWorkspace::cutTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(nullptr != trk && trk->cut())
    {
        int res = QMessageBox::question(this, tr("Cut Track..."), tr("Do you want to delete the original track?"), QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
        if(res == QMessageBox::Ok)
        {
            delete trk;
        }
    }

    emit sigChanged();
}

void CGisWorkspace::reverseTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(nullptr != trk)
    {
        trk->reverse();
    }

    emit sigChanged();
}

void CGisWorkspace::combineTrkByKey(const IGisItem::key_t& keyTrk)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    QList<IGisItem::key_t> keys;
    IGisItem * item = dynamic_cast<IGisItem*>(getItemByKey(keyTrk));
    if(item == nullptr)
    {
        return;
    }

    keys << keyTrk;

    IGisProject * project = dynamic_cast<IGisProject*>(item->parent());
    if(project == nullptr)
    {
        return;
    }

    const int N = project->childCount();
    for(int i = 0; i < N; i++)
    {
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(project->child(i));
        if(trk != nullptr)
        {
            const IGisItem::key_t& key = trk->getKey();
            if(key != keyTrk)
            {
                keys << key;
            }
        }
    }

    combineTrkByKey(keys, {keyTrk});
}

void CGisWorkspace::combineTrkByKey(const QList<IGisItem::key_t>& keys, const QList<IGisItem::key_t>& keysPreSel)
{
    if(keys.isEmpty())
    {
        return;
    }

    QMutexLocker lock(&IGisItem::mutexItems);

    CCombineTrk dlg(keys, keysPreSel, this);
    dlg.exec();

    emit sigChanged();
}

void CGisWorkspace::activityTrkByKey(const QList<IGisItem::key_t>& keys)
{
    if(keys.isEmpty())
    {
        return;
    }

    quint32 flags = CActivityTrk::selectActivity(this);
    if(0xFFFFFFFF != flags)
    {
        QMutexLocker lock(&IGisItem::mutexItems);
        for(const IGisItem::key_t& key : keys)
        {
            CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
            if(trk != nullptr)
            {
                trk->setActivity(flags);
            }
        }
    }
}

void CGisWorkspace::editTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(nullptr != trk)
    {
        if(!trk->setReadOnlyMode(false))
        {
            return;
        }

        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(nullptr != canvas)
        {
            canvas->setMouseEditTrk(*trk);
        }
    }
}

void CGisWorkspace::rangeTrkByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(getItemByKey(key));
    if(nullptr != trk)
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(nullptr != canvas)
        {
            canvas->setMouseRangeTrk(*trk);
        }
    }
}

void CGisWorkspace::copyTrkWithWptByKey(const IGisItem::key_t &key)
{
    QList<IGisItem::key_t> keys;

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    if(nullptr != trk)
    {
        keys << key;

        const CTrackData& t = trk->getTrackData();
        for(const CTrackData::trkpt_t& trkpt : t)
        {
            if(trkpt.isHidden() || trkpt.keyWpt.item.isEmpty())
            {
                continue;
            }

            keys << trkpt.keyWpt;
        }

        copyItemsByKey(keys);
    }
}

void CGisWorkspace::editRteByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(getItemByKey(key));
    if(nullptr != rte)
    {
        if(!rte->setReadOnlyMode(false))
        {
            return;
        }

        CCanvas *canvas = CMainWindow::self().getVisibleCanvas();
        if(nullptr != canvas)
        {
            canvas->setMouseEditRte(*rte);
        }
    }
}

void CGisWorkspace::reverseRteByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(getItemByKey(key));
    if(nullptr != rte)
    {
        rte->reverse();
    }
}

void CGisWorkspace::calcRteByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(getItemByKey(key));
    if(nullptr != rte)
    {
        rte->calc();
    }
}

void CGisWorkspace::resetRteByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(getItemByKey(key));
    if(rte != nullptr)
    {
        rte->reset();
    }
}


void CGisWorkspace::editAreaByKey(const IGisItem::key_t& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemOvlArea * area = dynamic_cast<CGisItemOvlArea*>(getItemByKey(key));
    if(area != nullptr)
    {
        if(!area->setReadOnlyMode(false))
        {
            return;
        }

        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas != nullptr)
        {
            canvas->setMouseEditArea(*area);
        }
    }
}

void CGisWorkspace::makeRteFromWpt(const QList<IGisItem::key_t>& keys)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CCreateRouteFromWpt dlg(keys, this);
    dlg.exec();
}


void CGisWorkspace::draw(QPainter& p, const QPolygonF& viewport, CGisDraw * gis)
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

        QTreeWidgetItem *item = treeWks->topLevelItem(i);

        IGisProject *project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            project->drawItem(p, viewport, blockedAreas, gis);
            continue;
        }
        IDevice *device = dynamic_cast<IDevice*>(item);
        if(nullptr != device)
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
        if(nullptr != project)
        {
            project->drawLabel(p, viewport, blockedAreas, fm, gis);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(nullptr != device)
        {
            device->drawLabel(p, viewport, blockedAreas, fm, gis);
            continue;
        }
    }
}

void CGisWorkspace::fastDraw(QPainter& p, const QRectF& viewport, CGisDraw *gis)
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
        if(nullptr != project)
        {
            project->drawItem(p, viewport, gis);
            continue;
        }
        IDevice * device = dynamic_cast<IDevice*>(item);
        if(nullptr != device)
        {
            device->drawItem(p, viewport, gis);
            continue;
        }
    }


    IGisItem * item = getItemByKey(keyWksSelection);
    if(item != nullptr)
    {
        item->drawHighlight(p);
    }
}


