/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "map/CMapIMG.h"

#include <blend2d/blend2d.h>

#include <QPainterPath>
#include <QRawFont>
#include <QtWidgets>
#include <algorithm>
#include <cmath>
#include <thread>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/GeoMath.h"
#include "helpers/CFileExt.h"
#include "helpers/CProgressDialog.h"
#include "helpers/Platform.h"
#include "map/Blend2dUtil.h"
#include "map/CMapDraw.h"
#include "map/garmin/CGarminTyp.h"
#include "map/garmin/IGarminStrTbl.h"
#include "misc.h"
#include "poi/IPoiItem.h"
#include "units/IUnit.h"

#undef DEBUG_SHOW_SECTION_BORDERS
#undef DEBUG_SHOW_SUBDIV_BORDERS

/** Map scale [m/px] from which line labels are drawn short, without prefix and suffix, as BaseCamp does. */
constexpr qreal kShortLabelThreshold = 5.0;
/** Maximum map scale [m/px] at which line labels are drawn; BaseCamp still draws them at its 5 km scale bar. */
constexpr qreal kLineLabelThreshold = 50.0;
/// Largest bend [deg] between two segments a street name may follow.
constexpr qreal kLabelMaxBend = 30.0;
/// Street names this close [deg] to vertical keep the direction of their line; all others read left to right.
constexpr qreal kLabelVertical = 10.0;

// Build a copy of @p img tiled horizontally to @p width pixels as an *owning*
// premultiplied BLImage. Unlike a create_from_data() view, the result owns its pixels,
// so Blend2D keeps them alive until the queued blit is rasterised — required once the
// rendering context renders asynchronously/multithreaded. Returns an empty image on
// failure.
static inline BLImage img2line(const BLImage& img, int width) {
  BLImageData src;
  BLImage newImage;
  const int height = img.height();
  if (width < 1 || height < 1 || img.get_data(&src) != BL_SUCCESS ||
      newImage.create(width, height, BL_FORMAT_PRGB32) != BL_SUCCESS) {
    return newImage;
  }

  BLImageData dst;
  if (newImage.make_mutable(&dst) != BL_SUCCESS) {
    return BLImage();
  }

  // A BLImage row may be padded, so stepping rows and copying one tile are two different
  // lengths - unlike QImage's ARGB32 bytesPerLine(), which is always exactly width * 4.
  const qsizetype bpl_src = static_cast<qsizetype>(src.stride);
  const qsizetype tileBytes = static_cast<qsizetype>(img.width()) * 4;
  const qsizetype bpl_dst = static_cast<qsizetype>(dst.stride);
  const qsizetype rowBytes = static_cast<qsizetype>(width) * 4;  // tightly packed destination row (ARGB32)
  const uchar* const srcBase = static_cast<const uchar*>(src.pixel_data);
  uchar* const dstBase = static_cast<uchar*>(dst.pixel_data);

  for (int i = 0; i < height; ++i) {
    const uchar* srcBits = srcBase + bpl_src * i;
    uchar* dstBits = dstBase + bpl_dst * i;

    qsizetype bytesToCopy = rowBytes;
    while (bytesToCopy > 0) {
      const qsizetype chunk = qMin(bytesToCopy, tileBytes);
      memcpy(dstBits, srcBits, static_cast<size_t>(chunk));
      dstBits += chunk;
      bytesToCopy -= chunk;
    }
  }
  return newImage;
}

static inline bool isCluttered(QVector<QRectF>& rectPois, const QRectF& rect) {
  for (const QRectF& rectPoi : rectPois) {
    if (rect.intersects(rectPoi)) {
      return true;
    }
  }
  rectPois << rect;
  return false;
}

/**
   @brief Find the longest stretch of a line without a bend over kLabelMaxBend

   Segments shorter than 1 px are taken along without testing their direction.

   @param line     the line in screen pixels
   @param stretch  receives the stretch's points
   @return the stretch's length, 0 if the line has no segment
 */
static qreal longestStraightStretch(const QPolygonF& line, QPolygonF& stretch) {
  qreal bestLength = 0;
  int bestFirst = 0;
  int bestLast = 0;

  int first = 0;
  qreal length = 0;
  qreal lastAngle = 0;
  bool hasAngle = false;

  for (int i = 0; i + 1 < line.size(); ++i) {
    const QLineF segment(line[i], line[i + 1]);
    const qreal segLength = segment.length();

    if (segLength >= 1.0) {
      const qreal angle = segment.angle();
      qreal bend = qAbs(angle - lastAngle);
      if (bend > 180) {
        bend = 360 - bend;
      }
      if (hasAngle && bend > kLabelMaxBend) {
        first = i;
        length = 0;
      }
      lastAngle = angle;
      hasAngle = true;
    }

    length += segLength;
    if (length > bestLength) {
      bestLength = length;
      bestFirst = first;
      bestLast = i + 1;
    }
  }

  stretch = line.mid(bestFirst, bestLast - bestFirst + 1);
  return bestLength;
}

/**
   @brief The area a label covers along a line

   @param path    the line the label follows
   @param from    start of the label as a fraction of the line's length
   @param to      end of the label as a fraction of the line's length
   @param height  the text height
   @return the label's section of the line, widened to the text height
 */
static QPainterPath labelOutline(const QPainterPath& path, qreal from, qreal to, qreal height) {
  QPainterPath section(path.pointAtPercent(from));
  const int steps = 8;
  for (int i = 1; i <= steps; ++i) {
    section.lineTo(path.pointAtPercent(from + (to - from) * i / steps));
  }

  QPainterPathStroker stroker;
  stroker.setWidth(height);
  stroker.setCapStyle(Qt::FlatCap);
  return stroker.createStroke(section);
}

