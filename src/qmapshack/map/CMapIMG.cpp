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
#include "map/garmin/CGarminStrTbl6.h"
#include "map/garmin/CGarminStrTbl8.h"
#include "map/garmin/CGarminStrTblUtf8.h"
#include "map/garmin/CGarminTyp.h"
#include "misc.h"
#include "poi/IPoiItem.h"
#include "units/IUnit.h"

#undef DEBUG_SHOW_SECT_DESC
#undef DEBUG_SHOW_TRE_DATA
#undef DEBUG_SHOW_SUBDIV_DATA
#undef DEBUG_SHOW_MAPLEVELS
#undef DEBUG_SHOW_SECTION_BORDERS
#undef DEBUG_SHOW_SUBDIV_BORDERS

/// Maximum map scale [m/px] at which street / way names are still drawn.
constexpr qreal kStreetNameThreshold = 5.0;

int CFileExt::cnt = 0;

static inline bool isCompletelyOutside(const QPolygonF& poly, const QRectF& viewport) {
  qreal north = -90.0 * DEG_TO_RAD;
  qreal south = 90.0 * DEG_TO_RAD;
  qreal west = 180.0 * DEG_TO_RAD;
  qreal east = -180.0 * DEG_TO_RAD;

  for (const QPointF& pt : poly) {
    if (north < pt.y()) {
      north = pt.y();
    }
    if (south > pt.y()) {
      south = pt.y();
    }
    if (west > pt.x()) {
      west = pt.x();
    }
    if (east < pt.x()) {
      east = pt.x();
    }
  }

  QRectF ref(west, north, east - west, south - north);

  if (ref.width() == 0) {
    ref.setWidth(0.00001);
  }
  if (ref.height() == 0) {
    ref.setHeight(0.00001);
  }

  return !viewport.intersects(ref);
}

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
      filename(filename),
      fm(CMainWindow::self().getMapFont()),
      selectedLanguage(NOIDX) {
  qDebug() << "------------------------------";
  qDebug() << "IMG: try to open" << filename;

  try {
    readBasics();
    processPrimaryMapData();
    setupTyp();
  } catch (const exce_t& e) {
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
    QMap<QString, subfile_desc_t>::iterator subfile = subfiles.begin();
    while (subfile != subfiles.end()) {
      if (!(*subfile).parts.contains("TYP")) {
        ++subfile;
        continue;
      }

      CFileExt file(filename);
      openFileCheckSuccess(QIODevice::ReadOnly, file);

      QByteArray array;
      readFile(file, (*subfile).parts["TYP"].offset, (*subfile).parts["TYP"].size, array);

      CGarminTyp typ;
      typ.decode(array, polygonProperties, polylineProperties, polygonDrawOrder, pointProperties);

      file.close();
      break;
    }
  }
}

void CMapIMG::readFile(CFileExt& file, quint32 offset, quint32 size, QByteArray& data) {
  if (offset + size > file.size()) {
    throw exce_t(eErrOpen, tr("Failed to read: ") + filename);
  }

  data = QByteArray::fromRawData(file.data(offset, size), size);
  // wenn mask == 0 ist kein xor noetig
  if (mask == 0) {
    return;
  }

#ifdef HOST_IS_64_BIT
  quint64* p64 = reinterpret_cast<quint64*>(data.data());
  for (quint32 i = 0; i < size / 8; ++i) {
    *p64++ ^= mask64;
  }
  quint32 rest = size % 8;
  quint8* p = reinterpret_cast<quint8*>(p64);
#else
  quint32* p32 = reinterpret_cast<quint32*>(data.data());
  for (quint32 i = 0; i < size / 4; ++i) {
    *p32++ ^= mask32;
  }
  quint32 rest = size % 4;
  quint8* p = reinterpret_cast<quint8*>(p32);
#endif

  for (quint32 i = 0; i < rest; ++i) {
    *p++ ^= mask;
  }
}

