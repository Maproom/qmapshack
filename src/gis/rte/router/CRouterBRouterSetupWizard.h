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

#include "CRouterBRouterSetup.h"
#include "CRouterBRouterTilesSelect.h"
#include "ui_IRouterBRouterSetupWizard.h"
#include <QWizard>
#include <canvas/CCanvas.h>

class CRouterBRouterSetupWizard : public QWizard, private Ui::IRouterBRouterSetupWizard
{
    Q_OBJECT
public:
    CRouterBRouterSetupWizard();
    virtual ~CRouterBRouterSetupWizard();

    int nextId() const override;
    void initializePage(const int id) override;
    bool validateCurrentPage() override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void slotCurrentIdChanged(const int id);
    void slotCustomButtonClicked(int id);
    void slotRadioLocalClicked();
    void slotRadioOnlineClicked();
    void slotCheckExpertClicked();
    void slotLocalToolSelectDirectory();
    void slotCreateOrUpdateLocalInstallClicked();
    void slotLocalDirectoryEditingFinished();
    void slotLocalDownloadLinkClicked(const QUrl & url);
    void slotLocalDownloadButtonClicked();
    void slotLocalDownloadButtonFinished(QNetworkReply * reply);
    void slotProfileClicked(const QModelIndex & index);
    void slotAvailableProfileClicked(const QModelIndex & index);
    void slotDisplayProfile(QString profile, const QString content);
    void slotAddProfileClicked();
    void slotDelProfileClicked();
    void slotProfileUpClicked();
    void slotProfileDownClicked();
    void slotOnlineConfigChanged();

private:
    enum { Page_ChooseMode, Page_LocalDirectory, Page_LocalInstallation, Page_Profiles,
               Page_LocalTiles, Page_OnlineUrl, Page_OnlineDetails, Page_LocalDetails };

    void beginChooseMode();
    bool validateChooseMode();

    void beginLocalDirectory();
    void updateLocalDirectory();

    void initLocalInstall();
    void beginLocalInstall();

    void initProfiles();
    void beginProfiles();
    QStringList selectedProfiles(const QListView * listView);
    QList<int> updateProfileView(QListView * listView, QStringList values);
    void updateProfiles();

    void initLocalTiles();
    void beginLocalTiles();

    void beginLocalDetails();
    void updateLocalDetails();
    bool validateLocalDetails();
    void resetLocalDetails();

    void beginOnlineDetails();
    void updateOnlineDetails();
    bool validateOnlineDetails();
    void resetOnlineDetails();

    void beginOnlineUrl();
    bool validateOnlineUrl();
    void resetOnlineUrl();

    CRouterBRouterSetup setup;

    bool localInstallExists;
    bool doLocalInstall;
    QUrl downloadUrl;

    QNetworkAccessManager * networkAccessManager;
};

#endif //CROUTERBROUTERSETUPWIZARD_H