namespace {
/// Convert a QColor to a Blend2D non-premultiplied 0xAARRGGBB colour.
inline BLRgba32 toBLColor(const QColor& c) { return BLRgba32(c.rgba()); }

// QPointF and BLPoint are both two contiguous doubles, so a QPolygonF's storage can be
// handed to Blend2D's bulk polygon/polyline calls without copying into a BLPath.
static_assert(sizeof(QPointF) == sizeof(BLPoint), "QPointF must be layout-compatible with BLPoint");
static_assert(sizeof(QPointF) == 2 * sizeof(double), "QPointF must be two doubles");

/// View a QPolygonF as a contiguous array of BLPoint for Blend2D bulk geometry calls.
inline const BLPoint* toBLPoints(const QPolygonF& poly) { return reinterpret_cast<const BLPoint*>(poly.constData()); }

BLStrokeCap toBLCap(Qt::PenCapStyle cap) {
  switch (cap) {
    case Qt::SquareCap:
      return BL_STROKE_CAP_SQUARE;
    case Qt::RoundCap:
      return BL_STROKE_CAP_ROUND;
    case Qt::FlatCap:
    default:
      return BL_STROKE_CAP_BUTT;
  }
}

BLStrokeJoin toBLJoin(Qt::PenJoinStyle join) {
  switch (join) {
    case Qt::BevelJoin:
      return BL_STROKE_JOIN_BEVEL;
    case Qt::RoundJoin:
      return BL_STROKE_JOIN_ROUND;
    case Qt::MiterJoin:
    case Qt::SvgMiterJoin:
    default:
      return BL_STROKE_JOIN_MITER_BEVEL;
  }
}

/**
   @brief Configure a context's stroke state from a QPen.

   Maps width, cap, join, colour and the simple Qt dash styles to their Blend2D
   counterparts. Qt dash patterns are expressed in pen-width units, so they are
   scaled by the stroke width here.

   @return false for Qt::NoPen, telling the caller to skip stroking entirely.
 */
bool applyPen(BLContext& ctx, const QPen& pen) {
  if (pen.style() == Qt::NoPen) {
    return false;
  }

  const double width = pen.widthF() > 0.0 ? pen.widthF() : 1.0;
  ctx.set_stroke_width(width);
  ctx.set_stroke_caps(toBLCap(pen.capStyle()));
  ctx.set_stroke_join(toBLJoin(pen.joinStyle()));

  BLArray<double> dashes;
  switch (pen.style()) {
    case Qt::DashLine:
      dashes.append(4.0 * width);
      dashes.append(2.0 * width);
      break;
    case Qt::DotLine:
      dashes.append(1.0 * width);
      dashes.append(2.0 * width);
      break;
    case Qt::DashDotLine:
      dashes.append(4.0 * width);
      dashes.append(2.0 * width);
      dashes.append(1.0 * width);
      dashes.append(2.0 * width);
      break;
    case Qt::DashDotDotLine:
      dashes.append(4.0 * width);
      dashes.append(2.0 * width);
      dashes.append(1.0 * width);
      dashes.append(2.0 * width);
      dashes.append(1.0 * width);
      dashes.append(2.0 * width);
      break;
    default:  // SolidLine (and custom dashes, treated as solid)
      break;
  }
  // An empty dash array clears any dash pattern left over from a previous type.
  ctx.set_stroke_dash_array(dashes);
  ctx.set_stroke_dash_offset(0.0);
  ctx.set_stroke_style(toBLColor(pen.color()));
  return true;
}

/// Blit an icon owned by a CGarminTyp property. Nothing is copied: BLImage is refcounted,
/// so Blend2D keeps the property's pixels alive until the deferred blit is rasterised.
void blitIcon(BLContext& ctx, double x, double y, const BLImage& img) {
  if (img.is_empty()) {
    return;
  }
  ctx.blit_image(BLPoint(x, y), img);
}

/// Blit the small blue bullet used as a fallback marker for cluttered points.
void blitBullet(BLContext& ctx, double x, double y) {
  static const BLImage bullet = toOwnedBLImage(QImage(":/icons/8x8/bullet_blue.png"));
  blitIcon(ctx, x, y, bullet);
}

/// Render a non-solid QBrush into a small premultiplied tile usable as a repeating
/// Blend2D pattern. Texture brushes return their image directly; hatch and dense
/// patterns are rasterised by Qt onto an 8x8 transparent tile (their period).
QImage brushToTile(const QBrush& brush) {
  if (brush.style() == Qt::TexturePattern) {
    return brush.textureImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
  }

  QImage tile(8, 8, QImage::Format_ARGB32_Premultiplied);
  tile.fill(Qt::transparent);
  QPainter p(&tile);
  p.fillRect(tile.rect(), brush);
  p.end();
  return tile;
}

/**
   @brief Convert a glyph outline (QPainterPath from QRawFont) into a BLPath.

   QPainterPath stores a cubic as a CurveToElement followed by two CurveToDataElements
   and never emits quadratics, so only move/line/cubic need handling.
 */
BLPath qPathToBLPath(const QPainterPath& pp) {
  BLPath path;
  const int count = pp.elementCount();
  for (int i = 0; i < count; ++i) {
    const QPainterPath::Element& e = pp.elementAt(i);
    switch (e.type) {
      case QPainterPath::MoveToElement:
        path.move_to(e.x, e.y);
        break;
      case QPainterPath::LineToElement:
        path.line_to(e.x, e.y);
        break;
      case QPainterPath::CurveToElement: {
        const QPainterPath::Element& c2 = pp.elementAt(i + 1);
        const QPainterPath::Element& end = pp.elementAt(i + 2);
        path.cubic_to(e.x, e.y, c2.x, c2.y, end.x, end.y);
        i += 2;
        break;
      }
      case QPainterPath::CurveToDataElement:
        // consumed together with its CurveToElement above
        break;
    }
  }
  return path;
}

/// The coloured glyph outline plus a pre-built white halo (the glyph unioned with its eight
/// ±1 px neighbours). Both are tessellated once and cached; drawing is then two path fills.
struct GlyphPaths {
  BLPath glyph;
  BLPath halo;
};

/// A resolved QRawFont together with its per-glyph outline cache.
struct FontGlyphs {
  QRawFont raw;
  QHash<quint32, GlyphPaths> glyphs;
};

/// Signature identifying a QFont for glyph-cache lookup (only attributes that affect the
/// resolved outlines matter).
quint64 fontKey(const QFont& f) {
  return quint64(qHashMulti(0, f.family(), f.pointSize(), f.pixelSize(), int(f.weight()), f.italic(), f.styleName()));
}

/**
   @brief Resolve @p font to a QRawFont and its glyph cache, memoised per thread.

   QRawFont::fromFont() (Qt's font matching) and the per-glyph outline extraction are the
   expensive parts of text drawing, and both are immutable for a given font, so they are
   cached across frames. The cache is thread-local: draw() is serialised per render thread,
   so no locking is needed and independent canvases never share (or race on) an entry.
 */
FontGlyphs& fontGlyphsFor(const QFont& font) {
  thread_local QHash<quint64, FontGlyphs> cache;
  const quint64 key = fontKey(font);
  auto it = cache.find(key);
  if (it == cache.end()) {
    FontGlyphs fg;
    fg.raw = QRawFont::fromFont(font);
    it = cache.insert(key, fg);
  }
  return it.value();
}

/// Fetch (building on first use) the cached outline + halo for a single glyph. The halo is
/// the union of the glyph shifted by its eight ±1 px neighbours, filled once under non-zero
/// winding — visually identical to eight separate offset draws but a single rasterisation.
const GlyphPaths& glyphPathsFor(FontGlyphs& fg, quint32 glyphIndex) {
  auto it = fg.glyphs.find(glyphIndex);
  if (it == fg.glyphs.end()) {
    GlyphPaths gp;
    gp.glyph = qPathToBLPath(fg.raw.pathForGlyph(glyphIndex));
    static const BLPoint offsets[8] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
    for (const BLPoint& o : offsets) {
      gp.halo.add_path(gp.glyph, o);
    }
    it = fg.glyphs.insert(glyphIndex, gp);
  }
  return it.value();
}

/**
   @brief Lay @p text out left-to-right from the origin into cached glyph + halo paths.

   Glyph mapping is cmap-only (no complex shaping) via QRawFont, which keeps Qt's font
   matching while emitting outlines Blend2D can fill. The glyph origin (0,0) is the pen
   position on the baseline, matching QPainter::drawText(). Per-glyph outlines come from the
   cache, so a repeated word only tessellates each distinct glyph once.
 */
GlyphPaths buildTextRun(FontGlyphs& fg, const QString& text) {
  GlyphPaths run;
  const QList<quint32> glyphs = fg.raw.glyphIndexesForString(text);
  const QList<QPointF> advances = fg.raw.advancesForGlyphIndexes(glyphs);
  QPointF pen(0, 0);
  for (qsizetype i = 0; i < glyphs.size(); ++i) {
    const GlyphPaths& gp = glyphPathsFor(fg, glyphs[i]);
    const BLPoint at(pen.x(), pen.y());
    run.glyph.add_path(gp.glyph, at);
    run.halo.add_path(gp.halo, at);
    pen += advances[i];
  }
  return run;
}

/// Fill @p gp's white halo then its coloured glyph(s) at @p base — two path fills. Callers
/// must have selected BL_FILL_RULE_NON_ZERO (the halo union relies on it).
void fillGlyphRun(BLContext& ctx, const GlyphPaths& gp, const BLPoint& base, BLRgba32 fill, BLRgba32 halo) {
  ctx.set_fill_style(halo);
  ctx.fill_path(base, gp.halo);
  ctx.set_fill_style(fill);
  ctx.fill_path(base, gp.glyph);
}
}  // namespace

CMapIMG::CMapIMG(const QString& filename, CMapDraw* parent)
    : IMap(eFeatVisibility | eFeatVectorItems | eFeatTypFile, parent),
      fm(CMainWindow::self().getMapFont()),
      selectedLanguage(NOIDX),
      img(filename) {
  qDebug() << "------------------------------";
  qDebug() << "IMG: try to open" << filename;

  try {
    img.read();

    // combine copyright sections
    copyright.clear();
    for (const QString& str : img.getCopyrights()) {
      if (!copyright.isEmpty()) {
        copyright += "\n";
      }
      copyright += str;
    }

    setupTyp();
  } catch (const CGarminImg::exce_t& e) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed ..."), e.msg, QMessageBox::Abort);
    return;
  }

  isActivated = true;
}

void CMapIMG::loadConfig(QSettings& cfg) {
  IMap::loadConfig(cfg);

  if (!typeFile.isEmpty()) {
    setupTyp();
  }
}

void CMapIMG::slotSetTypeFile(const QString& filename) {
  IMap::slotSetTypeFile(filename);
  setupTyp();
  CCanvas::triggerCompleteUpdate(CCanvas::eRedrawMap);
}

