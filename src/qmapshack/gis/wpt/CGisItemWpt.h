/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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

#ifndef CGISITEMWPT_H
#define CGISITEMWPT_H

#include <QCoreApplication>
#include <QPointer>

#include "gis/IGisItem.h"
#include "gis/tnv/CTwoNavProject.h"

struct IPoiItem;
class IGisProject;
class QDomNode;
class CScrOptWpt;
class CScrOptWptRadius;
class QSqlDatabase;
class CQlgtWpt;
class QTextEdit;
class QDir;
class CFitStream;
class CGisItemWpt : public IGisItem {
  Q_DECLARE_TR_FUNCTIONS(CGisItemWpt)
 public:
  enum geocacheservice_e { eGcCom, eOc, eTc, eGcSu, eUnknown = -1 };

  struct geocachelog_t {
    quint32 id = 0;
    QDateTime date;
    QString type;
    QString finderId;
    QString finder;
    bool textIsHtml = false;
    QString text;
  };

  /*
   * The Date the geocache was hidden is stored in the enclosing wpt's time
   */
  struct geocache_t {
    geocacheservice_e service = eUnknown;
    bool hasData = false;
    quint32 id = 0;
    bool available = true;
    bool archived = false;
    bool needsMaintenance = false;
    qreal difficulty = 0;
    qreal terrain = 0;
    QString name;
    QString owner;
    QString ownerId;
    QString type;
    QString container;
    // See ZGeocacheAttributes.txt for meanings.
    QMap<quint8, bool> attributes;
    bool shortDescIsHtml = false;
    QString shortDesc;
    bool longDescIsHtml = false;
    QString longDesc;
    QString hint;
    QString country;
    QString state;
    QString locale;
    QList<geocachelog_t> logs;

    const static QList<QString> attributeMeanings;
    static QList<QString> attributeMeaningsTranslated;
    static QList<QString> initAttributeMeaningsTranslated();
    QDateTime getLastFound() const;
    QString getLogs() const;
  };

  struct image_t {
    QImage pixmap;
    qreal direction = 0;
    QString info;
    QString filePath;
    QString fileName;
  };

  CGisItemWpt(const QPointF& pos, qreal ele, const QDateTime& time, const QString& name, const QString& icon,
              IGisProject* project);

  /**
     @brief Create a completely new waypoint
     @param pos       the waypoint's position [°]
     @param name      the waypoint's name
     @param icon      the waypoint's icon
     @param project   the project the waypoint is added to
   */
  CGisItemWpt(const QPointF& pos, const QString& name, const QString& icon, IGisProject* project);
  /**
     @brief Create a copy of an existing waypoint with a new position
     @param pos       the waypoint's new position [°]
     @param parentWpt the waypoint to copy
     @param project   the project the waypoint is added to
   */
  CGisItemWpt(const QPointF& pos, const CGisItemWpt& parentWpt, IGisProject* project);
  /**
     @brief Create a 1:1 copy of an existing waypoint (with new key)
     @param parentWpt the waypoint to copy
     @param project   the project the waypoint is added to
     @param idx       the index to insert the item. If -1 the item will be appended to it's group
   */
  CGisItemWpt(const CGisItemWpt& parentWpt, IGisProject* project, int idx, bool clone);
  /**
     @brief Create item from GPX.
     @param xml       the GPX section containing the item
     @param project   the project to append with item
   */
  CGisItemWpt(const QDomNode& xml, IGisProject* project);

  /**
     @brief Create item from list of changes
     @param hist      the change history
     @param project   the project to append with item
   */
  CGisItemWpt(const history_t& hist, const QString& dbHash, IGisProject* project);

  /**
     @brief Read item from database by it's database ID
     @param id        the item's ID in the database
     @param db        the database itself
     @param project   the project to append with item
   */
  CGisItemWpt(quint64 id, QSqlDatabase& db, IGisProject* project);

  /**
     @brief Read item from text stream with TwoNav encoding
     @param tnvWpt
     @param project
   */
  CGisItemWpt(const CTwoNavProject::wpt_t& tnvWpt, IGisProject* project);

  CGisItemWpt(const CQlgtWpt& wpt1, IGisProject* project = nullptr);

  CGisItemWpt(CFitStream& stream, IGisProject* project);

  CGisItemWpt(const wpt_t& data, IGisProject* project);

  virtual ~CGisItemWpt();

  IGisItem* createClone() override;

  /**
     @brief Save waypoint to GPX tree
     @param gpx   The <gpx> node to append by the waypoint
   */
  void save(QDomNode& gpx, bool strictGpx11) override;
  /**
     @brief Save waypoint to TwoNav waypoint file
     @param out   the text stream to write to
   */
  void saveTwoNav(QTextStream& out, const QDir& dir);
  /**
     @brief Save waypoint to TCX file
     @param  courseNode  The course node to append by the waypoint
     @param  crsPtDateTimeToBeSaved  course point dateTime to be saved (NOT the waypoint date and time !)
   */
  void saveTCX(QDomNode& courseNode, const QDateTime crsPtDateTimeToBeSaved);
  /**
     @brief Read serialized waypoint from a binary data stream
     @param stream  the data stream to read from
     @return A reference to the stream
   */
  QDataStream& operator<<(QDataStream& stream) override;
  /**
     @brief Serialize waypoint into a binary data stream
     @param stream  the data stream to write to.
     @return A reference to the stream
   */
  QDataStream& operator>>(QDataStream& stream) const override;

  void setName(const QString& str);
  void setPosition(const QPointF& pos);
  void setElevation(qint32 val);
  void setProximity(qreal val);
  void setIcon(const QString& name);
  void setComment(const QString& str) override;
  void setDescription(const QString& str) override;
  void setLinks(const QList<link_t>& links) override;
  void setImages(const QList<image_t>& imgs);
  void setTimestamp(const QDateTime& datetime);

