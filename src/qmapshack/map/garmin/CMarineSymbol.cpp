/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "map/garmin/CMarineSymbol.h"

#include <QPolygonF>
#include <QRgb>
#include <QStringList>
#include <QtMath>

namespace {
enum class Shape { eNone, eCan, eCone, eSphere, ePillar, eSpar, eBarrel, eBeacon };
enum class Topmark { eNone, eConesUp, eConesDown, eConesApart, eConesTogether, eSpheres, eSphere, eCross, eCan, eCone };

struct mark_t {
  Shape shape = Shape::eNone;
  Topmark topmark = Topmark::eNone;
  const char* colours = ""; /**< mkgmap colour name used when the data has none */
  bool verticalStripes = false;
};

/** Shape, topmark and default colours by type, as mkgmap's marine style assigns the types (IALA region A). */
mark_t markOf(quint32 type) {
  if ((type & 0xFFFF0000) != 0x10000) {
    return {};
  }
  switch (type & 0xFFFF) {
    case 0x0200:
      return {Shape::ePillar, Topmark::eNone, "", false};
    case 0x0202:
      return {Shape::ePillar, Topmark::eConesUp, "black-yellow", false};
    case 0x0203:
      return {Shape::ePillar, Topmark::eConesDown, "yellow-black", false};
    case 0x0204:
      return {Shape::ePillar, Topmark::eConesApart, "black-yellow-black", false};
    case 0x0205:
      return {Shape::ePillar, Topmark::eConesTogether, "yellow-black-yellow", false};
    case 0x0206:
      return {Shape::eBeacon, Topmark::eNone, "", false};
    case 0x0207:
      return {Shape::eSpar, Topmark::eNone, "", false};
    case 0x0208:
      return {Shape::ePillar, Topmark::eSpheres, "black-red-black", false};
    case 0x0209:
      return {Shape::eCan, Topmark::eCan, "red", false};
    case 0x020a:
      return {Shape::eCone, Topmark::eCone, "green", false};
    case 0x020b:
      return {Shape::ePillar, Topmark::eCross, "yellow", false};
    case 0x020c:
      return {Shape::eSphere, Topmark::eSphere, "red-white", true};
    case 0x020d:
      return {Shape::eBarrel, Topmark::eCross, "yellow", false};
    case 0x020e:
      return {Shape::eBeacon, Topmark::eConesUp, "black-yellow", false};
    case 0x020f:
      return {Shape::eBeacon, Topmark::eConesDown, "yellow-black", false};
    case 0x0210:
      return {Shape::eBeacon, Topmark::eConesApart, "black-yellow-black", false};
    case 0x0211:
      return {Shape::eBeacon, Topmark::eConesTogether, "yellow-black-yellow", false};
    case 0x0212:
      return {Shape::eBeacon, Topmark::eSpheres, "black-red-black", false};
    case 0x0213:
      return {Shape::eBeacon, Topmark::eCan, "red", false};
    case 0x0214:
      return {Shape::eBeacon, Topmark::eCone, "green", false};
    case 0x0215:
      return {Shape::eBeacon, Topmark::eCross, "yellow", false};
    default:
      return {};
  }
}

QRgb colourOf(const QString& name) {
  if (name == "red") {
    return 0xe30613;
  }
  if (name == "green") {
    return 0x00963f;
  }
  if (name == "yellow") {
    return 0xffd500;
  }
  if (name == "white") {
    return 0xffffff;
  }
  if (name == "black") {
    return 0x000000;
  }
  if (name == "orange") {
    return 0xf39200;
  }
  return 0xc0c0c0;
}

/** Light colour as on multicoloured charts (INT 1 P11): white as yellow. */
QRgb lightColourOf(quint8 colour) {
  static const QRgb colours[8] = {0xff00ff, 0xff0000, 0x00ff00, 0xffff00, 0x0000ff, 0xffff00, 0xee82ee, 0xffc200};
  return colours[colour & 0x07];
}

/** The body outline in local coordinates: the position at (0, 0), up is -y. */
QPolygonF outlineOf(Shape shape) {
  QPolygonF poly;
  switch (shape) {
    case Shape::eCan:
      poly << QPointF(-4.5, 0) << QPointF(-4.5, -10) << QPointF(4.5, -10) << QPointF(4.5, 0);
      break;
    case Shape::eCone:
      poly << QPointF(-5.5, 0) << QPointF(0, -13) << QPointF(5.5, 0);
      break;
    case Shape::eSphere:
      for (int a = 0; a <= 180; a += 15) {
        const qreal r = qDegreesToRadians(qreal(a));
        poly << QPointF(-6 * qCos(r), -6 * qSin(r));
      }
      break;
    case Shape::ePillar:
      poly << QPointF(-4, 0) << QPointF(-1.8, -16) << QPointF(1.8, -16) << QPointF(4, 0);
      break;
    case Shape::eSpar:
      poly << QPointF(-1.3, 0) << QPointF(-1.3, -16) << QPointF(1.3, -16) << QPointF(1.3, 0);
      break;
    case Shape::eBarrel:
      for (int a = 0; a <= 180; a += 15) {
        const qreal r = qDegreesToRadians(qreal(a));
        poly << QPointF(-7 * qCos(r), -4.5 * qSin(r));
      }
      break;
    case Shape::eBeacon:
      poly << QPointF(-2.2, 0) << QPointF(-2.2, -14) << QPointF(2.2, -14) << QPointF(2.2, 0);
      break;
    case Shape::eNone:
      break;
  }
  return poly;
}

/** The part of the convex @p poly with lo <= coordinate <= hi, x or y. */
QPolygonF clipConvex(const QPolygonF& poly, bool alongX, qreal lo, qreal hi) {
  const auto coord = [alongX](const QPointF& p) { return alongX ? p.x() : p.y(); };
  const auto cut = [&](const QPolygonF& in, qreal limit, bool keepGreater) {
    QPolygonF out;
    for (qsizetype i = 0; i < in.size(); ++i) {
      const QPointF a = in[i];
      const QPointF b = in[(i + 1) % in.size()];
      const bool ina = keepGreater ? coord(a) >= limit : coord(a) <= limit;
      const bool inb = keepGreater ? coord(b) >= limit : coord(b) <= limit;
      if (ina) {
        out << a;
      }
      if (ina != inb) {
        const qreal t = (limit - coord(a)) / (coord(b) - coord(a));
        out << a + (b - a) * t;
      }
    }
    return out;
  };
  return cut(cut(poly, lo, true), hi, false);
}

/** Maps local coordinates to the screen: leaning by @p lean [rad] around the position, then moved to it. */
struct transform_t {
  QPointF pos;
  qreal lean = 0;
  QPointF operator()(const QPointF& p) const {
    const qreal c = qCos(lean);
    const qreal s = qSin(lean);
    return pos + QPointF(p.x() * c - p.y() * s, p.x() * s + p.y() * c);
  }
  BLPath path(const QPolygonF& poly, bool close = true) const {
    BLPath path;
    for (qsizetype i = 0; i < poly.size(); ++i) {
      const QPointF p = (*this)(poly[i]);
      if (i == 0) {
        path.move_to(p.x(), p.y());
      } else {
        path.line_to(p.x(), p.y());
      }
    }
    if (close) {
      path.close();
    }
    return path;
  }
};

void fillStroke(BLContext& ctx, const BLPath& path, QRgb fill) {
  ctx.set_fill_style(BLRgba32(0xFF000000u | fill));
  ctx.fill_path(path);
  ctx.set_stroke_style(BLRgba32(0xFF000000u));
  ctx.set_stroke_width(1.0);
  ctx.stroke_path(path);
}

void drawCircle(BLContext& ctx, const QPointF& c, qreal r, QRgb fill) {
  ctx.set_fill_style(BLRgba32(0xFF000000u | fill));
  ctx.fill_circle(c.x(), c.y(), r);
  ctx.set_stroke_style(BLRgba32(0xFF000000u));
  ctx.set_stroke_width(1.0);
  ctx.stroke_circle(c.x(), c.y(), r);
}

/** The topmark above @p top (local coordinates), on a short staff. */
void drawTopmark(BLContext& ctx, const transform_t& tf, qreal top, Topmark topmark) {
  if (topmark == Topmark::eNone) {
    return;
  }
  const qreal base = top - 2;
  ctx.set_stroke_style(BLRgba32(0xFF000000u));
  ctx.set_stroke_width(1.0);
  ctx.stroke_path(tf.path(QPolygonF({QPointF(0, top), QPointF(0, base)}), false));

  const auto cone = [&](qreal bottom, bool up, QRgb fill) {
    const qreal h = 4.5;
    const QPolygonF tri = up ? QPolygonF({QPointF(-2.8, bottom), QPointF(0, bottom - h), QPointF(2.8, bottom)})
                             : QPolygonF({QPointF(-2.8, bottom - h), QPointF(0, bottom), QPointF(2.8, bottom - h)});
    fillStroke(ctx, tf.path(tri), fill);
  };
  switch (topmark) {
    case Topmark::eConesUp:
      cone(base, true, 0x000000);
      cone(base - 5.5, true, 0x000000);
      break;
    case Topmark::eConesDown:
      cone(base, false, 0x000000);
      cone(base - 5.5, false, 0x000000);
      break;
    case Topmark::eConesApart:
      cone(base, false, 0x000000);
      cone(base - 5.5, true, 0x000000);
      break;
    case Topmark::eConesTogether:
      cone(base, true, 0x000000);
      cone(base - 5.5, false, 0x000000);
      break;
    case Topmark::eSpheres:
      drawCircle(ctx, tf(QPointF(0, base - 2.3)), 2.2, 0x000000);
      drawCircle(ctx, tf(QPointF(0, base - 7.0)), 2.2, 0x000000);
      break;
    case Topmark::eSphere:
      drawCircle(ctx, tf(QPointF(0, base - 2.5)), 2.5, colourOf("red"));
      break;
    case Topmark::eCross: {
      ctx.set_stroke_style(BLRgba32(0xFF000000u | colourOf("yellow")));
      ctx.set_stroke_width(1.6);
      ctx.stroke_path(tf.path(QPolygonF({QPointF(-2.5, base), QPointF(2.5, base - 5)}), false));
      ctx.stroke_path(tf.path(QPolygonF({QPointF(-2.5, base - 5), QPointF(2.5, base)}), false));
      break;
    }
    case Topmark::eCan:
      fillStroke(ctx,
                 tf.path(QPolygonF(
                     {QPointF(-2.5, base), QPointF(-2.5, base - 4.5), QPointF(2.5, base - 4.5), QPointF(2.5, base)})),
                 colourOf("red"));
      break;
    case Topmark::eCone:
      cone(base, true, colourOf("green"));
      break;
    case Topmark::eNone:
      break;
  }
}

/** A light flare (P1): a teardrop from the position at @p angle [deg] right of up, in the light's colour. */
void drawFlare(BLContext& ctx, const QPointF& pos, QRgb colour, qreal length, qreal angle) {
  const transform_t tf{pos, qDegreesToRadians(angle)};
  QPolygonF drop;
  drop << QPointF(0, 0);
  const qreal w = length * 0.22;
  const qreal head = -length + w;
  for (int a = 180; a <= 360; a += 15) {
    const qreal r = qDegreesToRadians(qreal(a));
    drop << QPointF(w * qCos(r), head + w * qSin(r));
  }
  ctx.set_fill_style(BLRgba32(0xFF000000u | colour));
  ctx.fill_path(tf.path(drop));
  ctx.set_stroke_style(BLRgba32(0xFF000000u));
  ctx.set_stroke_width(0.8);
  ctx.stroke_path(tf.path(drop));
}

/** A five-pointed star at the position of a light (P1). */
void drawStar(BLContext& ctx, const QPointF& pos, qreal r) {
  BLPath path;
  for (int i = 0; i < 10; ++i) {
    const qreal a = qDegreesToRadians(-90.0 + i * 36.0);
    const qreal rr = (i % 2) ? r * 0.42 : r;
    const QPointF p = pos + QPointF(rr * qCos(a), rr * qSin(a));
    if (i == 0) {
      path.move_to(p.x(), p.y());
    } else {
      path.line_to(p.x(), p.y());
    }
  }
  path.close();
  ctx.set_fill_style(BLRgba32(0xFF000000u));
  ctx.fill_path(path);
}

/** The colour of the flare: the light's, magenta for several colours. */
bool flareColour(const CGarminMarine& marine, QRgb& colour) {
  quint8 found = 0;
  for (const CGarminMarine::light_t& light : marine.lights) {
    if (light.colour == 0) {
      continue;
    }
    if (found != 0 && found != light.colour) {
      colour = 0xff00ff;
      return true;
    }
    found = light.colour;
  }
  if (found == 0) {
    return false;
  }
  colour = lightColourOf(found);
  return true;
}
}  // namespace

