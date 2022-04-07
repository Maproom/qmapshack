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

#include "CPoiItem.h"
#include "gis/IGisItem.h"

#include <QCoreApplication>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QString>
#include <QStringList>


class CPoiItemPOI : public CPoiItem
{
    Q_DECLARE_TR_FUNCTIONS(CRawPoi)
public:
    //Dummy constructor for the usage of QMap
    CPoiItemPOI(){}
    CPoiItemPOI(const QStringList& data, const QPointF& coordinates, const quint64& key, const QString& category, const QString& garminIcon);

    // Overridden members
    virtual bool getGpxMode() const override {return false;}
    virtual QDomNode getGpx() const override {return QDomNode();}
    virtual QString getName() const override {return getNameOpt();}
    virtual QString getDesc() const override;
    virtual QList<IGisItem::link_t> getLinks() const override;
    virtual quint32 getEle() const override;

    const QString& getNameOpt(bool replaceEmptyByCategory = true) const;
    const QString& getCategory() const;
    uint getKey() const override;
    const QMap<QString, QString>& getData() const;
    const QStringList& getRawData() const;


private:
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
