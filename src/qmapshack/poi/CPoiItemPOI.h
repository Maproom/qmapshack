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

#ifndef CPOIITEMPOI_H
#define CPOIITEMPOI_H

#include "gis/IGisItem.h"
#include "IPoiItem.h"

#include <QCoreApplication>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QString>
#include <QStringList>


class CPoiItemPOI : public IPoiItem
{
    Q_DECLARE_TR_FUNCTIONS(CRawPoi)
public:
    [[deprecated("Only to enable use of CPoiItemPOI in QVector and similar until we upgrade to Qt >= 5.13. DO NOT USE!")]]
    CPoiItemPOI(){}
    CPoiItemPOI(const QStringList& data, const QPointF& coordinates, const quint64& key, const QString& category, const QString& garminIcon);

    // Overridden members
    using IPoiItem::getName;
    QString getName(bool& fallback) const override;
    QString getDesc() const override;
    QPointF getPos() const override {return pos;}
    QList<IGisItem::link_t> getLinks() const override;
    quint32 getEle() const override;
    using IPoiItem::getCategory;
    QString getCategory(bool& fallback) const override {fallback = false; return category;}

    uint getKey() const override;
    const QMap<QString, QString>& getData() const;
    const QStringList& getRawData() const;


private:
    QString name;
    QPointF pos;
    QString icon;
    QString category;
    /// <key, value>
    QMap<QString, QString> data;
    QStringList rawData;
    QStringList wikipediaRelatedKeys;
    QStringList wikidataRelatedKeys;
    QStringList nameRelatedKeys;
    quint64 key;
};

#endif // CPOIITEMPOI_H
