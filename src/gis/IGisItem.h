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

#ifndef IGISITEM_H
#define IGISITEM_H

#include <QTreeWidgetItem>

#include <QColor>
#include <QDateTime>
#include <QDomNode>
#include <QMap>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include "units/IUnit.h"

class CGisDraw;
class IScrOpt;
class IMouse;
class QSqlDatabase;
class IGisProject;


class IGisItem : public QTreeWidgetItem
{
public:
    struct history_event_t
    {
        QDateTime time;
        QString hash;
        QString icon;
        QString comment;
        QByteArray data;
    };

    struct history_t
    {
        history_t() : histIdxInitial(NOIDX), histIdxCurrent(NOIDX)
        {
        }

        void reset()
        {
            histIdxInitial = NOIDX;
            histIdxCurrent = NOIDX;
            events.clear();
        }

        qint32 histIdxInitial;
        qint32 histIdxCurrent;
        QList<history_event_t> events;
    };


    struct link_t
    {
        QUrl uri;
        QString text;
        QString type;
    };

    struct wpt_t
    {
        wpt_t() :
            lat(NOFLOAT),
            lon(NOFLOAT),
            ele(NOINT),
            magvar(NOINT),
            geoidheight(NOINT),
            sat(NOINT),
            hdop(NOINT),
            vdop(NOINT),
            pdop(NOINT),
            ageofdgpsdata(NOINT),
            dgpsid(NOINT)
        {
        }
        // -- all gpx tags - start
        qreal lat;
        qreal lon;
        qint32 ele;
        QDateTime time;
        qint32 magvar;
        qint32 geoidheight;
        QString name;
        QString cmt;
        QString desc;
        QString src;
        QList<link_t> links;
        QString sym;
        QString type;
        QString fix;
        qint32 sat;
        qint32 hdop;
        qint32 vdop;
        qint32 pdop;
        qint32 ageofdgpsdata;
        qint32 dgpsid;
        // -- all gpx tags - stop
        QMap<QString, QVariant> extensions;
    };

    /// never ever change these numbers. it will break binary data files
    enum type_e
    {
        eTypeWpt = 1
        , eTypeTrk = 2
        , eTypeRte = 3
        , eTypeOvl = 4
        , eTypeMax = 5
    };

    enum mark_e
    {
        eMarkNone      = 0
        ,eMarkChanged   = 0x00000001
    };

    struct key_t
    {
        bool operator==(const key_t& k) const
        {
            return (item == k.item) && (project == k.project) && (device == k.device);
        }
        bool operator!=(const key_t& k) const
        {
            return (item != k.item) || (project != k.project) || (device != k.device);
        }
        void clear()
        {
            item.clear();
            project.clear();
            device.clear();
        }
        QString item;
        QString project;
        QString device;
    };

    IGisItem(IGisProject *parent, type_e typ, int idx);
    virtual ~IGisItem();

    /// this mutex has to be locked when ever the item list is accessed.
    static QMutex mutexItems;

    /**
       @brief If the item is part of a database project it will update itself with the database content
     */
    void updateFromDB(quint64 id, QSqlDatabase& db);

    /**
       @brief Update the visual representation of the QTreeWidgetItem
       @param enable
       @param disable
     */
    virtual void updateDecoration(mark_e enable, mark_e disable);

    /**
       @brief Save the item's data into a GPX structure
       @param gpx       the files <gpx> tag to attach the data to
     */
    virtual void save(QDomNode& gpx) = 0;

    /**
       @brief Get key string to identify object
       @return
     */
    const key_t& getKey();

    /**
       @brief Get a hash over the items data.

       Every entry in the history has a hash over the item's serialized data. If the
       data changes a new history entry is created and a new hash calculated. Thus the
       has can be used to detect if an item has been changed between the last time the
       hash was read.

       @return The hash as a string reference.
     */
    const QString& getHash();

    /**
       @brief Get the hash stored in the database when the item was loaded

       @return The hash as a string
     */
    const QString& getLastDatabaseHash()
    {
        return lastDatabaseHash;
    }

    /**
       @brief Read the hash stored in the database
     */
    void setLastDatabaseHash(quint64 id, QSqlDatabase& db);

