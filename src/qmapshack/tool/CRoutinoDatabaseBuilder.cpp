/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "tool/CRoutinoDatabaseBuilder.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/rte/router/CRouterRoutino.h"
#include "gis/rte/router/routino/CRouterRoutinoPathSetup.h"
#include "helpers/CSettings.h"
#include "setup/IAppSetup.h"
#include "theme/CUiTheme.h"

CRoutinoDatabaseBuilder::CRoutinoDatabaseBuilder(QWidget* parent) : IToolShell(parent) {
  setupUi(this);
  CUiTheme::markLabel(labelError, CUiTheme::Role::eError);
  setTextBrowser(textBrowser);

  setObjectName(tr("Create Routino Database"));

  connect(toolSourceFiles, &QToolButton::clicked, this, &CRoutinoDatabaseBuilder::slotSelectSourceFiles);
  connect(toolTargetPath, &QToolButton::clicked, this, &CRoutinoDatabaseBuilder::slotSetupPaths);
  connect(comboTargetPath, &QComboBox::currentIndexChanged, this, &CRoutinoDatabaseBuilder::enabelStartButton);
  connect(pushStart, &QPushButton::clicked, this, &CRoutinoDatabaseBuilder::slotStart);
  connect(lineTargetPrefix, &QLineEdit::textChanged, this, &CRoutinoDatabaseBuilder::enabelStartButton);
  connect(labelHelp, &QLabel::linkActivated, this, &CRoutinoDatabaseBuilder::slotLinkActivated);
  // a prefix is a file name: a path in it would build and clean up outside the target path
  lineTargetPrefix->setValidator(new QRegularExpressionValidator(QRegularExpression("[^/\\\\]+"), this));
  comboTargetPath->setPlaceholderText(tr("Add a path with the button on the left"));

  pushStart->setDisabled(true);

  SETTINGS;
  fillPaths();
  const qint32 idx = comboTargetPath->findText(cfg.value("RoutinoDatabaseBuilder/targetPath", QString()).toString());
  if (idx >= 0) {
    comboTargetPath->setCurrentIndex(idx);
  }

  enabelStartButton();
}

CRoutinoDatabaseBuilder::~CRoutinoDatabaseBuilder() {
  // planetsplitter must end before buildDir is removed: Windows cannot delete open files
  if (cmd.state() != QProcess::NotRunning) {
    cmd.kill();
    cmd.waitForFinished();
  }
}

void CRoutinoDatabaseBuilder::slotSelectSourceFiles() {
  SETTINGS;
  QString path = cfg.value("RoutinoDatabaseBuilder/sourcePath", QDir::homePath()).toString();

  const QStringList& files = QFileDialog::getOpenFileNames(this, tr("Select files..."), path, "OSM Database (*.pbf)");
  if (files.isEmpty()) {
    return;
  }

  QFileInfo fi(files.first());
  path = fi.absolutePath();
  cfg.setValue("RoutinoDatabaseBuilder/sourcePath", path);

  listWidget->clear();
  for (const QString& file : files) {
    new QListWidgetItem(QIcon("://icons/Map.svgt"), file, listWidget);
  }

  enabelStartButton();
}

void CRoutinoDatabaseBuilder::fillPaths() {
  const QString current = comboTargetPath->currentText();
  comboTargetPath->blockSignals(true);
  comboTargetPath->clear();
  comboTargetPath->addItems(CRouterRoutino::self().getPaths());
  comboTargetPath->setCurrentIndex(qMax(0, comboTargetPath->findText(current)));
  comboTargetPath->blockSignals(false);
  enabelStartButton();
}

void CRoutinoDatabaseBuilder::slotSetupPaths() {
  QStringList paths = CRouterRoutino::self().getPaths();
  CRouterRoutinoPathSetup dlg(paths);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }
  CRouterRoutino::self().setPaths(paths);
  fillPaths();
}

void CRoutinoDatabaseBuilder::enabelStartButton() {
  pushStart->setDisabled(true);

  planetsplitter = QStandardPaths::findExecutable("planetsplitter");
  if (planetsplitter.isEmpty()) {
    planetsplitter = QStandardPaths::findExecutable("routino-planetsplitter");
  }
  labelError->setVisible(planetsplitter.isEmpty());

  if (planetsplitter.isEmpty()) {
    return;
  }

  if (listWidget->count() == 0) {
    return;
  }
  if (comboTargetPath->currentText().isEmpty()) {
    return;
  }

  if (lineTargetPrefix->text().isEmpty()) {
    return;
  }

  pushStart->setEnabled(true);
}