void CMapIMG::readBasics() {
  char tmpstr[64];
  qint64 fsize = QFileInfo(filename).size();

  CFileExt file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw exce_t(eErrOpen, tr("Failed to open: ") + filename);
  }

  mask = static_cast<quint8>(*file.data(0, 1));

  mask32 = mask;
  mask32 <<= 8;
  mask32 |= mask;
  mask32 <<= 8;
  mask32 |= mask;
  mask32 <<= 8;
  mask32 |= mask;

  mask64 = mask32;
  mask64 <<= 32;
  mask64 |= mask32;

  // read hdr_img_t
  QByteArray imghdr;
  readFile(file, 0, sizeof(hdr_img_t), imghdr);
  hdr_img_t* pImgHdr = reinterpret_cast<hdr_img_t*>(imghdr.data());

  if (strncmp(pImgHdr->signature, "DSKIMG", 7) != 0) {
    throw exce_t(errFormat, tr("Bad file format: ") + filename);
  }
  if (strncmp(pImgHdr->identifier, "GARMIN", 7) != 0) {
    throw exce_t(errFormat, tr("Bad file format: ") + filename);
  }

  mapdesc = QByteArray((const char*)pImgHdr->desc1, 20);
  mapdesc += pImgHdr->desc2;
  qDebug() << mapdesc;

  size_t blocksize = pImgHdr->blocksize();

  // 1st read FAT
  QByteArray FATblock;
  readFile(file, sizeof(hdr_img_t), sizeof(FATblock_t), FATblock);
  const FATblock_t* pFATBlock = reinterpret_cast<const FATblock_t*>(FATblock.data());

  size_t dataoffset = sizeof(hdr_img_t);

  // skip dummy blocks at the beginning
  while (dataoffset < static_cast<size_t>(fsize)) {
    if (pFATBlock->flag != 0x00) {
      break;
    }
    dataoffset += sizeof(FATblock_t);
    readFile(file, quint32(dataoffset), quint32(sizeof(FATblock_t)), FATblock);
    pFATBlock = reinterpret_cast<const FATblock_t*>(FATblock.data());
  }

  // start of new subfile part
  /*
      It is taken for granted that the single subfile parts are not
      fragmented within the file. Thus it is not really necessary to
      store and handle all block sequence numbers. Just the first one
      will give us the offset. This also implies that it is not necessary
      to care about FAT blocks with a non-zero part number.

      2007-03-31: Garmin's world base map seems to be coded different.
                  The part field seems to be rather a bit field than
                  a part number. As the total subfile size is given
                  for the first part only (for all others it's zero)
                  I use it to identify the 1st part of a subfile

      2007-05-26: Gmapsupp images by Sendmap code quite some bull shit,
                  too. The size is stored in every part and they do have
                  a part number. I introduced a set of subfile names
                  storing the subfile's name and type. The first part
                  with a size info and it's name / type not stored in the
                  set is used to get the location information.
   */
  QSet<QString> subfileNames;
  while (dataoffset < static_cast<size_t>(fsize)) {
    if (pFATBlock->flag != 0x01) {
      break;
    }

    memcpy(tmpstr, pFATBlock->name, sizeof(pFATBlock->name) + sizeof(pFATBlock->type));
    tmpstr[sizeof(pFATBlock->name) + sizeof(pFATBlock->type)] = 0;

    if (gar_load(quint32, pFATBlock->size) != 0 && !subfileNames.contains(tmpstr) && tmpstr[0] != 0x20) {
      subfileNames << tmpstr;

      memcpy(tmpstr, pFATBlock->name, sizeof(pFATBlock->name));
      tmpstr[sizeof(pFATBlock->name)] = 0;

      // skip MAPSORC.MPS section
      if (strcmp(tmpstr, "MAPSOURC") && strcmp(tmpstr, "SENDMAP2")) {
        subfile_desc_t& subfile = subfiles[tmpstr];
        subfile.name = tmpstr;

        memcpy(tmpstr, pFATBlock->type, sizeof(pFATBlock->type));
        tmpstr[sizeof(pFATBlock->type)] = 0;

        subfile_part_t& part = subfile.parts[tmpstr];
        part.size = gar_load(quint32, pFATBlock->size);
        part.offset = quint32(gar_load(uint16_t, pFATBlock->blocks[0]) * blocksize);
      }
    }

    dataoffset += sizeof(FATblock_t);
    readFile(file, quint32(dataoffset), quint32(sizeof(FATblock_t)), FATblock);
    pFATBlock = reinterpret_cast<const FATblock_t*>(FATblock.data());
  }

  if ((dataoffset == sizeof(hdr_img_t)) || (dataoffset >= static_cast<size_t>(fsize))) {
    throw exce_t(errFormat, tr("Failed to read file structure: ") + filename);
  }

  // gmapsupp.img files do not have a data offset field
  if (gar_load(quint32, pImgHdr->dataoffset) == 0) {
    pImgHdr->dataoffset = gar_load(quint32, dataoffset);
  }

  // sometimes there are dummy blocks at the end of the FAT
  //    if(gar_load(quint32, pImgHdr->dataoffset) != dataoffset)
  //    {
  //        dataoffset = gar_load(quint32, pImgHdr->dataoffset);
  //    }

#ifdef DEBUG_SHOW_SECT_DESC
  {
    QMap<QString, subfile_desc_t>::const_iterator subfile = subfiles.begin();
    while (subfile != subfiles.end()) {
      qDebug() << "--- subfile" << subfile->name << "---";
      QMap<QString, subfile_part_t>::const_iterator part = subfile->parts.begin();
      while (part != subfile->parts.end()) {
        qDebug() << part.key() << Qt::hex << part->offset << part->size;
        ++part;
      }
      ++subfile;
    }
  }
#endif  // DEBUG_SHOW_SECT_DESC

  maparea = QRectF();
  QMap<QString, subfile_desc_t>::iterator subfile = subfiles.begin();
  while (subfile != subfiles.end()) {
    if ((*subfile).parts.contains("GMP")) {
      throw exce_t(errFormat,
                   tr("File is NT format. QMapShack is unable to read map files with NT format: ") + filename);
    }

    readSubfileBasics(*subfile, file);

    ++subfile;
  }

  // combine copyright sections
  copyright.clear();
  for (const QString& str : std::as_const(copyrights)) {
    if (!copyright.isEmpty()) {
      copyright += "\n";
    }
    copyright += str;
  }

  qDebug() << "dimensions:\t" << "N" << (maparea.bottom() * RAD_TO_DEG) << "E" << (maparea.right() * RAD_TO_DEG) << "S"
           << (maparea.top() * RAD_TO_DEG) << "W" << (maparea.left() * RAD_TO_DEG);
}

