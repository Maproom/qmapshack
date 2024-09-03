/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
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

#include "gis/ovl/CGisItemOvlArea.h"

#include <QtWidgets>

#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/GeoMath.h"
#include "gis/ovl/CDetailsOvlArea.h"
#include "gis/ovl/CScrOptOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/proj_x.h"
#include "helpers/CDraw.h"

#define DEFAULT_COLOR 4
#define MIN_DIST_CLOSE_TO 10

IGisItem::key_t CGisItemOvlArea::keyUserFocus;

CGisItemOvlArea::CGisItemOvlArea(const SGisLine& line, const QString& name, IGisProject* project, int idx)
    : IGisItem(project, eTypeOvl, idx) {
  area.name = name;
  readAreaDataFromGisLine(line);

  flags |= eFlagCreatedInQms | eFlagWriteAllowed;

  setColor(str2color(""));
  setupHistory();
  updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemOvlArea::CGisItemOvlArea(const CGisItemOvlArea& parentArea, IGisProject* project, int idx, bool clone)
    : IGisItem(project, eTypeOvl, idx) {
  history = parentArea.history;
  loadHistory(history.histIdxCurrent);

  if (clone) {
    area.name += tr("_Clone");
    key.clear();
    history.events.clear();
    setupHistory();
  }

  if (parentArea.isOnDevice() || !parentArea.isReadOnly()) {
    flags |= eFlagWriteAllowed;
  } else {
    flags &= ~eFlagWriteAllowed;
  }

  setNogoFlag(parentArea.isNogo());

  deriveSecondaryData();
  updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemOvlArea::CGisItemOvlArea(const QDomNode& xml, IGisProject* project)
    : IGisItem(project, eTypeOvl, project->childCount()) {
  // --- start read and process data ----
  setColor(penForeground.color());
  readArea(xml, area);
  // --- stop read and process data ----

  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);
}

CGisItemOvlArea::CGisItemOvlArea(const history_t& hist, const QString& dbHash, IGisProject* project)
    : IGisItem(project, eTypeOvl, project->childCount()) {
  history = hist;
  loadHistory(hist.histIdxCurrent);
  if (!dbHash.isEmpty()) {
    lastDatabaseHash = dbHash;
  }
}

CGisItemOvlArea::CGisItemOvlArea(quint64 id, QSqlDatabase& db, IGisProject* project)
    : IGisItem(project, eTypeOvl, NOIDX) {
  loadFromDb(id, db);
}

CGisItemOvlArea::~CGisItemOvlArea() {
  // reset user focus if focused on this track
  if (key == keyUserFocus) {
    keyUserFocus.clear();
  }
}

IGisItem* CGisItemOvlArea::createClone() {
  int idx = -1;
  IGisProject* project = getParentProject();
  if (project) {
    idx = project->indexOfChild(this);
  }
  return new CGisItemOvlArea(*this, project, idx, true);
}

void CGisItemOvlArea::setSymbol() { setColor(str2color(area.color)); }

bool CGisItemOvlArea::isCloseTo(const QPointF& pos) {
  QMutexLocker lock(&mutexItems);

  qreal dist = GPS_Math_DistPointPolyline(polygonArea, pos);
  return dist < 20;
}

bool CGisItemOvlArea::isWithin(const QRectF& area, selflags_t flags) {
  QPolygonF l;
  getPolylineDegFromData(l);
  return (flags & eSelectionOvl) ? IGisItem::isWithin(area, flags, l) : false;
}

QPointF CGisItemOvlArea::getPointCloseBy(const QPoint& screenPos) {
  QMutexLocker lock(&mutexItems);

  qint32 i = 0;
  qint32 idx = NOIDX;
  qint32 d = NOINT;
  for (const QPointF& point : std::as_const(polygonArea)) {
    int tmp = (screenPos - point).manhattanLength();
    if (tmp < d) {
      idx = i;
      d = tmp;
    }
    i++;
  }

  return (idx < 0) ? NOPOINTF : polygonArea[idx];
}

void CGisItemOvlArea::readAreaDataFromGisLine(const SGisLine& l) {
  QMutexLocker lock(&mutexItems);

  area.pts.clear();

  for (int i = 0; i < l.size(); i++) {
    area.pts << pt_t();

    pt_t& areapt = area.pts.last();
    const point_t& pt = l[i];

    areapt.lon = pt.coord.x() * RAD_TO_DEG;
    areapt.lat = pt.coord.y() * RAD_TO_DEG;

    for (int n = 0; n < pt.subpts.size(); n++) {
      area.pts << pt_t();
      pt_t& areapt = area.pts.last();
      const subpt_t& sub = pt.subpts[n];

      areapt.lon = sub.coord.x() * RAD_TO_DEG;
      areapt.lat = sub.coord.y() * RAD_TO_DEG;
    }
  }

  deriveSecondaryData();
}

void CGisItemOvlArea::edit() {
  CDetailsOvlArea dlg(*this, nullptr);
  dlg.exec();

  deriveSecondaryData();
}

void CGisItemOvlArea::deriveSecondaryData() {
  qreal north = -90;
  qreal east = -180;
  qreal south = 90;
  qreal west = 180;

  for (const pt_t& pt : std::as_const(area.pts)) {
    if (pt.lon < west) {
      west = pt.lon;
    }
    if (pt.lon > east) {
      east = pt.lon;
    }
    if (pt.lat < south) {
      south = pt.lat;
    }
    if (pt.lat > north) {
      north = pt.lat;
    }
  }

  boundingRect = QRectF(QPointF(west * DEG_TO_RAD, north * DEG_TO_RAD), QPointF(east * DEG_TO_RAD, south * DEG_TO_RAD));

  QPolygonF line(area.pts.size());
  for (int i = 1; i < area.pts.size(); i++) {
    qreal a1, a2, d;
    const pt_t& pt11 = area.pts[i - 1];
    const pt_t& pt12 = area.pts[i];

    QPointF& pt21 = line[i - 1];
    QPointF& pt22 = line[i];

    d = GPS_Math_Distance(pt11.lon * DEG_TO_RAD, pt11.lat * DEG_TO_RAD, pt12.lon * DEG_TO_RAD, pt12.lat * DEG_TO_RAD,
                          a1, a2);

    pt22.rx() = pt21.x() + qCos(a1 * DEG_TO_RAD) * d;
    pt22.ry() = pt21.y() + qSin(a1 * DEG_TO_RAD) * d;
  }

  area.area = 0;
  int j = line.size() - 1;
  for (int i = 0; i < line.size(); i++) {
    area.area += (line[j].x() + line[i].x()) * (line[j].y() - line[i].y());
    j = i;
  }

  area.area = qAbs(area.area / 2);
}

void CGisItemOvlArea::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& /*blockedAreas*/, CGisDraw* gis) {
  QMutexLocker lock(&mutexItems);

  polygonArea.clear();
  if (!isVisible(boundingRect, viewport, gis)) {
    return;
  }

  QPointF pt1;

  for (const pt_t& pt : std::as_const(area.pts)) {
    pt1.setX(pt.lon);
    pt1.setY(pt.lat);
    pt1 *= DEG_TO_RAD;
    polygonArea << pt1;
  }

  gis->convertRad2Px(polygonArea);

  p.save();
  p.setOpacity(area.opacity ? 0.3 : 1.0);

  penBackground.setWidth(area.width + 2);
  p.setBrush(Qt::NoBrush);
  p.setPen(penBackground);
  p.drawPolygon(polygonArea);

  penForeground.setColor(color);
  penForeground.setWidth(area.width);
  p.setBrush(isNogo() ? getNogoTextureBrush() : QBrush(color, (Qt::BrushStyle)area.style));
  p.setPen(penForeground);
  p.drawPolygon(polygonArea);

  // close polygon (required by isCloseTo)
  const pt_t& pt = area.pts.first();
  pt1.setX(pt.lon);
  pt1.setY(pt.lat);
  pt1 *= DEG_TO_RAD;
  gis->convertRad2Px(pt1);
  polygonArea << pt1;

  p.restore();
}