void CMapIMG::setupTyp() {
  languages.clear();
  languages[0x00] = tr("Unspecified");
  languages[0x01] = tr("French");
  languages[0x02] = tr("German");
  languages[0x03] = tr("Dutch");
  languages[0x04] = tr("English");
  languages[0x05] = tr("Italian");
  languages[0x06] = tr("Finnish");
  languages[0x07] = tr("Swedish");
  languages[0x08] = tr("Spanish");
  languages[0x09] = tr("Basque");
  languages[0x0a] = tr("Catalan");
  languages[0x0b] = tr("Galician");
  languages[0x0c] = tr("Welsh");
  languages[0x0d] = tr("Gaelic");
  languages[0x0e] = tr("Danish");
  languages[0x0f] = tr("Norwegian");
  languages[0x10] = tr("Portuguese");
  languages[0x11] = tr("Slovak");
  languages[0x12] = tr("Czech");
  languages[0x13] = tr("Croatian");
  languages[0x14] = tr("Hungarian");
  languages[0x15] = tr("Polish");
  languages[0x16] = tr("Turkish");
  languages[0x17] = tr("Greek");
  languages[0x18] = tr("Slovenian");
  languages[0x19] = tr("Russian");
  languages[0x1a] = tr("Estonian");
  languages[0x1b] = tr("Latvian");
  languages[0x1c] = tr("Romanian");
  languages[0x1d] = tr("Albanian");
  languages[0x1e] = tr("Bosnian");
  languages[0x1f] = tr("Lithuanian");
  languages[0x20] = tr("Serbian");
  languages[0x21] = tr("Macedonian");
  languages[0x22] = tr("Bulgarian");

  polylineProperties.clear();
  polylineProperties[0x01] = CGarminTyp::polyline_property(0x01, Qt::blue, 6, Qt::SolidLine);
  polylineProperties[0x01].penBorderDay = QPen(Qt::black, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x01].penBorderNight = QPen(Qt::lightGray, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x01].hasBorder = true;
  polylineProperties[0x02] = CGarminTyp::polyline_property(0x02, 0xffcc9900, 4, Qt::SolidLine);
  polylineProperties[0x02].penBorderDay = QPen(Qt::black, 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x02].penBorderNight = QPen(Qt::lightGray, 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x02].hasBorder = true;
  polylineProperties[0x03] = CGarminTyp::polyline_property(0x03, Qt::yellow, 3, Qt::SolidLine);
  polylineProperties[0x03].penBorderDay = QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x03].penBorderNight = QPen(Qt::lightGray, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x03].hasBorder = true;
  polylineProperties[0x04] = CGarminTyp::polyline_property(0x04, 0xffffff00, 3, Qt::SolidLine);
  polylineProperties[0x04].penBorderDay = QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x04].penBorderNight = QPen(Qt::lightGray, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x04].hasBorder = true;
  polylineProperties[0x05] = CGarminTyp::polyline_property(0x05, 0xffdc7c5a, 2, Qt::SolidLine);
  polylineProperties[0x06] = CGarminTyp::polyline_property(0x06, Qt::gray, 2, Qt::SolidLine);
  polylineProperties[0x06].penBorderDay = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x06].penBorderNight = QPen(QColor(0xfff0f0f0), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x06].hasBorder = true;
  polylineProperties[0x07] = CGarminTyp::polyline_property(0x07, 0xffc46442, 1, Qt::SolidLine);
  polylineProperties[0x08] = CGarminTyp::polyline_property(0x08, 0xffe88866, 2, Qt::SolidLine);
  polylineProperties[0x09] = CGarminTyp::polyline_property(0x09, 0xffe88866, 2, Qt::SolidLine);
  polylineProperties[0x0A] = CGarminTyp::polyline_property(0x0A, 0xff808080, 2, Qt::SolidLine);
  polylineProperties[0x0B] = CGarminTyp::polyline_property(0x0B, 0xffc46442, 2, Qt::SolidLine);
  polylineProperties[0x0C] = CGarminTyp::polyline_property(0x0C, 0xff000000, 2, Qt::SolidLine);
  polylineProperties[0x14] = CGarminTyp::polyline_property(0x14, Qt::white, 2, Qt::DotLine);
  polylineProperties[0x14].penBorderDay = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x14].penBorderNight = QPen(Qt::lightGray, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  polylineProperties[0x14].hasBorder = true;
  polylineProperties[0x15] = CGarminTyp::polyline_property(0x15, 0xff000080, 2, Qt::SolidLine);
  polylineProperties[0x16] = CGarminTyp::polyline_property(0x16, 0xffE0E0E0, 1, Qt::SolidLine);
  polylineProperties[0x18] = CGarminTyp::polyline_property(0x18, 0xff0000ff, 2, Qt::SolidLine);
  polylineProperties[0x19] = CGarminTyp::polyline_property(0x19, 0xff00ff00, 2, Qt::SolidLine);
  polylineProperties[0x1A] = CGarminTyp::polyline_property(0x1A, 0xff000000, 2, Qt::SolidLine);
  polylineProperties[0x1B] = CGarminTyp::polyline_property(0x1B, 0xff000000, 2, Qt::SolidLine);
  polylineProperties[0x1C] = CGarminTyp::polyline_property(0x1C, 0xff00c864, 2, Qt::DotLine);
  polylineProperties[0x1D] = CGarminTyp::polyline_property(0x1D, 0xff00c864, 2, Qt::DotLine);
  polylineProperties[0x1E] = CGarminTyp::polyline_property(0x1E, 0xff00c864, 2, Qt::DotLine);
  polylineProperties[0x1F] = CGarminTyp::polyline_property(0x1F, 0xff0000ff, 2, Qt::SolidLine);
  polylineProperties[0x20] = CGarminTyp::polyline_property(0x20, 0xffb67824, 1, Qt::SolidLine);
  polylineProperties[0x21] = CGarminTyp::polyline_property(0x21, 0xffb67824, 2, Qt::SolidLine);
  polylineProperties[0x22] = CGarminTyp::polyline_property(0x22, 0xffb67824, 3, Qt::SolidLine);
  polylineProperties[0x23] = CGarminTyp::polyline_property(0x23, 0xffb67824, 1, Qt::SolidLine);
  polylineProperties[0x24] = CGarminTyp::polyline_property(0x24, 0xffb67824, 2, Qt::SolidLine);
  polylineProperties[0x25] = CGarminTyp::polyline_property(0x25, 0xffb67824, 3, Qt::SolidLine);
  polylineProperties[0x26] = CGarminTyp::polyline_property(0x26, 0xff0000ff, 2, Qt::DotLine);
  polylineProperties[0x27] = CGarminTyp::polyline_property(0x27, 0xffc46442, 4, Qt::SolidLine);
  polylineProperties[0x28] = CGarminTyp::polyline_property(0x28, 0xffaa0000, 2, Qt::SolidLine);
  polylineProperties[0x29] = CGarminTyp::polyline_property(0x29, 0xffff0000, 2, Qt::SolidLine);
  polylineProperties[0x2A] = CGarminTyp::polyline_property(0x2A, 0xff000000, 2, Qt::SolidLine);
  polylineProperties[0x2B] = CGarminTyp::polyline_property(0x2B, 0xff000000, 2, Qt::SolidLine);

  polylineProperties[0x01].strings[0x00] = tr("Major highway");
  polylineProperties[0x02].strings[0x00] = tr("Principal highway");
  polylineProperties[0x03].strings[0x00] = tr("Other highway");
  polylineProperties[0x04].strings[0x00] = tr("Arterial road");
  polylineProperties[0x05].strings[0x00] = tr("Collector road");
  polylineProperties[0x06].strings[0x00] = tr("Residential street");
  polylineProperties[0x07].strings[0x00] = tr("Alley/Private road");
  polylineProperties[0x08].strings[0x00] = tr("Highway ramp, low speed");
  polylineProperties[0x09].strings[0x00] = tr("Highway ramp, high speed");
  polylineProperties[0x0a].strings[0x00] = tr("Unpaved road");
  polylineProperties[0x0b].strings[0x00] = tr("Major highway connector");
  polylineProperties[0x0c].strings[0x00] = tr("Roundabout");
  polylineProperties[0x14].strings[0x00] = tr("Railroad");
  polylineProperties[0x15].strings[0x00] = tr("Shoreline");
  polylineProperties[0x16].strings[0x00] = tr("Trail");
  polylineProperties[0x18].strings[0x00] = tr("Stream");
  polylineProperties[0x19].strings[0x00] = tr("Timezone");
  polylineProperties[0x1a].strings[0x00] = tr("Ferry");
  polylineProperties[0x1b].strings[0x00] = tr("Ferry");
  polylineProperties[0x1c].strings[0x00] = tr("State/province border");
  polylineProperties[0x1d].strings[0x00] = tr("County/parish border");
  polylineProperties[0x1e].strings[0x00] = tr("International border");
  polylineProperties[0x1f].strings[0x00] = tr("River");
  polylineProperties[0x20].strings[0x00] = tr("Minor land contour");
  polylineProperties[0x21].strings[0x00] = tr("Intermediate land contour");
  polylineProperties[0x22].strings[0x00] = tr("Major land contour");
  polylineProperties[0x23].strings[0x00] = tr("Minor depth contour");
  polylineProperties[0x24].strings[0x00] = tr("Intermediate depth contour");
  polylineProperties[0x25].strings[0x00] = tr("Major depth contour");
  polylineProperties[0x26].strings[0x00] = tr("Intermittent stream");
  polylineProperties[0x27].strings[0x00] = tr("Airport runway");
  polylineProperties[0x28].strings[0x00] = tr("Pipeline");
  polylineProperties[0x29].strings[0x00] = tr("Powerline");
  polylineProperties[0x2a].strings[0x00] = tr("Marine boundary");
  polylineProperties[0x2b].strings[0x00] = tr("Hazard boundary");

  polygonProperties.clear();
  polygonProperties[0x01] = CGarminTyp::polygon_property(0x01, Qt::NoPen, 0xffd2c0c0, Qt::SolidPattern);
  polygonProperties[0x02] = CGarminTyp::polygon_property(0x02, Qt::NoPen, 0xfffbeab7, Qt::SolidPattern);
  polygonProperties[0x03] = CGarminTyp::polygon_property(0x03, Qt::NoPen, 0xffa4b094, Qt::SolidPattern);
  polygonProperties[0x04] = CGarminTyp::polygon_property(0x04, Qt::NoPen, 0xff808080, Qt::SolidPattern);
  polygonProperties[0x05] = CGarminTyp::polygon_property(0x05, Qt::NoPen, 0xfff0f0f0, Qt::SolidPattern);
  polygonProperties[0x06] = CGarminTyp::polygon_property(0x06, Qt::NoPen, 0xffcacaca, Qt::SolidPattern);
  polygonProperties[0x07] = CGarminTyp::polygon_property(0x07, Qt::NoPen, 0xfffeebcf, Qt::SolidPattern);
  polygonProperties[0x08] = CGarminTyp::polygon_property(0x08, Qt::NoPen, 0xfffde8d5, Qt::SolidPattern);
  polygonProperties[0x09] = CGarminTyp::polygon_property(0x09, Qt::NoPen, 0xfffee8b8, Qt::SolidPattern);
  polygonProperties[0x0a] = CGarminTyp::polygon_property(0x0a, Qt::NoPen, 0xfffdeac6, Qt::SolidPattern);
  polygonProperties[0x0b] = CGarminTyp::polygon_property(0x0b, Qt::NoPen, 0xfffddfbd, Qt::SolidPattern);
  polygonProperties[0x0c] = CGarminTyp::polygon_property(0x0c, Qt::NoPen, 0xffebeada, Qt::SolidPattern);
  polygonProperties[0x0d] = CGarminTyp::polygon_property(0x0d, Qt::NoPen, 0xfff8e3be, Qt::SolidPattern);
  polygonProperties[0x0e] = CGarminTyp::polygon_property(0x0e, Qt::NoPen, 0xffe0e0e0, Qt::SolidPattern);
  polygonProperties[0x13] = CGarminTyp::polygon_property(0x13, Qt::NoPen, 0xffcc9900, Qt::SolidPattern);
  polygonProperties[0x14] = CGarminTyp::polygon_property(0x14, Qt::NoPen, 0xffb7e999, Qt::SolidPattern);
  polygonProperties[0x15] = CGarminTyp::polygon_property(0x15, Qt::NoPen, 0xffb7e999, Qt::SolidPattern);
  polygonProperties[0x16] = CGarminTyp::polygon_property(0x16, Qt::NoPen, 0xffb7e999, Qt::SolidPattern);
  polygonProperties[0x17] = CGarminTyp::polygon_property(0x17, Qt::NoPen, 0xff90be00, Qt::SolidPattern);
  polygonProperties[0x18] = CGarminTyp::polygon_property(0x18, Qt::NoPen, 0xff00ff00, Qt::SolidPattern);
  polygonProperties[0x19] = CGarminTyp::polygon_property(0x19, Qt::NoPen, 0xfff8e3be, Qt::SolidPattern);
  polygonProperties[0x1a] = CGarminTyp::polygon_property(0x1a, Qt::NoPen, 0xffd3f5a5, Qt::SolidPattern);
  polygonProperties[0x1e] = CGarminTyp::polygon_property(0x1e, Qt::NoPen, 0xffb7e999, Qt::SolidPattern);
  polygonProperties[0x1f] = CGarminTyp::polygon_property(0x1f, Qt::NoPen, 0xffb7e999, Qt::SolidPattern);
  polygonProperties[0x20] = CGarminTyp::polygon_property(0x20, Qt::NoPen, 0xffb7e999, Qt::SolidPattern);
  polygonProperties[0x28] = CGarminTyp::polygon_property(0x28, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x29] = CGarminTyp::polygon_property(0x29, Qt::NoPen, 0xff0000ff, Qt::SolidPattern);
  polygonProperties[0x32] = CGarminTyp::polygon_property(0x32, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x3b] = CGarminTyp::polygon_property(0x3b, Qt::NoPen, 0xff0000ff, Qt::SolidPattern);
  polygonProperties[0x3c] = CGarminTyp::polygon_property(0x3c, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x3d] = CGarminTyp::polygon_property(0x3d, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x3e] = CGarminTyp::polygon_property(0x3e, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x3f] = CGarminTyp::polygon_property(0x3f, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x40] = CGarminTyp::polygon_property(0x40, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x41] = CGarminTyp::polygon_property(0x41, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x42] = CGarminTyp::polygon_property(0x42, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x43] = CGarminTyp::polygon_property(0x43, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x44] = CGarminTyp::polygon_property(0x44, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x45] = CGarminTyp::polygon_property(0x45, Qt::NoPen, 0xff0000ff, Qt::SolidPattern);
  polygonProperties[0x46] = CGarminTyp::polygon_property(0x46, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x47] = CGarminTyp::polygon_property(0x47, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x48] = CGarminTyp::polygon_property(0x48, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x49] = CGarminTyp::polygon_property(0x49, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
#ifdef Q_OS_WIN64
  polygonProperties[0x4a] = CGarminTyp::polygon_property(0x4a, 0xff000000, qRgba(255, 255, 255, 0), Qt::SolidPattern);
  polygonProperties[0x4b] = CGarminTyp::polygon_property(0x4b, 0xff000000, qRgba(255, 255, 255, 0), Qt::SolidPattern);
#else
  polygonProperties[0x4a] = CGarminTyp::polygon_property(0x4a, 0xff000000, Qt::transparent, Qt::NoBrush);
  polygonProperties[0x4b] = CGarminTyp::polygon_property(0x4b, 0xff000000, Qt::transparent, Qt::NoBrush);
#endif
  polygonProperties[0x4c] = CGarminTyp::polygon_property(0x4c, Qt::NoPen, 0xfff0e68c, Qt::SolidPattern);
  polygonProperties[0x4d] = CGarminTyp::polygon_property(0x4d, Qt::NoPen, 0xff00ffff, Qt::SolidPattern);
  polygonProperties[0x4e] = CGarminTyp::polygon_property(0x4e, Qt::NoPen, 0xffd3f5a5, Qt::SolidPattern);
  polygonProperties[0x4f] = CGarminTyp::polygon_property(0x4f, Qt::NoPen, 0xffd3f5a5, Qt::SolidPattern);
  polygonProperties[0x50] = CGarminTyp::polygon_property(0x50, Qt::NoPen, 0xffb7e999, Qt::SolidPattern);
  polygonProperties[0x51] = CGarminTyp::polygon_property(0x51, Qt::NoPen, 0xff0000ff, Qt::DiagCrossPattern);
  polygonProperties[0x52] = CGarminTyp::polygon_property(0x52, Qt::NoPen, 0xff4aca4a, Qt::SolidPattern);
  polygonProperties[0x53] = CGarminTyp::polygon_property(0x53, Qt::NoPen, 0xffbcedfa, Qt::SolidPattern);
  polygonProperties[0x54] = CGarminTyp::polygon_property(0x54, Qt::NoPen, 0xfffde8d5, Qt::SolidPattern);
  polygonProperties[0x59] = CGarminTyp::polygon_property(0x59, Qt::NoPen, 0xff0080ff, Qt::SolidPattern);
  polygonProperties[0x69] = CGarminTyp::polygon_property(0x69, Qt::NoPen, 0xFF0080ff, Qt::SolidPattern);

  polygonProperties[0x01].strings[0x00] = tr("Large urban area (&gt;200K)");
  polygonProperties[0x02].strings[0x00] = tr("Small urban area (&lt;200K)");
  polygonProperties[0x03].strings[0x00] = tr("Rural housing area");
  polygonProperties[0x04].strings[0x00] = tr("Military base");
  polygonProperties[0x05].strings[0x00] = tr("Parking lot");
  polygonProperties[0x06].strings[0x00] = tr("Parking garage");
  polygonProperties[0x07].strings[0x00] = tr("Airport");
  polygonProperties[0x08].strings[0x00] = tr("Shopping center");
  polygonProperties[0x09].strings[0x00] = tr("Marina");
  polygonProperties[0x0a].strings[0x00] = tr("University/College");
  polygonProperties[0x0b].strings[0x00] = tr("Hospital");
  polygonProperties[0x0c].strings[0x00] = tr("Industrial complex");
  polygonProperties[0x0d].strings[0x00] = tr("Reservation");
  polygonProperties[0x0e].strings[0x00] = tr("Airport runway");
  polygonProperties[0x13].strings[0x00] = tr("Man-made area");
  polygonProperties[0x19].strings[0x00] = tr("Sports complex");
  polygonProperties[0x18].strings[0x00] = tr("Golf course");
  polygonProperties[0x1a].strings[0x00] = tr("Cemetery");
  polygonProperties[0x14].strings[0x00] = tr("National park");
  polygonProperties[0x15].strings[0x00] = tr("National park");
  polygonProperties[0x16].strings[0x00] = tr("National park");
  polygonProperties[0x17].strings[0x00] = tr("City park");
  polygonProperties[0x1e].strings[0x00] = tr("State park");
  polygonProperties[0x1f].strings[0x00] = tr("State park");
  polygonProperties[0x20].strings[0x00] = tr("State park");
  polygonProperties[0x50].strings[0x00] = tr("Forest");
  polygonProperties[0x28].strings[0x00] = tr("Ocean");
  polygonProperties[0x29].strings[0x00] = tr("Blue (unknown)");
  polygonProperties[0x32].strings[0x00] = tr("Sea");
  polygonProperties[0x3b].strings[0x00] = tr("Blue (unknown)");
  polygonProperties[0x3c].strings[0x00] = tr("Large lake");
  polygonProperties[0x3d].strings[0x00] = tr("Large lake");
  polygonProperties[0x3e].strings[0x00] = tr("Medium lake");
  polygonProperties[0x3f].strings[0x00] = tr("Medium lake");
  polygonProperties[0x40].strings[0x00] = tr("Small lake");
  polygonProperties[0x41].strings[0x00] = tr("Small lake");
  polygonProperties[0x42].strings[0x00] = tr("Major lake");
  polygonProperties[0x43].strings[0x00] = tr("Major lake");
  polygonProperties[0x44].strings[0x00] = tr("Large lake");
  polygonProperties[0x46].strings[0x00] = tr("Blue (unknown)");
  polygonProperties[0x46].strings[0x00] = tr("Major River");
  polygonProperties[0x47].strings[0x00] = tr("Large River");
  polygonProperties[0x48].strings[0x00] = tr("Medium River");
  polygonProperties[0x49].strings[0x00] = tr("Small River");
  //    polygonProperties[0x4a].strings[0x00] = tr("Definition area");
  //    polygonProperties[0x4b].strings[0x00] = tr("Background");
  polygonProperties[0x4c].strings[0x00] = tr("Intermittent water");
  polygonProperties[0x51].strings[0x00] = tr("Wetland/Swamp");
  polygonProperties[0x4d].strings[0x00] = tr("Glacier");
  polygonProperties[0x4e].strings[0x00] = tr("Orchard/Plantation");
  polygonProperties[0x4f].strings[0x00] = tr("Scrub");
  polygonProperties[0x52].strings[0x00] = tr("Tundra");
  polygonProperties[0x53].strings[0x00] = tr("Flat");
  polygonProperties[0x54].strings[0x00] = tr("???");
  polygonDrawOrder.clear();
  for (int i = 0; i < 0x80; i++) {
    polygonDrawOrder << (0x7F - i);
  }

  pointProperties.clear();

  if (!typeFile.isEmpty()) {
    QFile file(typeFile);
    if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::warning(CMainWindow::self().getBestWidgetForParent(), tr("Read external type file..."),
                           tr("Failed to read type file: %1\nFall back to internal types.").arg(typeFile),
                           QMessageBox::Ok);
      typeFile.clear();
      setupTyp();
      return;
    }

    QByteArray array = file.readAll();
    CGarminTyp typ;
    typ.decode(array, polygonProperties, polylineProperties, polygonDrawOrder, pointProperties);

    file.close();
  } else {
    const QMap<QString, CGarminImg::subfile_desc_t>& subfiles = img.getSubfiles();
    QMap<QString, CGarminImg::subfile_desc_t>::const_iterator subfile = subfiles.begin();
    while (subfile != subfiles.end()) {
      if (!(*subfile).parts.contains("TYP")) {
        ++subfile;
        continue;
      }

      CFileExt file(img.getFilename());
      openFileCheckSuccess(QIODevice::ReadOnly, file);

      QByteArray array;
      img.readFile(file, (*subfile).parts["TYP"].offset, (*subfile).parts["TYP"].size, array);

      CGarminTyp typ;
      typ.decode(array, polygonProperties, polylineProperties, polygonDrawOrder, pointProperties);

      file.close();
      break;
    }
  }
}

