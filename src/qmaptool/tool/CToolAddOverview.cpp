/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "tool/CToolAddOverview.h"

#include "CMainWindow.h"
#include "canvas/IDrawContext.h"
#include "helpers/CSettings.h"
#include "items/CItemFile.h"
#include "setup/IAppSetup.h"

CToolAddOverview::CToolAddOverview(QWidget* parent) : IToolGui(parent) {
  setupUi(this);
  setObjectName(tr("Add Overviews"));

  labelHelp->setText(
      tr("Raster map files consume quite some memory if a larger area is displayed. "
         "Pre-calculated overview levels help to speed up loading and displaying the "
         "map. These overviews can be stored within the map file as well as an external "
         "file. GDAL can remove internally stored overviews, however it will not free "
         "the used space in the file. Therefore it's size will remain large. If you "
         "do not like that use the external option."));

  labelHelp->setVisible(CMainWindow::self().showToolHelp()->isChecked());
  connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);

  connect(itemList, &CItemListWidget::sigAddItem, this, &CToolAddOverview::slotAddItem);
  connect(checkBy2, &QCheckBox::checkStateChanged, this, &CToolAddOverview::slotSelectionChanged);
  connect(checkBy4, &QCheckBox::checkStateChanged, this, &CToolAddOverview::slotSelectionChanged);
  connect(checkBy8, &QCheckBox::checkStateChanged, this, &CToolAddOverview::slotSelectionChanged);
  connect(checkBy16, &QCheckBox::checkStateChanged, this, &CToolAddOverview::slotSelectionChanged);
  connect(checkBy32, &QCheckBox::checkStateChanged, this, &CToolAddOverview::slotSelectionChanged);
  connect(checkBy64, &QCheckBox::checkStateChanged, this, &CToolAddOverview::slotSelectionChanged);
  connect(checkRemove, &QCheckBox::checkStateChanged, this, &CToolAddOverview::slotSelectionChanged);

  connect(itemList, &CItemListWidget::sigSelectionChanged, this, &CToolAddOverview::slotMapSelectionChanged);

  connect(pushStart, &QPushButton::clicked, this, &CToolAddOverview::slotStart);
  connect(pushCancel, &QPushButton::clicked, &CShell::self(), &CShell::slotCancel);
  connect(&CShell::self(), &CShell::sigFinishedJob, this, &CToolAddOverview::slotFinished);

  CToolAddOverview::setupChanged();

  SETTINGS;
  cfg.beginGroup("ToolAddOverview");
  itemList->loadSettings(cfg);
  checkBy2->setChecked(cfg.value("by2", false).toBool());
  checkBy4->setChecked(cfg.value("by4", false).toBool());
  checkBy8->setChecked(cfg.value("by8", false).toBool());
  checkBy16->setChecked(cfg.value("by16", false).toBool());
  checkBy32->setChecked(cfg.value("by32", false).toBool());
  checkBy64->setChecked(cfg.value("by64", false).toBool());
  checkExternal->setChecked(cfg.value("useExternal", true).toBool());
  checkAllFiles->setChecked(cfg.value("allFiles", false).toBool());
  cfg.endGroup();
}

CToolAddOverview::~CToolAddOverview() {
  SETTINGS;
  cfg.beginGroup("ToolAddOverview");
  cfg.remove("");
  itemList->saveSettings(cfg);
  cfg.setValue("by2", checkBy2->isChecked());
  cfg.setValue("by4", checkBy4->isChecked());
  cfg.setValue("by8", checkBy8->isChecked());
  cfg.setValue("by16", checkBy16->isChecked());
  cfg.setValue("by32", checkBy32->isChecked());
  cfg.setValue("by64", checkBy64->isChecked());
  cfg.setValue("useExternal", checkExternal->isChecked());
  cfg.setValue("allFiles", checkAllFiles->isChecked());
  cfg.endGroup();
}

void CToolAddOverview::setupChanged() {
  bool hasGdaladdo = !IAppSetup::self().getGdaladdo().isEmpty();
  labelNoGdaladdo->setVisible(!hasGdaladdo);
  frame->setVisible(hasGdaladdo);
}

void CToolAddOverview::slotAddItem(const QString& filename, QListWidget* list) {
  CItemFile* item = new CItemFile(filename, list);
  connect(item, &CItemFile::sigChanged, itemList, &CItemListWidget::sigChanged);
}

void CToolAddOverview::slotMapSelectionChanged() {
  CMainWindow::self().getCanvas()->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
  slotSelectionChanged(Qt::Checked);
}

void CToolAddOverview::slotSelectionChanged(Qt::CheckState /*state*/) {
  bool enable = checkBy2->isChecked() || checkBy4->isChecked() || checkBy8->isChecked() || checkBy16->isChecked() ||
                checkBy32->isChecked() || checkBy64->isChecked() || checkRemove->isChecked();
  pushStart->setEnabled(enable && itemList->count());

  bool isRemove = checkRemove->isChecked();
  frameLevels->setDisabled(isRemove);
  checkExternal->setDisabled(isRemove);
}

void CToolAddOverview::buildCmd(QList<CShellCmd>& cmds, const IItem* iitem) {
  const CItemFile* item = dynamic_cast<const CItemFile*>(iitem);
  if (nullptr == item) {
    return;
  }

  item->getDrawContext()->unload();

  // remove previous overview
  QStringList args;
  if (checkRemove->isChecked()) {
    args << "-clean" << item->getFilename();
    cmds << CShellCmd(IAppSetup::self().getGdaladdo(), args);
    /// @todo: shrink the file
  } else {
    IDrawContext* context = item->getDrawContext();

    // add new ones
    args.clear();
    args << "-r";
    args << (context->is32BitRgb() ? "cubic" : "nearest");

    if (checkExternal->isChecked()) {
      args << "-ro";
    }

    args << item->getFilename();
    if (checkBy2->isChecked()) {
      args << "2";
    }
    if (checkBy4->isChecked()) {
      args << "4";
    }
    if (checkBy8->isChecked()) {
      args << "8";
    }
    if (checkBy16->isChecked()) {
      args << "16";
    }
    if (checkBy32->isChecked()) {
      args << "32";
    }
    if (checkBy64->isChecked()) {
      args << "64";
    }

    cmds << CShellCmd(IAppSetup::self().getGdaladdo(), args);
  }
}

void CToolAddOverview::slotStart() {
  start(itemList, checkAllFiles->isChecked());
  if (jobId > 0) {
    itemList->setEnabled(false);
    frameInput->setEnabled(false);
    pushStart->setEnabled(false);
    pushCancel->setEnabled(true);
  }
}

void CToolAddOverview::slotFinished(qint32 id) {
  if (finished(id)) {
    itemList->setEnabled(true);
    frameInput->setEnabled(true);
    pushStart->setEnabled(true);
    pushCancel->setEnabled(false);
  }

  if (checkAllFiles->isChecked()) {
    const int N = itemList->count();
    for (int n = 0; n < N; n++) {
      IItem* item = itemList->item(n);
      if (nullptr != item) {
        item->reload();
      }
    }
  } else {
    IItem* item = itemList->currentItem();
    if (nullptr != item) {
      item->reload();
    }
  }
}
