/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2020 Henri Hornburg <hrnbg@t-online.de>


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

#include "gis/CSetupWorkspace.h"

#include <QtWidgets>

#include "config.h"
#include "gis/CGisWorkspace.h"
#include "gis/CWksItemDelegate.h"
#include "helpers/CSettings.h"

CSetupWorkspace::CSetupWorkspace(CGisWorkspace* workspace, QWidget* parent) : QDialog(parent), workspace(workspace) {
  setupUi(this);

  SETTINGS;
  cfg.beginGroup("Database");
  checkSaveOnExit->setChecked(cfg.value("saveOnExit", true).toBool());
  spinSaveEvery->setValue(cfg.value("saveEvery", 5).toInt());
  checkDbUpdate->setChecked(cfg.value("listenUpdate", false).toBool());
  linePort->setText(cfg.value("port", "34123").toString());
  checkDeviceSupport->setChecked(cfg.value("device support", true).toBool());
  cfg.endGroup();

  connect(checkSaveOnExit, &QCheckBox::toggled, spinSaveEvery, &QSpinBox::setEnabled);

  CWksItemDelegate* delegate = dynamic_cast<CWksItemDelegate*>(workspace->getWksList().itemDelegate());
  if (delegate != nullptr) {
    const CWksItemDelegate::item_status_ctrl_t& itemStatusControl = delegate->getStatusItemsControl();

    const bool noProjectStatus = itemStatusControl.statusSizePrj == -1;
    comboFontSizeStatusPrj->setCurrentIndex(noProjectStatus ? 4 : itemStatusControl.statusSizePrj);
    groupPrj->setDisabled(noProjectStatus);

    const bool noItemStatus = itemStatusControl.statusSizeItem == -1;
    comboFontSizeStatusItem->setCurrentIndex(noItemStatus ? 4 : itemStatusControl.statusSizeItem);
    groupTrk->setDisabled(noItemStatus);
    groupWpt->setDisabled(noItemStatus);
    groupRte->setDisabled(noItemStatus);
    groupArea->setDisabled(noItemStatus);

    checkPrjKeywords->setChecked(itemStatusControl.prj.keywords);
    checkPrjGisStats->setChecked(itemStatusControl.prj.gisStats);
    checkPrjDistance->setChecked(itemStatusControl.prj.distance);
    checkPrjAscent->setChecked(itemStatusControl.prj.ascent);
    checkPrjDescent->setChecked(itemStatusControl.prj.descent);

    checkTrkRating->setChecked(itemStatusControl.trk.rating);
    checkTrkTags->setChecked(itemStatusControl.trk.tags);
    checkTrkDistance->setChecked(itemStatusControl.trk.distance);
    checkTrkAscent->setChecked(itemStatusControl.trk.ascent);
    checkTrkDescent->setChecked(itemStatusControl.trk.descent);

    checkWptRating->setChecked(itemStatusControl.wpt.rating);
    checkWptTags->setChecked(itemStatusControl.wpt.tags);
    checkWptElevation->setChecked(itemStatusControl.wpt.elevation);

    checkRteRating->setChecked(itemStatusControl.rte.rating);
    checkRteTags->setChecked(itemStatusControl.rte.tags);

    checkAreaRating->setChecked(itemStatusControl.area.rating);
    checkAreaTags->setChecked(itemStatusControl.area.tags);
  }

  const QList<QCheckBox*>& checkBoxesPrj = groupPrj->findChildren<QCheckBox*>();
  for (const QCheckBox* checkBox : checkBoxesPrj) {
    connect(checkBox, &QCheckBox::toggled, this, &CSetupWorkspace::slotGisDecorationsChanged);
  }
  const QList<QCheckBox*>& checkBoxesTrk = groupTrk->findChildren<QCheckBox*>();
  for (const QCheckBox* checkBox : checkBoxesTrk) {
    connect(checkBox, &QCheckBox::toggled, this, &CSetupWorkspace::slotGisDecorationsChanged);
  }
  const QList<QCheckBox*>& checkBoxesWpt = groupWpt->findChildren<QCheckBox*>();
  for (const QCheckBox* checkBox : checkBoxesWpt) {
    connect(checkBox, &QCheckBox::toggled, this, &CSetupWorkspace::slotGisDecorationsChanged);
  }
  const QList<QCheckBox*>& checkBoxesRte = groupRte->findChildren<QCheckBox*>();
  for (const QCheckBox* checkBox : checkBoxesRte) {
    connect(checkBox, &QCheckBox::toggled, this, &CSetupWorkspace::slotGisDecorationsChanged);
  }
  const QList<QCheckBox*>& checkBoxesArea = groupArea->findChildren<QCheckBox*>();
  for (const QCheckBox* checkBox : checkBoxesArea) {
    connect(checkBox, &QCheckBox::toggled, this, &CSetupWorkspace::slotGisDecorationsChanged);
  }

  connect(comboFontSizeStatusPrj, &QComboBox::currentIndexChanged, this, &CSetupWorkspace::slotGisDecorationsChanged);
  connect(comboFontSizeStatusItem, &QComboBox::currentIndexChanged, this, &CSetupWorkspace::slotGisDecorationsChanged);
}