quint8 CMapIMG::scale2bits(const QPointF& scale) {
  qint32 bits = 24;
  if (scale.x() >= 70000.0) {
    bits = 2;
  } else if (scale.x() >= 50000.0) {
    bits = 3;
  } else if (scale.x() >= 30000.0) {
    bits = 4;
  } else if (scale.x() >= 20000.0) {
    bits = 5;
  } else if (scale.x() >= 15000.0) {
    bits = 6;
  } else if (scale.x() >= 10000.0) {
    bits = 7;
  } else if (scale.x() >= 7000.0) {
    bits = 8;
  } else if (scale.x() >= 5000.0) {
    bits = 9;
  } else if (scale.x() >= 3000.0) {
    bits = 10;
  } else if (scale.x() >= 2000.0) {
    bits = 11;
  } else if (scale.x() >= 1500.0) {
    bits = 12;
  } else if (scale.x() >= 1000.0) {
    bits = 13;
  } else if (scale.x() >= 700.0) {
    bits = 14;
  } else if (scale.x() >= 500.0) {
    bits = 15;
  } else if (scale.x() >= 300.0) {
    bits = 16;
  } else if (scale.x() >= 200.0) {
    bits = 17;
  } else if (scale.x() >= 100.0) {
    bits = 18;
  } else if (scale.x() >= 70.0) {
    bits = 19;
  } else if (scale.x() >= 30.0) {
    bits = 20;
  } else if (scale.x() >= 15.0) {
    bits = 21;
  } else if (scale.x() >= 7.0) {
    bits = 22;
  } else if (scale.x() >= 3.0) {
    bits = 23;
  }

  return qMax(2, qMin(24, bits + getAdjustDetailLevel()));
}

