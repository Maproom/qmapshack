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

#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "gis/proj_x.h"
#include "gis/rte/router/brouter/CRouterBRouterSetup.h"
#include "gis/rte/router/brouter/CRouterBRouterSetupWizard.h"
#include "helpers/CWebPage.h"
#include "setup/IAppSetup.h"

#include <JlCompress.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>

CRouterBRouterSetupWizard::CRouterBRouterSetupWizard()
    : QWizard(CMainWindow::getBestWidgetForParent())
{
    setupUi(this);

    setup = new CRouterBRouterSetup(this);

    setButtonText(QWizard::CustomButton1, tr("Restore Default Values"));
    connect(this, &CRouterBRouterSetupWizard::currentIdChanged, this, &CRouterBRouterSetupWizard::slotCurrentIdChanged);
    connect(this, &CRouterBRouterSetupWizard::customButtonClicked, this, &CRouterBRouterSetupWizard::slotCustomButtonClicked);

    connect(radioLocal,  &QRadioButton::clicked, this, &CRouterBRouterSetupWizard::slotRadioLocalClicked);
    connect(radioOnline, &QRadioButton::clicked, this, &CRouterBRouterSetupWizard::slotRadioOnlineClicked);
    connect(checkExpert, &QCheckBox::clicked,    this, &CRouterBRouterSetupWizard::slotCheckExpertClicked);

    connect(lineLocalProfilesUrl, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotProfilesUrlEdited);
    connect(lineLocalSegmentsUrl, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotSegmentsUrlEdited);

    connect(toolLocalDir, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectDirectory);
    connect(toolJavaExecutable, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectJava);
    connect(pushLocalFindJava, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalPushFindJava);
    connect(lineLocalDir, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotLocalDirectoryEdited);
    connect(lineJavaExecutable, &QLineEdit::textEdited, this, &CRouterBRouterSetupWizard::slotLocalJavaExecutableEdited);

    connect(pushCreateOrUpdateLocalInstall, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotCreateOrUpdateLocalInstallClicked);

    localVersionsPage = new CWebPage(this);
    webLocalBRouterVersions->setPage(localVersionsPage);
    connect(localVersionsPage, &CWebPage::linkClicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadLinkClicked);
    connect(localVersionsPage, &QWebEnginePage::loadFinished, this, &CRouterBRouterSetupWizard::slotWebLocalBRouterVersionsLoadFinished);
    connect(pushLocalInstall, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadButtonClicked);

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
    connect(setup, &CRouterBRouterSetup::sigDisplayOnlineProfileFinished, this, &CRouterBRouterSetupWizard::slotDisplayProfile);
    connect(setup, &CRouterBRouterSetup::sigProfilesChanged, this, &CRouterBRouterSetupWizard::slotUpdateCurrentPage);
    connect(setup, &CRouterBRouterSetup::sigVersionChanged, this, &CRouterBRouterSetupWizard::slotUpdateCurrentPage);
    connect(setup, &CRouterBRouterSetup::sigError, this, &CRouterBRouterSetupWizard::slotSetupError);

    QStringListModel *profilesModel = new QStringListModel();
    listProfiles->setModel(profilesModel);

    QStringListModel *availableProfiles = new QStringListModel();
    listAvailableProfiles->setModel(availableProfiles);

    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouterSetupWizard::slotLocalDownloadButtonFinished);

    setup->load();
}

CRouterBRouterSetupWizard::~CRouterBRouterSetupWizard()
{
}

int CRouterBRouterSetupWizard::nextId() const
{
    switch(currentId())
    {
    case ePageChooseMode:
    {
        if (setup->installMode == CRouterBRouterSetup::eModeLocal)
        {
            if (setup->expertMode)
            {
                return ePageLocalDetails;
            }
            return ePageLocalDirectory;
        }
        if (setup->expertMode) //ModeOnline
        {
            return ePageOnlineDetails;
        }
        return ePageProfiles;
    }

    case ePageLocalDirectory:
    {
        if (doLocalInstall)
        {
            return ePageLocalInstallation;
        }
        return ePageProfiles;
    }

    case ePageLocalInstallation:
    {
        return ePageProfiles;
    }

    case ePageProfiles:
    {
        if (setup->installMode == CRouterBRouterSetup::eModeLocal)
        {
            return ePageLocalTiles;
        }
        break;
    }

    case ePageLocalTiles:
    {
        break;
    }

    case ePageOnlineDetails:
    {
        return ePageProfiles;
    }

    case ePageLocalDetails:
    {
        return ePageLocalDirectory;
    }
    }

    return -1;
}

