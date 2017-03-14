/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#include <QFileDialog>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "CMainWindow.h"
#include "CRouterBRouterSetup.h"
#include "CRouterBRouterSetupException.h"
#include "CRouterBRouterSetupWizard.h"
#include "CRouterBRouterToolShell.h"
#include "setup/IAppSetup.h"
#include "canvas/CCanvas.h"
#include <proj_api.h>
#include <JlCompress.h>

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

    connect(lineLocalProfilesUrl, &QLineEdit::cursorPositionChanged, this, &CRouterBRouterSetupWizard::slotLocalProfilesUrlCursorPositionChanged);
    connect(lineLocalProfilesUrl, &QLineEdit::editingFinished, this, &CRouterBRouterSetupWizard::slotLocalProfilesUrlEditingFinished);
    connect(toolLocalDir, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectDirectory);
    connect(toolJavaExecutable, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectJava);
    connect(pushLocalFindJava, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalPushFindJava);
    connect(lineLocalDir, &QLineEdit::cursorPositionChanged, this, &CRouterBRouterSetupWizard::slotLocalDirectoryCursonPositionChanged);
    connect(lineJavaExecutable, &QLineEdit::cursorPositionChanged, this, &CRouterBRouterSetupWizard::slotLocalJavaExecutableCursorPositionChanged);

    connect(pushCreateOrUpdateLocalInstall, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotCreateOrUpdateLocalInstallClicked);

    connect(pushLocalInstall, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadButtonClicked);

    connect(listProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotProfileClicked);
    connect(listAvailableProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotAvailableProfileClicked);
    connect(toolAddProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotAddProfileClicked);
    connect(toolDeleteProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotDelProfileClicked);
    connect(toolProfileUp, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotProfileUpClicked);
    connect(toolProfileDown, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotProfileDownClicked);

    connect(lineOnlineUrl, &QLineEdit::cursorPositionChanged, this, &CRouterBRouterSetupWizard::slotOnlineUrlCursorPositionChanged);
    connect(lineOnlineUrl, &QLineEdit::editingFinished, this, &CRouterBRouterSetupWizard::slotOnlineUrlEditingFinished);

    connect(setup, &CRouterBRouterSetup::sigOnlineConfigLoaded, this, &CRouterBRouterSetupWizard::slotOnlineConfigLoaded);
    connect(setup, &CRouterBRouterSetup::sigDisplayOnlineProfileFinished, this, &CRouterBRouterSetupWizard::slotDisplayProfile);
    connect(setup, &CRouterBRouterSetup::sigProfilesChanged, this, &CRouterBRouterSetupWizard::slotOnlineProfilesLoaded);
    connect(setup, &CRouterBRouterSetup::sigError, this, &CRouterBRouterSetupWizard::slotSetupError);

    QStringListModel *profilesModel = new QStringListModel();
    listProfiles->setModel(profilesModel);

    QStringListModel *availableProfiles = new QStringListModel();
    listAvailableProfiles->setModel(availableProfiles);

    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouterSetupWizard::slotLocalDownloadButtonFinished);

    try
    {
        setup->load();
    }
    catch (CRouterBRouterSetupException &e)
    {
        setup->onInvalidSetup();
    }
}

CRouterBRouterSetupWizard::~CRouterBRouterSetupWizard()
{
}

int CRouterBRouterSetupWizard::nextId() const
{
    switch(currentId())
    {
    case Page_ChooseMode:
    {
        if (setup->installMode == CRouterBRouterSetup::ModeLocal)
        {
            return Page_LocalDirectory;
        }
        if (setup->expertMode) //ModeOnline
        {
            return Page_OnlineUrl;
        }
        return Page_Profiles;
    }
    case Page_LocalDirectory:
    {
        if (setup->expertMode)
        {
            return Page_OnlineUrl;
        }
        if (doLocalInstall)
        {
            return Page_LocalInstallation;
        }
        return Page_Profiles;
    }
    case Page_LocalInstallation:
    {
        if (setup->expertMode)
        {
            return Page_LocalDetails;
        }
        return Page_Profiles;
    }
    case Page_Profiles:
    {
        if (setup->installMode == CRouterBRouterSetup::ModeLocal)
        {
            return Page_LocalTiles;
        }
        break;
    }
    case Page_LocalTiles:
    {
        break;
    }
    case Page_OnlineDetails:
    {
        return Page_Profiles;
    }
    case Page_OnlineUrl:
    {
        if (setup->installMode == CRouterBRouterSetup::ModeLocal)
        {
            if (doLocalInstall)
            {
                return Page_LocalInstallation;
            }
            if (setup->expertMode)
            {
                return Page_LocalDetails;
            }
            return Page_Profiles;
        }
        else if (setup->installMode == CRouterBRouterSetup::ModeOnline)
        {
            return Page_OnlineDetails;
        }
        break;
    }
    case Page_LocalDetails:
    {
        return Page_Profiles;
    }
    }
    return -1;
}