void CMapIMG::draw(IDrawContext::buffer_t& buf) /* override */
{
  if (map->needsRedraw()) {
    return;
  }

  QPointF bufferScale = buf.scale * buf.zoomFactor;

  if (isOutOfScale(bufferScale)) {
    return;
  }

  quint8 bits = scale2bits(bufferScale);

  qreal u1 = qMin(buf.ref1.x(), buf.ref4.x());
  qreal u2 = qMax(buf.ref2.x(), buf.ref3.x());
  qreal v1 = qMax(buf.ref1.y(), buf.ref2.y());
  qreal v2 = qMin(buf.ref4.y(), buf.ref3.y());

  QRectF viewport(u1, v1, u2 - u1, v2 - v1);
  QVector<QRectF> rectPois;

  polygons.clear();
  polylines.clear();
  pois.clear();
  points.clear();
  labels.clear();

  /**
     convertRad2Px() converts positions into screen coordinates. However the painter
     devices paints into the buffer which is a little bit larger than the screen.
     Thus we need the offset of the buffer's top left corner to the top left corner
     of the screen to adjust all drawings.
   */
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);

  /*
     The shared draw buffer is plain (non-premultiplied) ARGB32, but Blend2D only
     renders into premultiplied ARGB. Convert the buffer in place for the duration
     of this draw and restore the original format before returning, so the change
     stays invisible to the rest of the map stack.
   */
  buf.image.convertTo(QImage::Format_ARGB32_Premultiplied);

  bool ok = !map->needsRedraw();
  {
    BLImage blBuf;
    if (blBuf.create_from_data(buf.image.width(), buf.image.height(), BL_FORMAT_PRGB32, buf.image.bits(),
                               buf.image.bytesPerLine()) != BL_SUCCESS) {
      buf.image.convertTo(QImage::Format_ARGB32);
      return;
    }

    // use Blend2Ds parallel renderer
    BLContextCreateInfo cci{};
    cci.thread_count = std::min(std::thread::hardware_concurrency(), 4u);  // gains quickly diminish >4
    // Render synchronously rather than erroring if the thread pool can't be acquired.
    cci.flags = BL_CONTEXT_CREATE_FLAG_FALLBACK_TO_SYNC;

    BLContext ctx(blBuf, cci);
    ctx.set_global_alpha(getOpacity() / 100.0);
    /*
       The buffer is allocated at device resolution (pixelRatio larger) and tagged with
       QImage::setDevicePixelRatio(). QPainter applies that ratio implicitly, but Blend2D
       renders into the raw pixels and knows nothing about it. Scale the context by the
       pixel ratio so the logical coordinates produced by convertRad2Px() map to device
       pixels exactly as the QPainter text phase (and the rest of the map stack) expect.
     */
    ctx.scale(buf.image.devicePixelRatio(), buf.image.devicePixelRatio());
    ctx.translate(-pp.x(), -pp.y());

    if (ok) {
      try {
        CGarminImg::filter_t filter;
        filter.points = getShowPOIs();
        filter.polylines = getShowPolylines();
        filter.polygons = getShowPolygons();
        img.load(bits, viewport, filter, [this]() { return map->needsRedraw(); }, polygons, polylines, points, pois);
#if defined(DEBUG_SHOW_SECTION_BORDERS) || defined(DEBUG_SHOW_SUBDIV_BORDERS)
        // An aborted load is drawn again; its borders would show subdivisions not loaded.
        for (const CGarminImg::subfile_desc_t& subfile : img.getSubfiles()) {
          if (map->needsRedraw()) {
            break;
          }
          if (!subfile.area.intersects(viewport)) {
            continue;
          }
#ifdef DEBUG_SHOW_SECTION_BORDERS
          for (const CGarminImg::subdiv_desc_t& subdiv : subfile.subdivs) {
            if (qint32(subdiv.level) != CGarminImg::level(subfile, bits) || !subdiv.area.intersects(viewport)) {
              continue;
            }
            const QRectF& a = subdiv.area;
            qreal u[2] = {a.left(), a.right()};
            qreal v[2] = {a.top(), a.bottom()};

            QPolygonF poly;
            poly << a.bottomLeft() << a.bottomRight() << a.topRight() << a.topLeft();

            map->convertRad2Px(poly);

            ctx.set_stroke_width(2);
            ctx.set_stroke_style(BLRgba32(0xFFFF00FFu));  // magenta
            ctx.stroke_polygon(toBLPoints(poly), static_cast<size_t>(poly.size()));
          }
#endif
#ifdef DEBUG_SHOW_SUBDIV_BORDERS
          QPointF p1 = subfile.area.bottomLeft();
          QPointF p2 = subfile.area.bottomRight();
          QPointF p3 = subfile.area.topRight();
          QPointF p4 = subfile.area.topLeft();

          map->convertRad2Px(p1);
          map->convertRad2Px(p2);
          map->convertRad2Px(p3);
          map->convertRad2Px(p4);

          QPolygonF poly;
          poly << p1 << p2 << p3 << p4;
          ctx.set_stroke_width(1);
          ctx.set_stroke_style(BLRgba32(0xFF000000u));  // black
          ctx.stroke_polygon(toBLPoints(poly), static_cast<size_t>(poly.size()));
#endif
        }
#endif
      } catch (const std::bad_alloc&) {
        qWarning() << "GarminIMG: Allocation error. Abort map rendering.";
        ok = false;
      }
    }

    if (ok && !map->needsRedraw()) {
      drawPolygons(ctx, polygons);
    }
    if (ok && !map->needsRedraw()) {
      drawPolylines(ctx, polylines, bufferScale);
    }
    if (ok && !map->needsRedraw()) {
      drawPoints(ctx, points, rectPois);
    }
    if (ok && !map->needsRedraw()) {
      drawPois(ctx, pois, rectPois);
    }

    // Text and labels are drawn on top, in the same Blend2D context. Glyph outlines come
    // from QRawFont (see drawText()/drawLabels()); the context already carries the global
    // alpha, pixel-ratio scale and -pp translation, so no separate QPainter pass is needed.
    if (ok && !map->needsRedraw()) {
      drawText(ctx);
    }
    if (ok && !map->needsRedraw()) {
      drawLabels(ctx, labels);
    }

    // In multithreaded mode the queued rasterisation completes here.
    ctx.end();
  }

  buf.image.convertTo(QImage::Format_ARGB32);
}

