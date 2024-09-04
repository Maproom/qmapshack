/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2020 Henri Hornburg <hrnbg@t-online.de>

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

#include <QColor>
#include <QCoreApplication>
#include <QDateTime>
#include <QDomNode>
#include <QMap>
#include <QMutex>
#include <QPainter>
#include <QString>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QUrl>
#include <QVariant>

#include "units/IUnit.h"

class CGisDraw;
class IScrOpt;
class IMouse;
class QSqlDatabase;
class IGisProject;
struct searchValue_t;
enum searchProperty_e : unsigned int;

class IGisItem : public QTreeWidgetItem {
  Q_DECLARE_TR_FUNCTIONS(IGisItem)
 public:
  struct history_event_t {
    QDateTime time;
    QString hash;
    QString who = "QMapShack";
    QString icon;
    QString comment;
    QByteArray data;
  };

  struct history_t {
    history_t() : histIdxInitial(NOIDX), histIdxCurrent(NOIDX) {}

    void reset() {
      histIdxInitial = NOIDX;
      histIdxCurrent = NOIDX;
      events.clear();
    }

    qint32 histIdxInitial;
    qint32 histIdxCurrent;
    QList<history_event_t> events;
  };

  struct link_t {
    QUrl uri;
    QString text;
    QString type;
  };

  struct wpt_t {
    wpt_t()
        : lat(NOFLOAT),
          lon(NOFLOAT),
          ele(NOINT),
          magvar(NOINT),
          geoidheight(NOINT),
          sat(NOINT),
          hdop(NOINT),
          vdop(NOINT),
          pdop(NOINT),
          ageofdgpsdata(NOINT),
          dgpsid(NOINT) {}

    operator QPointF() const { return QPointF(lon, lat); }

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
  enum type_e { eTypeWpt = 1, eTypeTrk = 2, eTypeRte = 3, eTypeOvl = 4, eTypeMax = 5 };

  enum mark_e { eMarkNone = 0, eMarkChanged = 0x00000001, eMarkNotPart = 0x00000002, eMarkNotInDB = 0x00000004 };

  enum selection_e {
    eSelectionNone = 0,
    eSelectionExact = 0x00000001,
    eSelectionIntersect = 0x00000002,
    eSelectionTrk = 0x80000000,
    eSelectionWpt = 0x40000000,
    eSelectionRte = 0x20000000,
    eSelectionOvl = 0x10000000,
    eSelectionPoi = 0x08000000
  };

  using selflags_t = quint32;

  enum color_e {
    eColorBlack = 0,
    eColorDarkRed = 1,
    eColorDarkGreen = 2,
    eColorDarkYellow = 3,
    eColorDarkBlue = 4,
    eColorDarkMagenta = 5,
    eColorDarkCyan = 6,
    eColorLightGray = 7,
    eColorDarkGray = 8,
    eColorRed = 9,
    eColorGreen = 10,
    eColorYellow = 11,
    eColorBlue = 12,
    eColorMagenta = 13,
    eColorCyan = 14,
    eColorWhite = 15,
    eColorTransparent = 16
  };

  struct key_t {
    bool operator==(const key_t& k) const { return (item == k.item) && (project == k.project) && (device == k.device); }
    bool operator!=(const key_t& k) const { return (item != k.item) || (project != k.project) || (device != k.device); }
    void clear() {
      item.clear();
      project.clear();
      device.clear();
    }
    QString item;
    QString project;
    QString device;
  };

  IGisItem(IGisProject* parent, type_e typ, int idx);
  virtual ~IGisItem();

  /// this mutex has to be locked when ever the item list is accessed.
  static QRecursiveMutex mutexItems;

  static void init();
  static QMenu* getColorMenu(const QString& title, QObject* obj, const char* slot, QWidget* parent);
  static qint32 selectColor(QWidget* parent);

  /**
     @brief If the item is part of a database project it will update itself with the database content
   */
  virtual void updateFromDB(quint64 id, QSqlDatabase& db);

  /**
     @brief Update the visual representation of the QTreeWidgetItem
     @param enable
     @param disable
   */
  virtual void updateDecoration(quint32 enable, quint32 disable);

  /**
     @brief Save the item's data into a GPX structure
     @param gpx       the files <gpx> tag to attach the data to
   */
  virtual void save(QDomNode& gpx, bool strictGpx11) = 0;

  /**
     @brief Get key string to identify object
     @return
   */
  const key_t& getKey() const;

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
  const QString& getLastDatabaseHash();

  /**
     @brief Read the hash stored in the database
   */
  void setLastDatabaseHash(quint64 id, QSqlDatabase& db);

  /**
     @brief Get the icon attached to object
     @return
   */
  void setIcon(const QPixmap& icon);

  const QPixmap& getIcon() const { return icon; }

