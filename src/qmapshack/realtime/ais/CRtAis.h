/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

    This proAisgram is free software: you can redistribute it and/or modify
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

#ifndef CRTAIS_H
#define CRTAIS_H

#include "realtime/IRtSource.h"
#include "units/IUnit.h"

#include <QDateTime>
#include <QPointer>

class QTimer;
class QNetworkAccessManager;
class QNetworkReply;
class CRtAisInfo;

class CRtAis : public IRtSource
{
    Q_OBJECT
public:
    CRtAis(QTreeWidget* parent);
    virtual ~CRtAis() = default;

    struct ship_t
    {
        QPointF pos = NOPOINTF;
        QPointF point = NOPOINTF;

        QString name;
        QString callsign;
        QString imo;
        QString mmsi;
        qint32 timePosition = NOINT;
        qreal longitude = NOFLOAT;
        qreal latitude = NOFLOAT;
        qreal velocity = NOFLOAT;
        qreal heading = NOFLOAT;
        QString destination;
        bool aid = false;
    };

    /**
       @brief Setup sub-item

       The parent tree widget is used to setup a sub item with CRtAisInfo as widget.
     */
    void registerWithTreeWidget() override;
    void loadSettings(QSettings& cfg) override;
    void saveSettings(QSettings& cfg) const override;

    QString getDescription() const override;

    /**
       @brief Get the number of entries in the current record

       @return The number as integer.
     */
    qint32 getNumberOfShips() const;
    /**
       @brief Get flag to control visibility of vessel names

       @return True to show names.
     */
    bool getShowNames() const;

    ship_t* getShipByKey(const QString& key, bool& ok);

    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw* rt) override;
    void fastDraw(QPainter& p, const QRectF& viewport, CRtDraw* rt)  override;
    void mouseMove(const QPointF& pos) override;
    static const QString strIcon;
public slots:
    /**
       @brief Set visibility of vessel names

       @param yes   set true to show names
     */
    void slotSetShowNames(bool yes);

private:
    QPointer<CRtAisInfo> info;
    QMap<QString, ship_t> ships;
    bool showNames = true;

    QString keyFocus;
};

#endif //CRTAIS_H

