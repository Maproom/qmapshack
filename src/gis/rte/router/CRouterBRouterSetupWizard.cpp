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
#include "CRouterBRouterSetupWizard.h"
#include "CRouterBRouterSetupWizardToolShell.h"
#include "setup/IAppSetup.h"
#include "canvas/CCanvas.h"
#include <proj_api.h>
#include <QtNetwork>
#include <QtWidgets>

CRouterBRouterSetupWizard::CRouterBRouterSetupWizard()
    : QWizard(CMainWindow::getBestWidgetForParent())
{
    setupUi(this);
    connect(this, &CRouterBRouterSetupWizard::currentIdChanged, this, &CRouterBRouterSetupWizard::slotCurrentIdChanged);
    setup.load();
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
        if (setup.expertMode)
        {
            return Page_OnlineUrl;
        }
        if (setup.installMode == CRouterBRouterSetup::ModeLocal)
        {
            return Page_LocalDirectory;
        }
        else
        {
            return Page_Profiles;
        }
        break;
    }
    case Page_LocalDirectory:
    {
        return Page_LocalInstallation;
    }
    case Page_LocalInstallation:
    {
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
        switch(setup.installMode)
        {
        case CRouterBRouterSetup::ModeLocal:
        {
            return Page_LocalDirectory;
        }
        case CRouterBRouterSetup::ModeOnline:
        {
            return Page_OnlineDetails;
        }
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
    case Page_OnlineUrl:
    {
        initOnlineUrl();
        break;
    }
    default:
    {
    }
    }
}