void CMapIMG::readSubfileBasics(subfile_desc_t& subfile, CFileExt& file) {
  // test for mandatory subfile parts
  if (!(subfile.parts.contains("TRE") && subfile.parts.contains("RGN"))) {
    return;
  }

  // Both parts are guaranteed to exist by the guard above; bind them once instead of
  // repeating the map lookup for every offset computed below.
  const subfile_part_t& trePart = subfile.parts["TRE"];
  const subfile_part_t& rgnPart = subfile.parts["RGN"];

  QByteArray trehdr;
  readFile(file, trePart.offset, sizeof(hdr_tre_t), trehdr);
  hdr_tre_t* pTreHdr = reinterpret_cast<hdr_tre_t*>(trehdr.data());

  subfile.isTransparent = pTreHdr->POI_flags & 0x02;
  transparent = subfile.isTransparent ? true : transparent;

#ifdef DEBUG_SHOW_TRE_DATA
  qDebug() << "+++" << subfile.name << "+++";
  qDebug() << "TRE header length  :" << gar_load(uint16_t, pTreHdr->length);
  qDebug() << "TRE1 offset        :" << Qt::hex << gar_load(quint32, pTreHdr->tre1_offset);
  qDebug() << "TRE1 size          :" << dec << gar_load(quint32, pTreHdr->tre1_size);
  qDebug() << "TRE2 offset        :" << Qt::hex << gar_load(quint32, pTreHdr->tre2_offset);
  qDebug() << "TRE2 size          :" << dec << gar_load(quint32, pTreHdr->tre2_size);
#endif  // DEBUG_SHOW_TRE_DATA

  copyrights << QString(file.data(trePart.offset + gar_load(uint16_t, pTreHdr->length), 0x7FFF));

  // read map boundaries from header
  qint32 i32;
  i32 = gar_ptr_load(int24_t, &pTreHdr->northbound);
  subfile.north = GARMIN_RAD(i32);
  i32 = gar_ptr_load(int24_t, &pTreHdr->eastbound);
  subfile.east = GARMIN_RAD(i32);
  i32 = gar_ptr_load(int24_t, &pTreHdr->southbound);
  subfile.south = GARMIN_RAD(i32);
  i32 = gar_ptr_load(int24_t, &pTreHdr->westbound);
  subfile.west = GARMIN_RAD(i32);

  if (subfile.east == subfile.west) {
    subfile.east = -subfile.east;
  }

  if (subfile.west > 0 && subfile.east < 0) {
    subfile.east = -subfile.east;
  }

  subfile.area = QRectF(QPointF(subfile.west, subfile.north), QPointF(subfile.east, subfile.south));

  if (maparea.isNull()) {
    maparea = subfile.area;
  } else {
    maparea = maparea.united(subfile.area);
  }

#ifdef DEBUG_SHOW_TRE_DATA
  qDebug() << "bounding area (\260)" << (subfile.north * RAD_TO_DEG) << (subfile.east * RAD_TO_DEG)
           << (subfile.south * RAD_TO_DEG) << (subfile.west * RAD_TO_DEG);
  qDebug() << "bounding area (rad)" << subfile.area;
#endif  // DEBUG_SHOW_TRE_DATA

  QByteArray maplevel;
  readFile(file, trePart.offset + gar_load(quint32, pTreHdr->tre1_offset), gar_load(quint32, pTreHdr->tre1_size),
           maplevel);
  const tre_map_level_t* pMapLevel = reinterpret_cast<const tre_map_level_t*>(maplevel.data());

  if (pTreHdr->flag & 0x80) {
    throw exce_t(errLock, tr("File contains locked / encrypted data. Garmin does not "
                             "want you to use this file with any other software than "
                             "the one supplied by Garmin."));
  }

  quint32 nlevels = gar_load(quint32, pTreHdr->tre1_size) / sizeof(tre_map_level_t);
  quint32 nsubdivs = 0;
  quint32 nsubdivs_last = 0;

  // count subsections
  for (quint32 i = 0; i < nlevels; ++i) {
    maplevel_t ml;
    ml.inherited = TRE_MAP_INHER(pMapLevel);
    ml.level = TRE_MAP_LEVEL(pMapLevel);
    ml.bits = pMapLevel->bits;
    subfile.maplevels << ml;
    nsubdivs += gar_load(uint16_t, pMapLevel->nsubdiv);
    nsubdivs_last = gar_load(uint16_t, pMapLevel->nsubdiv);
#ifdef DEBUG_SHOW_MAPLEVEL_DATA
    qDebug() << "level" << TRE_MAP_LEVEL(pMapLevel) << "inherited" << TRE_MAP_INHER(pMapLevel) << "bits"
             << pMapLevel->bits << "#subdivs" << gar_load(uint16_t, pMapLevel->nsubdiv);
#endif  // DEBUG_SHOW_MAPLEVEL_DATA
    ++pMapLevel;
  }

  quint32 nsubdivs_next = nsubdivs - nsubdivs_last;

  //////////////////////////////////
  // read subdivision information
  //////////////////////////////////
  // point to first map level definition
  pMapLevel = (const tre_map_level_t*)maplevel.data();
  // number of subdivisions per map level
  quint32 nsubdiv = gar_load(uint16_t, pMapLevel->nsubdiv);

  // point to first 16 byte subdivision definition entry
  QByteArray subdiv_n;
  readFile(file, trePart.offset + gar_load(quint32, pTreHdr->tre2_offset), gar_load(quint32, pTreHdr->tre2_size),
           subdiv_n);
  tre_subdiv_next_t* pSubDivN = reinterpret_cast<tre_subdiv_next_t*>(subdiv_n.data());

  QVector<subdiv_desc_t> subdivs;
  subdivs.resize(nsubdivs);
  QVector<subdiv_desc_t>::iterator subdiv = subdivs.begin();
  QVector<subdiv_desc_t>::iterator subdiv_prev = subdivs.end();

  // absolute offset of RGN data
  QByteArray rgnhdr;
  readFile(file, rgnPart.offset, sizeof(hdr_rgn_t), rgnhdr);
  hdr_rgn_t* pRgnHdr = reinterpret_cast<hdr_rgn_t*>(rgnhdr.data());
  quint32 rgnoff = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset);

  quint32 rgnOffPolyg2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset_polyg2);
  quint32 rgnOffPolyl2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset_polyl2);
  quint32 rgnOffPoint2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset_point2);

  quint32 rgnLenPolyg2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->length_polyg2);
  quint32 rgnLenPolyl2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->length_polyl2);
  quint32 rgnLenPoint2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->length_point2);

  //     qDebug() << "***" << Qt::hex << subfile.parts["RGN"].offset << (subfile.parts["RGN"].offset +
  //     subfile.parts["RGN"].size); qDebug() << "+++" << Qt::hex << rgnOffPolyg2 << (rgnOffPolyg2 + rgnLenPolyg2);
  //     qDebug() << "+++" << Qt::hex << rgnOffPolyl2 << (rgnOffPolyl2 + rgnLenPolyl2);
  //     qDebug() << "+++" << Qt::hex << rgnOffPoint2 << (rgnOffPoint2 + rgnLenPoint2);

  // parse all 16 byte subdivision entries
  quint32 i;
  for (i = 0; i < nsubdivs_next; ++i, --nsubdiv) {
    subdiv->n = i;
    subdiv->next = gar_load(uint16_t, pSubDivN->next);
    subdiv->terminate = TRE_SUBDIV_TERM(pSubDivN);
    subdiv->rgn_start = gar_ptr_load(uint24_t, &pSubDivN->rgn_offset);
    subdiv->rgn_start += rgnoff;
    // skip if this is the first entry
    if (subdiv_prev != subdivs.end()) {
      subdiv_prev->rgn_end = subdiv->rgn_start;
    }

    subdiv->hasPoints = pSubDivN->elements & 0x10;
    subdiv->hasIdxPoints = pSubDivN->elements & 0x20;
    subdiv->hasPolylines = pSubDivN->elements & 0x40;
    subdiv->hasPolygons = pSubDivN->elements & 0x80;

    // if all subdivisions of this level have been parsed, switch to the next one
    if (nsubdiv == 0) {
      ++pMapLevel;
      nsubdiv = gar_load(uint16_t, pMapLevel->nsubdiv);
    }

    subdiv->level = TRE_MAP_LEVEL(pMapLevel);
    subdiv->shift = 24 - pMapLevel->bits;

    qint32 cx = gar_ptr_load(uint24_t, &pSubDivN->center_lng);
    subdiv->iCenterLng = cx;
    qint32 cy = gar_ptr_load(uint24_t, &pSubDivN->center_lat);
    subdiv->iCenterLat = cy;
    qint32 width = TRE_SUBDIV_WIDTH(pSubDivN) << subdiv->shift;
    qint32 height = gar_load(uint16_t, pSubDivN->height) << subdiv->shift;

    subdiv->north = GARMIN_RAD(cy + height + 1);
    subdiv->south = GARMIN_RAD(cy - height);
    subdiv->east = GARMIN_RAD(cx + width + 1);
    subdiv->west = GARMIN_RAD(cx - width);

    subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north), QPointF(subdiv->east, subdiv->south));

    subdiv->offsetPoints2 = 0;
    subdiv->lengthPoints2 = 0;
    subdiv->offsetPolylines2 = 0;
    subdiv->lengthPolylines2 = 0;
    subdiv->offsetPolygons2 = 0;
    subdiv->lengthPolygons2 = 0;

    subdiv_prev = subdiv;
    ++pSubDivN;
    ++subdiv;
  }

  // switch to last map level
  ++pMapLevel;
  // witch pointer to 14 byte subdivision sections
  tre_subdiv_t* pSubDivL = pSubDivN;
  // parse all 14 byte subdivision entries of last map level
  for (; i < nsubdivs; ++i) {
    subdiv->n = i;
    subdiv->next = 0;
    subdiv->terminate = TRE_SUBDIV_TERM(pSubDivL);
    subdiv->rgn_start = gar_ptr_load(uint24_t, &pSubDivL->rgn_offset);
    subdiv->rgn_start += rgnoff;
    subdiv_prev->rgn_end = subdiv->rgn_start;
    subdiv->hasPoints = pSubDivL->elements & 0x10;
    subdiv->hasIdxPoints = pSubDivL->elements & 0x20;
    subdiv->hasPolylines = pSubDivL->elements & 0x40;
    subdiv->hasPolygons = pSubDivL->elements & 0x80;

    subdiv->level = TRE_MAP_LEVEL(pMapLevel);
    subdiv->shift = 24 - pMapLevel->bits;

    qint32 cx = gar_ptr_load(uint24_t, &pSubDivL->center_lng);
    subdiv->iCenterLng = cx;
    qint32 cy = gar_ptr_load(uint24_t, &pSubDivL->center_lat);
    subdiv->iCenterLat = cy;
    qint32 width = TRE_SUBDIV_WIDTH(pSubDivL) << subdiv->shift;
    qint32 height = gar_load(uint16_t, pSubDivL->height) << subdiv->shift;

    subdiv->north = GARMIN_RAD(cy + height + 1);
    subdiv->south = GARMIN_RAD(cy - height);
    subdiv->east = GARMIN_RAD(cx + width + 1);
    subdiv->west = GARMIN_RAD(cx - width);

    subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north), QPointF(subdiv->east, subdiv->south));

    subdiv->offsetPoints2 = 0;
    subdiv->lengthPoints2 = 0;
    subdiv->offsetPolylines2 = 0;
    subdiv->lengthPolylines2 = 0;
    subdiv->offsetPolygons2 = 0;
    subdiv->lengthPolygons2 = 0;

    subdiv_prev = subdiv;
    ++pSubDivL;
    ++subdiv;
  }
  subdivs.last().rgn_end =
      gar_load(quint32, pRgnHdr->hdr_rgn_t::offset) + gar_load(quint32, pRgnHdr->hdr_rgn_t::length);

  // read extended NT elements
  if ((gar_load(uint16_t, pTreHdr->hdr_subfile_part_t::length) >= 0x9A) && pTreHdr->tre7_size &&
      (gar_load(uint16_t, pTreHdr->tre7_rec_size) >= sizeof(tre_subdiv2_t))) {
    // rgnoff = subfile.parts["RGN"].offset;
    //          qDebug() << subdivs.count() << (pTreHdr->tre7_size / pTreHdr->tre7_rec_size) << pTreHdr->tre7_rec_size;
    QByteArray subdiv2;
    readFile(file, trePart.offset + gar_load(quint32, pTreHdr->tre7_offset), gar_load(quint32, pTreHdr->tre7_size),
             subdiv2);
    tre_subdiv2_t* pSubDiv2 = reinterpret_cast<tre_subdiv2_t*>(subdiv2.data());

    //        const quint32 entries1 = gar_load(quint32, pTreHdr->tre7_size) / gar_load(quint32,
    //        pTreHdr->tre7_rec_size); const quint32 entries2 = subdivs.size();

    bool skipPois = (gar_load(uint16_t, pTreHdr->tre7_rec_size) != sizeof(tre_subdiv2_t));

    //         for(int i = 0; i < pTreHdr->tre7_rec_size; ++i){
    //             if(i%4 == 0) fprintf(stderr,"\n");
    //             fprintf(stderr,"%02X ", ((quint8*)pSubDiv2)[i]);
    //         }
    //         fprintf(stderr,"\n");

    subdiv = subdivs.begin();
    subdiv_prev = subdivs.begin();
    subdiv->offsetPolygons2 = gar_load(quint32, pSubDiv2->offsetPolygons) + rgnOffPolyg2;
    subdiv->offsetPolylines2 = gar_load(quint32, pSubDiv2->offsetPolyline) + rgnOffPolyl2;
    subdiv->offsetPoints2 = skipPois ? 0 : gar_load(quint32, pSubDiv2->offsetPoints) + rgnOffPoint2;

    ++subdiv;
    pSubDiv2 = reinterpret_cast<tre_subdiv2_t*>(reinterpret_cast<quint8*>(pSubDiv2) +
                                                gar_endian(uint16_t, pTreHdr->tre7_rec_size));

    while (subdiv != subdivs.end()) {
      //             for(int i = 0; i < pTreHdr->tre7_rec_size; ++i){
      //                 if(i%4 == 0) fprintf(stderr,"\n");
      //                 fprintf(stderr,"%02X ", ((quint8*)pSubDiv2)[i]);
      //             }
      //             fprintf(stderr,"\n");

      subdiv->offsetPolygons2 = gar_load(quint32, pSubDiv2->offsetPolygons) + rgnOffPolyg2;
      subdiv->offsetPolylines2 = gar_load(quint32, pSubDiv2->offsetPolyline) + rgnOffPolyl2;
      subdiv->offsetPoints2 = skipPois ? 0 : gar_load(quint32, pSubDiv2->offsetPoints) + rgnOffPoint2;

      subdiv_prev->lengthPolygons2 = subdiv->offsetPolygons2 - subdiv_prev->offsetPolygons2;
      subdiv_prev->lengthPolylines2 = subdiv->offsetPolylines2 - subdiv_prev->offsetPolylines2;
      subdiv_prev->lengthPoints2 = skipPois ? 0 : subdiv->offsetPoints2 - subdiv_prev->offsetPoints2;

      subdiv_prev = subdiv;

      ++subdiv;
      pSubDiv2 = reinterpret_cast<tre_subdiv2_t*>(reinterpret_cast<quint8*>(pSubDiv2) +
                                                  gar_endian(uint16_t, pTreHdr->tre7_rec_size));
    }

    subdiv_prev->lengthPolygons2 = rgnOffPolyg2 + rgnLenPolyg2 - subdiv_prev->offsetPolygons2;
    subdiv_prev->lengthPolylines2 = rgnOffPolyl2 + rgnLenPolyl2 - subdiv_prev->offsetPolylines2;
    subdiv_prev->lengthPoints2 = skipPois ? 0 : rgnOffPoint2 + rgnLenPoint2 - subdiv_prev->offsetPoints2;
  }

  subfile.subdivs = subdivs;