    /**
       @brief Get the icon attached to object
       @return
     */
    virtual const QPixmap& getIcon() const
    {
        return icon;
    }
    /**
       @brief Get name of this item.
       @return A reference to the internal string object
     */
    virtual const QString& getName() const = 0;

    /**
       @brief Get name of this item extended by the project name
       @return A string object.
     */
    virtual QString getNameEx() const;

    /**
       @brief Get a short string with the items properties to be displayed in tool tips or similar
       @return A string object.
     */
    virtual QString getInfo(bool allowEdit = false) const = 0;

    virtual const QString& getComment() const = 0;
    virtual const QString& getDescription() const = 0;
    virtual const QList<link_t>& getLinks() const = 0;


    virtual void setComment(const QString& str) = 0;
    virtual void setDescription(const QString& str) = 0;
    virtual void setLinks(const QList<link_t>& links) = 0;

    /**
        @brief Edit content of item.

        This is quite dependent on the item. The default implementation does nothing. It has to be
        overwritten and the item has to generate what ever is needed to edit/view it's details.

     */
    virtual void edit()
    {
    }

    /**
       @brief Get the dimension of the item

       All coordinates are in Rad. Items with no

       @return
     */
    virtual const QRectF& getBoundingRect() const
    {
        return boundingRect;
    }

    /**
       @brief Get screen option object to display and handle actions for this item.
       @param mouse     a pointer to the mouse object initiating the action
       @return A null pointer is returned if no screen option are available
     */
    virtual IScrOpt * getScreenOptions(const QPoint& origin, IMouse * mouse)
    {
        return 0;
    }

    /**
       @brief Get a point of the item that is close by the given screen pixel coordinate
       @param point     a point in screen pixels
       @return If no point is found NOPOINTF is returned.
     */
    virtual QPointF getPointCloseBy(const QPoint& point)
    {
        return NOPOINTF;
    }

    /**
       @brief Test if the item is close to a given pixel coordinate of the screen

       @param pos       the coordinate on the screen in pixel
       @return If no point can be found NOPOINTF is returned.
     */
    virtual bool isCloseTo(const QPointF& pos) = 0;

    /**
       @brief Receive the current mouse position

       The default does nothing. Override if needed.

       @param pos   the mouse position on the screen in pixel
     */
    virtual void mouseMove(const QPointF& pos)
    {
        Q_UNUSED(pos);
    }
    void mousePress(const QPointF& pos)
    {
        Q_UNUSED(pos);
    }
    void mouseRelease(const QPointF& pos)
    {
        Q_UNUSED(pos);
    }


    /**
       @brief Query if this item is read only
       @return True if it is read only.
     */
    bool isReadOnly() const;

    /**
       @brief Query if the item is imported and was changed
       @return True if content was changed.
     */
    bool isTainted() const;

    /**
       @brief Check if item is on a GPS device
       @return True if the item is stored on a device
     */
    bool isOnDevice() const;

    /**
       @brief Check if there are any pending unsaved changes
       @return True if the are changes to be safed
     */
    bool isChanged() const;

    /**
       @brief Set the read only mode.

       This is quite dependent on the item. The default implementation will display a
       message box with a warning and ask the user to confirm.

       @param readOnly      set true to make item read only

       @return Return true if the mode change has been accepted.
     */
    virtual bool setReadOnlyMode(bool readOnly);