  const QPixmap& getDisplayIcon() const { return displayIcon; }
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

  enum features_e {
    eFeatureNone = 0,
    eFeatureShowName = 0x00000001,
    eFeatureShowFullText = 0x00000002,
    eFeatureShowActivity = 0x00000004,
    eFeatureShowDateTime = 0x00000008,
    eFeatureShowLinks = 0x00000010
  };

  /**
     @brief Get a short string with the items properties to be displayed in tool tips or similar

     @param showName          set true if the first line should be the item's name
     @param features          a combination of features_e types

     @return A string object.
   */
  virtual QString getInfo(quint32 features) const = 0;

  virtual const QString& getComment() const = 0;
  virtual const QString& getDescription() const = 0;
  virtual const QList<link_t>& getLinks() const = 0;
  virtual QDateTime getTimestamp() const = 0;

  virtual void setComment(const QString& str) = 0;
  virtual void setDescription(const QString& str) = 0;
  virtual void setLinks(const QList<link_t>& links) = 0;

  /**
      @brief Edit content of item.

      This is quite dependent on the item. The default implementation does nothing. It has to be
      overwritten and the item has to generate what ever is needed to edit/view it's details.

   */
  virtual void edit() {}

  /**
     @brief Get the dimension of the item

     All coordinates are in Rad. Items with no

     @return
   */
  virtual const QRectF& getBoundingRect() const { return boundingRect; }

  /**
     @brief Get screen option object to display and handle actions for this item.
     @param mouse     a pointer to the mouse object initiating the action
     @return A null pointer is returned if no screen option are available
   */
  virtual IScrOpt* getScreenOptions(const QPoint& origin, IMouse* mouse) { return nullptr; }

  /**
     @brief Get a point of the item that is close by the given screen pixel coordinate
     @param point     a point in screen pixels
     @return If no point is found NOPOINTF is returned.
   */
  virtual QPointF getPointCloseBy(const QPoint& point) { return NOPOINTF; }

  /**
     @brief Test if the item is close to a given pixel coordinate of the screen

     @param pos       the coordinate on the screen in pixel
     @return If no point can be found NOPOINTF is returned.
   */
  virtual bool isCloseTo(const QPointF& pos) = 0;

  virtual bool isWithin(const QRectF& area, selflags_t mode) = 0;

  /**
     @brief Receive the current mouse position

     The default does nothing. Override if needed.

     @param pos   the mouse position on the screen in pixel
   */
  virtual void mouseMove(const QPointF& pos) { Q_UNUSED(pos); }

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
     @return The device type (IDevice::type_e). IDevice::eTypeNone if the item is not stored on a device.
   */
  qint32 isOnDevice() const;

  /**
     @brief Check if there are any pending unsaved changes
     @return True if the are changes to be saved
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

  virtual void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) = 0;
  virtual void drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis) {}
  virtual void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm,
                         CGisDraw* gis) = 0;
  virtual void drawHighlight(QPainter& p) = 0;

  virtual void gainUserFocus(bool yes) = 0;

  /**
     @brief Check for user focus

     @return True if the item has user focus. The default implementation is always false.
   */
  virtual bool hasUserFocus() const { return false; }

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
  const history_t& getHistory() const { return history; }

  /**
     @brief Load a given state of change from the history
     @param idx
   */
  void loadHistory(int idx);

  /**
     @brief Remove all history entries younger than the current selected one.
   */
  void cutHistoryAfter();

  /**
     @brief Remove all history entries older than the current selected one.
   */
  void cutHistoryBefore();

  /**
     @brief Take data of the most recent entry and apply meta information of first one

     All other entries are lost
   */
  void squashHistory();

  /**
     @brief Create a clone of itself and pass back the pointer

     Add the cloned item to the project with the same index as the original

     @return The pointer of the cloned item
   */
  virtual IGisItem* createClone() = 0;

  void setNogo(bool yes);
  bool isNogo() const { return bool(flags & eFlagNogo); }

  static const QBrush& getNogoTextureBrush();

  IGisProject* getParentProject() const;

