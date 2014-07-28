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

#ifndef CGISPROJECT_H
#define CGISPROJECT_H

#include "gis/IGisItem.h"

#include <QTreeWidgetItem>
#include <QDomElement>
#include <QDateTime>
#include <QUrl>

class CGisListWks;
class CGisDraw;


class CGisProject : public QTreeWidgetItem
{
    public:
        CGisProject(const QString& filename, const QString &key, CGisListWks * parent);
        virtual ~CGisProject();

        bool  isValid(){return valid;}
        const QString& getKey(){return key;}
        void getItemByPos(const QPointF& pos, QList<IGisItem*>& items);
        IGisItem * getItemByKey(const QString& key);
        void delItemByKey(const QString& key);

        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, const QFontMetricsF& fm, CGisDraw * gis);

        void save();
        void saveAs();

        QString getInfo();

        // Those are the URIs of the GPX extensions we support
        static const QString gpxx_ns;
        static const QString gpxtpx_ns;
        static const QString wptx1_ns;
        static const QString rmc_ns;
        static const QString ql_ns;
        static const QString gs_ns;

    private:
        struct metadata_t;

        void saveGpx(const QString& fn);
        QDomNode writeMetadata(QDomDocument& doc);

        void readMetadata(const QDomNode& xml, metadata_t& metadata);

        // Those are standard GPX/XML namespaces
        static const QString gpx_ns;
        static const QString xsi_ns;

        struct person_t
        {
            QString name;
            QString id;
            QString domain;
            IGisItem::link_t link;
        };

        struct copyright_t
        {
            QString author;
            QString year;
            QString license;
        };

        struct metadata_t
        {
            QString name;
            QString desc;
            person_t author;
            copyright_t copyright;
            QList<IGisItem::link_t> links;
            QDateTime time;
            QString keywords;
            QRectF bounds;
            // -- all gpx tags - stop
            QMap<QString, QVariant> extensions;

        };

        QString key;

        metadata_t metadata;
        QString filename;

        QDomElement xmlGpx;

        bool valid;
};

#endif //CGISPROJECT_H