    virtual void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis) = 0;
    virtual void drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
    {
    }
    virtual void drawLabel(QPainter& p, const QPolygonF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis) = 0;
    virtual void drawHighlight(QPainter& p) = 0;

    virtual void gainUserFocus(bool yes) = 0;

    /**
       @brief Check for user focus

       @return True if the item has user focus. The default implementation is always false.
     */
    virtual bool hasUserFocus() const
    {
        return false;
    }


    /**
       @brief Serialize object out of a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    virtual QDataStream& operator<<(QDataStream& stream) = 0;
    /**
       @brief Serialize object into a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    virtual QDataStream& operator>>(QDataStream& stream) const = 0;

    /**
       @brief Get read access to history of changes

       @return A reference to the history structure.
     */
    const history_t& getHistory() const
    {
        return history;
    }

    /**
       @brief Load a given state of change from the history
       @param idx
     */
    void loadHistory(int idx);

    /**
       @brief Remove all history entries younger than the current selected one.
     */
    void cutHistory();

    /**
       @brief Create a clone of itself and pass back the pointer

       Add the cloned item to the project with the same index as the original

       @return The pointer of the cloned item
     */
    virtual IGisItem * createClone() = 0;


    /**
       @brief Remove all HTML tags from a string
       @param str the string
       @return A string without HTML tags
     */
    static QString removeHtml(const QString &str);
    /**
       @brief Create a HTML formated text with comment, description and link section.

       Depending on the isReadOnly flag the section headers are links to trigger a function

       @param isReadOnly    true if the text should have no active links
       @param cmt           the comment string
       @param desc          the description string
       @param links         a list of links
       @param key           some key to be sent with the header links
       @return The formated text ready to be used.
     */
    static QString createText(bool isReadOnly, const QString& cmt, const QString& desc, const QList<link_t>& links, const QString& key = "");
    /**
       @brief Create a HTML formated text with description and link section.

       Depending on the isReadOnly flag the section headers are links to trigger a function

       @param isReadOnly    true if the text should have no active links
       @param desc          the description string
       @param links         a list of links
       @param key           some key to be sent with the header links
       @return The formated text ready to be used.
     */
    static QString createText(bool isReadOnly, const QString& desc, const QList<link_t>& links, const QString& key = "");
    /**
       @brief Create a HTML formated text with a link.

       Depending on the isReadOnly flag the section headers are links to trigger a function

       @param isReadOnly    true if the text should have no active links
       @param href          the link address
       @param str           the link's string
       @param key           some key to be sent with the link
       @return The formated text ready to be used.
     */
    static QString toLink(bool isReadOnly, const QString& href, const QString& str, const QString& key);

    /// a no key value that can be used to nullify references.
    const static QString noKey;

    const static QString noName;

    struct color_t
    {
        const char   *name;
        const QColor color;
        const QString bullet;
    };

    static const color_t colorMap[];
    static const size_t  colorMapSize;

protected:
    /// set icon of QTreeWidgetItem
    virtual void setSymbol() = 0;
    /// read waypoint data from an XML snippet
    void readWpt(const QDomNode& xml, wpt_t &wpt);
    /// write waypoint data to an XML snippet
    void writeWpt(QDomElement &xml, const wpt_t &wpt);
    /// generate a unique key from item's data
    virtual void genKey();
    /// setup the history structure right after the creation of the item
    void setupHistory();
    /// update current history entry (e.g. to save the flags)
    virtual void updateHistory();
    /// convert a color string from GPX to a QT color
    QColor str2color(const QString& name);
    /// convert a QT color to a string to be used in a GPX file
    QString color2str(const QColor &color);
    /// to optimize drawing of large polylines split the line into sections that are visible
    void splitLineToViewport(const QPolygonF& line, const QRectF& extViewport, QList<QPolygonF>& lines);
    /// call when ever you make a change to the item's data
    virtual void changed(const QString& what, const QString& icon);


    void loadFromDb(quint64 id, QSqlDatabase& db);

    bool isVisible(const QRectF& rect, const QPolygonF& viewport, CGisDraw * gis);
    bool isVisible(const QPointF& point, const QPolygonF& viewport, CGisDraw * gis);

    /// see flags_e for possible flags
    quint32 flags = 0;
    /// the item's unique key
    key_t key;
    /// each item has an icon for the tree widget
    QPixmap icon;
    /// the dimensions of the item
    QRectF boundingRect;
    /// that's where the real data is. An item is completely defined by it's history
    history_t history;
    /// the hash in the database when the item was loaded/saved
    QString lastDatabaseHash;

    enum flags_e
    {
        eFlagCreatedInQms   = 0x00000001
        ,eFlagWriteAllowed  = 0x00000002
        ,eFlagTainted       = 0x00000004
        ,eFlagWptBubble     = 0x00000100
    };

    static inline bool isBlocked(const QRectF& rect, const QList<QRectF> &blockedAreas)
    {
        foreach(const QRectF &r, blockedAreas)
        {
            if(rect.intersects(r))
            {
                return true;
            }
        }
        return false;
    }
};

QDataStream& operator>>(QDataStream& stream, IGisItem::history_t& h);
QDataStream& operator<<(QDataStream& stream, const IGisItem::history_t& h);

#endif //IGISITEM_H

