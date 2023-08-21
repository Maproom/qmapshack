/**********************************************************************************************
    Copyright (C) 2023 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CROUTERBROUTERDOWNLOADPAGE_H
#define CROUTERBROUTERDOWNLOADPAGE_H

#include <QLabel>
#include <QListView>
#include <QNetworkAccessManager>
#include <QStringListModel>
#include <QTextBrowser>
#include <QWizardPage>

class CRouterBRouterSetup;

struct CRouterBRouterReleaseItem {
 public:
  CRouterBRouterReleaseItem(const QString& _name, const QString& _description, const QString& _url);

 private:
  QString name;
  QString description;
  QString url;

  friend class CRouterBRouterDownloadPage;
};

class CRouterBRouterDownloadPage : public QWizardPage {
  Q_OBJECT
 public:
  CRouterBRouterDownloadPage();
  virtual ~CRouterBRouterDownloadPage();

  bool isComplete() const override;

  void initialize(QListView* listVersions, QTextBrowser* textVersionDetails, QLabel* labelInstallLink,
                  QPushButton* pushInstall, QTextBrowser* textLocalInstall, CRouterBRouterSetup* setup);
  void begin();

 signals:
  void sigBRouterJarChanged(const QString& fileName);

 private slots:
  void slotListVersionsClicked(const QModelIndex& index);
  void slotNetworkAccessManagerReplyReceived(QNetworkReply* reply);
  void slotLocalDownloadButtonClicked() const;

 private:
  enum { eRequestVersions, eRequestDownload };

  void updateModel() const;
  void localBRouterVersionsLoadFinished(QNetworkReply* reply);
  void localBRouterDownloadFinished(QNetworkReply* reply);
  void setComplete(bool newComplete);

  QList<CRouterBRouterReleaseItem> releaseItems;
  QStringListModel* model;
  QNetworkAccessManager* networkAccessManager;
  QListView* listVersions = nullptr;
  QTextBrowser* textVersionDetails = nullptr;
  QLabel* labelInstallLink = nullptr;
  QPushButton* pushInstall = nullptr;
  QTextBrowser* textLocalInstall = nullptr;
  CRouterBRouterSetup* setup = nullptr;
  QUrl downloadUrl;
  bool complete;

  friend class CRouterBRouterSetupWizard;
};

#endif  // CROUTERBROUTERDOWNLOADPAGE_H
