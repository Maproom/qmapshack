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

#include "gis/wpt/CGisItemWpt.h"

#include <QPainterPath>
#include <QtWidgets>
#include <QtXml>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/GeoMath.h"
#include "gis/prj/IGisProject.h"
#include "gis/wpt/CDetailsGeoCache.h"
#include "gis/wpt/CDetailsWpt.h"
#include "gis/wpt/CScrOptWpt.h"
#include "gis/wpt/CScrOptWptRadius.h"
#include "gis/wpt/CSetupIconAndName.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconManager.h"
#include "mouse/IMouse.h"
#include "poi/IPoiItem.h"
#include "units/IUnit.h"

IGisItem::key_t CGisItemWpt::keyUserFocus;
QMap<searchProperty_e, CGisItemWpt::fSearch> CGisItemWpt::keywordLambdaMap;
QList<QString> CGisItemWpt::geocache_t::attributeMeaningsTranslated;

CGisItemWpt::CGisItemWpt(const QPointF& pos, qreal ele, const QDateTime& time, const QString& name, const QString& icon,
                         IGisProject* project)
    : IGisItem(project, eTypeWpt, NOIDX) {
  wpt.name = name;
  wpt.sym = icon;
  wpt.lon = pos.x();
  wpt.lat = pos.y();
  wpt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);
  wpt.time = time;

  detBoundingRect();

  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);
}

/// used to add a new waypoint
CGisItemWpt::CGisItemWpt(const QPointF& pos, const QString& name, const QString& icon, IGisProject* project)
    : CGisItemWpt(pos, NOFLOAT, QDateTime::currentDateTimeUtc(), name, icon, project) {
  flags = eFlagCreatedInQms | eFlagWriteAllowed;
  qreal ele = CMainWindow::self().getElevationAt(pos * DEG_TO_RAD);
  wpt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);

  detBoundingRect();

  setupHistory();
  updateDecoration(eMarkChanged, eMarkNone);
}

/// used to move a copy of waypoint
CGisItemWpt::CGisItemWpt(const QPointF& pos, const CGisItemWpt& parentWpt, IGisProject* project)
    : IGisItem(project, eTypeWpt, NOIDX) {
  *this = parentWpt;
  wpt.lon = pos.x();
  wpt.lat = pos.y();
  wpt.time = QDateTime::currentDateTimeUtc();

  key.clear();
  history.events.clear();
  flags = eFlagCreatedInQms | eFlagWriteAllowed;

  qreal ele = CMainWindow::self().getElevationAt(pos * DEG_TO_RAD);
  wpt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);

  setNogoFlag(parentWpt.isNogo());

  detBoundingRect();

  setupHistory();
  updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create a copy of waypoint with new parent
