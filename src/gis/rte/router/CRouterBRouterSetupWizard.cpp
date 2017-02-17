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
#include "gis/rte/router/CRouterBRouterSetupWizard.h"
#include "setup/IAppSetup.h"
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
        if (setup.installMode == CRouterBRouterSetup::Mode_Local)
        {
            return Page_LocalDirectory;
        }
        break;
    }
    case Page_LocalDirectory:
    {
        return Page_LocalInstallation;
    }
    case Page_LocalInstallation:
    {
        return Page_LocalProfiles;
    }
    case Page_LocalProfiles:
    {
        return Page_LocalTiles;
    }
    case Page_LocalTiles:
    {
        break;
    }
    case Page_OnlineDetails:
    {
        return Page_OnlineProfiles;
    }
    case Page_OnlineProfiles:
    {
        break;
    }
    case Page_OnlineUrl:
    {
        switch(setup.installMode)
        {
        case CRouterBRouterSetup::Mode_Local:
        {
            return Page_LocalDirectory;
        }
        case CRouterBRouterSetup::Mode_Online:
        {
            return Page_OnlineDetails;
        }
        }
        break;
    }
    }
    return -1;
}

void CRouterBRouterSetupWizard::initializePage(int id)
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
    case Page_LocalProfiles:
    {
        initLocalProfiles();
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
    case Page_OnlineProfiles:
    {
        initOnlineProfiles();
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

void CRouterBRouterSetupWizard::cleanupPage(int id)
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
    case Page_LocalProfiles:
    {
        cleanupLocalProfiles();
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
    case Page_OnlineProfiles:
    {
        cleanupOnlineProfiles();
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

void CRouterBRouterSetupWizard::slotCurrentIdChanged(int id)
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
    case Page_LocalProfiles:
    {
        beginLocalProfiles();
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
    case Page_OnlineProfiles:
    {
        beginOnlineProfiles();
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
    case CRouterBRouterSetup::Mode_Local:
    {
        radioLocal->setChecked(true);
        radioOnline->setChecked(false);
        break;
    }
    case CRouterBRouterSetup::Mode_Online:
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
    setup.installMode = CRouterBRouterSetup::Mode_Local;
    currentPage()->setFinalPage(false);
}

void CRouterBRouterSetupWizard::slotRadioOnlineClicked()
{
    setup.installMode = CRouterBRouterSetup::Mode_Online;
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
        QDir dir(setup.localDir);
        QString brouterJarPath = dir.absoluteFilePath("brouter.jar");
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

void CRouterBRouterSetupWizard::initLocalProfiles()
{
    setup.readLocalProfiles();

    QStringListModel *localProfilesModel = new QStringListModel(setup.localProfiles);
    listLocalProfiles->setModel(localProfilesModel);

    setup.loadOnlineConfig();
    setup.loadOnlineProfiles();

    QStringListModel *availableProfiles = new QStringListModel(setup.onlineProfiles);

    listLocalAvailableProfiles->setModel(availableProfiles);

    connect(listLocalProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotLocalProfileClicked);
    connect(listLocalAvailableProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotLocalAvailableProfileClicked);
    connect(toolLocalAddProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalAddProfileClicked);
    connect(toolLocalDeleteProfile, &QToolButton::clicked, this, &CRouterBRouterSetupWizard::slotLocalDelProfileClicked);
}

void CRouterBRouterSetupWizard::beginLocalProfiles()
{

}

void CRouterBRouterSetupWizard::slotLocalProfileClicked(const QModelIndex & index)
{
    textLocalProfileContent->setText(setup.readLocalProfile(setup.localProfiles.at(index.row())));
}

void CRouterBRouterSetupWizard::slotLocalAvailableProfileClicked(const QModelIndex & index)
{
    textLocalProfileContent->setText(setup.readOnlineProfile(setup.localProfiles.at(index.row())));
}

void CRouterBRouterSetupWizard::slotLocalAddProfileClicked()
{
    QItemSelectionModel * selectModel = listLocalAvailableProfiles->selectionModel();
    QModelIndexList selected = selectModel->selectedIndexes();
    if (selected.size() == 1)
    {
        QString fileName = setup.onlineProfiles.at(selected.at(0).row()) + ".brf";
        QFile onlineFile(QDir(IAppSetup::getPlatformInstance()->defaultCachePath()).absoluteFilePath(fileName));
        if (onlineFile.exists())
        {
            QFile localFile(setup.getLocalProfileDir().absoluteFilePath(fileName));
            onlineFile.open(QIODevice::ReadOnly);
            localFile.open(QIODevice::WriteOnly);
            localFile.write(onlineFile.readAll());
        }
        setup.readLocalProfiles();
        (dynamic_cast<QStringListModel*>(listLocalProfiles->model()))->setStringList(setup.localProfiles);
    }
}

void CRouterBRouterSetupWizard::slotLocalDelProfileClicked()
{
    QItemSelectionModel * selectModel = listLocalProfiles->selectionModel();
    QModelIndexList selected = selectModel->selectedIndexes();
    if (selected.size() == 1)
    {
        QString filename = setup.getLocalProfileDir().absoluteFilePath(setup.localProfiles.at(selected.at(0).row()) + ".brf");
        QFile(filename).remove();

        setup.readLocalProfiles();
        (dynamic_cast<QStringListModel*>(listLocalProfiles->model()))->setStringList(setup.localProfiles);
    }
}

void CRouterBRouterSetupWizard::cleanupLocalProfiles()
{
    QAbstractItemModel *localProfiles = listLocalProfiles->model();
    if (localProfiles != nullptr)
    {
        delete localProfiles;
        listLocalProfiles->setModel(nullptr);
    }
    QAbstractItemModel *availableProfiles = listLocalAvailableProfiles->model();
    if (availableProfiles != nullptr)
    {
        delete availableProfiles;
        listLocalAvailableProfiles->setModel(nullptr);
    }
}

void CRouterBRouterSetupWizard::initLocalTiles()
{

}

void CRouterBRouterSetupWizard::beginLocalTiles()
{

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

void CRouterBRouterSetupWizard::initOnlineProfiles()
{
    setup.loadOnlineProfiles();
    QStringListModel * profilesModel = new QStringListModel;
    profilesModel->setStringList(setup.onlineProfiles);
    listOnlineProfiles->setModel(profilesModel);

    connect(listOnlineProfiles, &QListView::clicked, this, &CRouterBRouterSetupWizard::slotOnlineProfileClicked);
}

void CRouterBRouterSetupWizard::beginOnlineProfiles()
{
}

void CRouterBRouterSetupWizard::slotOnlineProfileClicked(const QModelIndex & index)
{
    textOnlineProfile->setText(setup.readOnlineProfile(setup.onlineProfiles.at(index.row())));
}

void CRouterBRouterSetupWizard::cleanupOnlineProfiles()
{
    QAbstractItemModel * model = listOnlineProfiles->model();
    if (model!=nullptr)
    {
        delete model;
    }
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

CRouterBRouterSetupWizardToolShell::CRouterBRouterSetupWizardToolShell(QTextBrowser *&textBrowser, QWidget * parent)
    : IToolShell(textBrowser,parent)
{
}

CRouterBRouterSetupWizardToolShell::~CRouterBRouterSetupWizardToolShell()
{
}

void CRouterBRouterSetupWizardToolShell::out(QString out)
{
    stdOut(out);
}

void CRouterBRouterSetupWizardToolShell::execute(QString dir, QString command, QStringList args)
{
    stdOut("cd " + dir);
    stdOut(command+" " + args.join(" ") + "\n");
    cmd.setWorkingDirectory(dir);
    cmd.start(command,args);
    cmd.waitForFinished();
}

void CRouterBRouterSetupWizardToolShell::finished(int exitCode, QProcess::ExitStatus status)
{
    this->exitCode = exitCode;
    this->exitStatus = status;
    if (status == QProcess::ExitStatus::NormalExit)
    {
        text->setTextColor(Qt::darkGreen);
        text->append(tr("!!! done !!!\n"));
    }
    else
    {
        text->setTextColor(Qt::darkRed);
        text->append(tr("!!! failed !!!\n"));
    }
}
