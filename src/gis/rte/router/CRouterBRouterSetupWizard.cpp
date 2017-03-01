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

#include "CMainWindow.h"
#include "CRouterBRouterSetupException.h"
#include "CRouterBRouterSetupWizard.h"
#include "CRouterBRouterToolShell.h"
#include "setup/IAppSetup.h"
#include "canvas/CCanvas.h"
#include <proj_api.h>
#include <QtNetwork>
#include <QtWidgets>

CRouterBRouterSetupWizard::CRouterBRouterSetupWizard()
    : QWizard(CMainWindow::getBestWidgetForParent())
{
    setupUi(this);
    setButtonText(QWizard::CustomButton1, tr("Restore Default Values"));
    connect(this, &CRouterBRouterSetupWizard::currentIdChanged, this, &CRouterBRouterSetupWizard::slotCurrentIdChanged);
    connect(this, &CRouterBRouterSetupWizard::customButtonClicked, this, &CRouterBRouterSetupWizard::slotCustomButtonClicked);
    setup.load();
    localInstallExists = false;
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
        if (setup.installMode == CRouterBRouterSetup::ModeLocal)
        {
            return Page_LocalDirectory;
        }
        if (setup.expertMode) //ModeOnline
        {
            return Page_OnlineUrl;
        }
        return Page_Profiles;
    }
    case Page_LocalDirectory:
    {
        if (setup.expertMode)
        {
            return Page_OnlineUrl;
        }
        if (doLocalInstall)
        {
            return Page_LocalInstallation;
        }
        if (localInstallExists)
        {
            return Page_Profiles;
        }
        break;
    }
    case Page_LocalInstallation:
    {
        if (setup.expertMode)
        {
            return Page_OnlineDetails;
        }
        return Page_Profiles;
    }
    case Page_Profiles:
    {
        if (setup.installMode == CRouterBRouterSetup::ModeLocal)
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
        if (setup.installMode == CRouterBRouterSetup::ModeLocal)
        {
            if (doLocalInstall)
            {
                return Page_LocalInstallation;
            }
            if (localInstallExists)
            {
                if (setup.expertMode)
                {
                    return Page_OnlineDetails;
                }
                return Page_Profiles;
            }
        }
        else if (setup.installMode == CRouterBRouterSetup::ModeOnline)
        {
            return Page_OnlineDetails;
        }
        break;
    }
    }
    return -1;
}

void CRouterBRouterSetupWizard::initializePage(const int id)
{
    switch(id)
    {
    case Page_ChooseMode:
    {
        initChooseMode();
        break;
    }
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
    case Page_Profiles:
    {
        initProfiles();
        break;
    }
    case Page_LocalTiles:
    {
        initLocalTiles();
        break;
    }
    case Page_OnlineDetails:
    {
        initOnlineDetails();
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
    case Page_OnlineUrl:
    {
        return validateOnlineUrl();
    }
    }
    return true;
}

void CRouterBRouterSetupWizard::cleanupPage(const int id)
{
    switch(id)
    {
    case Page_LocalInstallation:
    {
        cleanupLocalInstall();
        break;
    }
    case Page_Profiles:
    {
        cleanupProfiles();
        break;
    }
    }
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
        }
    }
}

void CRouterBRouterSetupWizard::accept()
{
    setup.save();
    QDialog::accept();
}

void CRouterBRouterSetupWizard::reject()
{
    QDialog::reject();
}

void CRouterBRouterSetupWizard::initChooseMode()
{
    connect(radioLocal,  &QRadioButton::clicked, this, &CRouterBRouterSetupWizard::slotRadioLocalClicked);
    connect(radioOnline, &QRadioButton::clicked, this, &CRouterBRouterSetupWizard::slotRadioOnlineClicked);
    connect(checkExpert, &QCheckBox::clicked,    this, &CRouterBRouterSetupWizard::slotCheckExpertClicked);
}

