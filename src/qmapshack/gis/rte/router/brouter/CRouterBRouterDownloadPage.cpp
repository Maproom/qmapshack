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

#include "gis/rte/router/brouter/CRouterBRouterDownloadPage.h"

#include <JlCompress.h>

#include <QCheckBox>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>

#include "gis/rte/router/brouter/CRouterBRouterSetup.h"

CRouterBRouterReleaseItem::CRouterBRouterReleaseItem(const QString& _name, const QString& _description,
                                                     const QString& _url)
    : name(_name), description(_description), url(_url) {}

CRouterBRouterDownloadPage::CRouterBRouterDownloadPage() : QWizardPage() {
  networkAccessManager = new QNetworkAccessManager(this);
  networkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
  connect(networkAccessManager, &QNetworkAccessManager::finished, this,
          &CRouterBRouterDownloadPage::slotNetworkAccessManagerReplyReceived);
  model = new QStringListModel(this);
}

CRouterBRouterDownloadPage::~CRouterBRouterDownloadPage() {}

void CRouterBRouterDownloadPage::initialize(QListView* listVersions, QTextBrowser* textVersionDetails,
                                            QLabel* labelInstallLink, QPushButton* pushInstall,
                                            QTextBrowser* textLocalInstall, CRouterBRouterSetup* setup) {
  this->listVersions = listVersions;
  this->textVersionDetails = textVersionDetails;
  this->labelInstallLink = labelInstallLink;
  this->pushInstall = pushInstall;
  this->textLocalInstall = textLocalInstall;
  this->setup = setup;
  this->listVersions->setModel(model);
}

void CRouterBRouterDownloadPage::begin() {
  setComplete(setup->isLocalBRouterValid);
  textVersionDetails->setVisible(false);
  textVersionDetails->clear();
  textLocalInstall->setVisible(false);
  textLocalInstall->clear();
  labelInstallLink->setText(tr("no brouter-version to install selected"));
  pushInstall->setEnabled(false);
  wizard()->setOption(QWizard::HaveCustomButton1, false);
  downloadUrl = QUrl();

  QNetworkRequest request(QUrl(setup->expertBinariesUrl));
  request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github+json"));
  request.setRawHeader(QByteArray("X-GitHub-Api-Version"), QByteArray("2022-11-28"));
  QNetworkReply* reply = networkAccessManager->get(request);
  reply->setProperty("requestType", eRequestVersions);
}

void CRouterBRouterDownloadPage::slotListVersionsClicked(const QModelIndex& index) {
  const QString& version = index.data().toString();
  for (const CRouterBRouterReleaseItem& item : std::as_const(releaseItems)) {
    if (item.name == version) {
      downloadUrl = QUrl(item.url);
      textVersionDetails->setText(item.description);
      textVersionDetails->setVisible(true);
      textLocalInstall->setVisible(false);
      labelInstallLink->setText(QString(tr("selected %1 for download and installation")).arg(downloadUrl.fileName()));
      pushInstall->setEnabled(true);
      break;
    }
  }
}

void CRouterBRouterDownloadPage::updateModel() const {
  QStringList versions;
  for (const CRouterBRouterReleaseItem& item : std::as_const(releaseItems)) {
    versions.append(item.name);
  }
  model->setStringList(versions);
}

void CRouterBRouterDownloadPage::slotNetworkAccessManagerReplyReceived(QNetworkReply* reply) {
  reply->deleteLater();
  switch (reply->property("requestType").toInt()) {
    case eRequestVersions: {
      localBRouterVersionsLoadFinished(reply);
      break;
    }

    case eRequestDownload: {
      localBRouterDownloadFinished(reply);
      break;
    }
  }
}

void CRouterBRouterDownloadPage::localBRouterVersionsLoadFinished(QNetworkReply* reply) {
  try {
    if (reply->error() != QNetworkReply::NoError) {
      throw tr("Network Error: %1").arg(reply->errorString());
    }
    QJsonParseError error;
    QByteArray content = reply->readAll();
    const QJsonDocument& document = QJsonDocument::fromJson(content, &error);
    if (error.error != QJsonParseError::NoError) {
      throw tr("Error parsing json: %1").arg(error.errorString());
    }
    if (!document.isArray()) {
      throw tr("Error parsing json: response is not an array: %1").arg(QString(content));
    }
    releaseItems.clear();
    const QJsonArray& values = document.array();
    for (const QJsonValue& value : values) {
      const QJsonObject& releaseObj = value.toObject();
      const QString& name = releaseObj["name"].toString();
      const QString& description = releaseObj["body"].toString();
      const QJsonArray& assets = releaseObj["assets"].toArray();
      for (const QJsonValue& assetValue : assets) {
        const QJsonObject& asset = assetValue.toObject();
        if (asset["content_type"].toString() == "application/x-zip-compressed") {
          const QString& url = asset["browser_download_url"].toString();
          releaseItems.append(CRouterBRouterReleaseItem(name, description, url));
          break;
        }
      }
    }
    if (releaseItems.size() == 0) {
      throw tr("No releases found at %1").arg(setup->expertBinariesUrl);
    }
  } catch (const QString& msg) {
    textLocalInstall->setTextColor(Qt::red);
    textLocalInstall->append(tr("loading brouter version-data failed: %1").arg(msg));
  }
  updateModel();
}

