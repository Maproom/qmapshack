/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/rte/router/brouter/CRouterBRouterSetupWizard.h"

#include <QFileDialog>
#include <QMessageBox>

#include "CMainWindow.h"
#include "gis/rte/router/brouter/CRouterBRouterSetup.h"

CRouterBRouterSetupWizard::CRouterBRouterSetupWizard() : QWizard(CMainWindow::getBestWidgetForParent()) {
  setupUi(this);

  setup = new CRouterBRouterSetup(this);

  setButtonText(QWizard::CustomButton1, tr("Restore Default Values"));
  connect(this, &CRouterBRouterSetupWizard::currentIdChanged, this, &CRouterBRouterSetupWizard::slotCurrentIdChanged);
  connect(this, &CRouterBRouterSetupWizard::customButtonClicked, this,
          &CRouterBRouterSetupWizard::slotCustomButtonClicked);

  connect(radioLocal, &QRadioButton::clicked, this, &CRouterBRouterSetupWizard::slotRadioLocalClicked);
  connect(radioOnline, &QRadioButton::clicked, this, &CRouterBRouterSetupWizard::slotRadioOnlineClicked);
  connect(checkExpert, &QCheckBox::clicked, this, &CRouterBRouterSetupWizard::slotCheckExpertClicked);

  connect(lineLocalProfilesUrl, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotProfilesUrlEdited);
  connect(lineLocalSegmentsUrl, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotSegmentsUrlEdited);

  connect(toolLocalDir, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectDirectory);
  connect(toolLocalBRouterJar, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectBRouterJar);
  connect(toolJavaExecutable, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectJava);
  connect(pushLocalFindJava, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalPushFindJava);
  connect(lineLocalDir, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotLocalDirectoryEdited);
  connect(lineLocalBRouterJar, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotLocalBRouterJarEdited);
  connect(lineJavaExecutable, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotLocalJavaExecutableEdited);

  connect(pushCreateOrUpdateLocalInstall, &QPushButton::clicked, this,
          &CRouterBRouterSetupWizard::slotCreateOrUpdateLocalInstallClicked);

  connect(listLocalInstallVersions, &QListView::clicked, pageLocalInstallation,
          &CRouterBRouterDownloadPage::slotListVersionsClicked);
  connect(pushLocalInstall, &QPushButton::clicked, pageLocalInstallation,
          &CRouterBRouterDownloadPage::slotLocalDownloadButtonClicked);

  connect(listProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotProfileClicked);
  connect(listAvailableProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotAvailableProfileClicked);
  connect(toolAddProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotAddProfileClicked);
  connect(toolDeleteProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotDelProfileClicked);
  connect(toolProfileUp, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotProfileUpClicked);
  connect(toolProfileDown, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotProfileDownClicked);

  connect(lineOnlineConfigUrl, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotOnlineConfigUrlEdited);
  connect(pushOnlineConfig, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotOnlineConfigButtonClicked);
  connect(lineOnlineProfilesUrl, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotProfilesUrlEdited);
  connect(lineOnlineService, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotOnlineServiceUrlEdited);

  connect(lineLocalBinariesUrl, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotBinariesUrlCursorEdited);

  connect(setup, &CRouterBRouterSetup::sigOnlineConfigLoaded, this, &CRouterBRouterSetupWizard::slotUpdateCurrentPage);
  connect(setup, &CRouterBRouterSetup::sigBinariesPageLoaded, this, &CRouterBRouterSetupWizard::slotUpdateCurrentPage);
  connect(setup, &CRouterBRouterSetup::sigSegmentsPageLoaded, this, &CRouterBRouterSetupWizard::slotUpdateCurrentPage);
  connect(setup, &CRouterBRouterSetup::sigDisplayOnlineProfileFinished, this,
          &CRouterBRouterSetupWizard::slotDisplayProfile);
  connect(setup, &CRouterBRouterSetup::sigProfilesChanged, this, &CRouterBRouterSetupWizard::slotUpdateCurrentPage);
  connect(setup, &CRouterBRouterSetup::sigVersionChanged, this, &CRouterBRouterSetupWizard::slotUpdateCurrentPage);
  connect(setup, &CRouterBRouterSetup::sigError, this, &CRouterBRouterSetupWizard::slotSetupError);

  QStringListModel* profilesModel = new QStringListModel();
  listProfiles->setModel(profilesModel);

  QStringListModel* availableProfiles = new QStringListModel();
  listAvailableProfiles->setModel(availableProfiles);

  setup->load();
}