#ifdef DEBUG_SHOW_SUBDIV_DATA
  {
    QVector<subdiv_desc_t>::iterator subdiv = subfile.subdivs.begin();
    while (subdiv != subfile.subdivs.end()) {
      qDebug() << "--- subdiv" << subdiv->n << "---";
      qDebug() << "RGN start          " << Qt::hex << subdiv->rgn_start;
      qDebug() << "RGN end            " << Qt::hex << subdiv->rgn_end;
      qDebug() << "center lng         " << GARMIN_DEG(subdiv->iCenterLng);
      qDebug() << "center lat         " << GARMIN_DEG(subdiv->iCenterLat);
      qDebug() << "has points         " << subdiv->hasPoints;
      qDebug() << "has indexed points " << subdiv->hasIdxPoints;
      qDebug() << "has polylines      " << subdiv->hasPolylines;
      qDebug() << "has polygons       " << subdiv->hasPolygons;
      qDebug() << "bounding area (m)  " << subdiv->area.topLeft() << subdiv->area.bottomRight();
      qDebug() << "map level          " << subdiv->level;
      qDebug() << "left shifts        " << subdiv->shift;

      qDebug() << "polyg off.         " << Qt::hex << subdiv->offsetPolygons2;
      qDebug() << "polyg len.         " << Qt::hex << subdiv->lengthPolygons2;
      qDebug() << "polyl off.         " << Qt::hex << subdiv->offsetPolylines2;
      qDebug() << "polyl len.         " << Qt::hex << subdiv->lengthPolylines2;
      qDebug() << "point off.         " << Qt::hex << subdiv->offsetPoints2;
      qDebug() << "point len.         " << Qt::hex << subdiv->lengthPoints2;
      ++subdiv;
    }
  }