void CMapIMG::drawPolygons(BLContext& ctx, polytype_t& lines) {
  const bool night = CMainWindow::self().isNight();

  // QPainter::drawPolygon fills using the odd-even rule; Blend2D defaults to non-zero.
  ctx.set_fill_rule(BL_FILL_RULE_EVEN_ODD);

  // Anchor the pattern phase to a fixed geographic point so textures/hatches stay put
  // when the map is panned. convertRad2Px() of a constant coordinate shifts by exactly
  // the pan delta each frame, so the pattern phase (vertexPx - anchorPx) depends only on
  // geography and zoom, not on the viewport offset. Without this the tile origin sits at
  // user-space (0,0) — a viewport-fixed pixel — and the pattern slides under the polygons.
  QPointF anchorPx(0.0, 0.0);  // lon/lat origin, radians
  map->convertRad2Px(anchorPx);

  // Bucket the polygon indices by type once, so each draw-order pass visits only the
  // polygons of its type instead of scanning the whole list once per type.
  QHash<quint32, QVector<qsizetype> > byType;
  for (qsizetype i = 0; i < lines.size(); ++i) {
    byType[lines[i].type].push_back(i);
  }

  // Fallback for types that have geometry but no matching property (magenta hatch,
  // known == false). Static so we never mutate polygonProperties while drawing.
  static const CGarminTyp::polygon_property unknownProperty;

  for (qsizetype n = polygonDrawOrder.size() - 1; n >= 0; --n) {
    const quint32 type = polygonDrawOrder[n];
    const auto bucket = byType.constFind(type);
    if (bucket == byType.constEnd()) {
      continue;
    }

    const auto propIt = polygonProperties.constFind(type);
    const CGarminTyp::polygon_property& property =
        (propIt != polygonProperties.constEnd()) ? propIt.value() : unknownProperty;
    if (!property.known) {
      qDebug() << "unknown polygon" << Qt::hex << type;
    }

    const QBrush& brush = night ? property.brushNight : property.brushDay;

    // Configure the fill style once per type. Solid colours map directly; texture
    // images and hatch patterns are realised as a repeating BLPattern. The pattern's
    // pixel data (tile) must outlive every fill that uses it, hence the outer scope.
    BLPattern pattern;
    if (brush.style() == Qt::SolidPattern) {
      ctx.set_fill_style(toBLColor(brush.color()));
    } else {
      const QImage tile = brushToTile(brush);
      // Owning copy so the pattern's pixels outlive this per-type scope and any
      // deferred fills that reference it (see toOwnedBLImage()).
      const BLImage tileBL = toOwnedBLImage(tile);
      if (!tileBL.is_empty()) {
        pattern.set_image(tileBL);
        pattern.set_extend_mode(BL_EXTEND_MODE_REPEAT);
        // REPEAT makes only the fractional offset matter; fmod keeps the translation small
        // so we don't lose sub-pixel precision at high zoom.
        pattern.translate(std::fmod(anchorPx.x(), tile.width()), std::fmod(anchorPx.y(), tile.height()));
        ctx.set_fill_style(pattern);
      } else {
        ctx.set_fill_style(toBLColor(brush.color()));
      }
    }

    const bool hasOutline = applyPen(ctx, property.pen);

    for (const qsizetype idx : *bucket) {
      QPolygonF& poly = lines[idx].pixel;
      map->convertRad2Px(poly);

      // Blend2D treats a polygon as implicitly closed, matching the closed BLPath used
      // before; stroke_polygon adds the closing edge just like QPainter::drawPolygon.
      const BLPoint* pts = toBLPoints(poly);
      const size_t n = static_cast<size_t>(poly.size());
      ctx.fill_polygon(pts, n);
      if (hasOutline) {
        ctx.stroke_polygon(pts, n);
      }
    }
  }
}

void CMapIMG::drawPolylines(BLContext& ctx, polytype_t& lines, const QPointF& scale) {
  textpaths.clear();
  const bool night = CMainWindow::self().isNight();
  QFont font = CMainWindow::self().getMapFont();

  font.setPointSize(9);
  font.setBold(false);

  QVector<qreal> lengths;
  lengths.reserve(100);

  QHash<quint32, QList<quint32> > dict;
  for (int i = 0; i < lines.count(); ++i) {
    dict[lines[i].type].push_back(i);
  }

  auto drawTypes = [&](const QList<quint32>& types) {
    for (quint32 type : types) {
      const auto prop = polylineProperties.constFind(type);
      if (prop == polylineProperties.constEnd()) {
        continue;
      }
      const CGarminTyp::polyline_property& property = *prop;

      const auto it = dict.constFind(type);
      if (it == dict.constEnd()) {
        continue;
      }
      const QList<quint32>& indices = *it;

      if (property.hasPixmap) {
        const BLImage& pixmap = night ? property.imgNight : property.imgDay;
        const qreal h = pixmap.height();

        for (quint32 idx : indices) {
          CGarminPolygon& item = lines[idx];
          QPolygonF& poly = item.pixel;
          const int size = poly.size();

          if (size < 2) {
            continue;
          }

          map->convertRad2Px(poly);

          lengths.resize(0);
          lengths.reserve(size);

          qreal u1 = poly[0].x();
          qreal v1 = poly[0].y();
          for (int i = 1; i < size; ++i) {
            qreal u2 = poly[i].x();
            qreal v2 = poly[i].y();

            qreal segLength = qSqrt((u2 - u1) * (u2 - u1) + (v2 - v1) * (v2 - v1));
            lengths << segLength;

            u1 = u2;
            v1 = v2;
          }

          if (scale.x() < kLineLabelThreshold && property.labelType != CGarminTyp::eNone) {
            QFont f(font);
            switch (property.labelType) {
              case CGarminTyp::eSmall:
                f.setPointSize(font.pointSize() - 2);
                break;
              case CGarminTyp::eLarge:
                f.setPointSize(font.pointSize() + 2);
                break;
              default:;
            }

            collectText(item, poly, f, night ? property.colorLabelNight : property.colorLabelDay,
                        scale.x() >= kShortLabelThreshold);
          }

          // Lay the pixmap along each straight segment of the polyline. The arc-length
          // breakpoints used to coincide with the polyline vertices, so we can index
          // the vertices directly instead of re-walking a QPainterPath.
          for (int i = 0; i + 1 < size; ++i) {
            const qreal segLength = lengths.at(i);
            if (segLength < 1.0) {
              continue;
            }

            const QPointF& p1 = poly[i];
            const QPointF& p2 = poly[i + 1];
            const double angle = std::atan2(p2.y() - p1.y(), p2.x() - p1.x());

            const BLImage seg = img2line(pixmap, static_cast<int>(segLength));
            if (seg.is_empty()) {
              continue;
            }

            ctx.save();
            ctx.translate(p1.x(), p1.y());
            ctx.rotate(angle);
            ctx.blit_image(BLPoint(0.0, -h / 2.0), seg);
            ctx.restore();
          }
        }
      } else {
        // First run: the background (border) line for bordered types, otherwise the
        // line itself. Either way the labels are collected here.
        const QPen& pen = property.hasBorder ? (night ? property.penBorderNight : property.penBorderDay)
                                             : (night ? property.penLineNight : property.penLineDay);
        const bool stroke = applyPen(ctx, pen);
        for (quint32 idx : indices) {
          drawLine(ctx, lines[idx], stroke, property, font, scale);
        }
      }
    }

    // 2nd run to draw the foreground lines over their borders.
    for (quint32 type : types) {
      const auto prop = polylineProperties.constFind(type);
      if (prop == polylineProperties.constEnd()) {
        continue;
      }
      const CGarminTyp::polyline_property& property = *prop;

      const auto it = dict.constFind(type);
      if (it == dict.constEnd()) {
        continue;
      }
      const QList<quint32>& indices = *it;

      if (property.hasBorder && !property.hasPixmap) {
        const QPen& pen = night ? property.penLineNight : property.penLineDay;
        if (applyPen(ctx, pen)) {
          for (quint32 idx : indices) {
            drawLine(ctx, lines[idx]);
          }
        }
      }
    }
  };

  // Types are drawn in ascending order, 0x01-0x03 last: Garmin draws them over all other lines, and maps put
  // overlays like cycle routes there.
  QList<quint32> types = dict.keys();
  std::sort(types.begin(), types.end());
  const auto top =
      std::stable_partition(types.begin(), types.end(), [](quint32 type) { return type < 0x01 || type > 0x03; });
  const QList<quint32> topTypes(top, types.end());
  types.resize(std::distance(types.begin(), top));
  drawTypes(types);
  drawTypes(topTypes);
}

void CMapIMG::drawLine(BLContext& ctx, CGarminPolygon& l, bool stroke, const CGarminTyp::polyline_property& property,
                       const QFont& font, const QPointF& scale) {
  QPolygonF& poly = l.pixel;
  const int size = poly.size();

  if (size < 2) {
    return;
  }

  map->convertRad2Px(poly);

  if (scale.x() < kLineLabelThreshold && property.labelType != CGarminTyp::eNone) {
    QFont f(font);
    switch (property.labelType) {
      case CGarminTyp::eSmall:
        f.setPointSize(font.pointSize() - 2);
        break;
      case CGarminTyp::eLarge:
        f.setPointSize(font.pointSize() + 2);
        break;
      default:;
    }

    collectText(l, poly, f, CMainWindow::self().isNight() ? property.colorLabelNight : property.colorLabelDay,
                scale.x() >= kShortLabelThreshold);
  }

  if (stroke) {
    ctx.stroke_polyline(toBLPoints(poly), static_cast<size_t>(poly.size()));
  }
}

void CMapIMG::drawLine(BLContext& ctx, const CGarminPolygon& l) {
  const QPolygonF& poly = l.pixel;
  if (poly.size() < 2) {
    return;
  }

  ctx.stroke_polyline(toBLPoints(poly), static_cast<size_t>(poly.size()));
}

void CMapIMG::collectText(const CGarminPolygon& item, const QPolygonF& line, const QFont& font, const QColor& color,
                          bool shortName) {
  // BaseCamp leaves minor contours and boundaries unlabelled zoomed out
  if (shortName && (item.type == 0x20 || item.type == 0x1c)) {
    return;
  }

  QString str;
  if (item.hasLabel()) {
    str = shortName ? item.getShortText() : item.getLabelText();
  }

  if (str.isEmpty()) {
    return;
  }

  textpath_t tp;
  tp.polyline = line;
  tp.font = font;
  tp.text = str;
  tp.color = color;
  tp.shield = IGarminStrTbl::shieldCode(item.labels.first());

  textpaths << tp;
}

bool CMapIMG::overlapsLabel(const QPainterPath& outline, const QList<QPainterPath>& placed) const {
  const QRectF bounds = outline.boundingRect();
  for (const strlbl_t& label : labels) {
    if (bounds.intersects(label.rect) && outline.intersects(QRectF(label.rect))) {
      return true;
    }
  }
  for (const QPainterPath& other : placed) {
    if (bounds.intersects(other.boundingRect()) && outline.intersects(other)) {
      return true;
    }
  }
  return false;
}

bool CMapIMG::intersectsWithExistingLabel(const QRect& rect) const {
  for (const strlbl_t& label : labels) {
    if (label.rect.intersects(rect)) {
      return true;
    }
  }

  return false;
}

void CMapIMG::addLabel(const CGarminPoint& pt, const QRect& rect, const CGarminTyp::point_property& property,
                       bool isNight) {
  QString str;
  if (pt.hasLabel()) {
    str = pt.getLabelText();
  }

  labels.push_back(strlbl_t());
  strlbl_t& strlbl = labels.last();
  strlbl.pt = pt.pos.toPoint();
  strlbl.str = str;
  strlbl.rect = rect;
  strlbl.property = property;
  strlbl.isNight = isNight;
}