void CRouterBRouterSetupWizard::initializePage(const int id)
{
    switch(id)
    {
    case ePageLocalDirectory:
    {
        initLocalDirectory();
        break;
    }

    case ePageLocalInstallation:
    {
        initLocalInstall();
        break;
    }

    case ePageLocalTiles:
    {
        initLocalTiles();
        break;
    }
    }
}

bool CRouterBRouterSetupWizard::validateCurrentPage()
{
    switch(currentId())
    {
    case ePageOnlineDetails:
    {
        return validateOnlineDetails();
    }

    case ePageLocalDetails:
    {
        return validateLocalDetails();
    }
    }

    return true;
}

void CRouterBRouterSetupWizard::slotCurrentIdChanged(const int id)
{
    switch(id)
    {
    case ePageChooseMode:
    {
        beginChooseMode();
        break;
    }

    case ePageLocalDirectory:
    {
        beginLocalDirectory();
        break;
    }

    case ePageLocalInstallation:
    {
        beginLocalInstall();
        break;
    }

    case ePageProfiles:
    {
        beginProfiles();
        break;
    }

    case ePageLocalTiles:
    {
        beginLocalTiles();
        break;
    }

    case ePageOnlineDetails:
    {
        beginOnlineDetails();
        break;
    }

    case ePageLocalDetails:
    {
        beginLocalDetails();
        break;
    }
    }
}

void CRouterBRouterSetupWizard::slotCustomButtonClicked(const int id)
{
    if (id == QWizard::CustomButton1)
    {
        const int page = currentId();
        switch (page)
        {
        case ePageOnlineDetails:
        {
            resetOnlineDetails();
            break;
        }

        case ePageLocalDetails:
        {
            resetLocalDetails();
            break;
        }
        }
    }
}

void CRouterBRouterSetupWizard::accept()
{
    setup->save();
    QDialog::accept();
}

void CRouterBRouterSetupWizard::reject()
{
    if (!pageLocalTiles->isComplete())
    {
        if (pageLocalTiles->raiseWarning())
        {
            return;
        }
    }
    QDialog::reject();
}

