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
#include "misc.h"
#include "setup/IAppSetup.h"

CWptIconDialog::CWptIconDialog(CMainWindow* parent) : QDialog(parent) {
  setupUi(this);
  setupList(nullptr);
  setupSignals();
}

void CWptIconDialog::setupSignals() { connect(toolPath, &QToolButton::clicked, this, &CWptIconDialog::slotSetupPath); }

void CWptIconDialog::setupList(QObject* obj) {
  listWidget->clear();

  QString currentIcon = obj == nullptr ? QString() : obj->objectName();
  QListWidgetItem* currentItem = nullptr;

  const QMap<QString, CWptIconManager::icon_t>& wptIcons = CWptIconManager::self().getWptIcons();
  QStringList keys = wptIcons.keys();

  std::sort(keys.begin(), keys.end(), sortByString);

  for (const QString& key : std::as_const(keys)) {
    const QString& icon = wptIcons[key].path;
    QPixmap pixmap = CWptIconManager::self().loadIcon(icon);

    QListWidgetItem* item = new QListWidgetItem(pixmap, key, listWidget);
    if (currentIcon == key) {
      currentItem = item;
    }
  }

  if (currentItem) {
    listWidget->setCurrentItem(currentItem);
    listWidget->scrollToItem(currentItem);
  }

  SETTINGS;
  QString path =
      cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString();
  labelIconPath->setProperty("path", path);
  labelIconPath->setText(path);
  labelIconPath->setToolTip(path);
}

CWptIconDialog::~CWptIconDialog() {}

void CWptIconDialog::slotSetupPath() {
  QString path = labelIconPath->property("path").toString();
  path = QFileDialog::getExistingDirectory(this, tr("Path to user icons..."), path);
  if (path.isEmpty()) {
    return;
  }

  SETTINGS;
  cfg.setValue("Paths/externalWptIcons", path);
  CWptIconManager::self().init();
  setupList(button == nullptr ? dynamic_cast<QObject*>(action) : dynamic_cast<QObject*>(button));
}