void CRouterBRouterSetupWizard::cleanupPage(const int id)
{
    switch(id)
    {
    case Page_ChooseMode:
    {
        cleanupChooseMode();
        break;
    }
    case Page_LocalDirectory:
    {
        cleanupLocalDirectory();
        break;
    }
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
    case Page_LocalTiles:
    {
        cleanupLocalTiles();
        break;
    }
    case Page_OnlineDetails:
    {
        cleanupOnlineDetails();
        break;
    }
    case Page_OnlineUrl:
    {
        cleanupOnlineUrl();
        break;
    }
    default:
    {
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
    default:
    {
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
}

void CRouterBRouterSetupWizard::slotRadioLocalClicked()
{
    setup.installMode = CRouterBRouterSetup::ModeLocal;
    currentPage()->setFinalPage(false);
}

void CRouterBRouterSetupWizard::slotRadioOnlineClicked()
{
    setup.installMode = CRouterBRouterSetup::ModeOnline;
    currentPage()->setFinalPage(!setup.expertMode);
}

void CRouterBRouterSetupWizard::slotCheckExpertClicked()
{
    setup.expertMode = checkExpert->isChecked();
    currentPage()->setFinalPage(false);
}

void CRouterBRouterSetupWizard::cleanupChooseMode()
{

}

void CRouterBRouterSetupWizard::initLocalDirectory()
{
    connect(toolLocalDir, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalToolSelectDirectory);
    connect(lineLocalDir, &QLineEdit::editingFinished, this, &CRouterBRouterSetupWizard::slotLocalDirectoryEditingFinished);
}

void CRouterBRouterSetupWizard::beginLocalDirectory()
{
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalToolSelectDirectory()
{
    setup.localDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"", QFileDialog::ShowDirsOnly);
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::slotLocalDirectoryEditingFinished()
{
    updateLocalDirectory();
}

void CRouterBRouterSetupWizard::updateLocalDirectory()
{
    lineLocalDir->setText(setup.localDir);
    if (setup.localDir.isEmpty())
    {
        labelLocalDirResult->setText(tr("please select BRouter installation directory"));
    }
    else
    {
        const QDir dir(setup.localDir);
        const QString brouterJarPath = dir.absoluteFilePath("brouter.jar");
        if (QFile(brouterJarPath).exists() and QDir(dir.absoluteFilePath(setup.localProfileDir)).exists())
        {
            labelLocalDirResult->setText(tr("existing BRouter installation"));
        }
        else
        {
            labelLocalDirResult->setText(tr("empty directory, continue with install on next page"));
        }
    }
}

void CRouterBRouterSetupWizard::cleanupLocalDirectory()
{

}

void CRouterBRouterSetupWizard::initLocalInstall()
{
    textLocalInstall->setVisible(false);
    const QUrl brouterUrl(setup.binariesUrl);
    webLocalBRouterVersions->load(brouterUrl);

    QWebPage *localVersionsPage = webLocalBRouterVersions->page();
    localVersionsPage->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    connect(localVersionsPage, &QWebPage::linkClicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadLinkClicked);
    connect(pushLocalInstall, &QPushButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalDownloadButtonClicked);
}

void CRouterBRouterSetupWizard::beginLocalInstall()
{
}

void CRouterBRouterSetupWizard::slotLocalDownloadLinkClicked(const QUrl & url)
{
    labelLocalInstallLink->setText(url.toString());
}

void CRouterBRouterSetupWizard::slotLocalDownloadButtonClicked()
{
    textLocalInstall->setVisible(true);
    CRouterBRouterSetupWizardToolShell shell(textLocalInstall,this);
    shell.out("download " + labelLocalInstallLink->text() + " started");

    QNetworkRequest request;
    request.setUrl(QUrl(labelLocalInstallLink->text()));

    QNetworkAccessManager networkAccessManager;
    QNetworkReply * reply = networkAccessManager.get(request);
    reply->deleteLater();

    QEventLoop eventLoop;
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    QFile outfile(QDir(lineLocalDir->text()).absoluteFilePath("brouter.zip"));
    outfile.open(QIODevice::WriteOnly);
    outfile.write(reply->readAll());
    outfile.close();

    shell.out("download " + labelLocalInstallLink->text() + " finished");

    shell.execute(lineLocalDir->text(),QString("unzip"),QStringList() << "-o" << "brouter.zip");
}

void CRouterBRouterSetupWizard::cleanupLocalInstall()
{

}

void CRouterBRouterSetupWizard::initProfiles()
{
    QStringListModel *profilesModel = new QStringListModel();
    listProfiles->setModel(profilesModel);

    QStringListModel *availableProfiles = new QStringListModel();
    listAvailableProfiles->setModel(availableProfiles);

    setup.loadOnlineConfig();

    connect(listProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotProfileClicked);
    connect(listAvailableProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotAvailableProfileClicked);
    connect(toolAddProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotAddProfileClicked);
    connect(toolDeleteProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotDelProfileClicked);
}

void CRouterBRouterSetupWizard::beginProfiles()
{
    updateProfiles();
}

void CRouterBRouterSetupWizard::slotProfileClicked(const QModelIndex & index)
{
    textProfileContent->setText(setup.getProfileContent(index.row()));
    listAvailableProfiles->clearSelection();
    toolDeleteProfile->setEnabled(true);
    toolAddProfile->setEnabled(false);
}

void CRouterBRouterSetupWizard::slotAvailableProfileClicked(const QModelIndex & index)
{
    const QString profile = listAvailableProfiles->model()->data(index).toString();
    textProfileContent->setText(setup.getOnlineProfileContent(profile));
    listProfiles->clearSelection();
    toolAddProfile->setEnabled(true);
    toolDeleteProfile->setEnabled(false);
}

void CRouterBRouterSetupWizard::slotAddProfileClicked()
{
    const QItemSelectionModel * selectModel = listAvailableProfiles->selectionModel();
    const QModelIndexList selected = selectModel->selectedIndexes();
    if (selected.size() == 1)
    {
        const QString profile = listAvailableProfiles->model()->data(selected.at(0)).toString();
        setup.installOnlineProfile(profile);
        updateProfiles();
    }
}

void CRouterBRouterSetupWizard::slotDelProfileClicked()
{
    const QItemSelectionModel * selectModel = listProfiles->selectionModel();
    const QModelIndexList selected = selectModel->selectedIndexes();
    if (selected.size() == 1)
    {
        const QString filename = setup.getProfileDir().absoluteFilePath(setup.getProfiles().at((selected.at(0).row())) + ".brf");
        QFile(filename).remove();
        updateProfiles();
    }
}

void CRouterBRouterSetupWizard::updateProfiles()
{
    setup.readProfiles();
    QStringList profiles = setup.getProfiles();
    QStringList available;
    for(QString profile:setup.onlineProfilesAvailable)
    {
        if (!profiles.contains(profile))
        {
            available << profile;
        }
    }
    (dynamic_cast<QStringListModel*>(listProfiles->model()))->setStringList(profiles);
    (dynamic_cast<QStringListModel*>(listAvailableProfiles->model()))->setStringList(available);
    toolAddProfile->setEnabled(false);
    toolDeleteProfile->setEnabled(false);
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
}

void CRouterBRouterSetupWizard::cleanupLocalTiles()
{
}

void CRouterBRouterSetupWizard::initOnlineDetails()
{
    setup.loadOnlineConfig();
}

void CRouterBRouterSetupWizard::beginOnlineDetails()
{
    lineOnlineHost->setText(setup.onlineServiceUrl);
    lineOnlineProfileUrl->setText(setup.onlineProfilesUrl);
}

void CRouterBRouterSetupWizard::cleanupOnlineDetails()
{

}

void CRouterBRouterSetupWizard::initOnlineUrl()
{
}

void CRouterBRouterSetupWizard::beginOnlineUrl()
{
    lineOnlineUrl->setText(setup.onlineWebUrl);
}

void CRouterBRouterSetupWizard::cleanupOnlineUrl()
{

}
