/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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
#include "gis/rte/router/IRouter.h"
#include "gis/search/CProjectFilterItem.h"
#include "gis/search/CSearch.h"
#include <QDebug>
#include <QMessageBox>
#include <QPointer>
#include <QTreeWidgetItem>

class CGisListWks;
class CGisDraw;
class CGisItemWpt;
class QDataStream;
class CDetailsPrj;
class IDevice;

class IGisProject : public QTreeWidgetItem
{
    Q_DECLARE_TR_FUNCTIONS(IGisProject)
public:
    enum type_e
    {
        eTypeGeoSearch
        , eTypeQms
        , eTypeGpx
        , eTypeDb
        , eTypeLostFound
        , eTypeTwoNav
        , eTypeSlf       // the Sigma Log Format
        , eTypeFit
        , eTypeTcx
        , eTypeSml
        , eTypeLog
        , eTypeQlb
    };

    /// flags used to serialize trivial flags in qms file
    enum flags_e
    {
        eFlagNoCorrelation      = 0x1
        , eFlagAutoSave         = 0x2
        , eFlagInvalidDataOk    = 0x4
    };

    enum sorting_roadbook_e
    {
        eSortRoadbookNone
        , eSortRoadbookTrackWithDouble
        , eSortRoadbookTrackWithoutDouble
        , eSortRoadbookTrackWithDetails
    };

    enum sorting_folder_e
    {
        eSortFolderTime
        , eSortFolderName
        , eSortFolderSymbol
        , eSortFolderRating
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

    static const QString filedialogAllSupported;
    static const QString filedialogFilterGPX;
    static const QString filedialogFilterTCX;
    static const QString filedialogFilterSML;
    static const QString filedialogFilterLOG;
    static const QString filedialogFilterQLB;
    static const QString filedialogFilterQMS;
    static const QString filedialogFilterSLF;
    static const QString filedialogFilterFIT;
    static const QString filedialogSaveFilters;
    static const QString filedialogLoadFilters;

    IGisProject(type_e type, const QString &filename, CGisListWks *parent);
    IGisProject(type_e type, const QString &filename, IDevice     *parent);
    virtual ~IGisProject();

    static IGisProject * create(const QString filename, CGisListWks * parent);

    /**
       @brief Ask to save the project before it is closed.

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
       @brief Returns true if a project of given format can be saved, false if it cannot be saved (just as .slf atm)
     */
    virtual bool canSave() const
    {
        return false;
    }

    /**
       @brief Return true if saving should be skipped.
     */
    virtual bool skipSave() const
    {
        return false;
    }

    virtual const QString getFileDialogFilter() const
    {
        return QString();
    }

    virtual const QString getFileExtension() const
    {
        return QString();
    }

    /**
       @brief Save the project using it's native format.
     */
    virtual bool save();

    /**
       @brief Save the project selecting one of the available formats.
     */
    bool saveAs(QString fn = QString(), QString filter = QString());

