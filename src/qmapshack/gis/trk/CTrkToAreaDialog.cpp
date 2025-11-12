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

#include "gis/trk/CTrkToAreaDialog.h"

#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"

CTrkToAreaDialog::CTrkToAreaDialog(IGisProject*& project, QString& areaName)
    : QDialog(CMainWindow::getBestWidgetForParent()),
      project(project),
      areaName(areaName) {
  setupUi(this);

  lineEditAreaName->setText(areaName);
  labelProjectName->setText(project->getName());
  buttonBoxEnabled();

  connect(pushButtonProject, &QPushButton::clicked, this, &CTrkToAreaDialog::slotProject);
  connect(lineEditAreaName, &QLineEdit::textChanged, this, &CTrkToAreaDialog::slotAreaChanged);
  connect(lineEditAreaName, &QLineEdit::textEdited, this, &CTrkToAreaDialog::slotAreaChanged);

  adjustSize();
}

CTrkToAreaDialog::~CTrkToAreaDialog() {}

void CTrkToAreaDialog::slotProject() {
  IGisProject* pr = CGisWorkspace::self().selectProject(true);
  if (nullptr == pr) {
    return;
  }
  project = pr;
  labelProjectName->setText(pr->getName());
}

void CTrkToAreaDialog::buttonBoxEnabled() { buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!areaName.isEmpty()); }

void CTrkToAreaDialog::slotAreaChanged(const QString& text) {
  areaName = text;
  buttonBoxEnabled();
}