void CRouterBRouterSetupWizard::beginChooseMode()
{
    switch(setup->installMode)
    {
    case CRouterBRouterSetup::eModeLocal:
    {
        radioLocal->setChecked(true);
        radioOnline->setChecked(false);
        break;
    }

    case CRouterBRouterSetup::eModeOnline:
    {
        radioLocal->setChecked(false);
        radioOnline->setChecked(true);
        break;
    }
    }

    checkExpert->setChecked(setup->expertMode);
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotRadioLocalClicked() const
{
    setup->installMode = CRouterBRouterSetup::eModeLocal;
}

void CRouterBRouterSetupWizard::slotRadioOnlineClicked() const
{
    setup->installMode = CRouterBRouterSetup::eModeOnline;
}

void CRouterBRouterSetupWizard::slotCheckExpertClicked() const
{
    setup->expertMode = checkExpert->isChecked();
}

void CRouterBRouterSetupWizard::initLocalDirectory()
{
    pageLocalDirectory->setSetup(setup);
}

void CRouterBRouterSetupWizard::beginLocalDirectory()
{
    updateLocalDirectory();
    doLocalInstall = false;
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotLocalToolSelectDirectory()
{
    setup->localDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly);
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalToolSelectJava()
{
    QFileDialog dialog(this,
                       tr("select Java Executable"),
                       QFileInfo(setup->localJavaExecutable).absolutePath(),
                       "Java Executable (java*)");
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec())
    {
        setup->localJavaExecutable = dialog.selectedFiles().first();
        updateLocalDirectory();
    }
}

void CRouterBRouterSetupWizard::slotLocalPushFindJava() const
{
    setup->localJavaExecutable = setup->findJava();
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalDirectoryEdited() const
{
    setup->localDir = lineLocalDir->text();
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalJavaExecutableEdited() const
{
    setup->localJavaExecutable = lineJavaExecutable->text();
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::updateLocalDirectory() const
{
    textLocalDirectory->setVisible(false);
    if (lineLocalDir->text() != setup->localDir)
    {
        lineLocalDir->setText(setup->localDir);
    }
    if (lineJavaExecutable->text() != setup->localJavaExecutable)
    {
        lineJavaExecutable->setText(setup->localJavaExecutable);
    }
    if (setup->localDir.isEmpty())
    {
        labelLocalDirResult->setText(tr("please select BRouter installation directory"));
        pushCreateOrUpdateLocalInstall->setVisible(false);
    }
    else if(!QDir(setup->localDir).exists())
    {
        labelLocalDirResult->setText(tr("selected directory does not exist"));
        pushCreateOrUpdateLocalInstall->setText(tr("create directory and install BRouter there"));
        pushCreateOrUpdateLocalInstall->setVisible(true);
    }
    else
    {
        if (setup->isLocalBRouterInstalled())
        {
            labelLocalDirResult->setText(tr("existing BRouter installation"));
            pushCreateOrUpdateLocalInstall->setText(tr("update existing BRouter installation"));
            pushCreateOrUpdateLocalInstall->setVisible(true);
        }
        else
        {
            labelLocalDirResult->setText(tr("empty directory, create new BRouter installation here"));
            pushCreateOrUpdateLocalInstall->setText(tr("create new BRouter installation"));
            pushCreateOrUpdateLocalInstall->setVisible(true);
        }
    }
    if (QFile(setup->localJavaExecutable).exists())
    {
        if (QFileInfo(setup->localJavaExecutable).baseName().startsWith("java"))
        {
            labelLocalJavaResult->setText(tr("seems to be a valid Java-executable"));
        }
        else
        {
            labelLocalJavaResult->setText(tr("doesn't seem to be a valid Java-executable"));
        }
    }
    else
    {
        labelLocalJavaResult->setText(tr("Java Executable not found"));
        labelLocalJavaResult->setVisible(true);
    }
    pageLocalDirectory->emitCompleteChanged();
}

void CRouterBRouterSetupWizard::slotCreateOrUpdateLocalInstallClicked()
{
    QDir outDir(setup->localDir);
    try
    {
        if(!outDir.exists())
        {
            if (!outDir.mkpath(outDir.absolutePath()))
            {
                throw tr("Error creating directory %1").arg(outDir.absolutePath());
            }
        }
        doLocalInstall = true;
        next();
    }
    catch (const QString &msg)
    {
        textLocalDirectory->setVisible(true);
        textLocalDirectory->setTextColor(Qt::red);
        textLocalDirectory->append(msg);
    }
}

void CRouterBRouterSetupWizard::initLocalInstall()
{
    pageLocalInstallation->setSetup(setup);
    localInstallLoaded = false;
    localVersionsPage->load(QUrl(setup->getBinariesUrl()));
}

void CRouterBRouterSetupWizard::slotWebLocalBRouterVersionsLoadFinished(bool ok)
{
    if (!localInstallLoaded)
    {
        if (!ok)
        {
            textLocalInstall->setVisible(true);
            textLocalInstall->setTextColor(Qt::red);
            textLocalInstall->append(tr("Error loading installation-page at %1").arg(setup->getBinariesUrl()));
        }
        else
        {
            localInstallLoaded = true;
        }
    }
}

void CRouterBRouterSetupWizard::beginLocalInstall()
{
    doLocalInstall = false;
    textLocalInstall->setVisible(false);
    textLocalInstall->clear();
    labelLocalInstallLink->setText(tr("no brouter-version to install selected"));
    pushLocalInstall->setEnabled(false);
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotLocalDownloadLinkClicked(const QUrl &url)
{
    downloadUrl = url;
    labelLocalInstallLink->setText(QString(tr("selected %1 for download and installation")).arg(url.fileName()));
    pushLocalInstall->setEnabled(true);
}

void CRouterBRouterSetupWizard::slotLocalDownloadButtonClicked()
{
    const QString& strUrl = downloadUrl.toString();

    if(!strUrl.startsWith("https"))
    {
        QMessageBox mbox;
        mbox.setWindowTitle(tr("Warning..."));
        mbox.setIcon(QMessageBox::Warning);
        mbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        mbox.setDefaultButton(QMessageBox::Abort);

        QString msg = tr("Download: %1<br/>"
                         "<br/>"
                         "This will download and install a zip file from a download location that is not secured "
                         "by any standard at all, using plain HTTP. Usually this should be HTTPS. The risk is "
                         "someone redirecting the request and sending you a replacement zip with malware. There "
                         "is no way for QMapShack to detect this. <br/>"
                         "If you do not understand this or if you are in doubt, do not proceed and abort. "
                         "Use the Web version of BRouter instead."
                         ).arg(strUrl);

        mbox.setText(msg);

        QCheckBox * checkAgree = new QCheckBox(tr("I understand the risk and wish to proceed."), &mbox);
        mbox.setCheckBox(checkAgree);
        connect(checkAgree, &QCheckBox::clicked, mbox.button(QMessageBox::Ok), &QPushButton::setEnabled);
        mbox.button(QMessageBox::Ok)->setDisabled(true);

        if(mbox.exec() != QMessageBox::Ok)
        {
            return;
        }
    }
    textLocalInstall->setVisible(true);
    textLocalInstall->setTextColor(Qt::darkGreen);
    textLocalInstall->append(tr("download %1 started").arg(downloadUrl.toString()));
    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(downloadUrl));
    reply->setProperty("fileName", downloadUrl.fileName());
}

void CRouterBRouterSetupWizard::slotLocalDownloadButtonFinished(QNetworkReply * reply)
{
    reply->deleteLater();
    try
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            throw tr("Network Error: %1").arg(reply->errorString());
        }
        const QString &fileName = reply->property("fileName").toString();
        QDir outDir(setup->localDir);
        if (!outDir.exists())
        {
            throw tr("Error directory %1 does not exist").arg(outDir.absolutePath());
        }
        QFile outfile(outDir.absoluteFilePath(fileName));
        try
        {
            if (!outfile.open(QIODevice::WriteOnly))
            {
                throw tr("Error creating file %1").arg(outfile.fileName());
            }
            if (outfile.write(reply->readAll()) < 0)
            {
                throw tr("Error writing to file %1").arg(outfile.fileName());
            }
            outfile.close();
            textLocalInstall->setTextColor(Qt::darkGreen);
            textLocalInstall->append(tr("download %1 finished").arg(outfile.fileName()));
            const QStringList &unzippedNames = JlCompress::extractDir(outfile.fileName(), setup->localDir);
            textLocalInstall->append(tr("unzipping:"));
            for (const QString unzipped : unzippedNames)
            {
                textLocalInstall->append(unzipped);
            }
            textLocalInstall->append(tr("ready."));
            pageLocalInstallation->emitCompleteChanged();
            setup->readLocalProfiles();
        }
        catch (const QString &msg)
        {
            if (outfile.isOpen())
            {
                outfile.close();
            }
            if (outfile.exists())
            {
                outfile.remove();
            }
            throw msg;
        }
    }
    catch (const QString &msg)
    {
        textLocalInstall->setTextColor(Qt::red);
        textLocalInstall->append(tr("download of brouter failed: %1").arg(msg));
    }
}

void CRouterBRouterSetupWizard::beginProfiles()
{
    isError = false;
    updateProfiles();
    if (setup->installMode == CRouterBRouterSetup::eModeLocal)
    {
        setup->readLocalProfiles();
    }
    if (setup->expertMode)
    {
        textProfileContent->setText(tr("retrieving available profiles from %1").arg(setup->onlineProfilesUrl));
        setup->loadLocalOnlineProfiles(true);
    }
    else
    {
        textProfileContent->setText(tr("retrieving online config from %1").arg(setup->getConfigUrl()));
        setup->loadOnlineConfig(true);
    }
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotProfileClicked(const QModelIndex & index) const
{
    const QString &profile = listProfiles->model()->data(index).toString();
    listAvailableProfiles->clearSelection();
    updateProfiles();
    setup->displayProfileAsync(profile);
}

void CRouterBRouterSetupWizard::slotAvailableProfileClicked(const QModelIndex & index) const
{
    const QString &profile = listAvailableProfiles->model()->data(index).toString();
    listProfiles->clearSelection();
    updateProfiles();
    setup->displayOnlineProfileAsync(profile);
}

void CRouterBRouterSetupWizard::slotDisplayProfile(const QString &profile, const QString content)
{
    labelProfileContent->setText(tr("content of profile"));
    textProfileContent->setText(content);
}

void CRouterBRouterSetupWizard::slotAddProfileClicked() const
{
    for (const QString &profile : selectedProfiles(listAvailableProfiles))
    {
        setup->addProfile(profile);
    }
}

void CRouterBRouterSetupWizard::slotDelProfileClicked() const
{
    for (const QString &profile : selectedProfiles(listProfiles))
    {
        setup->deleteProfile(profile);
    }
}

void CRouterBRouterSetupWizard::slotProfileUpClicked() const
{
    for (const QString &profile : selectedProfiles(listProfiles))
    {
        setup->profileUp(profile);
    }
}

void CRouterBRouterSetupWizard::slotProfileDownClicked() const
{
    for (const QString &profile : selectedProfiles(listProfiles))
    {
        setup->profileDown(profile);
    }
}

void CRouterBRouterSetupWizard::updateProfiles() const
{
    const QStringList &profiles = setup->getProfiles();
    QStringList available;
    for(const QString &profile:setup->onlineProfilesAvailable)
    {
        if (!profiles.contains(profile))
        {
            available << profile;
        }
    }

    QList<int> selected = updateProfileView(listProfiles, profiles);
    qSort(selected.begin(), selected.end());
    toolDeleteProfile->setEnabled(!selected.isEmpty());
    toolProfileUp->setEnabled(!selected.isEmpty() && selected.first() > 0);
    toolProfileDown->setEnabled(!selected.isEmpty() && selected.last() < profiles.size() - 1);
    if (isError)
    {
        toolAddProfile->setEnabled(false);
        labelProfileContent->setText(tr("Error:"));
        textProfileContent->setText(error + ": " + errorDetails);
    }
    else
    {
        toolAddProfile->setEnabled(!updateProfileView(listAvailableProfiles, available).isEmpty());
        listAvailableProfiles->setVisible(true);
        labelProfileContent->setText(tr("content of profile"));
        textProfileContent->clear();
    }
    pageProfiles->emitCompleteChanged();
}

QStringList CRouterBRouterSetupWizard::selectedProfiles(const QListView * listView) const
{
    const QItemSelectionModel * selectModel = listView->selectionModel();
    const QModelIndexList &selected = selectModel->selectedIndexes();
    const QAbstractItemModel * model = listView->model();
    QStringList selectedList;
    for (int i = 0; i < selected.size(); i++)
    {
        selectedList << model->data(selected.at(i)).toString();
    }
    return selectedList;
}

QList<int> CRouterBRouterSetupWizard::updateProfileView(QListView * listView, const QStringList &values) const
{
    QList<int> selected;
    const QStringList &selectedValues = selectedProfiles(listView);
    QStringListModel * listModel = (dynamic_cast<QStringListModel*>(listView->model()));
    QItemSelectionModel * selectModel = listView->selectionModel();
    listModel->setStringList(values);
    for (const QString &value : selectedValues)
    {
        if (values.contains(value))
        {
            int index = values.indexOf(value);
            selectModel->select(listModel->index(index), QItemSelectionModel::Select);
            selected << index;
        }
    }
    return selected;
}

void CRouterBRouterSetupWizard::initLocalTiles() const
{
    pageLocalTiles->setSetup(setup);
}

void CRouterBRouterSetupWizard::beginLocalTiles()
{
    pageLocalTiles->beginPage();
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::beginOnlineDetails()
{
    setOption(QWizard::HaveCustomButton1, true);
    setup->loadOnlineConfig(false);
    setup->loadLocalOnlineProfiles(false);
    setup->loadOnlineVersion();
}

void CRouterBRouterSetupWizard::updateOnlineDetails() const
{
    if (lineOnlineConfigUrl->text() != setup->expertConfigUrl)
    {
        lineOnlineConfigUrl->setText(setup->expertConfigUrl);
    }
    if (lineOnlineProfilesUrl->text() != setup->onlineProfilesUrl)
    {
        lineOnlineProfilesUrl->setText(setup->onlineProfilesUrl);
    }
    if (lineOnlineService->text() != setup->onlineServiceUrl)
    {
        lineOnlineService->setText(setup->onlineServiceUrl);
    }
    textOnlineDetails->setVisible(isError);
    if (setup->versionMajor == NOINT && setup->versionMinor == NOINT && setup->versionPatch == NOINT)
    {
        labelOnlineVersion->setText(tr("BRouter-Version: not accessible"));
    }
    else
    {
        labelOnlineVersion->setText(tr("BRouter-Version: %1.%2.%3").arg(setup->versionMajor).arg(setup->versionMinor).arg(setup->versionMinor));
    }
    if (isError)
    {
        textOnlineDetails->setText(error + ": " + errorDetails);
    }
}

bool CRouterBRouterSetupWizard::validateOnlineDetails() const
{
    setup->expertConfigUrl = lineOnlineConfigUrl->text();
    setup->onlineProfilesUrl = lineOnlineProfilesUrl->text();
    setup->onlineServiceUrl = lineOnlineService->text();
    return true;
}

void CRouterBRouterSetupWizard::resetOnlineDetails() const
{
    setup->resetOnlineConfigUrl();
    setup->resetOnlineProfilesUrl();
    setup->resetOnlineServiceUrl();

    updateOnlineDetails();
}

void CRouterBRouterSetupWizard::slotOnlineConfigUrlEdited()
{
    setup->expertConfigUrl = lineOnlineConfigUrl->text();
    isError = false;
    setup->loadOnlineConfig(false);
}

void CRouterBRouterSetupWizard::slotOnlineConfigButtonClicked() const
{
    setup->loadOnlineConfig(true);
}

void CRouterBRouterSetupWizard::slotBinariesUrlCursorEdited()
{
    setup->expertBinariesUrl = lineLocalBinariesUrl->text();
    isError = false;
    setup->loadExpertBinariesPage();
}

void CRouterBRouterSetupWizard::updateLocalDetails() const
{
    if (lineLocalBinariesUrl->text() != setup->expertBinariesUrl)
    {
        lineLocalBinariesUrl->setText(setup->expertBinariesUrl);
    }
    if (lineLocalProfilesUrl->text() != setup->onlineProfilesUrl)
    {
        lineLocalProfilesUrl->setText(setup->onlineProfilesUrl);
    }
    if (lineLocalSegmentsUrl->text() != setup->expertSegmentsUrl)
    {
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
    if (isError)
    {
        textLocalDetails->setText(error + ": " + errorDetails);
    }
}

void CRouterBRouterSetupWizard::beginLocalDetails()
{
    setOption(QWizard::HaveCustomButton1, true);
    isError = false;
    setup->loadExpertBinariesPage();
    setup->loadExpertSegmentsPage();
    setup->loadLocalOnlineProfiles(false);
}

void CRouterBRouterSetupWizard::slotProfilesUrlEdited()
{
    switch (currentId())
    {
    case ePageOnlineDetails:
    {
        setup->onlineProfilesUrl = lineOnlineProfilesUrl->text();
        break;
    }

    case ePageLocalDetails:
    {
        setup->onlineProfilesUrl = lineLocalProfilesUrl->text();
        break;
    }

    default:
        break;
    }

    isError = false;
    setup->loadLocalOnlineProfiles(false);
}

void CRouterBRouterSetupWizard::slotOnlineServiceUrlEdited()
{
    setup->onlineServiceUrl = lineOnlineService->text();
    isError = false;
    setup->loadOnlineVersion();
}

void CRouterBRouterSetupWizard::slotSegmentsUrlEdited()
{
    setup->expertSegmentsUrl = lineLocalSegmentsUrl->text();
    isError = false;
    setup->loadExpertSegmentsPage();
}

bool CRouterBRouterSetupWizard::validateLocalDetails() const
{
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

void CRouterBRouterSetupWizard::resetLocalDetails() const
{
    setup->resetBinariesUrl();
    setup->resetSegmentsUrl();
    setup->resetOnlineProfilesUrl();
    setup->resetLocalHost();
    setup->resetLocalPort();
    setup->resetLocalBindLocalonly();
    setup->resetLocalProfileDir();
    setup->resetLocalSegmentsDir();
    setup->resetLocalCustomProfileDir();
    setup->resetLocalMaxRunningTime();
    setup->resetLocalNumberThreads();
    setup->resetLocalJavaOpts();
    updateLocalDetails();
}

void CRouterBRouterSetupWizard::slotUpdateCurrentPage()
{
    isError = false;
    switch(currentId())
    {
    case ePageOnlineDetails:
    {
        updateOnlineDetails();
        break;
    }

    case ePageLocalDetails:
    {
        updateLocalDetails();
        break;
    }

    case ePageProfiles:
    {
        updateProfiles();
        break;
    }
    }
}

void CRouterBRouterSetupWizard::slotSetupError(const QString &error, const QString &details)
{
    isError = true;
    this->error = error;
    this->errorDetails = details;

    switch(currentId())
    {
    case ePageOnlineDetails:
    {
        updateOnlineDetails();
        break;
    }

    case ePageLocalDetails:
    {
        updateLocalDetails();
        break;
    }

    case ePageProfiles:
    {
        updateProfiles();
        break;
    }
    }
}
