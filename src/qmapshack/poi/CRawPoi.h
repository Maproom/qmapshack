/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#ifndef CRAWPOI_H
#define CRAWPOI_H

#include "gis/IGisItem.h"

#include <QCoreApplication>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QString>
#include <QStringList>

struct poi_t;

class CRawPoi
{
    Q_DECLARE_TR_FUNCTIONS(CRawPoi)
public:
    //Dummy constructor for the usage of QMap
    CRawPoi(){}
    CRawPoi(const QStringList& data, const QPointF& coordinates, const quint64& key, const QString &category, const QString &garminIcon);
    const QString& getCategory() const;
    const QString& getName(bool replaceEmptyByCategory = true) const;
    const QPointF &getCoordinates() const;
    const quint64 &getKey() const;
    const QMap<QString, QString> &getData() const;
    const QStringList& getRawData() const;
    QString getDesc() const;
    QList<IGisItem::link_t> getLinks() const;
    quint32 getEle() const;
    poi_t toPoi() const;


private:
    QString category;
    QPointF coordinates; // in radians
    /// <key, value>
    QMap<QString, QString> data;
    QStringList rawData;
    QStringList wikipediaRelatedKeys;
    QStringList wikidataRelatedKeys;
    QStringList nameRelatedKeys;
    QString garminIcon;
    quint64 key;
    QString name;
};

#endif // CRAWPOI_H