    /**
       @brief Save as strict GPX V 1.1 without any extensions and HTML
       @return True on success
     */
    bool saveAsStrictGpx11();

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
    const QString& getKey() const
    {
        genKey();
        return key;
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

    const metadata_t& getMetadata() const
    {
        return metadata;
    }

    /**
       @brief Get the sorting mode
       @return One of sorting_e
     */
    sorting_roadbook_e getSortingRoadbook() const
    {
        return sortingRoadbook;
    }

    sorting_folder_e getSortingFolder() const
    {
        return sortingFolder;
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
       @brief Set the sorting mode for the roadbook in the details dialog

       This will mark the project as changed.

       @param s the mode
     */
    void setSortingRoadbook(sorting_roadbook_e s);

    /**
       @brief Set the sorting mode for workspace folder

       This will mark the project as changed.

       @param s the mode
     */
    void setSortingFolder(sorting_folder_e s);

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

    void getItemsByKeys(const QList<IGisItem::key_t>& keys, QList<IGisItem*>& items);
    /**
       @brief Get a list of items that are close to a given pixel coordinate of the screen

       @note: The returned pointers are just for temporary use. Best you use them to get the item's key.

       @param pos       the coordinate on the screen in pixel
       @param items     a list the item's pointer is stored to.
     */
    void getItemsByPos(const QPointF& pos, QList<IGisItem*>& items);

    void getItemsByArea(const QRectF& area, IGisItem::selflags_t flags, QList<IGisItem *> &items);

    void getNogoAreas(QList<IGisItem *> &nogos) const;

    int getItemCountByType(IGisItem::type_e type) const
    {
        return cntItemsByType[type];
    }

    qreal getTotalDistance() const
    {
        return totalDistance;
    }
    qreal getTotalAscent() const
    {
        return totalAscent;
    }
    qreal getTotalDescent() const
    {
        return totalDescent;
    }
    qreal getTotalElapsedSeconds() const
    {
        return totalElapsedSeconds;
    }
    qreal getTotalElapsedSecondsMoving() const
    {
        return totalElapsedSecondsMoving;
    }

    bool doCorrelation() const
    {
        return !noCorrelation;
    }

    void switchOnCorrelation();

    void setAutoSave(bool on);

    void setInvalidDataOk(bool ok)
    {
        invalidDataOk = ok;
        setChanged();
    }

    bool getInvalidDataOk() const
    {
        return invalidDataOk;
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
       @brief Add a copy if the given item to the project

       Before the item is inserted the method will use it's key to find a duplicate item.
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
    bool isValid() const
    {
        return valid;
    }

    /**
       @brief Test if visibility check mark is set
       @return True if project is visible
     */
    bool isVisible() const;

    bool isAutoSave() const
    {
        return autoSave;
    }

    /**
       @brief Test if this project is handled by a device
       @return The device type (IDevice::type_e). IDevice::eTypeNone if the project is not stored on a device.
     */
    qint32 isOnDevice() const;

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
    virtual QDataStream& operator>>(QDataStream& stream) const;

    /**
       @brief writeMetadata
       @param doc
       @return
     */
    QDomNode writeMetadata(QDomDocument& doc, bool strictGpx11);

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

       @param yes set true to block updating items
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

    void setProjectFilter(const CSearch& search);
    void setWorkspaceFilter(const CSearch& search);
    void applyFilters();

    void confirmPendingAutoSave()
    {
        autoSavePending = false;
    }

    bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32& threshold, QPolygonF& polyline);

    void gainUserFocus(bool yes);

    bool hasUserFocus() const
    {
        return keyUserFocus == key;
    }

    static const QString& getUserFocus()
    {
        return keyUserFocus;
    }

    CProjectFilterItem* filterProject(bool filter);
    CProjectFilterItem* getProjectFilterItem()
    {
        return projectFilter;
    }
protected:
    void genKey() const;
    virtual void setupName(const QString& defaultName);
    void markAsSaved();
    void readMetadata(const QDomNode& xml, metadata_t& metadata);
    void updateItems();
    void updateItemCounters();
    void updateDecoration();
    void sortItems();
    void sortItems(QList<IGisItem*>& items) const;

    /**
       @brief Converts a string with HTML tags to a string without HTML depending on the device

       Some devices e.g. Garmin can not handle HTML.

       @param str   a string
       @return A string with HTML removed depending on the device
     */
    QString html2Dev(const QString& str);

    // Those are the URIs of the GPX extensions we support
    static const QString gpxx_ns;
    static const QString gpxtpx_ns;
    static const QString wptx1_ns;
    static const QString rmc_ns;
    static const QString ql_ns;
    static const QString gs_ns;
    static const QString tp1_ns;
    // Those are standard GPX/XML namespaces
    static const QString gpx_ns;
    static const QString xsi_ns;
    static const QString gpxdata_ns;

    static QString keyUserFocus;

    QPointer<CDetailsPrj> dlgDetails;

    type_e type;
    mutable QString key;
    QString filename;
    bool valid                  = false;
    bool noUpdate               = false;
    bool noCorrelation          = false;
    bool changedRoadbookMode    = false;
    bool autoSave               = false; ///< flag to show if auto save is on or off
    bool autoSavePending        = false; ///< flag to show if auto save event has been sent. will be reset by save()
    bool invalidDataOk          = false; ///< if set invalid data in GIS items will not raise any dialog

    metadata_t metadata;
    QString nameSuffix;

    sorting_roadbook_e sortingRoadbook  = eSortRoadbookNone;
    sorting_folder_e sortingFolder    = eSortFolderTime;

    qint32 cntItemsByType[IGisItem::eTypeMax];

    qint32 cntTrkPts                 = 0;
    qint32 cntWpts                   = 0;

    qreal totalDistance             = 0;
    qreal totalAscent               = 0;
    qreal totalDescent              = 0;
    quint32 totalElapsedSeconds       = 0;
    quint32 totalElapsedSecondsMoving = 0;

    QString hashTrkWpt[2];

    CSearch projectSearch= CSearch("");
    CSearch workspaceSearch= CSearch("");

    CProjectFilterItem* projectFilter = nullptr;
};
Q_DECLARE_METATYPE(IGisProject*)

class CProjectMountLock
{
public:
    CProjectMountLock(IGisProject& project)
        : project(project)
    {
        project.mount();
    }

    ~CProjectMountLock()
    {
        project.umount();
    }

private:
    IGisProject& project;
};

#endif //IGISPROJECT_H

