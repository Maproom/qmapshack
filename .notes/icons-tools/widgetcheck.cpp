/* Does a QSvgWidget icon follow the theme now?
 *
 * The setup dialogs draw their explanatory icons with QSvgWidget, which renders
 * through QSvgRenderer and never builds a QIcon -- so CSvgtIconEngine, an ICON
 * engine, never ran for them. They kept their authored navy on a dark theme while
 * the toolbars around them followed it. CSvgtIcon::load() closes that gap.
 *
 * Checks the two things that can regress:
 *   1. a themed load leaves no navy on a dark palette
 *   2. it still follows a palette change AFTER loading (the event filter)
 *
 * Renders the widget itself, not the file, so it exercises the real path.
 *
 * g++ -O2 -std=c++17 -fPIC -I../../src/qmapshack -I../../src/qmapshack/svgticon \
 *     widgetcheck.cpp ../../src/qmapshack/svgticon/CSvgtIcon.cpp \
 *     ../../src/qmapshack/svgticon/CSvgtIconEngine.cpp -o widgetcheck \
 *     $(pkg-config --cflags --libs Qt6SvgWidgets Qt6Svg Qt6Widgets Qt6Gui Qt6Core)
 * QT_QPA_PLATFORM=offscreen ./widgetcheck
 */

#include <QApplication>
#include <QImage>
#include <QPalette>
#include <QSvgWidget>

#include <cstdio>

#include "CSvgtIcon.h"

static bool navyish(QRgb p) {
  if (qAlpha(p) < 32) {
    return false;
  }
  const int dr = qRed(p), dg = qGreen(p), db = qBlue(p) - 128;
  return dr * dr + dg * dg + db * db < 40 * 40;
}

// Pixels distinctly different from the dark ground -- i.e. the drawing itself.
static int inkPixels(QSvgWidget& w) {
  QImage img = w.grab().toImage().convertToFormat(QImage::Format_ARGB32);
  int n = 0;
  for (int y = 0; y < img.height(); ++y) {
    for (int x = 0; x < img.width(); ++x) {
      const QRgb p = img.pixel(x, y);
      n += qAlpha(p) >= 32 && (qRed(p) > 0x60 || qGreen(p) > 0x60 || qBlue(p) > 0x60);
    }
  }
  return n;
}

static int navyPixels(QSvgWidget& w) {
  QImage img = w.grab().toImage().convertToFormat(QImage::Format_ARGB32);
  int n = 0;
  for (int y = 0; y < img.height(); ++y) {
    for (int x = 0; x < img.width(); ++x) {
      n += navyish(img.pixel(x, y));
    }
  }
  return n;
}

static QPalette darkPalette() {
  QPalette p;
  p.setColor(QPalette::Window, QColor(0x35, 0x35, 0x35));
  p.setColor(QPalette::WindowText, QColor(0xcc, 0xcc, 0xff));
  return p;
}

static QPalette lightPalette() {
  QPalette p;
  p.setColor(QPalette::Window, QColor(0xef, 0xef, 0xef));
  p.setColor(QPalette::WindowText, QColor(0x00, 0x00, 0x80));
  return p;
}

int main(int argc, char** argv) {
  // The resource lives in the built binary; point at the source tree instead.
  const QString icon = argc > 1 ? argv[1] : "../../src/icons/SelectRange.svg";

  QApplication::setPalette(darkPalette());
  QApplication app(argc, argv);
  QApplication::setPalette(darkPalette());

  int fails = 0;

  QSvgWidget w;
  w.resize(64, 64);
  CSvgtIcon::load(&w, icon);
  // A wrong path silently leaves the widget blank, and a blank widget has no navy
  // -- which would pass check 1 for the wrong reason. It did, during development.
  const int drawn = inkPixels(w);
  printf("%-52s %5d ink px   %s\n", "0. the icon actually rendered", drawn, drawn > 8 ? "OK" : "FAIL");
  fails += drawn <= 8;

  const int onDark = navyPixels(w);
  printf("%-52s %5d navy px  %s\n", "1. themed load on a dark palette", onDark, onDark <= 8 ? "OK" : "FAIL");
  fails += onDark > 8;

  // 2. A live theme switch must re-theme it. Swapping the application palette
  //    delivers ApplicationPaletteChange, which the filter listens for.
  QApplication::setPalette(lightPalette());
  QApplication::processEvents();
  const int onLight = navyPixels(w);
  printf("%-52s %5d navy px  %s\n", "2. follows a switch back to light", onLight, onLight > 8 ? "OK" : "FAIL");
  fails += onLight <= 8;

  printf("\n%s\n", fails ? "FAIL" : "QSvgWidget icons follow the theme");
  return fails ? 1 : 0;
}