void CRoutinoDatabaseBuilder::slotStart() {
  // the registered paths may have changed since this tab was opened
  const QString wanted = comboTargetPath->currentText();
  fillPaths();
  if (comboTargetPath->currentText() != wanted) {
    stdErr(tr("%1 is no longer a Routino path. Select the target again.").arg(wanted));
    return;
  }
  pushStart->setDisabled(true);

  sourceFiles.clear();
  const int N = listWidget->count();
  for (int n = 0; n < N; n++) {
    sourceFiles << listWidget->item(n)->text();
  }

  targetPrefix = lineTargetPrefix->text();
  targetPath = comboTargetPath->currentText();
  SETTINGS;
  cfg.setValue("RoutinoDatabaseBuilder/targetPath", targetPath);
  first = true;
  last = false;

  textBrowser->clear();

  buildDir = std::make_unique<QTemporaryDir>(QDir(targetPath).absoluteFilePath(".routino-build-XXXXXX"));
  if (!buildDir->isValid()) {
    stdErr(tr("Cannot create a folder in %1.").arg(targetPath));
    pushStart->setEnabled(true);
    return;
  }

  slotFinished(0, QProcess::NormalExit);
}

bool CRoutinoDatabaseBuilder::moveIntoTarget() {
  const QDir target(targetPath);
  const QFileInfoList& files = QDir(buildDir->path()).entryInfoList({targetPrefix + "-*.mem"}, QDir::Files);
  if (files.isEmpty()) {
    stdErr(tr("planetsplitter wrote no database."));
    return false;
  }
  for (const QFileInfo& file : files) {
    const QString& to = target.absoluteFilePath(file.fileName());
    // QFile::rename() does not overwrite
    if (QFile::exists(to) && !QFile::remove(to)) {
      stdErr(tr("Cannot replace %1. Is the database in use? Restart QMapShack and build it again.").arg(to));
      return false;
    }
    if (!QFile::rename(file.absoluteFilePath(), to)) {
      stdErr(tr("Cannot move %1 to %2.").arg(file.absoluteFilePath(), targetPath));
      return false;
    }
  }
  buildDir.reset();
  return true;
}

void CRoutinoDatabaseBuilder::finished(int exitCode, QProcess::ExitStatus status) {
  if (last) {
    pushStart->setEnabled(true);
    if (!moveIntoTarget()) {
      buildDir->setAutoRemove(false);
      stdErr(tr("The built files stay in %1.").arg(buildDir->path()));
      textBrowser->setTextColor(CUiTheme::foreground(CUiTheme::Role::eError));
      textBrowser->append(tr("!!! failed !!!\n"));
      return;
    }
    textBrowser->setTextColor(CUiTheme::foreground(CUiTheme::Role::eOk));
    textBrowser->append(tr("!!! done !!!\n"));
    CRouterRoutino::self().reloadDatabases();
    return;
  }

  IAppSetup* instance = IAppSetup::getPlatformInstance();
  if (sourceFiles.isEmpty()) {
    QStringList args;
    args << QString("--dir=%1").arg(buildDir->path());
    args << QString("--prefix=%1").arg(targetPrefix);
    args << QString("--tagging=%1").arg(instance->routinoPath("tagging.xml"));
    args << "--process-only";

    stdOut(planetsplitter + " " + args.join(" ") + "\n");
    cmd.start(planetsplitter, args);

    last = true;
  } else {
    QStringList args;
    args << QString("--dir=%1").arg(buildDir->path());
    args << QString("--prefix=%1").arg(targetPrefix);
    args << QString("--tagging=%1").arg(instance->routinoPath("tagging.xml"));

    if (first) {
      first = false;
      args << "--parse-only";
    } else {
      args << "--parse-only" << "--append";
    }

    args << sourceFiles.first();
    sourceFiles.pop_front();

    stdOut(planetsplitter + " " + args.join(" ") + "\n");
    cmd.start(planetsplitter, args);
  }
}

void CRoutinoDatabaseBuilder::slotLinkActivated(const QUrl& url) { QDesktopServices::openUrl(url); }