  /**
     @brief Remove all HTML tags from a string
     @param str the string
     @return A string without HTML tags
   */
  static QString removeHtml(const QString& str);
  /**
     @brief Create a HTML formatted text with comment, description and link section.

     Depending on the isReadOnly flag the section headers are links to trigger a function

     @param isReadOnly    true if the text should have no active links
     @param cmt           the comment string
     @param desc          the description string
     @param links         a list of links
     @param key           some key to be sent with the header links
     @return The formatted text ready to be used.
   */
  static QString createText(bool isReadOnly, const QString& cmt, const QString& desc, const QList<link_t>& links,
                            const QString& key = "");
  /**
     @brief Create a HTML formatted text with description and link section.

     Depending on the isReadOnly flag the section headers are links to trigger a function

     @param isReadOnly    true if the text should have no active links
     @param desc          the description string
     @param links         a list of links
     @param key           some key to be sent with the header links
     @return The formatted text ready to be used.
   */
  static QString createText(bool isReadOnly, const QString& desc, const QList<link_t>& links, const QString& key = "");
  /**
     @brief Create a HTML formatted text with a link.

     Depending on the isReadOnly flag the section headers are links to trigger a function

     @param isReadOnly    true if the text should have no active links
     @param href          the link address
     @param str           the link's string
     @param key           some key to be sent with the link
     @return The formated text ready to be used.
   */
  static QString toLink(bool isReadOnly, const QString& href, const QString& str, const QString& key);

  /**
     @brief Unified handler to get a new item name and a pointer to the traget project

     @param name      a reference to a string object with the default name and to receive the name
     @param project   a reference to a IGisProject pointer. On success it will point to the project instance
     @param itemtype  a string to be used for the item type in the dialogs


     @return Returns true on success. Otherwise false.
   */
  static bool getNameAndProject(QString& name, IGisProject*& project, const QString& itemtype);

  static IGisItem* newGisItem(quint32 type, quint64 id, QSqlDatabase& db, IGisProject* project);

  /// a no key value that can be used to nullify references.
  const static QString noKey;

  const static QString noName;

  struct color_t {
    QString name;
    QString label;
    QColor color;
    QString bullet;
    QString line;
  };

  static const QVector<color_t>& getColorMap() { return colorMap; }

  virtual const searchValue_t getValueByKeyword(searchProperty_e keyword) = 0;

  qreal getRating() const;
  void setRating(qreal rating);
  const QSet<QString>& getKeywords() const;
  QList<QString> getKeywordsSorted() const;
  void addKeywords(const QSet<QString>& otherKeywords);
  void removeKeywords(const QSet<QString>& otherKeywords);
  const QString getRatingKeywordInfo() const;

 protected:
  /// set icon of QTreeWidgetItem
  virtual void setSymbol() = 0;
  /// read waypoint data from an XML snippet
  void readWpt(const QDomNode& xml, wpt_t& wpt);
  /// write waypoint data to an XML snippet
  void writeWpt(QDomElement& xml, const wpt_t& wpt, bool strictGpx11);
  /// generate a unique key from item's data
  virtual void genKey() const;
  /// setup the history structure right after the creation of the item
  void setupHistory();
  /// update current history entry (e.g. to save the flags)
  virtual void updateHistory();
  /// convert a color string from GPX to a QT color
  QColor str2color(const QString& name);
  /// convert a QT color to a string to be used in a GPX file
  QString color2str(const QColor& color);
  /// to optimize drawing of large polylines split the line into sections that are visible
  void splitLineToViewport(const QPolygonF& line, const QRectF& extViewport, QList<QPolygonF>& lines);
  /// call when ever you make a change to the item's data
  virtual void changed(const QString& what, const QString& icon);

  void loadFromDb(quint64 id, QSqlDatabase& db);
  bool isVisible(const QRectF& rect, const QPolygonF& viewport, CGisDraw* gis);
  bool isVisible(const QPointF& point, const QPolygonF& viewport, CGisDraw* gis);
  bool isWithin(const QRectF& area, selflags_t flags, const QPolygonF& points);
  void setNogoFlag(bool yes);

  /**
     @brief Converts a string with HTML tags to a string without HTML depending on the device

     Some devices e.g. Garmin can not handle HTML.

     @param str   a string
     @return A string with HTML removed depending on the device
   */
  QString html2Dev(const QString& str, bool strictGpx11);

  /// see flags_e for possible flags
  quint32 flags = 0;
  /// the item's unique key
  mutable key_t key;
  /// each item has an icon for the tree widget
  QPixmap icon;
  QPixmap displayIcon;
  /// the dimensions of the item
  QRectF boundingRect;
  /// that's where the real data is. An item is completely defined by it's history
  history_t history;
  /// the hash in the database when the item was loaded/saved
  QString lastDatabaseHash;

  enum flags_e {
    eFlagCreatedInQms = 0x00000001,
    eFlagWriteAllowed = 0x00000002,
    eFlagTainted = 0x00000004,
    eFlagWptBubble = 0x00000100,
    eFlagNogo = 0x00000200
  };

  static QVector<color_t> colorMap;

  /// labeling the GisItems
  qreal rating = 0;
  QSet<QString> keywords;

 private:
  void showIcon();
};

QDataStream& operator>>(QDataStream& stream, IGisItem::history_t& h);
QDataStream& operator<<(QDataStream& stream, const IGisItem::history_t& h);

#endif  // IGISITEM_H