CGisItemWpt::CGisItemWpt(const CGisItemWpt& parentWpt, IGisProject* project, int idx, bool clone)
    : IGisItem(project, eTypeWpt, idx) {
  history = parentWpt.history;
  loadHistory(history.histIdxCurrent);

  if (clone) {
    wpt.name += tr("_Clone");
    key.clear();
    history.events.clear();
    setupHistory();
  }

  if (parentWpt.isOnDevice() || !parentWpt.isReadOnly()) {
    flags |= eFlagWriteAllowed;
  } else {
    flags &= ~eFlagWriteAllowed;
  }

  setNogoFlag(parentWpt.isNogo());

  detBoundingRect();
  updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create waypoint from GPX file
CGisItemWpt::CGisItemWpt(const QDomNode& xml, IGisProject* project)
    : IGisItem(project, eTypeWpt, project->childCount()) {
  readGpx(xml);
  detBoundingRect();

  CGisItemWpt::genKey();
  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::CGisItemWpt(const history_t& hist, const QString& dbHash, IGisProject* project)
    : IGisItem(project, eTypeWpt, project->childCount()) {
  history = hist;
  loadHistory(hist.histIdxCurrent);
  detBoundingRect();
  if (!dbHash.isEmpty()) {
    lastDatabaseHash = dbHash;
  }
}

CGisItemWpt::CGisItemWpt(quint64 id, QSqlDatabase& db, IGisProject* project) : IGisItem(project, eTypeWpt, NOIDX) {
  loadFromDb(id, db);
  detBoundingRect();
}

CGisItemWpt::CGisItemWpt(const CTwoNavProject::wpt_t& tnvWpt, IGisProject* project)
    : IGisItem(project, eTypeWpt, NOIDX) {
  readTwoNav(tnvWpt);
  detBoundingRect();

  CGisItemWpt::genKey();
  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::CGisItemWpt(CFitStream& stream, IGisProject* project)
    : IGisItem(project, eTypeWpt, NOIDX), proximity(NOFLOAT), posScreen(NOPOINTF) {
  readWptFromFit(stream);
  detBoundingRect();

  CGisItemWpt::genKey();
  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::CGisItemWpt(const wpt_t& data, IGisProject* project)
    : IGisItem(project, eTypeWpt, NOIDX), proximity(NOFLOAT), posScreen(NOPOINTF) {
  wpt = data;
  detBoundingRect();

  CGisItemWpt::genKey();
  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::~CGisItemWpt() {}

IGisItem* CGisItemWpt::createClone() {
  int idx = -1;
  IGisProject* project = getParentProject();
  if (project) {
    idx = project->indexOfChild(this);
  }
  return new CGisItemWpt(*this, project, idx, true);
}

void CGisItemWpt::setSymbol() { setIcon(); }

void CGisItemWpt::genKey() const {
  if (geocache.hasData) {
    key.item = QString::number(geocache.id);
  }
  IGisItem::genKey();
}

QString CGisItemWpt::getLastName(const QString& name) {
  SETTINGS;
  QString lastName = name;

  if (lastName.isEmpty()) {
    lastName = cfg.value("Waypoint/lastName", "wpt").toString();
  }

  const int s = lastName.size();
  if (s != 0) {
    int idx;
    for (idx = s; idx > 0; idx--) {
      if (!lastName[idx - 1].isDigit()) {
        break;
      }
    }

    if (idx == 0) {
      lastName = QString::number(lastName.toInt() + 1);
    } else if (idx < s) {
      lastName = lastName.left(idx) + QString::number(lastName.mid(idx).toInt() + 1);
    }
  }

  cfg.setValue("Waypoint/lastName", lastName);
  return lastName;
}

bool CGisItemWpt::getIconAndName(QString& icon, QString& name) {
  SETTINGS;
  QString lastIcon = cfg.value("Waypoint/lastIcon", "Waypoint").toString();

  if (name.isEmpty()) {
    name = getLastName("");
  }
  icon = lastIcon;

  CSetupIconAndName dlg(icon, name, CMainWindow::getBestWidgetForParent());
  if (dlg.exec() != QDialog::Accepted) {
    return false;
  }

  cfg.setValue("Waypoint/lastName", name);
  cfg.setValue("Waypoint/lastIcon", icon);

  return true;
}

void CGisItemWpt::newWpt(const QPointF& pt, const QString& name, const QString& desc, IGisProject* project) {
  SETTINGS;
  const QString& _icon = cfg.value("Waypoint/lastIcon", "Waypoint").toString();
  const QString& _name = name.isEmpty() ? getLastName("") : name;

  CGisItemWpt* wpt = new CGisItemWpt(pt, _name, _icon, project);
  if (!desc.isEmpty()) {
    wpt->setDescription(desc);
  }
  wpt->editInitial();

  cfg.setValue("Waypoint/lastName", wpt->getName());
  cfg.setValue("Waypoint/lastIcon", wpt->getIconName());
}

void CGisItemWpt::newWpt(const IPoiItem& poi, IGisProject* project, bool openEditWIndow) {
  SETTINGS;
  const QString& _icon = poi.icon.isEmpty() ? cfg.value("Waypoint/lastIcon", "Waypoint").toString() : poi.icon;
  const QString& _name = poi.name.isEmpty() ? getLastName("") : poi.name;

  CGisItemWpt* wpt = new CGisItemWpt(poi.pos * RAD_TO_DEG, _name, _icon, project);
  if (!poi.desc.isEmpty()) {
    wpt->setDescription(poi.desc);
  }
  if (!poi.links.isEmpty()) {
    wpt->setLinks(poi.links);
  }
  if (poi.ele != NOINT) {
    wpt->setElevation(poi.ele);
  }
  if (openEditWIndow) {
    wpt->editInitial();
  }

  wpt->setReadOnlyMode(true);
  cfg.setValue("Waypoint/lastName", wpt->getName());
  cfg.setValue("Waypoint/lastIcon", wpt->getIconName());
}

QString CGisItemWpt::getInfo(quint32 feature) const {
  QString str = "<div>";
  qint32 initialSize = str.size();

  if (feature & eFeatureShowName) {
    str = "<b>" + getName() + "</b>";
  }

  if (wpt.ele != NOINT) {
    if (str.size() > initialSize) {
      str += "<br/>\n";
    }
    QString val, unit;
    IUnit::self().meter2elevation(wpt.ele, val, unit);
    str += tr("Elevation: %1%2").arg(val, unit);
  }

  if (proximity != NOFLOAT) {
    if (str.size() > initialSize) {
      str += "<br/>\n";
    }
    QString val, unit;
    IUnit::self().meter2distance(proximity, val, unit);
    str += tr("Proximity: %1%2").arg(val, unit);
  }

  QString desc = removeHtml(wpt.desc).simplified();
  if (geocache.hasData) {
    if (str.size() > initialSize) {
      str += "<br/>\n";
    }

    str += QString(" %4 (%1, D %2, T %3)")
               .arg(geocache.container)
               .arg(geocache.difficulty, 0, 'f', 1)
               .arg(geocache.terrain, 0, 'f', 1)
               .arg(geocache.name);

    const QDateTime& lastFound = geocache.getLastFound();
    if (lastFound.isValid()) {
      str += "<br/>" + tr("Last found: %1").arg(IUnit::datetime2string(lastFound, IUnit::eTimeFormatLong, wpt));
    }

    const IGisProject* project = getParentProject();
    if (project != nullptr) {
      const QDateTime& projectDate = getParentProject()->getTime();
      if (projectDate.isValid()) {
        str +=
            "<br/>" + tr("Project created: %1").arg(IUnit::datetime2string(projectDate, IUnit::eTimeFormatLong, wpt));
      }
    }
  } else {
    if (desc.length()) {
      if (str.size() > initialSize) {
        str += "<br/>\n";
      }

      if ((feature & eFeatureShowFullText) || (desc.length() < 300)) {
        str += desc;
      } else {
        str += desc.left(297) + "...";
      }
    }
  }

  QString cmt = removeHtml(wpt.cmt).simplified();
  if ((cmt != desc) && cmt.length()) {
    if (str.size() > initialSize) {
      str += "<br/>\n";
    }

    if ((feature & eFeatureShowFullText) || (cmt.length() < 300)) {
      str += cmt;
    } else {
      str += cmt.left(297) + "...";
    }
  }
  if (feature & eFeatureShowDateTime) {
    if (wpt.time.isValid()) {
      if (str.size() > initialSize) {
        str += "<br/>\n";
      }
      str += tr("Created: %1")
                 .arg(IUnit::datetime2string(wpt.time, IUnit::eTimeFormatLong,
                                             QPointF(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD)));
    }
  }

  if ((feature & eFeatureShowLinks) && !wpt.links.isEmpty()) {
    for (const link_t& link : wpt.links) {
      if (link.type.isEmpty() || (link.type == "text/html")) {
        str += "<br/>\n";
        str += QString("<a href='%1'>%2</a>").arg(link.uri.toString(), link.text);
      }
    }
    // Add logging link separately, since the link to the geocache site is extracted from the gpx file.
    if (geocache.hasData && geocache.service == eGcCom) {
      str += " <a href='https://www.geocaching.com/play/geocache/" + wpt.name + "/log'>Log Geocache</a>";
    }
  }

  str += getRatingKeywordInfo();

  return str + "</div>";
}

IScrOpt* CGisItemWpt::getScreenOptions(const QPoint& origin, IMouse* mouse) {
  if (closeToRadius) {
    if (scrOptRadius.isNull()) {
      scrOptRadius = new CScrOptWptRadius(this, origin, mouse);
    }
    return scrOptRadius;
  } else {
    if (scrOptWpt.isNull()) {
      scrOptWpt = new CScrOptWpt(this, origin, mouse);
    }
    return scrOptWpt;
  }
}

QPointF CGisItemWpt::getPointCloseBy(const QPoint& point) {
  if (closeToRadius) {
    QPointF l = (QPointF(point) - posScreen);
    return posScreen + l * (radius / sqrt(QPointF::dotProduct(l, l)));
  } else {
    return posScreen;
  }
}

void CGisItemWpt::setIcon() {
  if (geocache.hasData) {
    if (geocache.available) {
      IGisItem::setIcon(CWptIconManager::self().getWptIconByName(geocache.type, focus));
    } else {
      IGisItem::setIcon(CWptIconManager::self().getWptIconByName("gray_" + geocache.type, focus));
    }
  } else {
    IGisItem::setIcon(CWptIconManager::self().getWptIconByName(wpt.sym, focus));
  }
}

void CGisItemWpt::setName(const QString& str) {
  SETTINGS;
  cfg.setValue("Waypoint/lastName", str);

  setText(CGisListWks::eColumnName, str);

  wpt.name = str;
  changed(tr("Changed name"), "://icons/48x48/EditText.png");
}

void CGisItemWpt::setPosition(const QPointF& pos) {
  wpt.lon = pos.x();
  wpt.lat = pos.y();

  detBoundingRect();

  changed(tr("Changed position"), "://icons/48x48/WptMove.png");
}

void CGisItemWpt::setElevation(qint32 val) {
  wpt.ele = val;
  changed(tr("Changed elevation"), "://icons/48x48/SetEle.png");
}

void CGisItemWpt::setProximity(qreal val) {
  if (val == NOFLOAT) {
    proximity = NOFLOAT;
    setNogoFlag(false);
    changed(tr("Removed proximity"), "://icons/48x48/WptDelProx.png");
  } else {
    proximity = qRound(val);
    changed(tr("Changed proximity"), "://icons/48x48/WptEditProx.png");
  }

  detBoundingRect();

  radius = NOFLOAT;  // radius is proximity in set on redraw
}

void CGisItemWpt::setIcon(const QString& name) {
  SETTINGS;
  cfg.setValue("Waypoint/lastIcon", name);

  wpt.sym = name;

  QPointF focus;
  QString path;
  CWptIconManager::self().getWptIconByName(name, focus, &path);

  changed(tr("Changed icon"), path);
}

void CGisItemWpt::setComment(const QString& str) {
  wpt.cmt = str;
  changed(tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemWpt::setDescription(const QString& str) {
  wpt.desc = str;
  changed(tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemWpt::setLinks(const QList<link_t>& links) {
  wpt.links = links;
  changed(tr("Changed links"), "://icons/48x48/Link.png");
}

void CGisItemWpt::setImages(const QList<image_t>& imgs) {
  images = imgs;
  changed(tr("Changed images"), "://icons/48x48/Image.png");
}

void CGisItemWpt::addImage(const image_t& img) {
  images.append(img);
  changed(tr("Add image"), "://icons/48x48/Image.png");
}

void CGisItemWpt::setTimestamp(const QDateTime& datetime) {
  wpt.time = datetime;
  changed(tr("Changed timestamp"), "://icons/48x48/Time.png");
}

bool CGisItemWpt::isCloseTo(const QPointF& pos) {
  closeToRadius = false;

  if (posScreen == NOPOINTF) {
    return false;
  }

  QPointF dist = (pos - posScreen);
  if (dist.manhattanLength() < 22) {
    return true;
  }
  if (radius == NOFLOAT) {
    return false;
  }

  closeToRadius = abs(QPointF::dotProduct(dist, dist) / radius - radius) < 22;
  return closeToRadius;
}

bool CGisItemWpt::isWithin(const QRectF& area, selflags_t flags) {
  return (flags & eSelectionWpt) ? area.contains(QPointF(wpt.lon, wpt.lat)) : false;
}

void CGisItemWpt::gainUserFocus(bool yes) { keyUserFocus = yes ? key : key_t(); }

void CGisItemWpt::edit() {
  if (geocache.hasData) {
    CDetailsGeoCache dlg(*this, CMainWindow::getBestWidgetForParent());
    dlg.exec();
  } else {
    CDetailsWpt dlg(*this, CMainWindow::getBestWidgetForParent());
    dlg.exec();
  }
}

void CGisItemWpt::editInitial() {
  CDetailsWpt dlg(*this, CMainWindow::getBestWidgetForParent());
  dlg.disableHistory();
  dlg.exec();
  squashHistory();
}

void CGisItemWpt::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) {
  posScreen = QPointF(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);

  if (proximity == NOFLOAT || proximity == 0. ? !isVisible(posScreen, viewport, gis)
                                              : !isVisible(boundingRect, viewport, gis)) {
    rectBubble = QRect();
    posScreen = NOPOINTF;
    return;
  }

  gis->convertRad2Px(posScreen);

  if (proximity == NOFLOAT) {
    radius = NOFLOAT;
  } else {
    // remember radius for isCloseTo-method
    radius = calcRadius(QPointF(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD), posScreen, proximity, gis);

    drawCircle(p, posScreen, radius, !hideArea && isNogo(), false);
  }

  drawBubble(p);

  p.drawPixmap(posScreen - focus, icon);

  blockedAreas << QRectF(posScreen - focus, icon.size());
}

void CGisItemWpt::drawItem(QPainter& p, const QRectF& /*viewport*/, CGisDraw* gis) {
  if (mouseIsOverBubble && !doBubbleMove && !doBubbleSize && rectBubble.isValid() && !isReadOnly()) {
    QPainterPath clip;
    clip.addRoundedRect(rectBubble, RECT_RADIUS, RECT_RADIUS);
    p.setClipPath(clip);

    QRect barTop(rectBubble.topLeft(), QSize(rectBubble.width(), 26));
    QRect barBottom(barTop);
    barBottom.moveBottomLeft(rectBubble.bottomLeft());
    barBottom.adjust(1, 0, -1, -1);
    barTop.adjust(1, 1, -1, 0);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(200, 200, 255, 150));
    p.drawRect(barTop);
    p.drawRect(barBottom);

    p.setBrush(Qt::white);
    p.drawRoundedRect(rectBubbleMove.adjusted(-2, -2, 2, 2), RECT_RADIUS, RECT_RADIUS);
    p.drawRoundedRect(rectBubbleEdit.adjusted(-2, -2, 2, 2), RECT_RADIUS, RECT_RADIUS);
    p.drawRoundedRect(rectBubbleSize.adjusted(-2, -2, 2, 2), RECT_RADIUS, RECT_RADIUS);

    p.drawPixmap(rectBubbleMove, QPixmap("://icons/32x32/MoveArrow.png"));
    p.drawPixmap(rectBubbleEdit, QPixmap("://icons/32x32/EditDetails.png"));
    p.drawPixmap(rectBubbleSize, QPixmap("://icons/32x32/SizeArrow.png"));
  }
}

void CGisItemWpt::drawLabel(QPainter& p, const QPolygonF& /*viewport*/, QList<QRectF>& blockedAreas,
                            const QFontMetricsF& fm, CGisDraw* /*gis*/) {
  if (flags & eFlagWptBubble) {
    return;
  }

  if (posScreen == NOPOINTF) {
    return;
  }

  QPointF pt = posScreen - focus;

  QRectF rect = fm.boundingRect(wpt.name);
  rect.adjust(-2, -2, 2, 2);

  // place label on top
  rect.moveCenter(pt + QPointF(icon.width() / 2, -fm.height()));
  if (CDraw::doesOverlap(blockedAreas, rect)) {
    // place label on bottom
    rect.moveCenter(pt + QPointF(icon.width() / 2, +fm.height() + icon.height()));
    if (CDraw::doesOverlap(blockedAreas, rect)) {
      // place label on right
      rect.moveCenter(pt + QPointF(icon.width() + rect.width() / 2, +fm.height()));
      if (CDraw::doesOverlap(blockedAreas, rect)) {
        // place label on left
        rect.moveCenter(pt + QPointF(-rect.width() / 2, +fm.height()));
        if (CDraw::doesOverlap(blockedAreas, rect)) {
          // failed to place label anywhere
          return;
        }
      }
    }
  }

  CDraw::text(wpt.name, p, rect.toRect(), Qt::darkBlue);
  blockedAreas << rect;
}

void CGisItemWpt::drawHighlight(QPainter& p) {
  if (posScreen == NOPOINTF) {
    return;
  }

  if (closeToRadius) {
    drawCircle(p, posScreen, radius, false, true);
  } else {
    p.drawImage(posScreen - QPointF(31, 31), QImage("://cursors/wptHighlightRed.png"));
  }
}

void CGisItemWpt::drawBubble(QPainter& p) {
  if (!(flags & eFlagWptBubble)) {
    return;
  }

  QString str = QString("<b>%1</b>").arg(getName());

  if (!removeHtml(wpt.desc).simplified().isEmpty()) {
    str += QString("<p>%1</p>").arg(wpt.desc);
  }

  if (!removeHtml(wpt.cmt).simplified().isEmpty()) {
    str += QString("<p>%1</p>").arg(wpt.cmt);
  }

  QTextDocument doc;
  doc.setHtml(str);
  doc.setTextWidth(widthBubble);

  rectBubble.setWidth(widthBubble);
  rectBubble.setHeight(doc.size().height());

  QPoint posBubble = posScreen.toPoint() + offsetBubble;
  rectBubble.moveTopLeft(posBubble);

  rectBubbleMove.moveTopLeft(rectBubble.topLeft() + QPoint(5, 5));
  rectBubbleEdit.moveTopLeft(rectBubbleMove.topRight() + QPoint(7, 0));
  rectBubbleSize.moveBottomRight(rectBubble.bottomRight() - QPoint(5, 5));

  QPolygonF frame = makePolyline(posScreen, rectBubble);
  p.setPen(CDraw::penBorderGray);
  p.setBrush(CDraw::brushBackWhite);
  p.drawPolygon(frame);

  p.save();
  p.translate(posBubble);
  p.setPen(Qt::black);
  doc.drawContents(&p);
  p.restore();
}

void CGisItemWpt::drawCircle(QPainter& p, const QPointF& pos, const qreal& r, const bool& nogo, const bool& selected) {
  QRect circle(pos.x() - r - 1, pos.y() - r - 1, 2 * r + 1, 2 * r + 1);
  p.save();
  p.setBrush(Qt::NoBrush);
  if (selected) {
    p.setPen(QPen(Qt::red, 3));
  } else {
    p.setPen(QPen(Qt::white, 3));
    p.drawEllipse(circle);
    p.setPen(QPen(Qt::red, 1));
  }
  p.drawEllipse(circle);
  if (nogo) {
    p.setBrush(getNogoTextureBrush());
    p.setPen(Qt::NoPen);
    p.drawEllipse(circle);
  }
  p.restore();
}

qreal CGisItemWpt::calcRadius(const QPointF& posRad, const QPointF& posPx, const qreal& radiusRad, CGisDraw* gis) {
  QPointF pt1 = posRad;
  pt1 = GPS_Math_Wpt_Projection(pt1, radiusRad, 90 * DEG_TO_RAD);
  gis->convertRad2Px(pt1);

  return pt1.x() - posPx.x();
}

QPolygonF CGisItemWpt::makePolyline(const QPointF& anchor, const QRectF& r) {
  QPolygonF poly1, poly2;
  poly1 << r.topLeft() << r.topRight() << r.bottomRight() << r.bottomLeft();

  if (!r.contains(anchor)) {
    qreal w = rectBubble.width() >> 1;
    qreal h = rectBubble.height() >> 1;

    if (w > 30) {
      w = 30;
    }
    if (h > 30) {
      h = 30;
    }

    w = h = qMin(w, h);

    if (anchor.x() < r.left()) {
      poly2 << anchor << (r.center() + QPoint(0, -h)) << (r.center() + QPoint(0, h)) << anchor;
    } else if (r.right() < anchor.x()) {
      poly2 << anchor << (r.center() + QPoint(0, -h)) << (r.center() + QPoint(0, h)) << anchor;
    } else if (anchor.y() < r.top()) {
      poly2 << anchor << (r.center() + QPoint(-w, 0)) << (r.center() + QPoint(w, 0)) << anchor;
    } else if (r.bottom() < anchor.y()) {
      poly2 << anchor << (r.center() + QPoint(-w, 0)) << (r.center() + QPoint(w, 0)) << anchor;
    }

    QPainterPath path1;
    path1.addRoundedRect(r, RECT_RADIUS, RECT_RADIUS);
    QPainterPath path2;
    path2.addPolygon(poly2);

    path1 = path1.united(path2);

    poly1 = path1.toFillPolygon();
  }

  return poly1;
}

void CGisItemWpt::removeLinksByType(const QString& type) {
  QList<IGisItem::link_t>::iterator link = wpt.links.begin();

  while (link != wpt.links.end()) {
    if (link->type == type) {
      link = wpt.links.erase(link);
      continue;
    }

    ++link;
  }
}

void CGisItemWpt::mouseMove(const QPointF& pos) {
  if (!hasBubble() || isReadOnly()) {
    return;
  }
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (!canvas) {
    return;
  }

  if (mouseIsOverBubble) {
    processMouseOverBubble(pos.toPoint());
    if (!rectBubble.contains(pos.toPoint())) {
      doBubbleMove = doBubbleSize = false;
      canvas->resetMouse();
      mouseIsOverBubble = false;
    }
  } else {
    if (rectBubble.contains(pos.toPoint())) {
      doBubbleMove = doBubbleSize = false;
      canvas->setMouseWptBubble(getKey());
      mouseIsOverBubble = true;
    }
  }
}

void CGisItemWpt::mouseDragged(const QPoint& /*start*/, const QPoint& /*last*/, const QPoint& pos) {
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (!canvas) {
    return;
  }
  if (!doBubbleMove && !doBubbleSize) {
    if (rectBubbleMove.contains(pos)) {
      offsetMouse = pos - rectBubble.topLeft();
      doBubbleMove = true;
    } else if (rectBubbleSize.contains(pos)) {
      offsetMouse = pos - rectBubble.bottomRight();
      doBubbleSize = true;
    } else {
      return;
    }
  }
  if (doBubbleMove) {
    offsetBubble = pos - posScreen.toPoint();
    offsetBubble -= offsetMouse;
  } else if (doBubbleSize) {
    qDebug() << offsetMouse;
    int width = pos.x() - rectBubble.left() - offsetMouse.x();
    if (width > 50) {
      widthBubble = width;
    }
  }
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CGisItemWpt::dragFinished(const QPoint& /*pos*/) {
  updateHistory();
  doBubbleMove = doBubbleSize = false;
}

void CGisItemWpt::leftClicked(const QPoint& pos) {
  if (rectBubbleEdit.contains(pos)) {
    CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
    if (canvas) {
      doBubbleMove = doBubbleSize = false;
      canvas->resetMouse();
    }
    mouseIsOverBubble = false;
    edit();
  }
}

void CGisItemWpt::toggleBubble() {
  if (flags & eFlagWptBubble) {
    flags &= ~eFlagWptBubble;
  } else {
    flags |= eFlagWptBubble;
  }
  updateHistory();
}

const searchValue_t CGisItemWpt::getValueByKeyword(searchProperty_e keyword) {
  if (keywordLambdaMap.contains(keyword)) {
    return keywordLambdaMap.value(keyword)(this);
  }
  return searchValue_t();
}

void CGisItemWpt::processMouseOverBubble(const QPoint& pos) {
  if (rectBubbleMove.contains(pos) || rectBubbleEdit.contains(pos) || rectBubbleSize.contains(pos)) {
    if (!doSpecialCursor) {
      CCanvas::setOverrideCursor(Qt::PointingHandCursor, "processMouseOverBubble");
      doSpecialCursor = true;
    }
  } else {
    if (doSpecialCursor) {
      CCanvas::restoreOverrideCursor("processMouseOverBubble");
      doSpecialCursor = false;
    }
  }
}

void CGisItemWpt::detBoundingRect() {
  if (proximity == NOFLOAT) {
    boundingRect = QRectF(QPointF(wpt.lon, wpt.lat) * DEG_TO_RAD, QPointF(wpt.lon, wpt.lat) * DEG_TO_RAD);
  } else {
    qreal diag = proximity * 1.414213562;
    QPointF cent(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);

    QPointF pt1 = GPS_Math_Wpt_Projection(cent, diag, 225 * DEG_TO_RAD);
    QPointF pt2 = GPS_Math_Wpt_Projection(cent, diag, 45 * DEG_TO_RAD);

    boundingRect = QRectF(pt1, pt2);
  }
}

const QList<QString> CGisItemWpt::geocache_t::attributeMeanings = {
    "QMS Attribute Flag",  // Not to be serialized in GPX files
    "Dogs",
    "Access or parking fee",
    "Climbing gear",
    "Boat",
    "Scuba gear",
    "Recommended for kids",
    "Takes less than an hour",
    "Scenic view",
    "Significant hike",
    "Difficult climbing",
    "May require wading",
    "May require swimming",
    "Available at all times",
    "Recommended at night",
    "Available during winter",
    "",
    "Poison plants",
    "Dangerous Animals",
    "Ticks",
    "Abandoned mines",
    "Cliff / falling rocks",
    "Hunting",
    "Dangerous area",
    "Wheelchair accessible",
    "Parking available",
    "Public transportation",
    "Drinking water nearby",
    "Public restrooms nearby",
    "Telephone nearby",
    "Picnic tables nearby",
    "Camping available",
    "Bicycles",
    "Motorcycles",
    "Quads",
    "Off-road vehicles",
    "Snowmobiles",
    "Horses",
    "Campfires",
    "Thorns",
    "Stealth required",
    "Stroller accessible",
    "Needs maintenance",
    "Watch for livestock",
    "Flashlight required",
    "",
    "Truck Driver/RV",
    "Field Puzzle",
    "UV Light Required",
    "Snowshoes",
    "Cross Country Skis",
    "Special Tool Required",
    "Night Cache",
    "Park and Grab",
    "Abandoned Structure",
    "Short hike (less than 1km)",
    "Medium hike (1km-10km)",
    "Long Hike (+10km)",
    "Fuel Nearby",
    "Food Nearby",
    "Wireless Beacon",
    "Partnership cache",
    "Seasonal Access",
    "Tourist Friendly",
    "Tree Climbing",
    "Front Yard (Private Residence)",
    "Teamwork Required",
    "GeoTour"};

QList<QString> CGisItemWpt::geocache_t::initAttributeMeaningsTranslated() {
  QList<QString> translated = {tr("QMS Attribute Flag"),  // Not to be serialized in GPX files
                               tr("Dogs"),
                               tr("Access or parking fee"),
                               tr("Climbing gear"),
                               tr("Boat"),
                               tr("Scuba gear"),
                               tr("Recommended for kids"),
                               tr("Takes less than an hour"),
                               tr("Scenic view"),
                               tr("Significant hike"),
                               tr("Difficult climbing"),
                               tr("May require wading"),
                               tr("May require swimming"),
                               tr("Available at all times"),
                               tr("Recommended at night"),
                               tr("Available during winter"),
                               "",
                               tr("Poison plants"),
                               tr("Dangerous Animals"),
                               tr("Ticks"),
                               tr("Abandoned mines"),
                               tr("Cliff / falling rocks"),
                               tr("Hunting"),
                               tr("Dangerous area"),
                               tr("Wheelchair accessible"),
                               tr("Parking available"),
                               tr("Public transportation"),
                               tr("Drinking water nearby"),
                               tr("Public restrooms nearby"),
                               tr("Telephone nearby"),
                               tr("Picnic tables nearby"),
                               tr("Camping available"),
                               tr("Bicycles"),
                               tr("Motorcycles"),
                               tr("Quads"),
                               tr("Off-road vehicles"),
                               tr("Snowmobiles"),
                               tr("Horses"),
                               tr("Campfires"),
                               tr("Thorns"),
                               tr("Stealth required"),
                               tr("Stroller accessible"),
                               tr("Needs maintenance"),
                               tr("Watch for livestock"),
                               tr("Flashlight required"),
                               "",
                               tr("Truck Driver/RV"),
                               tr("Field Puzzle"),
                               tr("UV Light Required"),
                               tr("Snowshoes"),
                               tr("Cross Country Skis"),
                               tr("Special Tool Required"),
                               tr("Night Cache"),
                               tr("Park and Grab"),
                               tr("Abandoned Structure"),
                               tr("Short hike (less than 1km)"),
                               tr("Medium hike (1km-10km)"),
                               tr("Long Hike (+10km)"),
                               tr("Fuel Nearby"),
                               tr("Food Nearby"),
                               tr("Wireless Beacon"),
                               tr("Partnership cache"),
                               tr("Seasonal Access"),
                               tr("Tourist Friendly"),
                               tr("Tree Climbing"),
                               tr("Front Yard (Private Residence)"),
                               tr("Teamwork Required"),
                               tr("GeoTour")};
  return translated;
}

QDateTime CGisItemWpt::geocache_t::getLastFound() const {
  QDateTime lastFound;
  for (const geocachelog_t& log : logs) {
    if (lastFound.isValid() == false || (log.type == "Found It" && log.date > lastFound)) {
      lastFound = log.date;
    }
  }
  return lastFound;
}

QString CGisItemWpt::geocache_t::getLogs() const {
  const QString& format = QLocale().dateTimeFormat(QLocale::ShortFormat);
  QString strLogs;
  for (const geocachelog_t& log : logs) {
    QString thislog = log.text;
    strLogs += "<p><b>" + log.date.date().toString(format) + ": " + log.type + tr(" by ") + log.finder + "</b></p><p>" +
               thislog.replace("\n", "<br/>") + "</p><hr>";
  }
  return strLogs;
}

QMap<searchProperty_e, CGisItemWpt::fSearch> CGisItemWpt::initKeywordLambdaMap() {
  QMap<searchProperty_e, CGisItemWpt::fSearch> map;
  map.insert(eSearchPropertyGeneralName, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    if (item->geocache.hasData) {
      searchValue.str1 = item->geocache.name + " - " + item->getName();
    } else {
      searchValue.str1 = item->getName();
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralFullText, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getInfo(eFeatureShowFullText | eFeatureShowName);
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralElevation, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    IUnit::self().meter2elevation(item->wpt.ele, searchValue.value1, searchValue.str1);
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralDate, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    if (item->wpt.time.isValid()) {
      searchValue.value1 = item->wpt.time.toSecsSinceEpoch();
      searchValue.str1 = "SsE";  // To differentiate Dates and Durations
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralComment, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getComment();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralDescription, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getDescription();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralRating, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getRating();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralKeywords, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = QStringList(item->getKeywords().values()).join(", ");
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralType, [](CGisItemWpt* /*item*/) {
    searchValue_t searchValue;
    searchValue.str1 = tr("waypoint");
    return searchValue;
  });
  // Geocache keywords
  map.insert(eSearchPropertyGeocacheDifficulty, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->geocache.difficulty;
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheTerrain, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->geocache.terrain;
    return searchValue;
  });
  map.insert(eSearchPropertyGeocachePositiveAttributes, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    const QList<quint8>& keys = item->geocache.attributes.keys();
    for (quint8 attr : keys) {
      if (attr >= item->geocache.attributeMeaningsTranslated.length()) {
        continue;
      }
      if (!item->geocache.attributes[attr])  // It is negated
      {
        continue;
      }
      searchValue.str1 += item->geocache.attributeMeaningsTranslated[attr] + ", ";
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheNegatedAttributes, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    const QList<quint8>& keys = item->geocache.attributes.keys();
    for (quint8 attr : keys) {
      if (attr >= item->geocache.attributeMeaningsTranslated.length()) {
        continue;
      }
      if (item->geocache.attributes[attr])  // It is not negated
      {
        continue;
      }
      searchValue.str1 += item->geocache.attributeMeaningsTranslated[attr] + ", ";
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheSize, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->geocache.container;
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheGCCode, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getName();
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheGCName, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->geocache.name;
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheStatus, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    if (!item->geocache.hasData) {
      return searchValue;
    }

    if (item->geocache.archived) {
      searchValue.str1 = tr("archived");
    } else if (item->geocache.available) {
      searchValue.str1 = tr("available");
    } else {
      searchValue.str1 = tr("not available");
    }

    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheGCType, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->geocache.type;
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheLoggedBy, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    for (const geocachelog_t& log : std::as_const(item->geocache.logs)) {
      searchValue.str1 += log.finder + ", ";
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheLastLogDate, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    if (item->geocache.logs.size() > 0) {
      searchValue.value1 = item->geocache.logs[0].date.toSecsSinceEpoch();
      searchValue.str1 = "SsE";  // To differentiate Dates and Durations
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheLastLogType, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    if (item->geocache.logs.size() > 0) {
      searchValue.str1 = item->geocache.logs[0].type;
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheLastLogBy, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    if (item->geocache.logs.size() > 0) {
      searchValue.str1 = item->geocache.logs[0].finder;
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeocacheGCOwner, [](CGisItemWpt* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->geocache.owner;
    return searchValue;
  });
  return map;
}
