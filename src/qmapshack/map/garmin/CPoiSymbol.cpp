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

#include "map/garmin/CPoiSymbol.h"

#include <QHash>
#include <QImage>
#include <QMutex>
#include <QPainter>
#include <QSvgRenderer>
#include <QtMath>
#include <cstring>

namespace {
constexpr qreal kSize = 16;   // side of a symbol, logical pixels
constexpr qreal kIcon = 11;   // side of the icon on a badge
constexpr qreal kSign = 14;   // side of a geographic sign
constexpr qreal kRadius = 3;  // badge corner

/** The icon of each type: `<set>/<name>` below `:/icons/garmin-poi/`; types as mkgmap's default style uses them. */
const QHash<quint32, const char*>& icons() {
  static const QHash<quint32, const char*> table = {
      {0x2a00, "maki/restaurant"},             // restaurant
      {0x2a01, "maki/fast-food"},              // american
      {0x2a02, "maki/restaurant-noodle"},      // asian
      {0x2a03, "maki/restaurant-bbq"},         // barbecue
      {0x2a04, "maki/restaurant-noodle"},      // chinese
      {0x2a05, "temaki/sandwich"},             // deli
      {0x2a06, "maki/globe"},                  // international
      {0x2a07, "maki/fast-food"},              // fast food
      {0x2a08, "maki/restaurant"},             // italian
      {0x2a09, "maki/restaurant"},             // mexican
      {0x2a0a, "maki/restaurant-pizza"},       // pizza
      {0x2a0b, "maki/restaurant-seafood"},     // seafood
      {0x2a0c, "temaki/meat"},                 // steak
      {0x2a0d, "temaki/donut"},                // bagel donut
      {0x2a0e, "maki/cafe"},                   // cafe
      {0x2a0f, "temaki/bread"},                // french
      {0x2a10, "maki/beer"},                   // german
      {0x2a11, "maki/teahouse"},               // british
      {0x2a12, "maki/restaurant"},             // speciality
      {0x2a13, "maki/restaurant"},             // other food
      {0x2b01, "maki/lodging"},                // hotel
      {0x2b02, "temaki/bunk_beds"},            // guesthouse, hostel, alpine hut
      {0x2b03, "temaki/tents"},                // camp
      {0x2b04, "maki/lodging"},                // resort
      {0x2b05, "maki/campsite"},               // camp, caravan
      {0x2b06, "temaki/hut"},                  // basic hut
      {0x2b07, "maki/shelter"},                // wilderness hut
      {0x2c01, "maki/amusement-park"},         // theme park
      {0x2c02, "maki/museum"},                 // museum, historic
      {0x2c03, "maki/library"},                // library
      {0x2c04, "maki/attraction"},             // attraction, viewpoint, artwork
      {0x2c05, "maki/school"},                 // school
      {0x2c06, "maki/park"},                   // park
      {0x2c07, "maki/zoo"},                    // zoo
      {0x2c08, "maki/stadium"},                // stadium, pitch
      {0x2c09, "maki/music"},                  // concert hall
      {0x2c0a, "maki/alcohol-shop"},           // wine cellar
      {0x2c0b, "maki/place-of-worship"},       // place of worship
      {0x2c0c, "maki/volcano"},                // volcano
      {0x2c0d, "maki/star"},                   // other tourism
      {0x2d01, "maki/theatre"},                // theatre
      {0x2d02, "maki/bar"},                    // bar, pub
      {0x2d03, "maki/cinema"},                 // cinema
      {0x2d04, "maki/casino"},                 // casino
      {0x2d05, "maki/golf"},                   // golf
      {0x2d06, "maki/skiing"},                 // skiing
      {0x2d07, "maki/bowling-alley"},          // bowling
      {0x2d08, "temaki/ice_skating"},          // ice rink
      {0x2d09, "maki/swimming"},               // swimming
      {0x2d0a, "maki/fitness-centre"},         // sports centre
      {0x2d0b, "maki/airfield"},               // airfield
      {0x2e01, "maki/shop"},                   // department store
      {0x2e02, "maki/grocery"},                // supermarket
      {0x2e03, "maki/shop"},                   // general store
      {0x2e04, "temaki/shopping_mall"},        // mall
      {0x2e05, "maki/pharmacy"},               // pharmacy
      {0x2e06, "maki/convenience"},            // convenience
      {0x2e07, "maki/clothing-store"},         // clothes
      {0x2e08, "maki/garden-centre"},          // garden centre
      {0x2e09, "maki/hardware"},               // hardware
      {0x2e0a, "temaki/plant"},                // organic
      {0x2e0b, "temaki/electronic"},           // computer
      {0x2e0c, "maki/shop"},                   // other shop
      {0x2e0e, "maki/convenience"},            // convenience
      {0x2e0f, "maki/florist"},                // florist
      {0x2e10, "maki/gift"},                   // gift
      {0x2e11, "maki/music"},                  // music
      {0x2e12, "temaki/tennis"},               // sports
      {0x2e13, "maki/alcohol-shop"},           // wine, alcohol
      {0x2e14, "temaki/book_store"},           // books
      {0x2f01, "maki/fuel"},                   // fuel
      {0x2f02, "maki/car-rental"},             // car rental
      {0x2f03, "maki/car-repair"},             // car repair
      {0x2f04, "maki/airport"},                // airport
      {0x2f05, "maki/post"},                   // post office
      {0x2f06, "maki/bank"},                   // bank, atm
      {0x2f07, "maki/car"},                    // car dealer
      {0x2f08, "maki/rail"},                   // station
      {0x2f09, "maki/harbor"},                 // marina
      {0x2f0a, "temaki/junk_car"},             // car wrecker
      {0x2f0b, "maki/parking"},                // parking
      {0x2f0c, "maki/toilet"},                 // toilets
      {0x2f0d, "temaki/car_pool"},             // car club
      {0x2f0e, "temaki/car_wash"},             // car wash
      {0x2f10, "maki/hairdresser"},            // hairdresser
      {0x2f12, "maki/telephone"},              // telephone
      {0x2f13, "maki/bicycle"},                // bicycle
      {0x2f14, "temaki/social_facility"},      // nursing home
      {0x2f15, "temaki/post_box"},             // post box, recycling
      {0x2f16, "temaki/truck"},                // truck stop
      {0x2f17, "maki/bus"},                    // bus stop, taxi
      {0x3001, "maki/police"},                 // police
      {0x3002, "maki/hospital"},               // hospital
      {0x3003, "maki/town-hall"},              // town hall, embassy
      {0x3004, "temaki/courthouse"},           // courthouse
      {0x3005, "maki/residential-community"},  // community centre
      {0x3006, "temaki/passport_checkpoint"},  // border control
      {0x3007, "maki/prison"},                 // prison
      {0x3008, "maki/fire-station"},           // fire station
      {0x2000, "osmic/exit-14"},               // motorway exit
      {0x230f, "maki/highway-rest-area"},      // services
      {0x3200, "maki/barrier"},                // barrier
      {0x4a00, "maki/picnic-site"},            // picnic
      {0x4c00, "maki/information"},            // information
      {0x5000, "maki/drinking-water"},         // drinking water
      {0x5900, "maki/airport"},                // airport
      {0x5904, "maki/heliport"},               // heliport
      {0x6403, "maki/cemetery"},               // cemetery
      {0x640b, "temaki/military"},             // military
      {0x6411, "maki/communications-tower"},   // tower
      {0x6503, "maki/water"},                  // bay
      {0x6505, "maki/water"},                  // canal
      {0x6508, "maki/waterfall"},              // waterfall
      {0x650a, "temaki/snow"},                 // glacier
      {0x650c, "temaki/islet_tree"},           // island
      {0x650d, "maki/water"},                  // lake
      {0x650f, "maki/dam"},                    // reservoir
      {0x6511, "osmic/spring-14"},             // spring
      {0x6512, "maki/water"},                  // stream
      {0x6513, "maki/wetland"},                // wetland
      {0x6601, "temaki/adit_profile"},         // cave
      {0x6603, "maki/water"},                  // basin
      {0x6604, "maki/beach"},                  // beach
      {0x6605, "temaki/bench"},                // bench
      {0x6606, "temaki/cape_landform"},        // cape
      {0x6607, "temaki/cliff_falling_rocks"},  // cliff
      {0x6612, "maki/natural"},                // nature reserve
      {0x6614, "temaki/boulder1"},             // rock
      {0x6616, "maki/mountain"},               // peak
      {0x6618, "temaki/tree_broadleaved"},     // wood
  };
  return table;
}

/** Geographic points get a coloured sign without a badge. */
bool isSign(quint32 type) { return (type >> 8) >= 0x64; }

QRgb colourOf(quint32 type) {
  switch (type >> 8) {
    case 0x2a:  // food and drink
      return 0xd86f00;
    case 0x2b:  // lodging
      return 0x1f5fa8;
    case 0x2c:  // attractions
      return 0x8e44ad;
    case 0x2d:  // entertainment and sport
      return 0x13867a;
    case 0x2e:  // shopping
      return 0xb0337f;
    case 0x30:  // emergency and government
      return 0xc62828;
    case 0x65:  // water
      return 0x1f6fb5;
    case 0x64:  // man-made features
    case 0x66:  // land features
      return 0x6b4a2b;
    default:  // services, transport and the rest
      return 0x4a5a6a;
  }
}

/** The icon of @p type in one colour, @p side device pixels square. */
QImage tinted(const QString& path, int side, QRgb colour) {
  QImage img(side, side, QImage::Format_ARGB32_Premultiplied);
  img.fill(Qt::transparent);
  QPainter p(&img);
  p.setRenderHint(QPainter::Antialiasing);
  QSvgRenderer(path).render(&p, QRectF(0, 0, side, side));
  p.setCompositionMode(QPainter::CompositionMode_SourceIn);
  p.fillRect(img.rect(), QColor::fromRgb(colour));
  return img;
}

/** The symbol of @p type rendered for @p scale device pixels per logical pixel. */
BLImage render(quint32 type, const char* icon, qreal scale) {
  const int side = qCeil(kSize * scale);
  QImage img(side, side, QImage::Format_ARGB32_Premultiplied);
  img.fill(Qt::transparent);
  QPainter p(&img);
  p.setRenderHint(QPainter::Antialiasing);
  p.scale(scale, scale);
  const QString path = QString(":/icons/garmin-poi/%1.svg").arg(icon);
  const QPointF centre(kSize / 2, kSize / 2);
  if (isSign(type)) {
    // a light rim keeps the sign readable on any area colour
    const int px = qRound(kSign * scale);
    const QImage rim = tinted(path, px, 0xffffff);
    const QPointF topLeft = centre - QPointF(kSign / 2, kSign / 2);
    p.setOpacity(0.8);
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        p.drawImage(QRectF(topLeft + QPointF(dx, dy), QSizeF(kSign, kSign)), rim);
      }
    }
    p.setOpacity(1);
    p.drawImage(QRectF(topLeft, QSizeF(kSign, kSign)), tinted(path, px, colourOf(type)));
  } else {
    const qreal half = kSize / 2 - 0.5;
    p.setPen(QPen(Qt::white, 1));
    p.setBrush(QColor::fromRgb(colourOf(type)));
    p.drawRoundedRect(QRectF(centre.x() - half, centre.y() - half, 2 * half, 2 * half), kRadius, kRadius);
    p.drawImage(QRectF(centre - QPointF(kIcon / 2, kIcon / 2), QSizeF(kIcon, kIcon)),
                tinted(path, qRound(kIcon * scale), 0xffffff));
  }
  p.end();

  // QImage's premultiplied ARGB32 and Blend2D's PRGB32 share the pixel layout
  BLImage out(side, side, BL_FORMAT_PRGB32);
  BLImageData data;
  if (out.make_mutable(&data) != BL_SUCCESS) {
    return BLImage();
  }
  for (int y = 0; y < side; ++y) {
    std::memcpy(static_cast<uchar*>(data.pixel_data) + y * data.stride, img.constScanLine(y), side * 4);
  }
  return out;
}

