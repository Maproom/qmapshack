/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CDETAILSGEOCACHE_H
#define CDETAILSGEOCACHE_H

#include "ui_IDetailsGeoCache.h"
#include <QDialog>

class CGisItemWpt;
class QNetworkAccessManager;

class CDetailsGeoCache : public QDialog, private Ui::IDetailsGeoCache
{
    Q_OBJECT
public:
    CDetailsGeoCache(CGisItemWpt& wpt, QWidget * parent);
    virtual ~CDetailsGeoCache();

private slots:
    void slotHintChanged(bool on);
    void slotLinkClicked(const QUrl& url);
    void slotCollectSpoiler();
    void slotRequestFinished(QNetworkReply * reply);

private:
    CGisItemWpt& wpt;
    QNetworkAccessManager * networkManager;
};

#endif //CDETAILSGEOCACHE_H