void CRouterBRouterSetupWizard::beginChooseMode()
{
    switch(setup.installMode)
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
    checkExpert->setChecked(setup.expertMode);
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotRadioLocalClicked()
{
    setup.installMode = CRouterBRouterSetup::ModeLocal;
}

void CRouterBRouterSetupWizard::slotRadioOnlineClicked()
{
    setup.installMode = CRouterBRouterSetup::ModeOnline;
}

void CRouterBRouterSetupWizard::slotCheckExpertClicked()
{
    setup.expertMode = checkExpert->isChecked();
}

void CRouterBRouterSetupWizard::initLocalDirectory()
{
    connect(toolLocalDir, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectDirectory);
    connect(lineLocalDir, &QLineEdit::editingFinished, this, &CRouterBRouterSetupWizard::slotLocalDirectoryEditingFinished);
    connect(pushCreateOrUpdateLocalInstall, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotCreateOrUpdateLocalInstallClicked);
}

void CRouterBRouterSetupWizard::beginLocalDirectory()
{
    updateLocalDirectory();
    doLocalInstall = false;
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotLocalToolSelectDirectory()
{
    setup.localDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"", QFileDialog::ShowDirsOnly);
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalDirectoryEditingFinished()
{
    setup.localDir = lineLocalDir->text();
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::updateLocalDirectory()
{
    lineLocalDir->setText(setup.localDir);
    if (setup.localDir.isEmpty())
    {
        labelLocalDirResult->setText(tr("please select BRouter installation directory"));
        pushCreateOrUpdateLocalInstall->setVisible(false);
        localInstallExists = false;
    }
    else if(!QDir(setup.localDir).exists())
    {
        labelLocalDirResult->setText(tr("selected directory does not exist"));
        pushCreateOrUpdateLocalInstall->setText(tr("create directory and install BRouter there"));
        pushCreateOrUpdateLocalInstall->setVisible(true);
        localInstallExists = false;
    }
    else
    {
        if (setup.isLocalBRouterInstalled())
        {
            labelLocalDirResult->setText(tr("existing BRouter installation"));
            pushCreateOrUpdateLocalInstall->setText(tr("update existing BRouter installation"));
            pushCreateOrUpdateLocalInstall->setVisible(true);
            localInstallExists = true;
        }
        else
        {
            labelLocalDirResult->setText(tr("empty directory, create new BRouter installation here"));
            pushCreateOrUpdateLocalInstall->setText(tr("create new BRouter installation"));
            pushCreateOrUpdateLocalInstall->setVisible(true);
            localInstallExists = false;
        }
    }
}

void CRouterBRouterSetupWizard::slotCreateOrUpdateLocalInstallClicked()
{
    doLocalInstall = true;
    next();
}

void CRouterBRouterSetupWizard::initLocalInstall()
{
    const QUrl brouterUrl(setup.binariesUrl);
    webLocalBRouterVersions->load(brouterUrl);

    QWebPage *localVersionsPage = webLocalBRouterVersions->page();
    localVersionsPage->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    networkAccessManager = new QNetworkAccessManager(this);

    connect(localVersionsPage, &QWebPage::linkClicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadLinkClicked);
    connect(pushLocalInstall, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadButtonClicked);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouterSetupWizard::slotLocalDownloadButtonFinished);
}

void CRouterBRouterSetupWizard::beginLocalInstall()
{
    doLocalInstall = false;
    textLocalInstall->setVisible(false);
    textLocalInstall->clear();
    labelLocalInstallLink->setText("no brouter-version to install selected");
    pushLocalInstall->setEnabled(false);
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotLocalDownloadLinkClicked(const QUrl & url)
{
    downloadUrl = url;
    labelLocalInstallLink->setText("selected " + url.fileName() + " for download and installation");
    pushLocalInstall->setEnabled(true);
}

void CRouterBRouterSetupWizard::slotLocalDownloadButtonClicked()
{
    textLocalInstall->setVisible(true);
    CRouterBRouterToolShell shell(textLocalInstall,this);
    shell.out("download " + downloadUrl.toString() + " started");

    QNetworkRequest request;
    request.setUrl(downloadUrl);

    QNetworkReply * reply = networkAccessManager->get(request);
    reply->setProperty("fileName",downloadUrl.fileName());
}

void CRouterBRouterSetupWizard::slotLocalDownloadButtonFinished(QNetworkReply * reply)
{
    reply->deleteLater();
    QString fileName = reply->property("fileName").toString();
    QDir outDir(setup.localDir);
    if (!outDir.exists())
    {
        outDir.mkpath(outDir.absolutePath());
    }
    QFile outfile(outDir.absoluteFilePath(fileName));
    outfile.open(QIODevice::WriteOnly);
    outfile.write(reply->readAll());
    outfile.close();

    CRouterBRouterToolShell shell(textLocalInstall,this);
    shell.out("download " + outfile.fileName() + " finished");
    shell.execute(lineLocalDir->text(),QString("unzip"),QStringList() << "-o" << fileName);
    setup.updateLocalProfiles();
}

void CRouterBRouterSetupWizard::cleanupLocalInstall()
{
    delete networkAccessManager;
}

void CRouterBRouterSetupWizard::initProfiles()
{
    QStringListModel *profilesModel = new QStringListModel();
    listProfiles->setModel(profilesModel);

    QStringListModel *availableProfiles = new QStringListModel();
    listAvailableProfiles->setModel(availableProfiles);

    connect(listProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotProfileClicked);
    connect(listAvailableProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotAvailableProfileClicked);
    connect(toolAddProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotAddProfileClicked);
    connect(toolDeleteProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotDelProfileClicked);
    connect(toolProfileUp, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotProfileUpClicked);
    connect(toolProfileDown, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotProfileDownClicked);
    connect(&setup, &CRouterBRouterSetup::displayOnlineProfileFinished, this, &CRouterBRouterSetupWizard::slotDisplayProfile);
    connect(&setup, &CRouterBRouterSetup::profilesChanged, this, &CRouterBRouterSetupWizard::updateProfiles);

    setup.loadOnlineConfig();
}

void CRouterBRouterSetupWizard::beginProfiles()
{
    updateProfiles();
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::slotProfileClicked(const QModelIndex & index)
{
    const QString profile = listProfiles->model()->data(index).toString();
    setup.displayProfileAsync(profile);
    listAvailableProfiles->clearSelection();
    updateProfiles();
}

void CRouterBRouterSetupWizard::slotAvailableProfileClicked(const QModelIndex & index)
{
    const QString profile = listAvailableProfiles->model()->data(index).toString();
    setup.displayOnlineProfileAsync(profile);
    listProfiles->clearSelection();
    updateProfiles();
}

void CRouterBRouterSetupWizard::slotDisplayProfile(QString profile, const QString content)
{
    textProfileContent->setText(content);
}

void CRouterBRouterSetupWizard::slotAddProfileClicked()
{
    for (QString profile : selectedProfiles(listAvailableProfiles))
    {
        setup.addProfile(profile);
    }
}

void CRouterBRouterSetupWizard::slotDelProfileClicked()
{
    for (QString profile : selectedProfiles(listProfiles))
    {
        setup.deleteProfile(profile);
    }
}

void CRouterBRouterSetupWizard::slotProfileUpClicked()
{
    for (QString profile : selectedProfiles(listProfiles))
    {
        setup.profileUp(profile);
    }
}

void CRouterBRouterSetupWizard::slotProfileDownClicked()
{
    for (QString profile : selectedProfiles(listProfiles))
    {
        setup.profileDown(profile);
    }
}

void CRouterBRouterSetupWizard::updateProfiles()
{
    QStringList profiles = setup.getProfiles();
    QStringList available;
    for(QString profile:setup.onlineProfilesAvailable)
    {
        if (!profiles.contains(profile))
        {
            available << profile;
        }
    }

    QList<int> selected = updateProfileView(listProfiles, profiles);
    qSort(selected.begin(),selected.end());
    toolDeleteProfile->setEnabled(!selected.isEmpty());
    toolProfileUp->setEnabled(!selected.isEmpty() and selected.first() > 0);
    toolProfileDown->setEnabled(!selected.isEmpty() and selected.last() < profiles.size()-1);
    toolAddProfile->setEnabled(!updateProfileView(listAvailableProfiles, available).isEmpty());
}

QStringList CRouterBRouterSetupWizard::selectedProfiles(const QListView * listView)
{
    const QItemSelectionModel * selectModel = listView->selectionModel();
    const QModelIndexList selected = selectModel->selectedIndexes();
    const QAbstractItemModel * model = listView->model();
    QStringList selectedList;
    for (int i = 0; i < selected.size(); i++)
    {
        selectedList << model->data(selected.at(i)).toString();
    }
    return selectedList;
}

QList<int> CRouterBRouterSetupWizard::updateProfileView(QListView * listView, QStringList values)
{
    QList<int> selected;
    QStringList selectedValues = selectedProfiles(listView);
    QStringListModel * listModel = (dynamic_cast<QStringListModel*>(listView->model()));
    QItemSelectionModel * selectModel = listView->selectionModel();
    listModel->setStringList(values);
    for (QString value : selectedValues)
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

void CRouterBRouterSetupWizard::cleanupProfiles()
{
    const QAbstractItemModel *localProfiles = listProfiles->model();
    if (localProfiles != nullptr)
    {
        delete localProfiles;
        listProfiles->setModel(nullptr);
    }
    const QAbstractItemModel *availableProfiles = listAvailableProfiles->model();
    if (availableProfiles != nullptr)
    {
        delete availableProfiles;
        listAvailableProfiles->setModel(nullptr);
    }
}

void CRouterBRouterSetupWizard::initLocalTiles()
{
    widgetLocalTilesSelect->setSetup(&setup);
}

void CRouterBRouterSetupWizard::beginLocalTiles()
{
    widgetLocalTilesSelect->initialize();
    setOption(QWizard::HaveCustomButton1, false);
}

void CRouterBRouterSetupWizard::initOnlineDetails()
{
    connect(&setup, &CRouterBRouterSetup::onlineConfigChanged, this, &CRouterBRouterSetupWizard::beginOnlineDetails);
    setup.loadOnlineConfig();
}

void CRouterBRouterSetupWizard::beginOnlineDetails()
{
    labelOnlineProfileUrl->setText(tr("Profile-Url"));
    lineOnlineProfileUrl->setText(setup.onlineProfilesUrl);
    if (setup.installMode == CRouterBRouterSetup::ModeLocal)
    {
        labelOnlineHost->setText(tr("Hostname"));
        labelLocalPort->setText(tr("Portnummer"));
        lineOnlineHost->setText(setup.localHost);
        lineLocalPort->setText(setup.localPort);
        labelLocalPort->setVisible(true);
        lineLocalPort->setVisible(true);
    }
    else if (setup.installMode == CRouterBRouterSetup::ModeOnline)
    {
        labelOnlineHost->setText(tr("Service-Url"));
        lineOnlineHost->setText(setup.onlineServiceUrl);
        labelLocalPort->setVisible(false);
        lineLocalPort->setVisible(false);
    }
    else
    {
        throw new CRouterBRouterSetupException();
    }
    setOption(QWizard::HaveCustomButton1, true);
}

bool CRouterBRouterSetupWizard::validateOnlineDetails()
{
    setup.onlineProfilesUrl = lineOnlineProfileUrl->text();

    if (setup.installMode == CRouterBRouterSetup::ModeLocal)
    {
        setup.localHost = lineOnlineHost->text();
        setup.localPort = lineLocalPort->text();
    }
    else if (setup.installMode == CRouterBRouterSetup::ModeOnline)
    {
        setup.onlineServiceUrl = lineOnlineHost->text();
    }
    else
    {
        throw new CRouterBRouterSetupException();
    }
    return true;
}

void CRouterBRouterSetupWizard::resetOnlineDetails()
{
    if (setup.installMode == CRouterBRouterSetup::ModeLocal)
    {
        setup.resetLocalHost();
        setup.resetLocalPort();
        setup.resetOnlineServiceUrl();
    }
    else if (setup.installMode == CRouterBRouterSetup::ModeOnline)
    {
        setup.resetOnlineProfilesUrl();
        setup.resetOnlineServiceUrl();
    }
    else
    {
        throw new CRouterBRouterSetupException();
    }
    beginOnlineDetails();
}

void CRouterBRouterSetupWizard::beginOnlineUrl()
{
    lineOnlineUrl->setText(setup.onlineWebUrl);
    setOption(QWizard::HaveCustomButton1, true);
}

bool CRouterBRouterSetupWizard::validateOnlineUrl()
{
    setup.onlineWebUrl = lineOnlineUrl->text();
    return true;
}

void CRouterBRouterSetupWizard::resetOnlineUrl()
{
    setup.resetOnlineWebUrl();
    beginOnlineUrl();
}
