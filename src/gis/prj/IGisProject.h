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
        , eTypeTwoNav
    };

    enum sorting_e
    {
        eSortNone
        , eSortTime
        , eSortTrackWithDouble
        , eSortTrackWithoutDouble
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

    /**
       @brief Ask to save the project befor it is closed.

       If the project is closed, the user is asked if the project should be saved and saved on user request.

       @return True if the operation is aborted. False on "save" and "no".
     */
    bool askBeforClose();

    IGisProject& operator=(const IGisProject& p)
    {
        key      = p.key;
        metadata = p.metadata;
        return *this;
    }

    /**
       @brief Summon the project details dialog.
     */
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

    /**
       @brief Get the project type enumeration.

       @Note: usually dynamic_cast should be used to get a pointer of correct type.
              However if the project is serialized, a type id is needed.

       @return One of type_e
     */
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
    QString getDeviceKey() const;


    QPixmap getIcon() const;

    /**
       @brief Get the project's name
       @return The name from metadata.name
     */
    QString getName() const;
    /**
       @brief Get the project's name extended with the parent's name.
       @return The name from metadata.nam appended with either the device name or the database parent folder's name.
     */
    QString getNameEx() const;

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

    /**
       @brief Get the sorting mode
       @return One of sorting_e
     */
    sorting_e getSorting() const
    {
        return sorting;
    }

    void setName(const QString& str);
    void setKeywords(const QString& str);
    void setDescription(const QString& str);
    void setLinks(const QList<IGisItem::link_t>& links);
    /**
       @brief Set change mark
     */
    void setChanged();

    /**
       @brief Set the sorting mode_t

       This will mark the project as changed.

       @param s the mode
     */
    void setSorting(sorting_e s);

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
    void getItemsByPos(const QPointF& pos, QList<IGisItem*>& items);


    int getItemCountByType(IGisItem::type_e type)
    {
        return cntItemsByType[type];
    }

    /**
       @brief Receive the current mouse position

       Iterate over all items and pass the position

       @param pos   the mouse position on the screen in pixel
     */
    virtual void mouseMove(const QPointF& pos);

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
       @brief Test if visibility check mark is set
       @return True if project is visible
     */
    bool isVisible() const;

    /**
       @brief Test if this project is handled by a device
       @return True if handled by a device
     */
    bool isOnDevice() const;

    /**
       @brief Test if project has been changed
       @return True if changed.
     */
    bool isChanged() const;

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

    /**
       @brief Mount volume the project's file is stored at

       This is only valid for projects located on GPS devices.
       For all other projects the method does nothing.
     */
    void mount();
    /**
       @brief Umount volume the project's file is stored at

       This is only valid for projects located on GPS devices.
       For all other projects the method does nothing.
     */
    void umount();

    /**
       @brief Removed the projects file from disk.

       This is only valid for projects located on GPS devices.
       For all other projects the method does nothing.
     */
    bool remove();

    /**
       @brief Block update of items.

        Use this to speed up actions with many items, e.g. copy actions.
        If the blocking is stopped (yes == false) updateItems() is called.

       @param yes set true to block updateing items
     */
    void blockUpdateItems(bool yes);

    /**
       @brief  Return state of current update block
       @return True if updates are blocked.
     */
    bool blockUpdateItems() const
    {
        return noUpdate;
    }

protected:
    void genKey();
    virtual void setupName(const QString& defaultName);
    void markAsSaved();
    void readMetadata(const QDomNode& xml, metadata_t& metadata);
    void updateItems();
    void updateItemCounters();



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
    bool noUpdate;

    metadata_t metadata;
    QString nameSuffix;

    QPointer<CDetailsPrj> dlgDetails;

    sorting_e sorting;

    int cntItemsByType[IGisItem::eTypeMax];
};

#endif //IGISPROJECT_H

