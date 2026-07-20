/* End-to-end: does a REAL icon lose its navy when the engine themes it dark?
 *
 * strokecheck.cpp proves the mechanism on a synthetic document. This drives the
 * actual CSvgtIconEngine::recolored() over actual src/icons/*.svg files and
 * counts navy pixels in the rendered result -- the thing the user reported
 * ("many icons still have navy blue lines or areas").
 *
 * Navy strokes were the last literal #000080 in the set: the fill pass themed
 * fills only, and a themed fill with a differently-themed stroke needs the
 * two-colour class (.paper-ink) that this checks.
 *
 * g++ -O2 -std=c++17 -fPIC -I../../src/qmapshack/svgticon darkcheck.cpp \
 *     ../../src/qmapshack/svgticon/CSvgtIconEngine.cpp -o darkcheck \
 *     $(pkg-config --cflags --libs Qt6Svg Qt6Gui Qt6Core)
 * QT_QPA_PLATFORM=offscreen ./darkcheck ../../src/icons
 */

#include "CSvgtIconEngine.h"

#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>

#include <cstdio>

// The reviewed dark ink/paper pair (CSvgtIconEngine::roleColor, fixed mode).
static const QColor kInk(0xcc, 0xcc, 0xff);
static const QColor kPaper(0x35, 0x35, 0x35);
static const QColor kLead(0xe0, 0xe0, 0xe0);
static const QColor kMark(0x66, 0xaa, 0xff);

// Anything this close to #000080 reads as "still navy" against a dark ground. Also catches
// #0000ff at distance 128 -- outside the radius, so pure blue is reported separately below.
static bool navyish(QRgb p) {
  if (qAlpha(p) < 32) {
    return false;
  }
  const int dr = qRed(p), dg = qGreen(p), db = qBlue(p) - 128;
  return dr * dr + dg * dg + db * db < 40 * 40;
}

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  const QString dir = argc > 1 ? argv[1] : "src/icons";

  int scanned = 0, dirty = 0;
  for (const QFileInfo& fi : QDir(dir).entryInfoList({"*.svg"}, QDir::Files, QDir::Name)) {
    QFile f(fi.absoluteFilePath());
    if (!f.open(QIODevice::ReadOnly)) {
      continue;
    }
    const QByteArray themed = CSvgtIconEngine::recolored(f.readAll(), kInk, kPaper, kLead, kMark);

    QSvgRenderer r(themed);
    if (!r.isValid()) {
      printf("RENDER-FAILED  %s\n", qPrintable(fi.fileName()));
      ++dirty;
      continue;
    }
    QImage img(64, 64, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    {
      QPainter p(&img);
      r.render(&p);
    }

    int navy = 0;
    for (int y = 0; y < img.height(); ++y) {
      for (int x = 0; x < img.width(); ++x) {
        navy += navyish(img.pixel(x, y));
      }
    }
    ++scanned;
    if (navy > 8) {  // a few AA pixels along an edge are not a navy shape
      printf("%5d navy px  %s\n", navy, qPrintable(fi.fileName()));
      ++dirty;
    }
  }

  printf("\n%d icons themed dark | %d still showing navy\n", scanned, dirty);
  return dirty ? 1 : 0;
}