#endif  // DEBUG_SHOW_SUBDIV_DATA

  //     qDebug() << "***" << Qt::hex << subfile.parts["RGN"].offset << (subfile.parts["RGN"].offset +
  //     subfile.parts["RGN"].size); qDebug() << "+++" << Qt::hex << rgnOffPolyg2 << (rgnOffPolyg2 +
  //     pRgnHdr->length_polyg2); qDebug() << "+++" << Qt::hex << rgnOffPolyl2 << (rgnOffPolyl2 +
  //     pRgnHdr->length_polyl2); qDebug() << "+++" << Qt::hex << rgnOffPoint2 << (rgnOffPoint2 +
  //     pRgnHdr->length_point2);

  if (subfile.parts.contains("LBL")) {
    const subfile_part_t& lblPart = subfile.parts["LBL"];

    QByteArray lblhdr;
    readFile(file, lblPart.offset, sizeof(hdr_lbl_t), lblhdr);
    hdr_lbl_t* pLblHdr = reinterpret_cast<hdr_lbl_t*>(lblhdr.data());

    quint32 offsetLbl1 = lblPart.offset + gar_load(quint32, pLblHdr->lbl1_offset);
    quint32 offsetLbl6 = lblPart.offset + gar_load(quint32, pLblHdr->lbl6_offset);

    QByteArray nethdr;
    quint32 offsetNet1 = 0;
    hdr_net_t* pNetHdr = nullptr;
    if (subfile.parts.contains("NET")) {
      const subfile_part_t& netPart = subfile.parts["NET"];
      readFile(file, netPart.offset, sizeof(hdr_net_t), nethdr);
      pNetHdr = reinterpret_cast<hdr_net_t*>(nethdr.data());
      offsetNet1 = netPart.offset + gar_load(quint32, pNetHdr->net1_offset);
    }

    quint16 codepage = 0;
    if (gar_load(uint16_t, pLblHdr->length) > 0xAA) {
      codepage = gar_load(uint16_t, pLblHdr->codepage);
    }

    //         qDebug() << file.fileName() << Qt::hex << offsetLbl1 << offsetLbl6 << offsetNet1;

    switch (pLblHdr->coding) {
      case 0x06:
        subfile.strtbl = new CGarminStrTbl6(codepage, mask, this);
        break;

      case 0x09:
        subfile.strtbl = new CGarminStrTbl8(codepage, mask, this);
        break;

      case 0x0A:
        subfile.strtbl = new CGarminStrTblUtf8(codepage, mask, this);
        break;

      default:
        qWarning() << "Unknown label coding" << Qt::hex << pLblHdr->coding;
    }

    if (nullptr != subfile.strtbl) {
      subfile.strtbl->registerLBL1(offsetLbl1, gar_load(quint32, pLblHdr->lbl1_length), pLblHdr->addr_shift);
      subfile.strtbl->registerLBL6(offsetLbl6, gar_load(quint32, pLblHdr->lbl6_length));
      if (nullptr != pNetHdr) {
        subfile.strtbl->registerNET1(offsetNet1, gar_load(quint32, pNetHdr->net1_length), pNetHdr->net1_addr_shift);
      }
    }
  }
}