void CMapIMG::drawPoints(BLContext& ctx, pointtype_t& pts, QVector<QRectF>& rectPois) {
  const bool night = CMainWindow::self().isNight();
  static const CGarminTyp::point_property unknownProperty;
  pointtype_t::iterator pt = pts.begin();
  while (pt != pts.end()) {
    map->convertRad2Px(pt->pos);

    const auto propIt = pointProperties.constFind(pt->type);
    const CGarminTyp::point_property& property =
        (propIt != pointProperties.constEnd()) ? propIt.value() : unknownProperty;

    const BLImage& icon = night ? property.imgNight : property.imgDay;
    const QSizeF size(icon.width(), icon.height());

    if (isCluttered(rectPois, QRectF(pt->pos, size))) {
      if (size.width() <= 8 && size.height() <= 8) {
        blitIcon(ctx, pt->pos.x() - (size.width() / 2), pt->pos.y() - (size.height() / 2), icon);
      } else {
        blitBullet(ctx, pt->pos.x() - 4, pt->pos.y() - 4);
      }
      ++pt;
      continue;
    }

    blitIcon(ctx, pt->pos.x() - (size.width() / 2), pt->pos.y() - (size.height() / 2), icon);

    if (CMainWindow::self().isPoiText() && property.labelType != CGarminTyp::eNone) {
      // calculate bounding rectangle with a border of 2 px
      QRect rect = fm.boundingRect(pt->getLabelText());
      rect.adjust(0, 0, 4, 4);
      rect.moveCenter(pt->pos.toPoint());

      // if no intersection was found, add label to list
      if (!intersectsWithExistingLabel(rect)) {
        addLabel(*pt, rect, property, night);
      }
    }
    ++pt;
  }
}

void CMapIMG::drawPois(BLContext& ctx, pointtype_t& pts, QVector<QRectF>& rectPois) {
  const bool night = CMainWindow::self().isNight();
  static const CGarminTyp::point_property unknownProperty;
  for (CGarminPoint& pt : pts) {
    map->convertRad2Px(pt.pos);

    const auto propIt = pointProperties.constFind(pt.type);
    const CGarminTyp::point_property& property =
        (propIt != pointProperties.constEnd()) ? propIt.value() : unknownProperty;
    const BLImage& icon = night ? property.imgNight : property.imgDay;
    const QSizeF size(icon.width(), icon.height());

    if (isCluttered(rectPois, QRectF(pt.pos, size))) {
      if (size.width() <= 8 && size.height() <= 8) {
        blitIcon(ctx, pt.pos.x() - (size.width() / 2), pt.pos.y() - (size.height() / 2), icon);
      } else {
        blitBullet(ctx, pt.pos.x() - 4, pt.pos.y() - 4);
      }
      continue;
    }

    blitIcon(ctx, pt.pos.x() - (size.width() / 2), pt.pos.y() - (size.height() / 2), icon);

    if (CMainWindow::self().isPoiText()) {
      // calculate bounding rectangle with a border of 2 px
      QRect rect = fm.boundingRect(pt.getLabelText());
      rect.adjust(0, 0, 4, 4);
      rect.moveCenter(pt.pos.toPoint());

      // if no intersection was found, add label to list
      if (!intersectsWithExistingLabel(rect)) {
        addLabel(pt, rect, property, night);
      }
    }
  }
}

void CMapIMG::drawLabels(BLContext& ctx, const QVector<strlbl_t>& lbls) {
  // See drawText(): non-zero winding for glyph outlines and the unioned halo.
  ctx.set_fill_rule(BL_FILL_RULE_NON_ZERO);

  QFont f = CMainWindow::self().getMapFont();
  QVector<QFont> fonts(8, f);
  fonts[CGarminTyp::eSmall].setPointSize(f.pointSize() - 2);
  fonts[CGarminTyp::eLarge].setPointSize(f.pointSize() + 2);

  const BLRgba32 halo = toBLColor(Qt::white);

  for (const strlbl_t& lbl : lbls) {
    const int type = lbl.property.labelType;
    FontGlyphs& fg = fontGlyphsFor(fonts[type]);

    // Match CDraw::text()'s placement: centre the metrics bounding box on the anchor and
    // use its top-left as the baseline-left pen origin.
    QRect r = QFontMetrics(fonts[type]).boundingRect(lbl.str);
    r.moveCenter(lbl.pt);

    const GlyphPaths run = buildTextRun(fg, lbl.str);
    const BLRgba32 fill = toBLColor(lbl.isNight ? lbl.property.colorLabelNight : lbl.property.colorLabelDay);
    fillGlyphRun(ctx, run, BLPoint(r.left(), r.top()), fill, halo);
  }
}

void CMapIMG::drawText(BLContext& ctx) {
  // Glyph outlines want non-zero winding (drawPolygons leaves the context on even-odd), and
  // the cached halo is a union of offset copies that only fills correctly under non-zero.
  ctx.set_fill_rule(BL_FILL_RULE_NON_ZERO);

  // outlines of the street names placed so far
  QList<QPainterPath> placed;

  for (const textpath_t& textpath : std::as_const(textpaths)) {
    if (textpath.shield != 0) {
      drawShield(ctx, textpath, placed);
      continue;
    }

    const QFont& font = textpath.font;
    const QFontMetricsF fm(font);
    // Letters are set one by one, so the label is as wide as their advances add up to, which can be more than
    // the advance of the whole text.
    const QString& text = textpath.text;
    QVector<qreal> advances;
    advances.reserve(text.size());
    qreal width = 0;
    for (const QChar& c : text) {
      advances << fm.horizontalAdvance(c);
      width += advances.last();
    }

    // The label goes on the line's longest stretch without a sharp bend, or not at all.
    QPolygonF stretch;
    const qreal length = longestStraightStretch(textpath.polyline, stretch);
    if (length < width) {
      continue;
    }

    QPainterPath path;
    path.addPolygon(stretch);

    // Glyph outlines are rasterised by Blend2D from QRawFont (cached across frames), keeping
    // Qt's font matching at the same resolved size. Glyph ids are looked up once per label.
    FontGlyphs& fg = fontGlyphsFor(font);
    const QList<quint32> glyphIds = fg.raw.glyphIndexesForString(textpath.text);
    const BLRgba32 fill = toBLColor(textpath.color);
    const BLRgba32 halo = toBLColor(Qt::white);

    // vertical middle of the text on the line; glyphs sit on their baseline
    const qreal baselineShift = (fm.ascent() - fm.descent()) / 2;

    // centre the label on the stretch
    qreal offset = (length - width) / 2;

    // Read left to right; within kLabelVertical of vertical the label keeps the direction of its line. The label is
    // centred, so reversing keeps its place.
    QPointF point1 = path.pointAtPercent(offset / length);
    QPointF point2 = path.pointAtPercent((offset + width) / length);
    const qreal dx = point2.x() - point1.x();
    const qreal dy = point2.y() - point1.y();
    const bool vertical = qAbs(dx) <= qAbs(dy) * qTan(qDegreesToRadians(kLabelVertical));
    if (!vertical && dx < 0) {
      path = path.toReversed();
    }

    // A name that would overlap a point label or another name is dropped.
    const QPainterPath outline = labelOutline(path, offset / length, (offset + width) / length, fm.height());
    if (overlapsLabel(outline, placed)) {
      continue;
    }
    placed << outline;

    // draw string letter by letter and adjust angle
    const int size = text.size();
    qreal percent2 = offset / length;
    point2 = path.pointAtPercent(percent2);

    for (int i = 0; i < size; ++i) {
      // percent1 = percent2;
      percent2 = (offset + advances[i]) / length;

      point1 = point2;
      point2 = path.pointAtPercent(percent2);

      // BLContext::rotate() takes radians; atan2() also resolves the quadrant the old
      // qAtan()+180 fix-up handled by hand.
      const qreal angle = std::atan2(point2.y() - point1.y(), point2.x() - point1.x());

      ctx.save();
      ctx.translate(point1.x(), point1.y());
      ctx.rotate(angle);
      ctx.translate(0, baselineShift);

      if (i < glyphIds.size()) {
        fillGlyphRun(ctx, glyphPathsFor(fg, glyphIds[i]), BLPoint(0, 0), fill, halo);
      }

      ctx.restore();

      offset += advances[i];
    }
  }
}

