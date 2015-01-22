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

#ifndef IGISPROJECT_H
#define IGISPROJECT_H

#include "gis/IGisItem.h"
#include <QMessageBox>
#include <QPointer>
#include <QTreeWidgetItem>

class CGisListWks;
class IGisItem;
class CGisDraw;
class QDataStream;
class CDetailsPrj;
class IDevice;

class IGisProject : public QTreeWidgetItem
{
public:
    enum type_e
    {
        eTypeGoogle
        , eTypeQms
        , eTypeGpx
        , eTypeDb
        , eTypeLostFound
    };

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
        metadata_t() : time(QDateTime::currentDateTimeUtc())
        {
        }
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

    IGisProject(type_e type, const QString& filename, CGisListWks * parent);
    IGisProject(type_e type, const QString &filename, IDevice *parent);
    virtual ~IGisProject();

    void edit();

    /**
       @brief Save the project using it's native format.
     */
    virtual bool save() = 0;
    /**
       @brief Save the project selecting one of the available formats.
     */
    virtual bool saveAs() = 0;

    virtual void setFilename(const QString& fn)
    {
        filename = fn;
    }
    virtual QString getFilename() const
    {
        return filename;
    }

    type_e getType() const
    {
        return type;
    }

    /**
       @brief Get unique project key.
       @return A MD5 hash string
     */
    const QString& getKey()
    {
        genKey(); return key;
    }

    /**
       @brief Get the unique key of the device the project is attached to
       @return If the project is not attached to a device the string is empty
    */
    QString getDeviceKey();

    QString getName() const;

    const QDateTime& getTime() const
    {
        return metadata.time;
    }
    const QString& getKeywords() const
    {
        return metadata.keywords;
    }
    const QString& getDescription() const
    {
        return metadata.desc;
    }
    const QList<IGisItem::link_t>& getLinks() const
    {
        return metadata.links;
    }

    void setName(const QString& str);
    void setKeywords(const QString& str);
    void setDescription(const QString& str);
    void setLinks(const QList<IGisItem::link_t>& links);

    /**
       @brief Get a short metadata summary
       @return Informational string.
     */
    virtual QString getInfo() const;
    /**
       @brief Get a temporary pointer to the item with matching key
       @param key
       @return If no item is found 0 is returned.
     */
    IGisItem * getItemByKey(const IGisItem::key_t &key);

    /**
       @brief Get a list of items that are close to a given pixel coordinate of the screen

       @note: The returned pointers are just for temporary use. Best you use them to get the item's key.

       @param pos       the coordinate on the screen in pixel
       @param items     a list the item's pointer is stored to.
     */
    void getItemByPos(const QPointF& pos, QList<IGisItem*>& items);

    /**
       @brief Delete items with matching key
       @param key
     */
    bool delItemByKey(const IGisItem::key_t &key, QMessageBox::StandardButtons &last);

    /**
       @brief Call IGisItem::edit() method for items with given key

       @param key   a MD5 hash key
     */
    void editItemByKey(const IGisItem::key_t &key);

    /**
       @brief Add a copy if the gven item to the project

       Befor the item is inserted the method will use it's key to find a duplicat item.
       If there is an item with the same item key a copy option dialog is shown. Depending
       the result the action is performed or aborted. The result will be copied into
       lastResult to repeat the same decision on subsequent items.

       @param item          pointer to item
       @param off           the offset into the tree widget, -1 for none
       @param lastResult    a reference to hold the last result of the copy option dialog
     */
    void insertCopyOfItem(IGisItem *item, int off, int &lastResult);

    /**
       @brief Check if the project was initialized correctly.

       For example a if a GPX file does not load correctly the project is invalid.

       @return True if project is valid
     */
    bool  isValid() const
    {
        return valid;
    }

    /**
       @brief Test if this project is handled by a device
       @return True if handled by a device
    */
    bool isOnDevice() const;


    void drawItem(QPainter& p, const QPolygonF &viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
    void drawLabel(QPainter& p, const QPolygonF &viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
    void drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis);

    /**
       @brief Serialize object out of a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    virtual QDataStream& operator<<(QDataStream& stream);

    /**
       @brief Serialize object into a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    virtual QDataStream& operator>>(QDataStream& stream);

    /**
       @brief writeMetadata
       @param doc
       @return
     */
    QDomNode writeMetadata(QDomDocument& doc);

protected:
    void genKey();
    void setupName(const QString& defaultName);
    void markAsSaved();
    void readMetadata(const QDomNode& xml, metadata_t& metadata);
    void changed();

    // Those are the URIs of the GPX extensions we support
    static const QString gpxx_ns;
    static const QString gpxtpx_ns;
    static const QString wptx1_ns;
    static const QString rmc_ns;
    static const QString ql_ns;
    static const QString gs_ns;
    // Those are standard GPX/XML namespaces
    static const QString gpx_ns;
    static const QString xsi_ns;

    type_e type;
    QString key;
    QString filename;
    bool valid;

    metadata_t metadata;

    QPointer<CDetailsPrj> dlgDetails;
};

#endif //IGISPROJECT_H

