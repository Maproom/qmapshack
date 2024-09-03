/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CROUTERBROUTERTILESSELECT_H
#define CROUTERBROUTERTILESSELECT_H

#include <QDateTime>
#include <QDir>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

class CRouterBRouterTilesSelectArea;
class CRouterBRouterTilesStatus;
class CRouterBRouterSetup;
class QWebEnginePage;

class CRouterBRouterTilesSelect : public QWidget {
  Q_OBJECT
 public:
  CRouterBRouterTilesSelect(QWidget* parent);
  virtual ~CRouterBRouterTilesSelect();

  void setSetup(CRouterBRouterSetup* setup);
  void initialize();
  void cancelDownload() const;

  static QString formatSize(const qint64 size);
  static QPoint tileFromFileName(const QString& fileName);
  static QString fileNameFromTile(const QPoint tile);

  bool isInitialized() const { return initialized; }
  bool isDownloading() const { return downloading; }
  bool isDownloadSelected() const { return downloadSelected; }

  static const QPoint noTile;
  static const int minTileLat;
  static const int maxTileLat;
  static const int minTileLon;
  static const int maxTileLon;
  static const int tileSize;
  static const QString patternTileName;
  static const QString patternDate;
  static const QString patternSize;
  static const QRegularExpression regExpTileName;
  static const QRegularExpression regExpDate;
  static const QRegularExpression regExpSize;
  static const QString formatDate;
  static const QLocale localeDate;

 signals:
  void sigCompleteChanged();

 private slots:
  void slotTileClicked(const QPoint& tile);
  void slotTileToolTipChanged(const QPoint& tile) const;
  void slotClearSelection();
  void slotDeleteSelected();
  void slotSelectOutdated();
  void slotDownload();
  void slotLoadOnlineTilesRequestFinished(bool ok);
  void slotDownloadFinished(QNetworkReply* reply);
  void slotDownloadReadReady();

 private:
  QDir segmentsDir() const;
  void update();
  void updateStatus();
  void updateButtons() const;
  void updateTiles() const;
  void afterSlotLoadOnlineTilesRequestFinishedRunJavascript(const QVariant& v);
  void error(const QString& error) const;
  void segmentsError(const QString& msg) const;
  void clearError() const;

  CRouterBRouterTilesStatus* getTileStatus(QPoint tile) const;

  CRouterBRouterSetup* setup;

  QVBoxLayout* outerLayout;
  QWidget* widgetSelect;
  QLabel* statusLabel;
  QProgressBar* statusProgress;
  QLabel* errorLabel;

  QPushButton* pushSelectOutdated;
  QPushButton* pushClearSelection;
  QPushButton* pushDeleteSelection;
  QPushButton* pushDownload;

  CRouterBRouterTilesSelectArea* selectArea;

  QWebEnginePage* tilesWebPage;

  QNetworkAccessManager* tilesDownloadManager;
  QVector<QNetworkReply*> tilesDownloadManagerReplies;

  QHash<QString, CRouterBRouterTilesStatus*> tilesDownloadStatus;

  bool downloading{false};
  bool downloadSelected{false};
  bool initialized{false};
};

#endif  // CROUTERBROUTERTILESSELECT_H
