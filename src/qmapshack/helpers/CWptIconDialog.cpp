/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CWptIconDialog.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconManager.h"
#include "setup/IAppSetup.h"

CWptIconDialog::CWptIconDialog(CMainWindow* parent) : QDialog(parent) {
  setupUi(this);
  setupExternalWptIconPath();

  connect(toolPath, &QToolButton::clicked, this, &CWptIconDialog::slotSetupPath);
  connect(widgetIconSelect, &CWptIconSelectWidget::sigSelectedIcon, this, &CWptIconDialog::slotSlectedIcon);
}

void CWptIconDialog::setupExternalWptIconPath() {
  SETTINGS;
  QString path =
      cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString();
  labelIconPath->setProperty("path", path);
  labelIconPath->setText(path);
  labelIconPath->setToolTip(path);
}

CWptIconDialog::~CWptIconDialog() {}

void CWptIconDialog::slotSlectedIcon(const QString& name) {
  emit sigSelectedIcon(name);
  accept();
}

void CWptIconDialog::slotSetupPath() {
  QString path = labelIconPath->property("path").toString();
  path = QFileDialog::getExistingDirectory(this, tr("Path to user icons..."), path);
  if (path.isEmpty()) {
    return;
  }

  SETTINGS;
  cfg.setValue("Paths/externalWptIcons", path);
  setupExternalWptIconPath();

  CWptIconManager::self().init();
}