bool CMarineSymbol::hasSymbol(quint32 type) {
  const quint32 r = type & 0x1FF00;
  return r == 0x10100 || r == 0x10400 || markOf(type).shape != Shape::eNone;
}

QRectF CMarineSymbol::bounds(quint32 type) {
  if ((type & 0x1FF00) == 0x10400) {
    return QRectF(-8, -8, 16, 16);
  }
  // down to a lit buoy's flare below the position
  return QRectF(-8, -30, 20, 38);
}

void CMarineSymbol::draw(BLContext& ctx, const QPointF& pos, quint32 type, const CGarminMarine& marine) {
  const quint32 r = type & 0x1FF00;

  if (r == 0x10400) {
    // K40: a dotted danger line around the position
    ctx.set_fill_style(BLRgba32(0xA0A8D8FFu));
    ctx.fill_circle(pos.x(), pos.y(), 7);
    ctx.set_fill_style(BLRgba32(0xFF000000u));
    for (int a = 0; a < 360; a += 30) {
      const qreal rad = qDegreesToRadians(qreal(a));
      ctx.fill_circle(pos.x() + 7 * qCos(rad), pos.y() + 7 * qSin(rad), 0.8);
    }
    return;
  }

  QRgb flare = 0;
  const bool lit = flareColour(marine, flare);

  if (r == 0x10100) {
    // P1: a light is a flare from a star, magenta when the colour is unknown
    drawFlare(ctx, pos, lit ? flare : 0xd000d0, 16, 30);
    drawStar(ctx, pos, 4.5);
    return;
  }

  const mark_t mark = markOf(type);
  if (mark.shape == Shape::eNone) {
    return;
  }

  // buoys lean (Q20-Q26), beacons stand upright (Q80)
  const bool beacon = mark.shape == Shape::eBeacon;
  const transform_t tf{pos, beacon ? 0.0 : qDegreesToRadians(15.0)};

  // a lit buoy's flare points down to the right, clear of the body
  if (lit) {
    drawFlare(ctx, pos, flare, 12, 120);
  }

  const QPolygonF outline = outlineOf(mark.shape);
  const QRectF box = outline.boundingRect();
  QStringList colours = QString(marine.colour != 0 ? CGarminMarine::colourName(marine.colour) : mark.colours)
                            .split('-', Qt::SkipEmptyParts);
  if (colours.isEmpty()) {
    colours << (beacon ? "black" : "white");
  }

  // Q4: bands from top to bottom; Q5: vertical stripes
  const bool stripes = mark.verticalStripes && colours.size() > 1;
  const int n = stripes ? 4 : colours.size();
  for (int i = 0; i < n; ++i) {
    const QPolygonF part =
        stripes ? clipConvex(outline, true, box.left() + box.width() * i / n, box.left() + box.width() * (i + 1) / n)
                : clipConvex(outline, false, box.top() + box.height() * i / n, box.top() + box.height() * (i + 1) / n);
    if (part.size() >= 3) {
      ctx.set_fill_style(BLRgba32(0xFF000000u | colourOf(colours[i % colours.size()])));
      ctx.fill_path(tf.path(part));
    }
  }
  ctx.set_stroke_style(BLRgba32(0xFF000000u));
  ctx.set_stroke_width(1.0);
  ctx.stroke_path(tf.path(outline));

  drawTopmark(ctx, tf, box.top(), mark.topmark);

  // waterline and position circle
  ctx.set_stroke_style(BLRgba32(0xFF000000u));
  ctx.set_stroke_width(1.0);
  ctx.stroke_path(transform_t{pos, 0}.path(QPolygonF({QPointF(-7, 0), QPointF(7, 0)}), false));
  drawCircle(ctx, pos, 1.8, 0xffffff);
}