/** The rendered symbol, cached per type and scale; maps are drawn in several threads. */
BLImage symbolOf(quint32 type, const char* icon, qreal scale) {
  static QMutex mutex;
  static QHash<quint64, BLImage> cache;
  const quint64 key = (quint64(type) << 16) | quint64(qRound(scale * 100));
  QMutexLocker lock(&mutex);
  auto it = cache.find(key);
  if (it == cache.end()) {
    it = cache.insert(key, render(type, icon, scale));
  }
  return it.value();
}
}  // namespace

bool CPoiSymbol::hasSymbol(quint32 type) { return icons().contains(type); }

QRectF CPoiSymbol::bounds(quint32 /*type*/) { return QRectF(-kSize / 2, -kSize / 2, kSize, kSize); }

void CPoiSymbol::draw(BLContext& ctx, const QPointF& pos, quint32 type) {
  const char* icon = icons().value(type, nullptr);
  if (icon == nullptr) {
    return;
  }
  // render for the device pixels the context maps a logical pixel to, as the canvas scales by its pixel ratio
  const BLMatrix2D m = ctx.final_transform();
  const qreal scale = qMax(1.0, qSqrt(m.m00 * m.m00 + m.m01 * m.m01));
  const BLImage img = symbolOf(type, icon, scale);
  if (img.is_empty()) {
    return;
  }
  // the image's own size in logical pixels: no resampling at a fractional ratio
  const double side = img.width() / scale;
  ctx.blit_image(BLRect(pos.x() - side / 2, pos.y() - side / 2, side, side), img);
}
