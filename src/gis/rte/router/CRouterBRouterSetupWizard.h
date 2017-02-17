/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsessr norbert.truchsess@t-online.de

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

#ifndef CROUTERBROUTERSETUPWIZARD_H
#define CROUTERBROUTERSETUPWIZARD_H

#include "CRouterBRouterSetup.h"
#include "ui_IRouterBRouterSetupWizard.h"
#include "tool/IToolShell.h"
#include <QWizard>

class CRouterBRouterSetupWizardToolShell : public IToolShell
{
    Q_OBJECT
public:
    CRouterBRouterSetupWizardToolShell(QTextBrowser *&textBrowser, QWidget * parent);
    virtual ~CRouterBRouterSetupWizardToolShell();

    void out(QString out);
    void execute(QString dir, QString command, QStringList args);
    QProcess::ExitStatus getExitStatus();
    int getExitCode();

private:
    void finished(int exitCode, QProcess::ExitStatus status) override;
    QProcess::ExitStatus exitStatus;
    int exitCode;
};

class CRouterBRouterSetupWizard : public QWizard, private Ui::IRouterBRouterSetupWizard
{
    Q_OBJECT
public:
    CRouterBRouterSetupWizard();
    virtual ~CRouterBRouterSetupWizard();

    int nextId() const override;
    void initializePage(int id) override;
    void cleanupPage(int id) override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void slotCurrentIdChanged(int id);
    void slotRadioLocalClicked();
    void slotRadioOnlineClicked();
    void slotCheckExpertClicked();
    void slotLocalToolSelectDirectory();
    void slotLocalDirectoryEditingFinished();
    void slotLocalDownloadLinkClicked(const QUrl & url);
    void slotLocalDownloadButtonClicked();
    void slotLocalProfileClicked(const QModelIndex & index);
    void slotLocalAvailableProfileClicked(const QModelIndex & index);
    void slotLocalAddProfileClicked();
    void slotLocalDelProfileClicked();
    void slotOnlineProfileClicked(const QModelIndex & index);

private:
    enum { Page_ChooseMode, Page_LocalDirectory, Page_LocalInstallation, Page_LocalProfiles,
               Page_LocalTiles, Page_OnlineUrl, Page_OnlineDetails, Page_OnlineProfiles };

    void initChooseMode();
    void beginChooseMode();
    void cleanupChooseMode();

    void initLocalDirectory();
    void beginLocalDirectory();
    void updateLocalDirectory();
    void cleanupLocalDirectory();

    void initLocalInstall();
    void beginLocalInstall();
    void cleanupLocalInstall();

    void initLocalProfiles();
    void beginLocalProfiles();
    void cleanupLocalProfiles();

    void initLocalTiles();
    void beginLocalTiles();
    void cleanupLocalTiles();

    void initOnlineDetails();
    void beginOnlineDetails();
    void cleanupOnlineDetails();

    void initOnlineProfiles();
    void beginOnlineProfiles();
    void cleanupOnlineProfiles();

    void initOnlineUrl();
    void beginOnlineUrl();
    void cleanupOnlineUrl();

    CRouterBRouterSetup setup;
};

#endif //CROUTERBROUTERSETUPWIZARD_H