CSetupWorkspace::~CSetupWorkspace() {}

void CSetupWorkspace::slotGisDecorationsChanged() {
  CWksItemDelegate* delegate = dynamic_cast<CWksItemDelegate*>(workspace->getWksList().itemDelegate());
  if (delegate == nullptr) {
    return;
  }
  CWksItemDelegate::item_status_ctrl_t itemStatusControl;

  const int indexProject = comboFontSizeStatusPrj->currentIndex();
  const bool noProjectStatus = indexProject == 4;
  itemStatusControl.statusSizePrj = noProjectStatus ? -1 : indexProject;
  groupPrj->setDisabled(noProjectStatus);

  const int indexItem = comboFontSizeStatusItem->currentIndex();
  const bool noItemStatus = indexItem == 4;
  itemStatusControl.statusSizeItem = noItemStatus ? -1 : indexItem;
  groupTrk->setDisabled(noItemStatus);
  groupWpt->setDisabled(noItemStatus);
  groupRte->setDisabled(noItemStatus);
  groupArea->setDisabled(noItemStatus);

  itemStatusControl.prj.keywords = checkPrjKeywords->isChecked();
  itemStatusControl.prj.gisStats = checkPrjGisStats->isChecked();
  itemStatusControl.prj.distance = checkPrjDistance->isChecked();
  itemStatusControl.prj.ascent = checkPrjAscent->isChecked();
  itemStatusControl.prj.descent = checkPrjDescent->isChecked();

  itemStatusControl.trk.rating = checkTrkRating->isChecked();
  itemStatusControl.trk.tags = checkTrkTags->isChecked();
  itemStatusControl.trk.distance = checkTrkDistance->isChecked();
  itemStatusControl.trk.ascent = checkTrkAscent->isChecked();
  itemStatusControl.trk.descent = checkTrkDescent->isChecked();

  itemStatusControl.wpt.rating = checkWptRating->isChecked();
  itemStatusControl.wpt.tags = checkWptTags->isChecked();
  itemStatusControl.wpt.elevation = checkWptElevation->isChecked();

  itemStatusControl.rte.rating = checkRteRating->isChecked();
  itemStatusControl.rte.tags = checkRteTags->isChecked();

  itemStatusControl.area.rating = checkAreaRating->isChecked();
  itemStatusControl.area.tags = checkAreaTags->isChecked();

  delegate->setStatusItemsControl(itemStatusControl);
}

void CSetupWorkspace::accept() {
  SETTINGS;
  cfg.beginGroup("Database");
  cfg.setValue("saveOnExit", checkSaveOnExit->isChecked());
  cfg.setValue("saveEvery", spinSaveEvery->value());
  cfg.setValue("listenUpdate", checkDbUpdate->isChecked());
  cfg.setValue("port", linePort->text());
  cfg.setValue("device support", checkDeviceSupport->isChecked());
  cfg.endGroup();

  QMessageBox::information(this, tr("Setup database..."),
                           tr("Changes to database settings will become active after an application's restart."),
                           QMessageBox::Ok);

  QDialog::accept();
}