void CMapIMG::drawShield(BLContext& ctx, const textpath_t& textpath, QList<QPainterPath>& placed) {
  const QFontMetricsF fm(textpath.font);
  const qreal pad = 3;
  const qreal h = fm.height() + 2;
  qreal w = fm.horizontalAdvance(textpath.text) + 2 * pad;
  if (textpath.shield == 0x03) {
    // round: an ellipse needs room left and right of the text
    w = qMax(w * 1.3, h);
  }

  QPolygonF stretch;
  const qreal length = longestStraightStretch(textpath.polyline, stretch);
  if (length < w) {
    return;
  }
  QPainterPath path;
  path.addPolygon(stretch);
  const QPointF centre = path.pointAtPercent(0.5);
  const QRectF box(centre.x() - w / 2, centre.y() - h / 2, w, h);

  QPainterPath outline;
  outline.addRect(box);
  if (overlapsLabel(outline, placed)) {
    return;
  }
  placed << outline;

  // the shields' look, from BaseCamp; map symbols, not themed UI colours
  const BLRgba32 white(0xFFFFFFFF);
  const BLRgba32 black(0xFF000000);
  const BLRgba32 blue(0xFF2F6CA8);
  const BLRgba32 red(0xFFD7262E);
  BLRgba32 text = black;

  // the polylines leave their pen on the context: a dash pattern, caps and join
  ctx.save();
  ctx.set_stroke_dash_array(BLArray<double>());
  ctx.set_stroke_caps(BL_STROKE_CAP_BUTT);
  ctx.set_stroke_join(BL_STROKE_JOIN_MITER_CLIP);
  ctx.set_stroke_width(1);
  ctx.set_stroke_style(black);
  switch (textpath.shield) {
    case 0x01:  // interstate: red top, blue below, both rounded on the outside
      ctx.fill_round_rect(box.x(), box.y(), w, h, h / 4, h / 4, red);
      ctx.fill_round_rect(box.x(), box.y() + h / 3, w, h - h / 3, h / 4, h / 4, blue);
      ctx.fill_rect(BLRect(box.x(), box.y() + h / 3, w, h / 4), blue);
      text = white;
      break;
    case 0x02:  // shield
      ctx.fill_round_rect(box.x(), box.y(), w, h, h / 4, h / 4, white);
      ctx.stroke_round_rect(box.x(), box.y(), w, h, h / 4);
      break;
    case 0x03:  // round
      ctx.fill_ellipse(centre.x(), centre.y(), w / 2, h / 2, white);
      ctx.stroke_ellipse(centre.x(), centre.y(), w / 2, h / 2);
      break;
    case 0x04:  // hbox: blue, red bars top and bottom
      ctx.fill_rect(BLRect(box.x(), box.y(), w, h), blue);
      ctx.fill_rect(BLRect(box.x(), box.y(), w, 2), red);
      ctx.fill_rect(BLRect(box.x(), box.bottom() - 2, w, 2), red);
      text = white;
      break;
    case 0x05:  // box: white, black bars top and bottom
      ctx.fill_rect(BLRect(box.x(), box.y(), w, h), white);
      ctx.stroke_rect(box.x(), box.y(), w, h);
      ctx.fill_rect(BLRect(box.x(), box.y(), w, 2), black);
      ctx.fill_rect(BLRect(box.x(), box.bottom() - 2, w, 2), black);
      break;
    default:  // oval
      ctx.fill_round_rect(box.x(), box.y(), w, h, h / 2, h / 2, white);
      ctx.stroke_round_rect(box.x(), box.y(), w, h, h / 2);
  }

  FontGlyphs& fg = fontGlyphsFor(textpath.font);
  const GlyphPaths run = buildTextRun(fg, textpath.text);
  const qreal textWidth = fm.horizontalAdvance(textpath.text);
  const BLPoint base(centre.x() - textWidth / 2, centre.y() + (fm.ascent() - fm.descent()) / 2);
  ctx.set_fill_style(text);
  ctx.fill_path(base, run.glyph);
  ctx.restore();
}

void CMapIMG::getToolTip(const QPoint& px, QString& infotext) const /* override */
{
  QString str;

  // Append every value of a dict (skipping the "-" placeholder) as newline-separated lines.
  const auto appendValues = [&str](const QMultiMap<QString, QString>& dict) {
    for (const QString& value : dict.values()) {
      if (value == "-") {
        continue;
      }
      if (!str.isEmpty()) {
        str += "\n";
      }
      str += value;
    }
  };

  QMultiMap<QString, QString> dict;
  getInfoPoints(points, px, dict);
  getInfoPoints(pois, px, dict);
  getInfoPolylines(px, dict);
  appendValues(dict);

  // Fall back to polygon (area) info only when nothing closer was found.
  if (str.isEmpty()) {
    dict.clear();
    getInfoPolygons(px, dict);
    appendValues(dict);
  }

  if (!infotext.isEmpty() && !str.isEmpty()) {
    infotext += "\n" + str;
  } else {
    infotext += str;
  }
}

void CMapIMG::findPOICloseBy(const QPoint& pt, IPoiItem& poi) const /*override;*/
{
  for (auto& list : {points, pois}) {
    for (const CGarminPoint& point : list) {
      QPoint x = pt - QPoint(point.pos.x(), point.pos.y());
      if (x.manhattanLength() < 10) {
        QPointF radPos = point.pos;
        map->convertPx2Rad(radPos);
        poi.pos = radPos;
        if (!point.labels.isEmpty()) {
          poi.desc = point.getLabelText();
        }
        poi.name = point.getName();
        if (poi.name.isEmpty()) {
          if (pointProperties.contains(point.type)) {
            poi.name = pointProperties[point.type].strings[selectedLanguage != NOIDX ? selectedLanguage : 0];
          } else {
            poi.name = QString(" (%1)").arg(point.type, 2, 16, QChar('0'));
          }
        }
        return;
      }
    }
  }
}

void CMapIMG::getInfoPoints(const pointtype_t& points, const QPoint& pt, QMultiMap<QString, QString>& dict) const {
  for (const CGarminPoint& point : points) {
    QPoint x = pt - QPoint(point.pos.x(), point.pos.y());
    if (x.manhattanLength() < 10) {
      if (point.hasLabel()) {
        dict.insert(tr("Point of Interest"), point.getLabelText());
      } else {
        if (pointProperties.contains(point.type)) {
          dict.insert(tr("Point of Interest"),
                      pointProperties[point.type].strings[selectedLanguage != NOIDX ? selectedLanguage : 0]);
        } else {
          dict.insert(tr("Point of Interest"), QString(" (%1)").arg(point.type, 2, 16, QChar('0')));
        }
      }
    }
  }
}

void CMapIMG::getInfoPolylines(const QPoint& pt, QMultiMap<QString, QString>& dict) const {
  PJ_UV p1, p2;         // the two points of the polyline close to pt
  qreal u;              // ratio u the tangent point will divide d_p1_p2
  qreal shortest = 20;  // shortest distance so far

  QPointF resPt = pt;
  QString key;
  QStringList value;
  quint32 type = 0;

  bool found = false;

  for (const CGarminPolygon& line : polylines) {
    int len = line.pixel.size();
    // need at least 2 points
    if (len < 2) {
      continue;
    }

    // see http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
    for (int i = 1; i < len; ++i) {
      p1.u = line.pixel[i - 1].x();
      p1.v = line.pixel[i - 1].y();
      p2.u = line.pixel[i].x();
      p2.v = line.pixel[i].y();

      qreal dx = p2.u - p1.u;
      qreal dy = p2.v - p1.v;

      // distance between p1 and p2
      qreal d_p1_p2 = qSqrt(dx * dx + dy * dy);

      u = ((pt.x() - p1.u) * dx + (pt.y() - p1.v) * dy) / (d_p1_p2 * d_p1_p2);

      if (u < 0.0 || u > 1.0) {
        continue;
      }

      // coord. (x,y) of the point on line defined by [p1,p2] close to pt
      qreal x = p1.u + u * dx;
      qreal y = p1.v + u * dy;

      qreal distance = qSqrt((x - pt.x()) * (x - pt.x()) + (y - pt.y()) * (y - pt.y()));

      if (distance < shortest) {
        type = line.type;
        value.clear();
        value << (line.hasLabel() ? line.getInfoText() : "-");

        resPt.setX(x);
        resPt.setY(y);
        shortest = distance;
        found = true;
      } else if (distance == shortest) {
        if (line.hasLabel()) {
          value << line.getInfoText();
        }
      }
    }
  }

  value.removeDuplicates();

  if (!found) {
    return;
  }

  if (selectedLanguage != NOIDX) {
    key = polylineProperties[type].strings[selectedLanguage];
  }

  if (!key.isEmpty()) {
    dict.insert(key + QString("(%1)").arg(type, 2, 16, QChar('0')), value.join("\n"));
  } else {
    if (polylineProperties[type].strings.isEmpty()) {
      dict.insert(tr("Unknown") + QString("(%1)").arg(type, 2, 16, QChar('0')), value.join("\n"));
    } else {
      dict.insert(polylineProperties[type].strings[0] + QString("(%1)").arg(type, 2, 16, QChar('0')), value.join("\n"));
    }
  }

  //    pt = resPt.toPoint();
}

void CMapIMG::getInfoPolygons(const QPoint& pt, QMultiMap<QString, QString>& dict) const {
  PJ_UV p1, p2;  // the two points of the polyline close to pt
  const qreal x = pt.x();
  const qreal y = pt.y();

  for (const CGarminPolygon& line : polygons) {
    int npol = line.pixel.size();
    if (npol > 2) {
      bool c = false;
      // see http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
      for (int i = 0, j = npol - 1; i < npol; j = i++) {
        p1.u = line.pixel[j].x();
        p1.v = line.pixel[j].y();
        p2.u = line.pixel[i].x();
        p2.v = line.pixel[i].y();

        if ((((p2.v <= y) && (y < p1.v)) || ((p1.v <= y) && (y < p2.v))) &&
            (x < (p1.u - p2.u) * (y - p2.v) / (p1.v - p2.v) + p2.u)) {
          c = !c;
        }
      }

      if (c) {
        if (line.labels.size()) {
          dict.insert(tr("Area"), IGarminStrTbl::displayText(line.labels).simplified());
        } else {
          if (selectedLanguage != NOIDX) {
            if (polygonProperties[line.type].strings[selectedLanguage].size()) {
              dict.insert(tr("Area"), polygonProperties[line.type].strings[selectedLanguage]);
            }
          } else {
            if (polygonProperties[line.type].strings[0].size()) {
              dict.insert(tr("Area"), polygonProperties[line.type].strings[0]);
            }
          }
        }
      }
    }
  }
}

bool CMapIMG::findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold,
                                  QPolygonF& polyline) /* override */
{
  for (const CGarminPolygon& line : std::as_const(polylines)) {
    if (line.pixel.size() < 2) {
      continue;
    }
    if (0x20 <= line.type && line.type <= 0x25) {
      continue;
    }

    qreal dist1 = GPS_Math_DistPointPolyline(line.pixel, pt1, threshold);
    qreal dist2 = GPS_Math_DistPointPolyline(line.pixel, pt2, threshold);

    if (dist1 < threshold && dist2 < threshold) {
      polyline = line.coords;
      threshold = qMin(dist1, dist2);
    }
  }

  return !polyline.isEmpty();
}