  /**
     @brief Silently append list of links

     Devices uses links to reference multimedia content attached to the waypoint.
     These links have to be added to the list of normal links. See removeLinksByType()
     on how to remove these links again.

     @param links  list of links.
   */
  void appendLinks(const QList<link_t>& links) { wpt.links = links + wpt.links; }

  /**
     @brief Silently append list of images

     This is used to restore images from a device. As these images where part of the waypoint
     object in the first place they have to be added to the waypoint again without creating
     a new history entry.

     @param imgs  list of images
   */
  void appendImages(const QList<image_t>& imgs) { images += imgs; }

  /**
     @brief Append the list of images by a single image.
     @param img   a single image
   */
  void addImage(const image_t& img);

  const QString& getName() const override { return wpt.name.isEmpty() ? noName : wpt.name; }

  QString getInfo(quint32 feature) const override;
  QPointF getPosition() const { return QPointF(wpt.lon, wpt.lat); }

  qint32 getElevation() const { return wpt.ele; }
  qreal getProximity() const { return proximity; }
  const QString& getIconName() const { return wpt.sym; }
  const QString& getComment() const override { return wpt.cmt; }
  const QString& getDescription() const override { return wpt.desc; }
  const geocache_t& getGeoCache() const { return geocache; }
  const QList<link_t>& getLinks() const override { return wpt.links; }
  const QList<image_t>& getImages() const { return images; }

  QDateTime getTimestamp() const override { return wpt.time; }

  IScrOpt* getScreenOptions(const QPoint& origin, IMouse* mouse) override;

  QPointF getPointCloseBy(const QPoint& point) override;

  void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) override;
  void drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis) override;
  void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm,
                 CGisDraw* gis) override;
  void drawHighlight(QPainter& p) override;
  bool isCloseTo(const QPointF& pos) override;
  bool isWithin(const QRectF& area, selflags_t flags) override;
  void mouseMove(const QPointF& pos) override;
  void mouseDragged(const QPoint& start, const QPoint& last, const QPoint& pos);
  void dragFinished(const QPoint& pos);
  void leftClicked(const QPoint& pos);
  bool isGeocache() { return geocache.hasData; }

  bool hasRadius() { return proximity < NOFLOAT; }

  qreal getRadius() { return radius; }

  void gainUserFocus(bool yes) override;

  void edit() override;
  void editInitial();

  /**
     @brief Remove all links from the waypoint's link list with a given type

     This is used by devices that use links to attach multimedia items to a waypoint like images.
     These links only make sense on the device. Therefor the links have to be removed after the
     waypoint has been loaded from the device.

     @param type
   */
  void removeLinksByType(const QString& type);

  void toggleBubble();
  bool hasBubble() { return bool(flags & eFlagWptBubble); }

  void setHideArea(bool hide) { hideArea = hide; }

  void genKey() const override;
  const searchValue_t getValueByKeyword(searchProperty_e keyword) override;

  static QString getLastName(const QString& name);
  static void newWpt(const QPointF& pt, const QString& name, const QString& desc, IGisProject* project);
  static void newWpt(const IPoiItem& poi, IGisProject* project, bool openEditWIndow = true);
  static bool getIconAndName(QString& icon, QString& name);

  static void drawCircle(QPainter& p, const QPointF& pos, const qreal& r, const bool& avoid, const bool& selected);
  static qreal calcRadius(const QPointF& posRad, const QPointF& posPx, const qreal& radiusRad, CGisDraw* gis);

  static void init() {
    keywordLambdaMap = initKeywordLambdaMap();
    geocache_t::attributeMeaningsTranslated = geocache_t::initAttributeMeaningsTranslated();
  }

 private:
  void setIcon();
  void setSymbol() override;
  void readGpx(const QDomNode& xml);
  void readTwoNav(const CTwoNavProject::wpt_t& tnvWpt);
  void readWptFromFit(CFitStream& stream);
  void readGcExt(const QDomNode& xmlCache);
  void writeGcExt(QDomNode& xmlCache);
  void drawBubble(QPainter& p);
  QPolygonF makePolyline(const QPointF& anchor, const QRectF& r);
  void processMouseOverBubble(const QPoint& pos);
  void detBoundingRect();

  static key_t keyUserFocus;

  // --- start all waypoint data ----
  wpt_t wpt;
  qreal proximity = NOFLOAT;
  qreal radius = NOFLOAT;
  bool closeToRadius = false;
  bool hideArea = false;
  geocache_t geocache;
  QList<image_t> images;

  QPointF focus;
  QPointF posScreen = NOPOINTF;

  // additional data, common to all IGisItems, is found in IItem //

  // --- stop all waypoint data ----

  QPointer<CScrOptWpt> scrOptWpt;
  QPointer<CScrOptWptRadius> scrOptRadius;

  bool doSpecialCursor = false;
  bool doBubbleMove = false;
  bool doBubbleSize = false;
  bool mouseIsOverBubble = false;
  QRect rectBubble;
  QRect rectBubbleMove{0, 0, 16, 16};
  QRect rectBubbleEdit{0, 0, 16, 16};
  QRect rectBubbleSize{0, 0, 16, 16};

  QPoint offsetMouse;
  QPoint offsetBubble{-320, -150};
  quint32 widthBubble = 300;

  using fSearch = std::function<const searchValue_t(CGisItemWpt*)>;
  static QMap<searchProperty_e, fSearch> keywordLambdaMap;
  static QMap<searchProperty_e, fSearch> initKeywordLambdaMap();
};

#endif  // CGISITEMWPT_H
