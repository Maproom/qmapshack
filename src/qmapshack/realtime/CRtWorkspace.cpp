/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "realtime/CRtDraw.h"
#include "realtime/CRtSelectSource.h"
#include "realtime/CRtWorkspace.h"
#include "realtime/opensky/CRtOpenSky.h"

#include <QtWidgets>

CRtWorkspace * CRtWorkspace::pSelf = nullptr;

CRtWorkspace::CRtWorkspace(QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);
    connect(this, &CRtWorkspace::sigChanged, this, &CRtWorkspace::slotChanged);

    menu = new QMenu(treeWidget);
    menu->addAction(actionAddSource);
    menu->addAction(actionDeleteSource);

    connect(treeWidget, &QTreeWidget::itemChanged, this, &CRtWorkspace::slotItemChanged);
    connect(treeWidget, &QTreeWidget::customContextMenuRequested, this, &CRtWorkspace::slotContextMenu);
    connect(treeWidget, &QTreeWidget::itemClicked, this, &CRtWorkspace::slotItemClicked);
    connect(actionAddSource, &QAction::triggered, this, &CRtWorkspace::slotAddSource);
    connect(actionDeleteSource, &QAction::triggered, this, &CRtWorkspace::slotDeleteSource);

    SETTINGS;
    cfg.beginGroup("Realtime");
    treeWidget->header()->restoreState(cfg.value("treeWks/state", treeWidget->header()->saveState()).toByteArray());
    const int N = cfg.value("numberOfSources", 0).toInt();
    for(int n = 0; n < N; n++)
    {
        cfg.beginGroup(QString("source%1").arg(n));
        IRtSource * source = IRtSource::create(cfg.value("type", IRtSource::eTypeNone).toInt(), treeWidget);
        if(source != nullptr)
        {
            connect(source, &IRtSource::sigChanged, this, &CRtWorkspace::sigChanged);
            source->loadSettings(cfg);
        }

        cfg.endGroup();
    }

    treeWidget->expandAll();

    cfg.endGroup();

    labelHelp->setText(tr("To add a realtime source do a right click on the list above. "));
    slotChanged();
}

CRtWorkspace::~CRtWorkspace()
{
    SETTINGS;
    cfg.beginGroup("Realtime");
    cfg.remove("");
    cfg.setValue("treeWks/state", treeWidget->header()->saveState());

    const int N = treeWidget->topLevelItemCount();
    cfg.setValue("numberOfSources", N);
    for(int n = 0; n < N; n++)
    {
        IRtSource * source = dynamic_cast<IRtSource*>(treeWidget->topLevelItem(n));
        if(source == nullptr)
        {
            continue;
        }
        cfg.beginGroup(QString("source%1").arg(n));
        cfg.setValue("type", source->type);
        source->saveSettings(cfg);
        cfg.endGroup();
    }

    cfg.endGroup();
}

void CRtWorkspace::draw(QPainter& p, const QPolygonF &viewport, CRtDraw *rt) const
{
    QMutexLocker lock(&IRtSource::mutex);
    QList<QRectF> blockedAreas;

    const int N = treeWidget->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IRtSource * item = dynamic_cast<IRtSource*>(treeWidget->topLevelItem(n));
        if(item == nullptr)
        {
            continue;
        }

        item->drawItem(p, viewport, blockedAreas, rt);
    }
}

void CRtWorkspace::fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt) const
{
    QMutexLocker lock(&IRtSource::mutex);
    const int N = treeWidget->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IRtSource * item = dynamic_cast<IRtSource*>(treeWidget->topLevelItem(n));
        if(item == nullptr)
        {
            continue;
        }

        item->fastDraw(p, viewport, rt);
    }
}

void CRtWorkspace::addSource(IRtSource * source)
{
    QMutexLocker lock(&IRtSource::mutex);
    treeWidget->insertTopLevelItem(treeWidget->topLevelItemCount(), source);
    source->registerWithTreeWidget();
    connect(source, &IRtSource::sigChanged, this, &CRtWorkspace::sigChanged);
    emit sigChanged();
}

bool CRtWorkspace::hasSourceOfType(int type) const
{
    QMutexLocker lock(&IRtSource::mutex);

    const int N = treeWidget->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IRtSource * item = dynamic_cast<IRtSource*>(treeWidget->topLevelItem(n));
        if(item == nullptr)
        {
            continue;
        }

        if(item->type == type)
        {
            return true;
        }
    }

    return false;
}

void CRtWorkspace::mouseMove(const QPointF& pos)
{
    QMutexLocker lock(&IRtSource::mutex);
    const int N = treeWidget->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IRtSource * item = dynamic_cast<IRtSource*>(treeWidget->topLevelItem(n));
        if(item == nullptr)
        {
            continue;
        }
        item->mouseMove(pos);
    }
}

void CRtWorkspace::slotItemChanged(QTreeWidgetItem * item, int column)
{
    QMutexLocker lock(&IRtSource::mutex);

    if(column == IRtSource::eColumnCheckBox)
    {
        IRtSource * source = dynamic_cast<IRtSource*>(item);
        if(source != nullptr)
        {
            source->blockSignals(source->checkState(IRtSource::eColumnCheckBox) == Qt::Unchecked);
        }

        emit sigChanged();
    }
}

void CRtWorkspace::slotContextMenu(const QPoint& point)
{
    actionDeleteSource->setEnabled(false);
    QTreeWidgetItem * item = treeWidget->currentItem();

    if(item != nullptr)
    {
        IRtSource * source = dynamic_cast<IRtSource*>(item);
        if(source != nullptr)
        {
            actionDeleteSource->setEnabled(true);
            source->contextMenu(menu);
        }
        else
        {
            QTreeWidgetItem * child = item;
            item = item->parent();
            while(item != nullptr)
            {
                IRtSource * source = dynamic_cast<IRtSource*>(item);
                if(source != nullptr)
                {
                    source->contextMenuChild(child, menu);
                    break;
                }
                item = item->parent();
            }
        }
    }

    QPoint p = mapToGlobal(point);
    menu->exec(p);
}

void CRtWorkspace::slotItemClicked(QTreeWidgetItem * item, int column)
{
    if(item == nullptr)
    {
        return;
    }
    IRtSource * source = dynamic_cast<IRtSource*>(item);
    if(source != nullptr)
    {
        source->itemClicked(column);
    }
    else
    {
        QTreeWidgetItem * child = item;
        item = item->parent();
        while(item != nullptr)
        {
            IRtSource * source = dynamic_cast<IRtSource*>(item);
            if(source != nullptr)
            {
                source->itemClicked(child, column);
                break;
            }
            item = item->parent();
        }
    }
}

void CRtWorkspace::slotAddSource()
{
    CRtSelectSource dlg(*this);
    dlg.exec();
}

void CRtWorkspace::slotDeleteSource()
{
    int res = QMessageBox::question(this, tr("Delete Source..."), tr("Do you really want to remove the realtime source?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(res != QMessageBox::Yes)
    {
        return;
    }

    QMutexLocker lock(&IRtSource::mutex);

    IRtSource * source = dynamic_cast<IRtSource*>(treeWidget->currentItem());
    if(source == nullptr)
    {
        return;
    }

    delete source;
    emit sigChanged();
}

void CRtWorkspace::slotChanged()
{
    frame->setVisible(treeWidget->topLevelItemCount() == 0);
}