void CRouterBRouterDownloadPage::slotLocalDownloadButtonClicked() const {
  const QString& strUrl = downloadUrl.toString();

  if (!strUrl.startsWith("https")) {
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
                     "Use the Web version of BRouter instead.")
                      .arg(strUrl);

    mbox.setText(msg);

    QCheckBox* checkAgree = new QCheckBox(tr("I understand the risk and wish to proceed."), &mbox);
    mbox.setCheckBox(checkAgree);
    connect(checkAgree, &QCheckBox::clicked, mbox.button(QMessageBox::Ok), &QPushButton::setEnabled);
    mbox.button(QMessageBox::Ok)->setDisabled(true);

    if (mbox.exec() != QMessageBox::Ok) {
      return;
    }
  }
  pushInstall->setEnabled(false);
  textVersionDetails->setVisible(false);
  textLocalInstall->clear();
  textLocalInstall->setVisible(true);
  textLocalInstall->setTextColor(Qt::darkGreen);
  textLocalInstall->append(tr("download %1 started").arg(downloadUrl.toString()));
  QNetworkReply* reply = networkAccessManager->get(QNetworkRequest(downloadUrl));
  reply->setProperty("requestType", eRequestDownload);
  reply->setProperty("fileName", downloadUrl.fileName());
}

void CRouterBRouterDownloadPage::localBRouterDownloadFinished(QNetworkReply* reply) {
  try {
    if (reply->error() != QNetworkReply::NoError) {
      throw tr("Network Error: %1").arg(reply->errorString());
    }
    const QString& fileName = reply->property("fileName").toString();
    const QDir outDir(setup->localDir);
    if (!outDir.exists()) {
      throw tr("Error directory %1 does not exist").arg(outDir.absolutePath());
    }
    QDir downloadDir = setup->getDownloadDir();
    QFile outfile(downloadDir.absoluteFilePath(fileName));
    QStringList messageList;
    try {
      if (!outfile.open(QIODevice::WriteOnly)) {
        throw tr("Error creating file %1").arg(outfile.fileName());
      }
      if (outfile.write(reply->readAll()) < 0) {
        throw tr("Error writing to file %1").arg(outfile.fileName());
      }
      outfile.close();
      textLocalInstall->setTextColor(Qt::darkGreen);
      textLocalInstall->append(tr("download %1 finished").arg(outfile.fileName()));
      const QStringList& unzippedNames = JlCompress::extractDir(outfile.fileName(), downloadDir.path());
      textLocalInstall->append(tr("unzipping:"));
      for (const QString& unzipped : unzippedNames) {
        textLocalInstall->append(unzipped);
      }
      textLocalInstall->append(tr("installing into %1").arg(setup->localDir));
      setup->installLocalBRouter(messageList);
      for (const QString& message : std::as_const(messageList)) {
        textLocalInstall->append(message);
      }
      messageList.clear();
      downloadDir.removeRecursively();
      textLocalInstall->append(tr("installation successful"));
      setup->checkLocalBRouterInstallation();
      setComplete(setup->isLocalBRouterValid);
      setup->readLocalProfiles();
    } catch (const QString& msg) {
      for (const QString& message : std::as_const(messageList)) {
        textLocalInstall->append(message);
      }
      if (outfile.isOpen()) {
        outfile.close();
      }
      if (outfile.exists()) {
        outfile.remove();
      }
      if (downloadDir.exists() && !downloadDir.isEmpty()) {
        downloadDir.removeRecursively();
      }
      throw msg;
    }
  } catch (const QString& msg) {
    textLocalInstall->setTextColor(Qt::red);
    textLocalInstall->append(tr("installation of brouter failed: %1").arg(msg));
  }
}

void CRouterBRouterDownloadPage::setComplete(bool newComplete) {
  if (newComplete != complete) {
    complete = newComplete;
    emit completeChanged();
  }
}

bool CRouterBRouterDownloadPage::isComplete() const { return complete; }
