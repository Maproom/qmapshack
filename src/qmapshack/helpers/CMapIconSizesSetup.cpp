/**********************************************************************************************
    Copyright (C) 2025 Jürgen Beck <>

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

#include "helpers/CMapIconSizesSetup.h"

#include <QPushButton>
#include <QSlider>

#include "helpers/CSettings.h"
#include "helpers/CWptIconManager.h"
#include "poi/IPoiFile.h"

CMapIconSizesSetup::CMapIconSizesSetup(QWidget* parent)
    : QDialog(parent), imageWpt("://icons/48x48/Waypoint.png"), imagePoi("://icons/48x48/Poi.png") {
  setupUi(this);

  int labelSize = qMax(sliderWpt->maximum(), sliderPoi->maximum()) + 2;
  labelWpt->setFixedSize(labelSize, labelSize);
  labelPoi->setFixedSize(labelSize, labelSize);

  connect(sliderWpt, &QSlider::valueChanged, this, &CMapIconSizesSetup::slotSetIconSizeWpt);
  connect(sliderPoi, &QSlider::valueChanged, this, &CMapIconSizesSetup::slotSetIconSizePoi);

  SETTINGS;
  sliderWpt->setValue(cfg.value("Icons/sizeWpt", DEFAULTICONSIZE).toInt());
  slotSetIconSizeWpt(sliderWpt->value());
  sliderPoi->setValue(cfg.value("Icons/sizePoi", DEFAULTICONSIZE).toInt());
  slotSetIconSizePoi(sliderPoi->value());

  QPushButton* resetButton = buttonBox->button(QDialogButtonBox::RestoreDefaults);
  connect(resetButton, &QPushButton::clicked, this, &CMapIconSizesSetup::slotResetClicked);
}

CMapIconSizesSetup::~CMapIconSizesSetup() {}

void CMapIconSizesSetup::accept() {
  int done = QDialog::Rejected;
  SETTINGS;
  int size;
  size = sliderWpt->value();
  if (size != cfg.value("Icons/sizeWpt", DEFAULTICONSIZE).toInt()) {
    cfg.setValue("Icons/sizeWpt", size);
    CWptIconManager::self().setIconSize(size);
    done = QDialog::Accepted;
  }
  size = sliderPoi->value();
  if (size != cfg.value("Icons/sizePoi", DEFAULTICONSIZE).toInt()) {
    cfg.setValue("Icons/sizePoi", size);
    IPoiFile::setIconSize(size);
    done = QDialog::Accepted;
  }
  QDialog::done(done);
}

void CMapIconSizesSetup::slotSetIconSizeWpt(int size) {
  valueWpt->setNum(size);
  labelWpt->setPixmap(QPixmap::fromImage(imageWpt.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

void CMapIconSizesSetup::slotSetIconSizePoi(int size) {
  valuePoi->setNum(size);
  labelPoi->setPixmap(QPixmap::fromImage(imagePoi.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

void CMapIconSizesSetup::slotResetClicked(bool checked) {
  sliderWpt->setValue(DEFAULTICONSIZE);
  sliderPoi->setValue(DEFAULTICONSIZE);
}
