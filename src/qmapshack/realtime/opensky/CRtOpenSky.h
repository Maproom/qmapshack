/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CRTOPENSKY_H
#define CRTOPENSKY_H

#include "realtime/IRtSource.h"
#include "units/IUnit.h"

#include <QDateTime>

class QTimer;
class QNetworkAccessManager;
class QNetworkReply;

class CRtOpenSky : public IRtSource
{
    Q_OBJECT
public:
    CRtOpenSky(QTreeWidget * parent);
    virtual ~CRtOpenSky() = default;

    /**
       @brief Setup sub-item

       The parent tree widgte is used to setup a subitem with CRtOpenSkyInfo as widget.
     */
    void registerWithTreeWidget() override;
    void loadSettings(QSettings& cfg) override;
    void saveSettings(QSettings& cfg) const override;

    QString getDescription() const override;

    /**
       @brief Get the timestamp of the last OpenSky update

       @return The timestamo as QDateTime instance
     */
    const QDateTime& getTimestamp() const;

    /**
       @brief Get the number of entries in the current record

       @return The number as integer.
     */
    qint32 getNumberOfAircrafts() const;
    /**
       @brief Get flag to control visibility of callsign names

       @return True to show names.
     */
    bool getShowNames() const;

    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) const override;

    static const QString strIcon;
public slots:
    /**
       @brief Set visiblity of callsign names

       @param yes   set true to show names
     */
    void slotSetShowNames(bool yes);

private slots:
    /**
       @brief Request a new dataset from OpenSky
     */
    void slotUpdate();
    /**
       @brief Handle incomming dataset from OpenSky
       @param reply
     */
    void slotRequestFinished(QNetworkReply* reply);

private:
    QTimer * timer;
    QNetworkAccessManager * networkAccessManager;

    struct aircraft_t
    {
        QString callsign;
        QPointF pos = NOPOINTF;
        qreal heading = 0;
    };

    QDateTime timestamp;
    QMap<QString, aircraft_t> aircrafts;
    bool showNames = true;
};

#endif //CRTOPENSKY_H

