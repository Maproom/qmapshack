/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
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

#include "gis/prj/IGisProject.h"
#include "config.h"

#include <QNetworkAccessManager>
#include <QObject>

class CGisListWks;
class QLineEdit;
class CGeoSearchConfig;

class CGeoSearch : public QObject, public IGisProject
{
    Q_OBJECT
public:
    CGeoSearch(CGisListWks * parent, CGeoSearchConfig *config);
    virtual ~CGeoSearch();

private slots:
    void slotChangeSymbol();
    void slotSelectService();
    void slotNominatimSelected();
    void slotGeonamesSearchSelected();
    void slotGeonamesAddressSelected();
    void slotGoogleSelected();
    void slotSetupGeoSearch();
    void slotStartSearch();
    void slotRequestFinished(QNetworkReply* reply);
    void slotConfigChanged();

private:
    QLineEdit * edit;

    QAction * actSymbol;

    QNetworkAccessManager networkAccessManager;

    CGeoSearchConfig* searchConfig;

    void createErrorItem(const QString& status);
};

#endif //CSEARCHGOOGLE_H

