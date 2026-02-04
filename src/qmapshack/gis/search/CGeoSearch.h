/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CGEOSEARCH_H
#define CGEOSEARCH_H

#include <QNetworkAccessManager>
#include <QObject>

#include "config.h"
#include "gis/prj/IGisProject.h"
#include "gis/search/CGeoSearchConfig.h"

class CGisListWks;
class QLineEdit;

class CGeoSearch : public QObject, public IGisProject {
  Q_OBJECT
 public:
  CGeoSearch(CGisListWks* parent);
  virtual ~CGeoSearch();

  bool skipSave() const override { return true; }
  bool isInputEnabled() const { return inputEnabled; }
  const bool isChanged() const override { return false; }

  QPixmap getWptIcon() const;
  const QString& getLastAddress() const { return lastAddress; }

  void selectService(const QRect& rect);
  void changeSymbol();
  void startSearch(const QString& address);
  QString getServiceName() const;

 private slots:
  void slotServiceSelected(CGeoSearchConfig::service_e service, bool checked);
  void slotSetupGeoSearch();
  void slotRequestFinished(QNetworkReply* reply);
  void slotConfigChanged();
  void slotAccuResults(bool yes);
  void slotResetResults();

 private:
  static inline QString trRichText(const char* msg) { return "<div>" + tr(msg) + "</div>"; }
  QAction* addService(CGeoSearchConfig::service_e service, const QString& name, QMenu* menu);
  void requestNominatim(QString& addr) const;
  void requestGeonamesSearch(QString& addr) const;
  void requestGeonamesAddress(QString& addr) const;
  void requestGoogle(QString& addr) const;

  void parseNominatim(const QByteArray& data);
  void parseGeonamesSearch(const QByteArray& data);
  void parseGeonamesAddress(const QByteArray& data);
  void parseGoogle(const QByteArray& data);

  void createErrorItem(const QString& status);

  void setIcon();

  QNetworkAccessManager* networkAccessManager;
  CGeoSearchConfig* searchConfig;
  QTreeWidgetItem* itemStatus = nullptr;
  bool inputEnabled = true;
  QString lastAddress;
  CGeoSearchConfig::service_e lastService = CGeoSearchConfig::service_e::eServiceNone;
};

#endif  // CSEARCHGOOGLE_H