void CMapIMG::processPrimaryMapData() {
  /*
   * Query all subfiles for possible maplevels.
   * Exclude basemap to avoid pollution.
   */
  for (const subfile_desc_t& subfile : std::as_const(subfiles)) {
    for (const maplevel_t& maplevel : subfile.maplevels) {
      if (!maplevel.inherited) {
        map_level_t ml;
        ml.bits = maplevel.bits;
        ml.level = maplevel.level;
        ml.useBaseMap = false;
        maplevels << ml;
      }
    }
  }

  /* Sort all entries, note that stable sort should insure that basemap is preferred when available. */
  std::stable_sort(maplevels.begin(), maplevels.end(), map_level_t::GreaterThan);
  /* Delete any duplicates for obvious performance reasons. */
  auto where = std::unique(maplevels.begin(), maplevels.end());
  maplevels.erase(where, maplevels.end());

#ifdef DEBUG_SHOW_MAPLEVELS
  for (int i = 0; i < maplevels.count(); ++i) {
    map_level_t& ml = maplevels[i];
    qDebug() << ml.bits << ml.level << ml.useBaseMap;
  }
#endif
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

  QVector<map_level_t>::const_iterator maplevel = maplevels.constEnd();
  do {
    --maplevel;
    if (bits >= maplevel->bits) {
      break;
    }
  } while (maplevel != maplevels.constBegin());

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
        loadVisibleData(false, polygons, polylines, points, pois, maplevel->level, viewport, ctx);
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

void CMapIMG::loadVisibleData(bool fast, polytype_t& polygons, polytype_t& polylines, pointtype_t& points,
                              pointtype_t& pois, unsigned level, const QRectF& viewport, BLContext& ctx) {
#ifndef Q_OS_WIN32
  CFileExt file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }
#endif

  for (const subfile_desc_t& subfile : std::as_const(subfiles)) {
    //        qDebug() << "-------";
    //        qDebug() << (viewport.topLeft() * RAD_TO_DEG) << (viewport.bottomRight() * RAD_TO_DEG);
    //        qDebug() << (subfile.area.topLeft() * RAD_TO_DEG) << (subfile.area.bottomRight() * RAD_TO_DEG);
    //        qDebug() << subfile.area.intersects(viewport);

    if (!subfile.area.intersects(viewport)) {
      continue;
    }

    if (map->needsRedraw()) {
      break;
    }

#ifdef Q_OS_WIN32
    CFileExt file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
      return;
    }
#endif

    QByteArray rgndata;
    readFile(file, subfile.parts["RGN"].offset, subfile.parts["RGN"].size, rgndata);

    // qDebug() << "rgn range" << Qt::hex << subfile.parts["RGN"].offset << (subfile.parts["RGN"].offset +
    // subfile.parts["RGN"].size);

    const QVector<subdiv_desc_t>& subdivs = subfile.subdivs;
    // collect polylines
    for (const subdiv_desc_t& subdiv : subdivs) {
      // if(subdiv.level == level) qDebug() << "subdiv:" << subdiv.level << level <<  subdiv.area << viewport <<
      // subdiv.area.intersects(viewport);
      if (subdiv.level != level || !subdiv.area.intersects(viewport)) {
        continue;
      }
      if (map->needsRedraw()) {
        break;
      }
      loadSubDiv(file, subdiv, subfile.strtbl, rgndata, fast, viewport, polylines, polygons, points, pois);

#ifdef DEBUG_SHOW_SECTION_BORDERS
      const QRectF& a = subdiv.area;
      qreal u[2] = {a.left(), a.right()};
      qreal v[2] = {a.top(), a.bottom()};

      QPolygonF poly;
      poly << a.bottomLeft() << a.bottomRight() << a.topRight() << a.topLeft();

      map->convertRad2Px(poly);

      ctx.set_stroke_width(2);
      ctx.set_stroke_style(BLRgba32(0xFFFF00FFu));  // magenta
      ctx.stroke_polygon(toBLPoints(poly), static_cast<size_t>(poly.size()));
#endif  // DEBUG_SHOW_SECTION_BORDERS
    }

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
#endif  // DEBUG_SHOW_SUBDIV_BORDERS

#ifdef Q_OS_WIN32
    file.close();
#else
    file.free();
#endif
  }

#ifndef Q_OS_WIN32
  file.close();
#endif
}