CRouterBRouterSetupWizard::~CRouterBRouterSetupWizard() {}

int CRouterBRouterSetupWizard::nextId() const {
  switch (currentId()) {
    case ePageChooseMode: {
      if (setup->installMode == CRouterBRouterSetup::eModeLocal) {
        if (setup->expertMode) {
          return ePageLocalDetails;
        }
        return ePageLocalDirectory;
      }
      if (setup->expertMode)  // ModeOnline
      {
        return ePageOnlineDetails;
      }
      return ePageProfiles;
    }

    case ePageLocalDirectory: {
      if (doLocalInstall) {
        return ePageLocalInstallation;
      }
      return ePageProfiles;
    }

    case ePageLocalInstallation: {
      return ePageProfiles;
    }

    case ePageProfiles: {
      if (setup->installMode == CRouterBRouterSetup::eModeLocal) {
        return ePageLocalTiles;
      }
      break;
    }

    case ePageLocalTiles: {
      break;
    }

    case ePageOnlineDetails: {
      return ePageProfiles;
    }

    case ePageLocalDetails: {
      return ePageLocalDirectory;
    }
  }

  return -1;
}

void CRouterBRouterSetupWizard::initializePage(const int id) {
  switch (id) {
    case ePageLocalDirectory: {
      initLocalDirectory();
      break;
    }

    case ePageLocalInstallation: {
      initLocalInstall();
      break;
    }

    case ePageLocalTiles: {
      initLocalTiles();
      break;
    }
  }
}

bool CRouterBRouterSetupWizard::validateCurrentPage() {
  switch (currentId()) {
    case ePageOnlineDetails: {
      return validateOnlineDetails();
    }

    case ePageLocalDetails: {
      return validateLocalDetails();
    }
  }

  return true;
}

void CRouterBRouterSetupWizard::slotCurrentIdChanged(const int id) {
  switch (id) {
    case ePageChooseMode: {
      beginChooseMode();
      break;
    }

    case ePageLocalDirectory: {
      beginLocalDirectory();
      break;
    }

    case ePageLocalInstallation: {
      beginLocalInstall();
      break;
    }

    case ePageProfiles: {
      beginProfiles();
      break;
    }

    case ePageLocalTiles: {
      beginLocalTiles();
      break;
    }

    case ePageOnlineDetails: {
      beginOnlineDetails();
      break;
    }

    case ePageLocalDetails: {
      beginLocalDetails();
      break;
    }
  }
}

void CRouterBRouterSetupWizard::slotCustomButtonClicked(const int id) {
  if (id == QWizard::CustomButton1) {
    const int page = currentId();
    switch (page) {
      case ePageOnlineDetails: {
        resetOnlineDetails();
        break;
      }

      case ePageLocalDetails: {
        resetLocalDetails();
        break;
      }
    }
  }
}

void CRouterBRouterSetupWizard::accept() {
  setup->save();
  QDialog::accept();
}

void CRouterBRouterSetupWizard::reject() {
  if (!pageLocalTiles->isComplete()) {
    if (pageLocalTiles->raiseWarning()) {
      return;
    }
  }
  QDialog::reject();
}

