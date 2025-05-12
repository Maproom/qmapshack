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

#ifndef CGISITEMRTE_H
#define CGISITEMRTE_H

#include <routino.h>

#include <QPen>
#include <QPointer>

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"

class QDomNode;
class IGisProject;
class CQlgtRoute;
class CScrOptRte;

class CGisItemRte : public IGisItem, public IGisLine {
  Q_DECLARE_TR_FUNCTIONS(CGisItemRte)
 public:
  enum focusmode_e { eFocusMouseMove, eFocusMouseClick };

  struct subpt_t {
    enum type_e { eTypeNone, eTypeWpt, eTypeJunct };

    qreal lon = NOFLOAT;
    qreal lat = NOFLOAT;
    quint8 type = eTypeNone;

    qreal turn = NOFLOAT;
    qreal bearing = NOFLOAT;
    QStringList streets;

    QString instruction;
    qreal distance = 0;
    QDateTime time;

    qint32 ele = NOINT;
  };

  struct rtept_t : public wpt_t {
    rtept_t() { fakeSubpt.type = subpt_t::eTypeWpt; }

    void updateIcon();

    QPixmap icon;
    QPointF focus;

    subpt_t fakeSubpt;
    QVector<subpt_t> subpts;
  };

  struct rte_t {
    // -- all gpx tags - start
    QString name;
    QString cmt;
    QString desc;
    QString src;
    QList<link_t> links;
    quint64 number = 0;
    QString type;
    QVector<rtept_t> pts;
    // -- all gpx tags - stop

    QString lastRoutedWith;
    QDateTime lastRoutedTime;

    qreal totalDistance = NOFLOAT;
    quint32 totalTime = 0;
    qreal ascent = NOFLOAT;
    qreal descent = NOFLOAT;
    qint32 minElevation = NOINT;
    qint32 maxElevation = -NOINT;
  };

  CGisItemRte(const QDomNode& xml, IGisProject* parent);
  CGisItemRte(const CGisItemRte& parentRte, IGisProject* project, int idx, bool clone);
  CGisItemRte(const history_t& hist, const QString& dbHash, IGisProject* project);
  CGisItemRte(quint64 id, QSqlDatabase& db, IGisProject* project);
  CGisItemRte(const CQlgtRoute& rte1, IGisProject* project = nullptr);
  CGisItemRte(const SGisLine& l, const QString& name, IGisProject* project, int idx);
  virtual ~CGisItemRte();

  IGisItem* createClone() override;

  QDataStream& operator<<(QDataStream& stream) override;
  QDataStream& operator>>(QDataStream& stream) const override;

  const QString& getName() const override { return rte.name.isEmpty() ? noName : rte.name; }

  QString getInfo(quint32 feature) const override;
  IScrOpt* getScreenOptions(const QPoint& origin, IMouse* mouse) override;
  QPointF getPointCloseBy(const QPoint& screenPos) override;
  void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) override;
  void drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis) override;
  void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm,
                 CGisDraw* gis) override;
  void drawHighlight(QPainter& p) override;
  void save(QDomNode& gpx, bool strictGpx11) override;
  bool isCloseTo(const QPointF& pos) override;
  bool isWithin(const QRectF& area, selflags_t flags) override;
  /**
     @brief Switch user focus on and off.

     If the focus is switched on any other route having the focus will loose it.

     @param yes   set true to gain focus.
   */
  void gainUserFocus(bool yes) override;
  /**
     @brief Make sure the route has lost focus.

     If the route has the focus, keyUserFocus will be reset. In all other cases nothing will be done.

   */
  void looseUserFocus();
  /**
     @brief Check for user focus

     @return True if the route has user focus
   */
  bool hasUserFocus() const override { return key == keyUserFocus; }

  /**
     @brief Get the key of the current track with user focus

     @return If no route has the focus an empty string is returned
   */
  static const key_t& getKeyUserFocus() { return keyUserFocus; }

  void setDataFromPolyline(const SGisLine& l) override;

  void getPolylineFromData(SGisLine& l) const override;
  void getPolylineDegFromData(QPolygonF& polygon) const override;

  const QString& getComment() const override { return rte.cmt; }
  const QString& getDescription() const override { return rte.desc; }
  const QList<link_t>& getLinks() const override { return rte.links; }

  const rte_t& getRoute() const { return rte; }

  QDateTime getTimestamp() const override { return QDateTime(); }

  void setName(const QString& str);
  void setComment(const QString& str) override;
  void setDescription(const QString& str) override;
  void setLinks(const QList<link_t>& links) override;

  void calc();
  void reset();
  void edit() override;
  void reverse();
  void toTrack();

  QPointF setMouseFocusByPoint(const QPoint& pt, focusmode_e fmode, const QString& owner);

  void setResult(Routino_Output* route, const QString& options);
  void setResult(const QDomDocument& xml, const QString& options);
  void setResultFromBRouter(const QDomDocument& xml, const QString& options);

  bool isCalculated();

  const searchValue_t getValueByKeyword(searchProperty_e keyword) override;

 private:
  void deriveSecondaryData();
  void setElevation(qreal ele, subpt_t& subpt, qreal& lastEle);
  void setSymbol() override;
  void readRte(const QDomNode& xml, rte_t& rte);
  void readRouteDataFromGisLine(const SGisLine& l);
  const subpt_t* getSubPtByIndex(quint32 idx);

  static key_t keyUserFocus;

  static const QPen penBackground;
  QPen penForeground{Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
  QPen penForegroundFocus{Qt::magenta, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};

  rte_t rte;
  QPolygonF line;

  const subpt_t* mouseMoveFocus = nullptr;

  QPointer<CScrOptRte> scrOpt;

  using fSearch = std::function<const searchValue_t(CGisItemRte*)>;
  static QMap<searchProperty_e, fSearch> keywordLambdaMap;
  static QMap<searchProperty_e, fSearch> initKeywordLambdaMap();
};

#endif  // CGISITEMRTE_H
