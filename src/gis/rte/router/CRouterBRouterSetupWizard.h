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

#ifndef CROUTERBROUTERSETUPWIZARD_H
#define CROUTERBROUTERSETUPWIZARD_H

#include "ui_IRouterBRouterSetupWizard.h"

class CCanvas;
class CRouterBRouterSetup;
class CRouterBRouterTilesSelect;
class CRouterBRouterToolShell;

class CRouterBRouterSetupWizard : public QWizard, private Ui::IRouterBRouterSetupWizard
{
    Q_OBJECT
public:
    CRouterBRouterSetupWizard();
    virtual ~CRouterBRouterSetupWizard();

    int nextId() const override;
    void initializePage(int id) override;
    bool validateCurrentPage() override;

    enum { Page_ChooseMode, Page_LocalDirectory, Page_LocalInstallation, Page_Profiles,
               Page_LocalTiles, Page_OnlineUrl, Page_OnlineDetails, Page_LocalDetails };

public slots:
    void accept() override;
    void reject() override;

private slots:
    void slotCurrentIdChanged(const int id);
    void slotCustomButtonClicked(int id);
    void slotRadioLocalClicked() const;
    void slotRadioOnlineClicked() const;
    void slotCheckExpertClicked() const;
    void slotLocalToolSelectDirectory();
    void slotLocalToolSelectJava();
    void slotLocalPushFindJava() const;
    void slotCreateOrUpdateLocalInstallClicked();
    void slotLocalDirectoryEditingFinished() const;
    void slotLocalJavaExecutableFinished() const;
    void slotWebLocalBRouterVersionsLoadFinished(bool ok);
    void slotLocalDownloadLinkClicked(const QUrl & url);
    void slotLocalDownloadButtonClicked();
    void slotLocalDownloadButtonFinished(QNetworkReply * reply);
    void slotProfileClicked(const QModelIndex & index) const;
    void slotAvailableProfileClicked(const QModelIndex & index) const;
    void slotDisplayProfile(const QString &profile, const QString content);
    void slotAddProfileClicked();
    void slotDelProfileClicked();
    void slotProfileUpClicked();
    void slotProfileDownClicked();
    void slotOnlineConfigChanged();
    void slotSetupError(const QString &error, const QString &details);
    void slotProfilesChanged();

private:
    void beginChooseMode();
    bool validateChooseMode() const;

    void initLocalDirectory();
    void beginLocalDirectory();
    void updateLocalDirectory() const;

    void initLocalInstall();
    void beginLocalInstall();

    void initProfiles() const;
    void beginProfiles();
    void updateProfiles() const;
    QStringList selectedProfiles(const QListView * listView) const;
    QList<int> updateProfileView(QListView * listView, const QStringList &values) const;

    void initLocalTiles() const;
    void beginLocalTiles();

    void beginLocalDetails();
    void updateLocalDetails() const;
    bool validateLocalDetails() const;
    void resetLocalDetails() const;

    void beginOnlineDetails();
    void updateOnlineDetails() const;
    bool validateOnlineDetails() const;
    void resetOnlineDetails() const;

    void beginOnlineUrl();
    bool validateOnlineUrl() const;
    void resetOnlineUrl();

    CRouterBRouterSetup * setup;

    bool doLocalInstall;
    QUrl downloadUrl;

    QNetworkAccessManager * networkAccessManager;

    bool isError { false };
    QString error;
    QString errorDetails;
};

#endif //CROUTERBROUTERSETUPWIZARD_H