void CRouterBRouterSetupWizard::beginChooseMode() {
  switch (setup->installMode) {
    case CRouterBRouterSetup::eModeLocal: {
      radioLocal->setChecked(true);
      radioOnline->setChecked(false);
      break;
    }

    case CRouterBRouterSetup::eModeOnline: {
      radioLocal->setChecked(false);
      radioOnline->setChecked(true);
      break;
    }
  }

  checkExpert->setChecked(setup->expertMode);
  setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotRadioLocalClicked() const { setup->installMode = CRouterBRouterSetup::eModeLocal; }

void CRouterBRouterSetupWizard::slotRadioOnlineClicked() const {
  setup->installMode = CRouterBRouterSetup::eModeOnline;
}

void CRouterBRouterSetupWizard::slotCheckExpertClicked() const { setup->expertMode = checkExpert->isChecked(); }

void CRouterBRouterSetupWizard::initLocalDirectory() { pageLocalDirectory->setSetup(setup); }

void CRouterBRouterSetupWizard::beginLocalDirectory() {
  updateLocalDirectory();
  doLocalInstall = false;
  setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotLocalToolSelectDirectory() {
  setup->localDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly);
  updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalToolSelectBRouterJar() {
  QFileDialog dialog(this, tr("select BRouter jar file"), setup->localDir, "Jar File (*.jar)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
    const QStringList& files = dialog.selectedFiles();
    setup->setLocalBRouterJar(files.isEmpty() ? "" : QFileInfo(files.first()).fileName());
    updateLocalDirectory();
  }
}

void CRouterBRouterSetupWizard::slotLocalToolSelectJava() {
  QFileDialog dialog(this, tr("select Java Executable"), QFileInfo(setup->localJavaExecutable).absolutePath(),
                     "Java Executable (java*)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
    const QStringList& files = dialog.selectedFiles();
    setup->setJava(files.isEmpty() ? "" : files.first());
    updateLocalDirectory();
  }
}

void CRouterBRouterSetupWizard::slotLocalPushFindJava() const {
  setup->setJava(setup->findJava());
  updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalDirectoryEdited() const {
  setup->localDir = lineLocalDir->text();
  updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalBRouterJarEdited() const {
  setup->setLocalBRouterJar(lineLocalBRouterJar->text());
  updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalJavaExecutableEdited() const {
  setup->setJava(lineJavaExecutable->text());
  updateLocalDirectory();
}

void CRouterBRouterSetupWizard::updateLocalDirectory() const {
  textLocalDirectory->setVisible(false);
  if (lineLocalDir->text() != setup->localDir) {
    lineLocalDir->setText(setup->localDir);
  }

  const CRouterBRouterLocalSetupStatus& status = setup->checkLocalBRouterInstallation();

  if (setup->localDir.isEmpty()) {
    labelLocalDirResult->setText(tr("please select BRouter installation directory"));
    pushCreateOrUpdateLocalInstall->setVisible(false);
    lineLocalBRouterJar->setVisible(false);
    toolLocalBRouterJar->setVisible(false);
    labelLocalBRouterResult->setVisible(false);
  } else if (!QDir(setup->localDir).exists()) {
    labelLocalDirResult->setText(tr("selected directory does not exist"));
    pushCreateOrUpdateLocalInstall->setText(tr("create directory and install BRouter there"));
    pushCreateOrUpdateLocalInstall->setVisible(true);
    lineLocalBRouterJar->setVisible(false);
    toolLocalBRouterJar->setVisible(false);
    labelLocalBRouterResult->setVisible(false);
  } else if (status.isLocalBRouterJar) {
    if (status.isValidBRouterVersion) {
      labelLocalDirResult->setText(tr("is an existing BRouter version %1.%2.%3 installation")
                                       .arg(setup->versionMajor)
                                       .arg(setup->versionMinor)
                                       .arg(setup->versionPatch));
    } else {
      labelLocalDirResult->setText(tr("seems to be an existing BRouter installation"));
    }
    pushCreateOrUpdateLocalInstall->setText(tr("update existing BRouter installation"));
    pushCreateOrUpdateLocalInstall->setVisible(true);
    if (setup->expertMode) {
      if (lineLocalBRouterJar->text() != setup->localBRouterJar) {
        lineLocalBRouterJar->setText(setup->localBRouterJar);
      }
      lineLocalBRouterJar->setVisible(true);
      toolLocalBRouterJar->setVisible(true);
      labelLocalBRouterResult->setText(tr("is a valid BRouter jar-file (optionally select a different *jar file)"));
      labelLocalBRouterResult->setVisible(true);
    } else {
      lineLocalBRouterJar->setVisible(false);
      toolLocalBRouterJar->setVisible(false);
      labelLocalBRouterResult->setVisible(false);
    }
  } else if (status.isLocalBRouterCandidate) {
    labelLocalDirResult->setText(
        tr("It seems there is an existing BRouter installation but the *.jar file is not valid or outdated."));
    pushCreateOrUpdateLocalInstall->setText(tr("update existing BRouter installation"));
    pushCreateOrUpdateLocalInstall->setVisible(true);
    if (setup->expertMode) {
      if (lineLocalBRouterJar->text() != setup->localBRouterJar) {
        lineLocalBRouterJar->setText(setup->localBRouterJar);
      }
      lineLocalBRouterJar->setVisible(true);
      toolLocalBRouterJar->setVisible(true);
      labelLocalBRouterResult->setText(tr("Choose a different *.jar file or create/update the installation."));
      labelLocalBRouterResult->setVisible(true);
    } else {
      lineLocalBRouterJar->setVisible(false);
      toolLocalBRouterJar->setVisible(false);
      labelLocalBRouterResult->setVisible(false);
    }
  } else {
    labelLocalDirResult->setText(tr("empty directory, create new BRouter installation here"));
    pushCreateOrUpdateLocalInstall->setText(tr("create new BRouter installation"));
    pushCreateOrUpdateLocalInstall->setVisible(true);
    lineLocalBRouterJar->setVisible(false);
    toolLocalBRouterJar->setVisible(false);
    labelLocalBRouterResult->setVisible(false);
  }
  if (lineJavaExecutable->text() != setup->localJavaExecutable) {
    lineJavaExecutable->setText(setup->localJavaExecutable);
  }
  if (status.isJavaExisting) {
    if (status.isJavaValid) {
      labelLocalJavaResult->setText(tr("seems to be a valid Java-executable"));
      if (status.isJavaOutdated) {
        textLocalDirectory->setVisible(true);
        textLocalDirectory->setTextColor(Qt::red);
        textLocalDirectory->setText(
            tr("Your Java version %1 seems to be older than the required version %2.\n"
               "BRouter will probably not work as expected.\n"
               "Please check the logs if Brouter fails to start.")
                .arg(setup->javaMajorVersion == NOINT ? tr("unknown") : QString::number(setup->javaMajorVersion))
                .arg(setup->classMajorVersion));
      }
    } else {
      labelLocalJavaResult->setText(tr("doesn't seem to be a valid Java-executable"));
    }
  } else {
    labelLocalJavaResult->setText(tr("Java Executable not found"));
    labelLocalJavaResult->setVisible(true);
  }
  pageLocalDirectory->setComplete(status.isLocalBRouterJar && status.isValidBRouterVersion && status.isJavaValid &&
                                  !status.isJavaOutdated);
}

void CRouterBRouterSetupWizard::slotCreateOrUpdateLocalInstallClicked() {
  QDir outDir(setup->localDir);
  try {
    if (!outDir.exists()) {
      if (!outDir.mkpath(outDir.absolutePath())) {
        throw tr("Error creating directory %1").arg(outDir.absolutePath());
      }
    }
    doLocalInstall = true;
    next();
  } catch (const QString& msg) {
    textLocalDirectory->setVisible(true);
    textLocalDirectory->setTextColor(Qt::red);
    textLocalDirectory->append(msg);
  }
}

void CRouterBRouterSetupWizard::initLocalInstall() {
  pageLocalInstallation->initialize(listLocalInstallVersions, textLocalInstallVersionDetails, labelLocalInstallLink,
                                    pushLocalInstall, textLocalInstall, setup);
}

void CRouterBRouterSetupWizard::beginLocalInstall() { pageLocalInstallation->begin(); }

void CRouterBRouterSetupWizard::beginProfiles() {
  isError = false;
  updateProfiles();
  if (setup->installMode == CRouterBRouterSetup::eModeLocal) {
    setup->readLocalProfiles();
  }
  if (setup->expertMode) {
    textProfileContent->setText(tr("retrieving available profiles from %1").arg(setup->onlineProfilesUrl));
    setup->loadLocalOnlineProfiles(true);
  } else {
    textProfileContent->setText(tr("retrieving online config from %1").arg(setup->getConfigUrl()));
    setup->loadOnlineConfig(true);
  }
  setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotProfileClicked(const QModelIndex& index) const {
  const QString& profile = listProfiles->model()->data(index).toString();
  listAvailableProfiles->clearSelection();
  updateProfiles();
  setup->displayProfileAsync(profile);
}

void CRouterBRouterSetupWizard::slotAvailableProfileClicked(const QModelIndex& index) const {
  const QString& profile = listAvailableProfiles->model()->data(index).toString();
  listProfiles->clearSelection();
  updateProfiles();
  setup->displayOnlineProfileAsync(profile);
}

void CRouterBRouterSetupWizard::slotDisplayProfile(const QString& profile, const QString content) {
  labelProfileContent->setText(tr("content of profile"));
  textProfileContent->setText(content);
}

void CRouterBRouterSetupWizard::slotAddProfileClicked() const {
  const QStringList& profiles = selectedProfiles(listAvailableProfiles);
  for (const QString& profile : profiles) {
    setup->addProfile(profile);
  }
}

void CRouterBRouterSetupWizard::slotDelProfileClicked() const {
  const QStringList& profiles = selectedProfiles(listProfiles);
  for (const QString& profile : profiles) {
    setup->deleteProfile(profile);
  }
}

void CRouterBRouterSetupWizard::slotProfileUpClicked() const {
  const QStringList& profiles = selectedProfiles(listProfiles);
  for (const QString& profile : profiles) {
    setup->profileUp(profile);
  }
}

void CRouterBRouterSetupWizard::slotProfileDownClicked() const {
  const QStringList& profiles = selectedProfiles(listProfiles);
  for (const QString& profile : profiles) {
    setup->profileDown(profile);
  }
}

void CRouterBRouterSetupWizard::updateProfiles() const {
  const QStringList& profiles = setup->getProfiles();
  QStringList available;
  for (const QString& profile : std::as_const(setup->onlineProfilesAvailable)) {
    if (!profiles.contains(profile)) {
      available << profile;
    }
  }

  QList<int> selected = updateProfileView(listProfiles, profiles);
  std::sort(selected.begin(), selected.end());
  toolDeleteProfile->setEnabled(!selected.isEmpty());
  toolProfileUp->setEnabled(!selected.isEmpty() && selected.first() > 0);
  toolProfileDown->setEnabled(!selected.isEmpty() && selected.last() < profiles.size() - 1);
  if (isError) {
    toolAddProfile->setEnabled(false);
    labelProfileContent->setText(tr("Error:"));
    textProfileContent->setText(error + ": " + errorDetails);
  } else {
    toolAddProfile->setEnabled(!updateProfileView(listAvailableProfiles, available).isEmpty());
    listAvailableProfiles->setVisible(true);
    labelProfileContent->setText(tr("content of profile"));
    textProfileContent->clear();
  }
  pageProfiles->emitCompleteChanged();
}

QStringList CRouterBRouterSetupWizard::selectedProfiles(const QListView* listView) const {
  const QItemSelectionModel* selectModel = listView->selectionModel();
  const QModelIndexList& selected = selectModel->selectedIndexes();
  const QAbstractItemModel* model = listView->model();
  QStringList selectedList;
  for (int i = 0; i < selected.size(); i++) {
    selectedList << model->data(selected.at(i)).toString();
  }
  return selectedList;
}

QList<int> CRouterBRouterSetupWizard::updateProfileView(QListView* listView, const QStringList& values) const {
  QList<int> selected;
  const QStringList& selectedValues = selectedProfiles(listView);
  QStringListModel* listModel = (dynamic_cast<QStringListModel*>(listView->model()));
  QItemSelectionModel* selectModel = listView->selectionModel();
  listModel->setStringList(values);
  for (const QString& value : selectedValues) {
    if (values.contains(value)) {
      int index = values.indexOf(value);
      selectModel->select(listModel->index(index), QItemSelectionModel::Select);
      selected << index;
    }
  }
  return selected;
}

void CRouterBRouterSetupWizard::initLocalTiles() const { pageLocalTiles->setSetup(setup); }

void CRouterBRouterSetupWizard::beginLocalTiles() {
  pageLocalTiles->beginPage();
  setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::beginOnlineDetails() {
  setOption(QWizard::HaveCustomButton1, true);
  setup->loadOnlineConfig(false);
  setup->loadLocalOnlineProfiles(false);
  setup->loadOnlineVersion();
}

void CRouterBRouterSetupWizard::updateOnlineDetails() const {
  if (lineOnlineConfigUrl->text() != setup->expertConfigUrl) {
    lineOnlineConfigUrl->setText(setup->expertConfigUrl);
  }
  if (lineOnlineProfilesUrl->text() != setup->onlineProfilesUrl) {
    lineOnlineProfilesUrl->setText(setup->onlineProfilesUrl);
  }
  if (lineOnlineService->text() != setup->onlineServiceUrl) {
    lineOnlineService->setText(setup->onlineServiceUrl);
  }
  textOnlineDetails->setVisible(isError);
  if (setup->versionMajor == NOINT && setup->versionMinor == NOINT && setup->versionPatch == NOINT) {
    labelOnlineVersion->setText(tr("BRouter-Version: not accessible"));
  } else {
    labelOnlineVersion->setText(
        tr("BRouter-Version: %1.%2.%3").arg(setup->versionMajor).arg(setup->versionMinor).arg(setup->versionMinor));
  }
  if (isError) {
    textOnlineDetails->setText(error + ": " + errorDetails);
  }
}

bool CRouterBRouterSetupWizard::validateOnlineDetails() const {
  setup->expertConfigUrl = lineOnlineConfigUrl->text();
  setup->onlineProfilesUrl = lineOnlineProfilesUrl->text();
  setup->onlineServiceUrl = lineOnlineService->text();
  return true;
}

void CRouterBRouterSetupWizard::resetOnlineDetails() const {
  setup->resetOnlineConfigUrl();
  setup->resetOnlineProfilesUrl();
  setup->resetOnlineServiceUrl();

  updateOnlineDetails();
}

void CRouterBRouterSetupWizard::slotOnlineConfigUrlEdited() {
  setup->expertConfigUrl = lineOnlineConfigUrl->text();
  isError = false;
  setup->loadOnlineConfig(false);
}

void CRouterBRouterSetupWizard::slotOnlineConfigButtonClicked() const { setup->loadOnlineConfig(true); }

void CRouterBRouterSetupWizard::slotBinariesUrlCursorEdited() {
  setup->expertBinariesUrl = lineLocalBinariesUrl->text();
  isError = false;
  setup->loadExpertBinariesPage();
}

void CRouterBRouterSetupWizard::updateLocalDetails() const {
  if (lineLocalBinariesUrl->text() != setup->expertBinariesUrl) {
    lineLocalBinariesUrl->setText(setup->expertBinariesUrl);
  }
  if (lineLocalProfilesUrl->text() != setup->onlineProfilesUrl) {
    lineLocalProfilesUrl->setText(setup->onlineProfilesUrl);
  }
  if (lineLocalSegmentsUrl->text() != setup->expertSegmentsUrl) {
    lineLocalSegmentsUrl->setText(setup->expertSegmentsUrl);
  }
  lineLocalHost->setText(setup->localHost);
  lineLocalPort->setText(setup->localPort);
  checkLocalBindLocalonly->setChecked(setup->localBindLocalonly);
  lineLocalProfiles->setText(setup->localProfileDir);
  lineLocalSegments->setText(setup->localSegmentsDir);
  lineLocalCustomProfiles->setText(setup->localCustomProfileDir);
  lineLocalMaxRuntime->setText(setup->localMaxRunningTime);
  lineLocalNumberThreads->setText(setup->localNumberThreads);
  lineLocalJavaOpts->setText(setup->localJavaOpts);
  textLocalDetails->setVisible(isError);
  if (isError) {
    textLocalDetails->setText(error + ": " + errorDetails);
  }
}

void CRouterBRouterSetupWizard::beginLocalDetails() {
  setOption(QWizard::HaveCustomButton1, true);
  isError = false;
  setup->loadExpertBinariesPage();
  setup->loadExpertSegmentsPage();
  setup->loadLocalOnlineProfiles(false);
}

void CRouterBRouterSetupWizard::slotProfilesUrlEdited() {
  switch (currentId()) {
    case ePageOnlineDetails: {
      setup->onlineProfilesUrl = lineOnlineProfilesUrl->text();
      break;
    }

    case ePageLocalDetails: {
      setup->onlineProfilesUrl = lineLocalProfilesUrl->text();
      break;
    }

    default:
      break;
  }

  isError = false;
  setup->loadLocalOnlineProfiles(false);
}

void CRouterBRouterSetupWizard::slotOnlineServiceUrlEdited() {
  setup->onlineServiceUrl = lineOnlineService->text();
  isError = false;
  setup->loadOnlineVersion();
}

void CRouterBRouterSetupWizard::slotSegmentsUrlEdited() {
  setup->expertSegmentsUrl = lineLocalSegmentsUrl->text();
  isError = false;
  setup->loadExpertSegmentsPage();
}

bool CRouterBRouterSetupWizard::validateLocalDetails() const {
  setup->expertBinariesUrl = lineLocalBinariesUrl->text();
  setup->expertSegmentsUrl = lineLocalSegmentsUrl->text();
  setup->onlineProfilesUrl = lineLocalProfilesUrl->text();
  setup->localHost = lineLocalHost->text();
  setup->localPort = lineLocalPort->text();
  setup->localBindLocalonly = checkLocalBindLocalonly->isChecked();
  setup->localProfileDir = lineLocalProfiles->text();
  setup->localSegmentsDir = lineLocalSegments->text();
  setup->localCustomProfileDir = lineLocalCustomProfiles->text();
  setup->localMaxRunningTime = lineLocalMaxRuntime->text();
  setup->localNumberThreads = lineLocalNumberThreads->text();
  setup->localJavaOpts = lineLocalJavaOpts->text();
  return true;
}

void CRouterBRouterSetupWizard::resetLocalDetails() const {
  setup->resetBinariesUrl();
  setup->resetSegmentsUrl();
  setup->resetOnlineProfilesUrl();
  setup->resetLocalHost();
  setup->resetLocalPort();
  setup->resetLocalBindLocalonly();
  setup->resetLocalBRouterJar();
  setup->resetLocalProfileDir();
  setup->resetLocalSegmentsDir();
  setup->resetLocalCustomProfileDir();
  setup->resetLocalMaxRunningTime();
  setup->resetLocalNumberThreads();
  setup->resetLocalJavaOpts();
  updateLocalDetails();
}

void CRouterBRouterSetupWizard::slotUpdateCurrentPage() {
  isError = false;
  switch (currentId()) {
    case ePageOnlineDetails: {
      updateOnlineDetails();
      break;
    }

    case ePageLocalDetails: {
      updateLocalDetails();
      break;
    }

    case ePageProfiles: {
      updateProfiles();
      break;
    }
  }
}

void CRouterBRouterSetupWizard::slotSetupError(const QString& error, const QString& details) {
  isError = true;
  this->error = error;
  this->errorDetails = details;

  switch (currentId()) {
    case ePageOnlineDetails: {
      updateOnlineDetails();
      break;
    }

    case ePageLocalDetails: {
      updateLocalDetails();
      break;
    }

    case ePageProfiles: {
      updateProfiles();
      break;
    }
  }
}
