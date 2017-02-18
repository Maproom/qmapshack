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

    void out(const QString out);
    void execute(const QString dir, const QString command, const QStringList args);
    QProcess::ExitStatus getExitStatus();
    const int getExitCode();

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
    void initializePage(const int id) override;
    void cleanupPage(const int id) override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void slotCurrentIdChanged(const int id);
    void slotRadioLocalClicked();
    void slotRadioOnlineClicked();
    void slotCheckExpertClicked();
    void slotLocalToolSelectDirectory();
    void slotLocalDirectoryEditingFinished();
    void slotLocalDownloadLinkClicked(const QUrl & url);
    void slotLocalDownloadButtonClicked();
    void slotProfileClicked(const QModelIndex & index);
    void slotAvailableProfileClicked(const QModelIndex & index);
    void slotAddProfileClicked();
    void slotDelProfileClicked();

private:
    enum { Page_ChooseMode, Page_LocalDirectory, Page_LocalInstallation, Page_Profiles,
               Page_LocalTiles, Page_OnlineUrl, Page_OnlineDetails };

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

    void initProfiles();
    void beginProfiles();
    void cleanupProfiles();

    void initLocalTiles();
    void beginLocalTiles();
    void cleanupLocalTiles();

    void initOnlineDetails();
    void beginOnlineDetails();
    void cleanupOnlineDetails();

    void initOnlineUrl();
    void beginOnlineUrl();
    void cleanupOnlineUrl();

    CRouterBRouterSetup setup;
};

#endif //CROUTERBROUTERSETUPWIZARD_H