void CMapIMG::loadSubDiv(CFileExt& file, const subdiv_desc_t& subdiv, IGarminStrTbl* strtbl, const QByteArray& rgndata,
                         bool fast, const QRectF& viewport, polytype_t& polylines, polytype_t& polygons,
                         pointtype_t& points, pointtype_t& pois) {
  if (subdiv.rgn_start == subdiv.rgn_end && !subdiv.lengthPolygons2 && !subdiv.lengthPolylines2 &&
      !subdiv.lengthPoints2) {
    return;
  }
  // fprintf(stderr, "loadSubDiv\n");
  //      qDebug() << "---------" << file.fileName() << "---------";

  const quint8* pRawData = reinterpret_cast<const quint8*>(rgndata.constData());

  quint32 opnt = 0, oidx = 0, opline = 0, opgon = 0;
  quint32 objCnt = subdiv.hasIdxPoints + subdiv.hasPoints + subdiv.hasPolylines + subdiv.hasPolygons;

  const quint16* pOffset = reinterpret_cast<const quint16*>(pRawData + subdiv.rgn_start);

  // test for points
  if (subdiv.hasPoints) {
    opnt = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
  }
  // test for indexed points
  if (subdiv.hasIdxPoints) {
    if (opnt) {
      oidx = gar_load(uint16_t, *pOffset);
      oidx += subdiv.rgn_start;
      ++pOffset;
    } else {
      oidx = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
    }
  }
  // test for polylines
  if (subdiv.hasPolylines) {
    if (opnt || oidx) {
      opline = gar_load(uint16_t, *pOffset);
      opline += subdiv.rgn_start;
      ++pOffset;
    } else {
      opline = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
    }
  }
  // test for polygons
  if (subdiv.hasPolygons) {
    if (opnt || oidx || opline) {
      opgon = gar_load(uint16_t, *pOffset);
      opgon += subdiv.rgn_start;
      ++pOffset;
    } else {
      opgon = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
    }
  }

#ifdef DEBUG_SHOW_POLY_DATA
  qDebug() << "--- Subdivision" << subdiv.n << "---";
  qDebug() << "address:" << Qt::hex << subdiv.rgn_start << "- " << subdiv.rgn_end;
  qDebug() << "points:            " << Qt::hex << opnt;
  qDebug() << "indexed points:    " << Qt::hex << oidx;
  qDebug() << "polylines:         " << Qt::hex << opline;
  qDebug() << "polygons:          " << Qt::hex << opgon;
#endif  // DEBUG_SHOW_POLY_DATA

  CGarminPolygon p;

  // decode points
  if (subdiv.hasPoints && !fast && getShowPOIs()) {
    const quint8* pData = pRawData + opnt;
    const quint8* pEnd = pRawData + (oidx ? oidx : opline ? opline : opgon ? opgon : subdiv.rgn_end);
    while (pData < pEnd) {
      CGarminPoint p;
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, pData);

      // skip points outside our current viewport
      if (!viewport.contains(p.pos)) {
        continue;
      }

      if (strtbl) {
        p.isLbl6 ? strtbl->get(file, p.lbl_ptr, IGarminStrTbl::poi, p.labels)
                 : strtbl->get(file, p.lbl_ptr, IGarminStrTbl::norm, p.labels);
      }

      points.push_back(p);
    }
  }

  // decode indexed points
  if (subdiv.hasIdxPoints && !fast && getShowPOIs()) {
    const quint8* pData = pRawData + oidx;
    const quint8* pEnd = pRawData + (opline ? opline : opgon ? opgon : subdiv.rgn_end);
    while (pData < pEnd) {
      CGarminPoint p;
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, pData);

      // skip points outside our current viewport
      if (!viewport.contains(p.pos)) {
        continue;
      }

      if (strtbl) {
        p.isLbl6 ? strtbl->get(file, p.lbl_ptr, IGarminStrTbl::poi, p.labels)
                 : strtbl->get(file, p.lbl_ptr, IGarminStrTbl::norm, p.labels);
      }

      pois.push_back(p);
    }
  }

  // decode polylines
  if (subdiv.hasPolylines && !fast && getShowPolylines()) {
    CGarminPolygon::cnt = 0;
    const quint8* pData = pRawData + opline;
    const quint8* pEnd = pRawData + (opgon ? opgon : subdiv.rgn_end);
    while (pData < pEnd) {
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, true, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      } else if (strtbl && p.lbl_in_NET && p.lbl_info) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::net, p.labels);
      }

      polylines.push_back(p);
    }
  }

  // decode polygons
  if (subdiv.hasPolygons && getShowPolygons()) {
    CGarminPolygon::cnt = 0;
    const quint8* pData = pRawData + opgon;
    const quint8* pEnd = pRawData + subdiv.rgn_end;

    while (pData < pEnd) {
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, false, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info && !fast) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      } else if (strtbl && p.lbl_in_NET && p.lbl_info && !fast) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::net, p.labels);
      }
      polygons.push_back(p);
    }
  }

  //         qDebug() << "--- Subdivision" << subdiv.n << "---";
  //         qDebug() << "adress:" << Qt::hex << subdiv.rgn_start << "- " << subdiv.rgn_end;
  //         qDebug() << "polyg off: " << Qt::hex << subdiv.offsetPolygons2;
  //         qDebug() << "polyg len: " << Qt::hex << subdiv.lengthPolygons2 << dec << subdiv.lengthPolygons2;
  //         qDebug() << "polyg end: " << Qt::hex << subdiv.lengthPolygons2 + subdiv.offsetPolygons2;
  //         qDebug() << "polyl off: " << Qt::hex << subdiv.offsetPolylines2;
  //         qDebug() << "polyl len: " << Qt::hex << subdiv.lengthPolylines2 << dec << subdiv.lengthPolylines2;
  //         qDebug() << "polyl end: " << Qt::hex << subdiv.lengthPolylines2 + subdiv.offsetPolylines2;
  //         qDebug() << "point off: " << Qt::hex << subdiv.offsetPoints2;
  //         qDebug() << "point len: " << Qt::hex << subdiv.lengthPoints2 << dec << subdiv.lengthPoints2;
  //         qDebug() << "point end: " << Qt::hex << subdiv.lengthPoints2 + subdiv.offsetPoints2;

  if (subdiv.lengthPolygons2 && getShowPolygons()) {
    const quint8* pData = pRawData + subdiv.offsetPolygons2;
    const quint8* pEnd = pData + subdiv.lengthPolygons2;
    while (pData < pEnd) {
      //             qDebug() << "rgn offset:" << Qt::hex << (rgnoff + (pData - pRawData));
      pData += p.decode2(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, false, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info && !fast) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      }

      polygons.push_back(p);
    }
  }

  if (subdiv.lengthPolylines2 && !fast && getShowPolylines()) {
    const quint8* pData = pRawData + subdiv.offsetPolylines2;
    const quint8* pEnd = pData + subdiv.lengthPolylines2;
    while (pData < pEnd) {
      //             qDebug() << "rgn offset:" << Qt::hex << (rgnoff + (pData - pRawData));
      pData += p.decode2(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, true, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      }

      polylines.push_back(p);
    }
  }

  if (subdiv.lengthPoints2 && !fast && getShowPOIs()) {
    const quint8* pData = pRawData + subdiv.offsetPoints2;
    const quint8* pEnd = pData + subdiv.lengthPoints2;
    while (pData < pEnd) {
      CGarminPoint p;
      //             qDebug() << "rgn offset:" << Qt::hex << (rgnoff + (pData - pRawData));
      pData += p.decode2(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, pData, pEnd);

      // skip points outside our current viewport
      if (!viewport.contains(p.pos)) {
        continue;
      }

      if (strtbl) {
        p.isLbl6 ? strtbl->get(file, p.lbl_ptr, IGarminStrTbl::poi, p.labels)
                 : strtbl->get(file, p.lbl_ptr, IGarminStrTbl::norm, p.labels);
      }
      pois.push_back(p);
    }
  }
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

  QMap<quint32, CGarminTyp::polyline_property>::iterator props = polylineProperties.begin();
  QMap<quint32, CGarminTyp::polyline_property>::iterator end = polylineProperties.end();
  for (; props != end; ++props) {
    const quint32& type = props.key();
    const CGarminTyp::polyline_property& property = props.value();

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

        if (scale.x() < kStreetNameThreshold && property.labelType != CGarminTyp::eNone) {
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

          collectText(item, poly, f, h, night ? property.colorLabelNight : property.colorLabelDay);
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
      const int lineWidth = pen.width();

      for (quint32 idx : indices) {
        drawLine(ctx, lines[idx], stroke, lineWidth, property, font, scale);
      }
    }
  }

  // 2nd run to draw the foreground lines over their borders.
  for (props = polylineProperties.begin(); props != end; ++props) {
    const quint32& type = props.key();
    const CGarminTyp::polyline_property& property = props.value();

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
}