void CGisItemOvlArea::drawLabel(QPainter& p, const QPolygonF& /*viewport*/, QList<QRectF>& blockedAreas,
                                const QFontMetricsF& fm, CGisDraw* /*gis*/) {
  QMutexLocker lock(&mutexItems);

  if (polygonArea.isEmpty()) {
    return;
  }
  QPointF pt = getPolygonCentroid(polygonArea);
  QRectF rect = fm.boundingRect(area.name);
  rect.adjust(-2, -2, 2, 2);
  rect.moveCenter(pt);

  CDraw::text(getName(), p, pt, Qt::darkBlue);
  blockedAreas << rect;
}

void CGisItemOvlArea::drawHighlight(QPainter& p) {
  QMutexLocker lock(&mutexItems);

  if (polygonArea.isEmpty() || key == keyUserFocus) {
    return;
  }
  p.setBrush(Qt::NoBrush);
  p.setPen(QPen(QColor(255, 0, 0, 100), 11, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  p.drawPolygon(polygonArea);
}

void CGisItemOvlArea::gainUserFocus(bool yes) { keyUserFocus = yes ? key : key_t(); }

const searchValue_t CGisItemOvlArea::getValueByKeyword(searchProperty_e keyword) {
  if (keywordLambdaMap.contains(keyword)) {
    return keywordLambdaMap.value(keyword)(this);
  }
  return searchValue_t();
}

QPointF CGisItemOvlArea::getPolygonCentroid(const QPolygonF& polygon) {
  qreal x = 0;
  qreal y = 0;
  int len = polygon.size();

  for (int i = 0; i < len; i++) {
    x = x + polygon[i].x();
    y = y + polygon[i].y();
  }
  x = x / len;
  y = y / len;

  return QPointF(x, y);
}

IScrOpt* CGisItemOvlArea::getScreenOptions(const QPoint& origin, IMouse* mouse) {
  if (scrOpt.isNull()) {
    scrOpt = new CScrOptOvlArea(this, origin, mouse);
  }
  return scrOpt;
}

QString CGisItemOvlArea::getInfo(quint32 feature) const {
  QString unit, val;
  QString str = "<div>";

  if (feature & eFeatureShowName) {
    str += "<b>" + getName() + "</b>";
  }

  IUnit::self().meter2area(area.area, val, unit);
  str += "<br/>\n" + tr("Area: %1%2").arg(val, unit);

  QString desc = removeHtml(area.desc).simplified();
  if (desc.count()) {
    if (!str.isEmpty()) {
      str += "<br/>\n";
    }

    if ((feature & eFeatureShowFullText) || (desc.count() < 300)) {
      str += desc;
    } else {
      str += desc.left(297) + "...";
    }
  }

  QString cmt = removeHtml(area.cmt).simplified();
  if ((cmt != desc) && cmt.count()) {
    if (!str.isEmpty()) {
      str += "<br/>\n";
    }

    if ((feature & eFeatureShowFullText) || (cmt.count() < 300)) {
      str += cmt;
    } else {
      str += cmt.left(297) + "...";
    }
  }

  if ((feature & eFeatureShowLinks) && !area.links.isEmpty()) {
    for (const link_t& link : area.links) {
      if (link.type.isEmpty() || (link.type == "text/html")) {
        str += "<br/>\n";
        str += QString("<a href='%1'>%2</a>").arg(link.uri.toString(), link.text);
      }
    }
  }

  str += getRatingKeywordInfo();

  return str + "</div>";
}

void CGisItemOvlArea::getPolylineFromData(SGisLine& l) const {
  QMutexLocker lock(&mutexItems);

  l.clear();
  for (const pt_t& pt : area.pts) {
    l << point_t(QPointF(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD));
  }
}

void CGisItemOvlArea::getPolylineDegFromData(QPolygonF& polygon) const {
  QMutexLocker lock(&mutexItems);

  polygon.clear();
  for (const pt_t& pt : area.pts) {
    polygon << QPointF(pt.lon, pt.lat);
  }
}

void CGisItemOvlArea::setDataFromPolyline(const SGisLine& l) {
  QMutexLocker lock(&mutexItems);

  readAreaDataFromGisLine(l);

  flags |= eFlagTainted;

  changed(tr("Changed area shape."), "://icons/48x48/AreaMove.png");
  updateDecoration(eMarkChanged, eMarkNone);
}

void CGisItemOvlArea::setName(const QString& str) {
  setText(CGisListWks::eColumnName, str);
  area.name = str;
  changed(tr("Changed name."), "://icons/48x48/EditText.png");
}

void CGisItemOvlArea::setWidth(qint32 w) {
  area.width = w;
  changed(tr("Changed border width."), "://icons/48x48/TextBold.png");
}

void CGisItemOvlArea::setStyle(qint32 s) {
  area.style = s;
  changed(tr("Changed fill pattern."), "://icons/48x48/Pattern.png");
}

void CGisItemOvlArea::setOpacity(bool yes) {
  area.opacity = yes;
  changed(tr("Changed opacity."), "://icons/48x48/Opacity.png");
}

void CGisItemOvlArea::setComment(const QString& str) {
  area.cmt = str;
  changed(tr("Changed comment."), "://icons/48x48/EditText.png");
}

void CGisItemOvlArea::setDescription(const QString& str) {
  area.desc = str;
  changed(tr("Changed description."), "://icons/48x48/EditText.png");
}

void CGisItemOvlArea::setLinks(const QList<link_t>& links) {
  area.links = links;
  changed(tr("Changed links"), "://icons/48x48/Link.png");
}

void CGisItemOvlArea::setColor(int idx) {
  if (idx >= colorMap.size()) {
    return;
  }
  setColor(colorMap[idx].color);
  changed(tr("Changed color"), "://icons/48x48/SelectColor.png");
}

void CGisItemOvlArea::setColor(const QColor& c) {
  int n;

  for (n = 0; n < colorMap.size(); n++) {
    if (colorMap[n].color == c) {
      colorIdx = n;
      color = colorMap[n].color;
      bullet = QPixmap(colorMap[n].bullet);
      break;
    }
  }

  if (n == colorMap.size()) {
    colorIdx = DEFAULT_COLOR;
    color = colorMap[DEFAULT_COLOR].color;
    bullet = QPixmap(colorMap[DEFAULT_COLOR].bullet);
  }

  setIcon(color.name());
}

void CGisItemOvlArea::setIcon(const QString& c) {
  area.color = c;
  QPixmap icon = QPixmap("://icons/48x48/Area.png");

  QPixmap mask(icon.size());
  mask.fill(str2color(c));
  mask.setMask(icon.createMaskFromColor(Qt::transparent));
  IGisItem::setIcon(mask.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QMap<searchProperty_e, CGisItemOvlArea::fSearch> CGisItemOvlArea::keywordLambdaMap =
    CGisItemOvlArea::initKeywordLambdaMap();
QMap<searchProperty_e, CGisItemOvlArea::fSearch> CGisItemOvlArea::initKeywordLambdaMap() {
  QMap<searchProperty_e, CGisItemOvlArea::fSearch> map;
  map.insert(eSearchPropertyGeneralName, [](CGisItemOvlArea* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getName();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralFullText, [](CGisItemOvlArea* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getInfo(eFeatureShowFullText | eFeatureShowName);
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralComment, [](CGisItemOvlArea* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getComment();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralDescription, [](CGisItemOvlArea* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getDescription();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralRating, [](CGisItemOvlArea* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getRating();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralKeywords, [](CGisItemOvlArea* item) {
    searchValue_t searchValue;
    searchValue.str1 = QStringList(item->getKeywords().values()).join(", ");
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralType, [](CGisItemOvlArea* /*item*/) {
    searchValue_t searchValue;
    searchValue.str1 = tr("area");
    return searchValue;
  });
  // Area specific
  map.insert(eSearchPropertyAreaArea, [](CGisItemOvlArea* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->area.area;
    return searchValue;
  });
  return map;
}