void CRouterBRouterSetupWizard::initializePage(const int id)
{
    switch(id)
    {
    case Page_LocalDirectory:
    {
        initLocalDirectory();
        break;
    }
    case Page_LocalInstallation:
    {
        initLocalInstall();
        break;
    }
    case Page_LocalTiles:
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
    case Page_OnlineDetails:
    {
        return validateOnlineDetails();
    }
    case Page_LocalDetails:
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
    case Page_ChooseMode:
    {
        beginChooseMode();
        break;
    }
    case Page_LocalDirectory:
    {
        beginLocalDirectory();
        break;
    }
    case Page_LocalInstallation:
    {
        beginLocalInstall();
        break;
    }
    case Page_Profiles:
    {
        beginProfiles();
        break;
    }
    case Page_LocalTiles:
    {
        beginLocalTiles();
        break;
    }
    case Page_OnlineDetails:
    {
        beginOnlineDetails();
        break;
    }
    case Page_OnlineUrl:
    {
        beginOnlineUrl();
        break;
    }
    case Page_LocalDetails:
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
        case Page_OnlineDetails:
        {
            resetOnlineDetails();
            break;
        }
        case Page_OnlineUrl:
        {
            resetOnlineUrl();
            break;
        }
        case Page_LocalDetails:
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
    case CRouterBRouterSetup::ModeLocal:
    {
        radioLocal->setChecked(true);
        radioOnline->setChecked(false);
        break;
    }
    case CRouterBRouterSetup::ModeOnline:
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
    setup->installMode = CRouterBRouterSetup::ModeLocal;
}

void CRouterBRouterSetupWizard::slotRadioOnlineClicked() const
{
    setup->installMode = CRouterBRouterSetup::ModeOnline;
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
    setup->localDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"", QFileDialog::ShowDirsOnly);
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

void CRouterBRouterSetupWizard::slotLocalDirectoryCursonPositionChanged() const
{
    setup->localDir = lineLocalDir->text();
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalJavaExecutableCursorPositionChanged() const
{
    setup->localJavaExecutable = lineJavaExecutable->text();
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::updateLocalDirectory() const
{
    textLocalDirectory->setVisible(false);
    lineLocalDir->setText(setup->localDir);
    lineJavaExecutable->setText(setup->localJavaExecutable);
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
            labelLocalJavaResult->setText("seems to be a valid Java-executable");
        }
        else
        {
            labelLocalJavaResult->setText("doesn't seem to be a valid Java-executable");
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
    connect(webLocalBRouterVersions, &QWebView::loadFinished, this, &CRouterBRouterSetupWizard::slotWebLocalBRouterVersionsLoadFinished);
    webLocalBRouterVersions->load(QUrl(setup->binariesUrl));
    QWebPage *localVersionsPage = webLocalBRouterVersions->page();
    localVersionsPage->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(localVersionsPage, &QWebPage::linkClicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadLinkClicked);
}

void CRouterBRouterSetupWizard::slotWebLocalBRouterVersionsLoadFinished(bool ok)
{
    if (!ok)
    {
        textLocalInstall->setVisible(true);
        textLocalInstall->setTextColor(Qt::red);
        textLocalInstall->append(tr("Error loading installation-page at %1").arg(setup->binariesUrl));
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
    textLocalInstall->setVisible(true);
    textLocalInstall->setTextColor(Qt::darkGreen);
    textLocalInstall->append(tr("download %1 started").arg(downloadUrl.toString()));
    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(downloadUrl));
    reply->setProperty("fileName",downloadUrl.fileName());
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
            const QStringList &unzippedNames = JlCompress::extractDir(outfile.fileName(),setup->localDir);
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
    if (setup->installMode == CRouterBRouterSetup::ModeLocal)
    {
        textProfileContent->setText(tr("retrieving available profiles from %1").arg(setup->onlineProfilesUrl));
        setup->loadLocalOnlineProfiles();
        setup->readLocalProfiles();
    }
    else
    {
        Q_ASSERT(setup->installMode == CRouterBRouterSetup::ModeOnline);
        textProfileContent->setText(tr("retrieving available profiles from %1").arg(setup->onlineWebUrl));
        setup->loadOnlineConfig();
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

void CRouterBRouterSetupWizard::slotAddProfileClicked()
{
    for (QString profile : selectedProfiles(listAvailableProfiles))
    {
        setup->addProfile(profile);
    }
}

void CRouterBRouterSetupWizard::slotDelProfileClicked()
{
    for (QString profile : selectedProfiles(listProfiles))
    {
        setup->deleteProfile(profile);
    }
}

void CRouterBRouterSetupWizard::slotProfileUpClicked()
{
    for (QString profile : selectedProfiles(listProfiles))
    {
        setup->profileUp(profile);
    }
}

void CRouterBRouterSetupWizard::slotProfileDownClicked()
{
    for (QString profile : selectedProfiles(listProfiles))
    {
        setup->profileDown(profile);
    }
}

void CRouterBRouterSetupWizard::slotOnlineProfilesLoaded()
{
    isError = false;
    switch(currentId())
    {
    case Page_Profiles:
    {
        updateProfiles();
        break;
    }
    case Page_LocalDetails:
    {
        updateLocalDetails();
        break;
    }
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
    qSort(selected.begin(),selected.end());
    toolDeleteProfile->setEnabled(!selected.isEmpty());
    toolProfileUp->setEnabled(!selected.isEmpty() && selected.first() > 0);
    toolProfileDown->setEnabled(!selected.isEmpty() && selected.last() < profiles.size()-1);
    if (isError)
    {
        toolAddProfile->setEnabled(false);
        labelProfileContent->setText(tr("Error:"));
        textProfileContent->setText(error + ": "+ errorDetails);
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
    updateOnlineDetails();
}

void CRouterBRouterSetupWizard::updateOnlineDetails() const
{
    lineOnlineProfileUrl->setText(setup->onlineProfilesUrl);
    lineOnlineService->setText(setup->onlineServiceUrl);
}

bool CRouterBRouterSetupWizard::validateOnlineDetails() const
{
    setup->onlineProfilesUrl = lineOnlineProfileUrl->text();
    setup->onlineServiceUrl = lineOnlineService->text();
    return true;
}

void CRouterBRouterSetupWizard::resetOnlineDetails() const
{
    setup->resetOnlineProfilesUrl();
    setup->resetOnlineServiceUrl();

    updateOnlineDetails();
}

void CRouterBRouterSetupWizard::beginOnlineUrl()
{
    setOption(QWizard::HaveCustomButton1, true);
    isError = false;
    pageOnlineUrl->setComplete(false);
    setup->loadOnlineConfig();
}

void CRouterBRouterSetupWizard::slotOnlineUrlCursorPositionChanged()
{
    pageOnlineUrl->setComplete(false);
}

void CRouterBRouterSetupWizard::slotOnlineUrlEditingFinished()
{
    setup->onlineWebUrl = lineOnlineUrl->text();
    isError = false;
    setup->loadOnlineConfig();
}

void CRouterBRouterSetupWizard::updateOnlineUrl()
{
    lineOnlineUrl->setText(setup->onlineWebUrl);
    textOnlineUrl->setVisible(isError);
    if (isError)
    {
        textOnlineUrl->setText(error + ": "+ errorDetails);
    }
    else
    {
        pageOnlineUrl->setComplete(true);
    }
}

void CRouterBRouterSetupWizard::resetOnlineUrl()
{
    setup->resetOnlineWebUrl();
    beginOnlineUrl();
}

void CRouterBRouterSetupWizard::updateLocalDetails() const
{
    lineLocalProfilesUrl->setText(setup->onlineProfilesUrl);
    lineLocalHost->setText(setup->localHost);
    lineLocalPort->setText(setup->localPort);
    lineLocalProfiles->setText(setup->localProfileDir);
    lineLocalSegments->setText(setup->localSegmentsDir);
    lineLocalCustomProfiles->setText(setup->localCustomProfileDir);
    lineLocalMaxRuntime->setText(setup->localMaxRunningTime);
    lineLocalNumberThreads->setText(setup->localNumberThreads);
    lineLocalJavaOpts->setText(setup->localJavaOpts);
    textLocalDetails->setVisible(isError);
    pageLocalDetails->setComplete(!isError);
    if (isError)
    {
        textLocalDetails->setText(error + ": " + errorDetails);
    }
}

void CRouterBRouterSetupWizard::beginLocalDetails()
{
    setOption(QWizard::HaveCustomButton1, true);
    isError = false;
    setup->loadLocalOnlineProfiles();
}

void CRouterBRouterSetupWizard::slotLocalProfilesUrlCursorPositionChanged() const
{
    pageLocalDetails->setComplete(false);
}

void CRouterBRouterSetupWizard::slotLocalProfilesUrlEditingFinished()
{
    setup->onlineProfilesUrl = lineLocalProfilesUrl->text();
    isError = false;
    setup->loadLocalOnlineProfiles();
}

bool CRouterBRouterSetupWizard::validateLocalDetails() const
{
    setup->localHost = lineLocalHost->text();
    setup->localPort = lineLocalPort->text();
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
    setup->resetOnlineProfilesUrl();
    setup->resetLocalHost();
    setup->resetLocalPort();
    setup->resetLocalProfileDir();
    setup->resetLocalSegmentsDir();
    setup->resetLocalCustomProfileDir();
    setup->resetLocalMaxRunningTime();
    setup->resetLocalNumberThreads();
    setup->resetLocalJavaOpts();
    updateLocalDetails();
}

void CRouterBRouterSetupWizard::slotOnlineConfigLoaded()
{
    isError = false;
    switch(currentId())
    {
    case Page_OnlineUrl:
    {
        updateOnlineUrl();
        break;
    }
    case Page_OnlineDetails:
    {
        updateOnlineDetails();
        break;
    }
    case Page_LocalDetails:
    {
        updateLocalDetails();
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
    case Page_OnlineUrl:
    {
        updateOnlineUrl();
        break;
    }
    case Page_LocalDetails:
    {
        updateLocalDetails();
        break;
    }
    case Page_Profiles:
    {
        updateProfiles();
        break;
    }
    }
}