void CMapIMG::drawLine(BLContext& ctx, CGarminPolygon& l, bool stroke, int lineWidth,
                       const CGarminTyp::polyline_property& property, const QFont& font, const QPointF& scale) {
  QPolygonF& poly = l.pixel;
  const int size = poly.size();

  if (size < 2) {
    return;
  }

  map->convertRad2Px(poly);

  if (scale.x() < kStreetNameThreshold && property.labelType != CGarminTyp::eNone) {
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

    collectText(l, poly, f, lineWidth,
                CMainWindow::self().isNight() ? property.colorLabelNight : property.colorLabelDay);
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

void CMapIMG::collectText(const CGarminPolygon& item, const QPolygonF& line, const QFont& font, qint32 lineWidth,
                          const QColor& color) {
  QString str;
  if (item.hasLabel()) {
    str = item.getLabelText();
  }

  if (str.isEmpty()) {
    return;
  }

  textpath_t tp;
  tp.polyline = line;
  tp.font = font;
  tp.text = str;
  tp.lineWidth = lineWidth;
  tp.color = color;

  const int size = line.size();
  for (int i = 1; i < size; ++i) {
    const QPointF& p1 = line[i - 1];
    const QPointF& p2 = line[i];
    qreal dx = p2.x() - p1.x();
    qreal dy = p2.y() - p1.y();
    tp.lengths << qSqrt(dx * dx + dy * dy);
  }

  textpaths << tp;
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
      QRect rect = fm.boundingRect(pt->labels.join(" "));
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
      QRect rect = fm.boundingRect(pt.labels.join(" "));
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

  for (const textpath_t& textpath : std::as_const(textpaths)) {
    QPainterPath path;
    QFont font = textpath.font;
    QFontMetricsF fm(font);

    path.addPolygon(textpath.polyline);

    // get path length and string length
    qreal length = qAbs(path.length());
    qreal width = fm.size(Qt::TextSingleLine, textpath.text).width();

    // adjust font size until string fits into polyline
    while (width > (length * 0.7)) {
      font.setPointSize(font.pointSize() - 1);
      fm = QFontMetricsF(font);
      width = fm.size(Qt::TextSingleLine, textpath.text).width();

      if ((font.pointSize() < 6)) {
        break;
      }
    }

    // no way to draw a readable string - skip
    if ((font.pointSize() < 6)) {
      continue;
    }

    fm = QFontMetricsF(font);
    // Layout still uses QFontMetricsF (above); glyph outlines are rasterised by Blend2D from
    // QRawFont (cached across frames), keeping Qt's font matching at the same resolved size.
    // Glyph ids for the whole label are looked up once here rather than per character.
    FontGlyphs& fg = fontGlyphsFor(font);
    const QList<quint32> glyphIds = fg.raw.glyphIndexesForString(textpath.text);
    const BLRgba32 fill = toBLColor(textpath.color);
    const BLRgba32 halo = toBLColor(Qt::white);

    // adjust exact offset to first half of segment
    const QVector<qreal>& lengths = textpath.lengths;
    const qreal ref = (length - width) / 2;
    qreal offset = 0;

    for (int i = 0; i < lengths.size(); ++i) {
      const qreal d = lengths[i];

      if ((offset + d / 2) >= ref) {
        offset = ref;
        break;
      }
      if ((offset + d) >= ref) {
        offset += d / 2;
        break;
      }
      offset += d;
    }

    // get starting angle of first two letters
    const QString& text = textpath.text;
    qreal percent1 = offset / length;
    qreal percent2 = (offset + fm.size(Qt::TextSingleLine, text.left(2)).width()) / length;

    QPointF point1 = path.pointAtPercent(percent1);
    QPointF point2 = path.pointAtPercent(percent2);

    // flip path if string start is E->W direction
    // this helps, sometimes, in 50 % of the cases :)
    if (point2.x() - point1.x() < 0) {
      path = path.toReversed();
    }

    // draw string letter by letter and adjust angle
    const int size = text.size();
    percent2 = offset / length;
    point2 = path.pointAtPercent(percent2);

    for (int i = 0; i < size; ++i) {
      // percent1 = percent2;
      percent2 = (offset + fm.size(Qt::TextSingleLine, text[i]).width()) / length;

      point1 = point2;
      point2 = path.pointAtPercent(percent2);

      // BLContext::rotate() takes radians; atan2() also resolves the quadrant the old
      // qAtan()+180 fix-up handled by hand.
      const qreal angle = std::atan2(point2.y() - point1.y(), point2.x() - point1.x());

      ctx.save();
      ctx.translate(point1.x(), point1.y());
      ctx.rotate(angle);
      ctx.translate(0, -(textpath.lineWidth + 2));

      if (i < glyphIds.size()) {
        fillGlyphRun(ctx, glyphPathsFor(fg, glyphIds[i]), BLPoint(0, 0), fill, halo);
      }

      ctx.restore();

      offset += fm.size(Qt::TextSingleLine, text[i]).width();
    }
  }
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
          poi.name = point.labels.first();
          poi.desc = point.getLabelText();
        } else {
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
        value << (line.hasLabel() ? line.getLabelText() : "-");

        resPt.setX(x);
        resPt.setY(y);
        shortest = distance;
        found = true;
      } else if (distance == shortest) {
        if (line.hasLabel()) {
          value << line.getLabelText();
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
          dict.insert(tr("Area"), line.labels.join(" ").simplified());
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
