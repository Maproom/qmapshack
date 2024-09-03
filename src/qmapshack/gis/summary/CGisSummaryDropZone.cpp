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

#include "gis/summary/CGisSummaryDropZone.h"

#include <QtWidgets>

#include "gis/CGisListWks.h"
#include "gis/db/CDBProject.h"
#include "helpers/CSelectCopyAction.h"

CGisSummaryDropZone::CGisSummaryDropZone(const CGisSummary::dropzone_t& dropZone, QWidget* parent)
    : QLabel(parent), folders(dropZone.folders) {
  setText(dropZone.name);

  QStringList folderNames;
  for (const CGisSummary::folder_t& folder : folders) {
    folderNames << folder.name;
  }

  setToolTip(folderNames.join("\n") + "\n\n" + tr("Drag-n-drop items from the workspace into this drop zone."));
  setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  setAutoFillBackground(true);
  setBackgroundRole(QPalette::Mid);
  setForegroundRole(QPalette::Text);
  setAcceptDrops(true);
}

void CGisSummaryDropZone::setHighlighted(bool yes) { setBackgroundRole(yes ? QPalette::Dark : QPalette::Mid); }

void CGisSummaryDropZone::dragEnterEvent(QDragEnterEvent* e) {
  QObject* source = e->source();
  if (source == nullptr || source->objectName() != "treeWks") {
    return QLabel::dragEnterEvent(e);
  }

  e->setDropAction(Qt::CopyAction);
  setHighlighted(true);
  if (e->proposedAction() == Qt::CopyAction) {
    e->acceptProposedAction();
  }
}

void CGisSummaryDropZone::dragMoveEvent(QDragMoveEvent* e) {
  QObject* source = e->source();
  if (source == nullptr || source->objectName() != "treeWks") {
    return QLabel::dragMoveEvent(e);
  }

  if (e->proposedAction() == Qt::CopyAction) {
    e->acceptProposedAction();
  }
}

void CGisSummaryDropZone::dragLeaveEvent(QDragLeaveEvent* e) {
  setHighlighted(false);
  e->accept();
}

void CGisSummaryDropZone::dropEvent(QDropEvent* e) {
  if (e->proposedAction() == Qt::CopyAction) {
    setHighlighted(false);
    e->acceptProposedAction();
  } else {
    return QLabel::dropEvent(e);
  }

  CGisListWks* wks = dynamic_cast<CGisListWks*>(e->source());
  if (wks == nullptr) {
    return QLabel::dropEvent(e);
  }

  QList<IGisItem*> gisItems;
  const QList<QTreeWidgetItem*>& items = wks->selectedItems();
  for (QTreeWidgetItem* item : items) {
    IGisItem* gisItem = dynamic_cast<IGisItem*>(item);
    if (gisItem != nullptr) {
      gisItems << gisItem;
    }
  }

  CSelectSaveAction::result_e saveActionForAll = CSelectSaveAction::eResultSkip;
  CSelectCopyAction::result_e copyActionForAll = CSelectCopyAction::eResultSkip;
  for (const CGisSummary::folder_t& folder : folders) {
    CDBProject* project = new CDBProject(folder.db, folder.id, nullptr);
    for (IGisItem* gisItem : std::as_const(gisItems)) {
      project->insertCopyOfItem(gisItem, -1, copyActionForAll);
    }

    project->save(saveActionForAll);
    delete project;
  }
}
