// Throwaway: at devicePixelRatio 2, how close is each source to ground truth?
#include <QGuiApplication>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QTextStream>

// mean per-pixel premultiplied difference
static double diff(const QImage& a0, const QImage& b0) {
  QImage a = a0.convertToFormat(QImage::Format_ARGB32);
  QImage b = b0.convertToFormat(QImage::Format_ARGB32);
  double sum = 0;
  for (int y = 0; y < a.height(); ++y) {
    for (int x = 0; x < a.width(); ++x) {
      const QRgb p = a.pixel(x, y), q = b.pixel(x, y);
      const double fa = qAlpha(p) / 255.0, fb = qAlpha(q) / 255.0;
      sum += (qAbs(qRed(p) * fa - qRed(q) * fb) + qAbs(qGreen(p) * fa - qGreen(q) * fb) +
              qAbs(qBlue(p) * fa - qBlue(q) * fb) + qAbs(qAlpha(p) - qAlpha(q))) / 4.0;
    }
  }
  return sum / (a.width() * a.height());
}

// Get the real device pixels, upscaling as the painter would if the source is short.
static QImage toImg(const QPixmap& pm, int sz) {
  QImage img = pm.toImage();
  img.setDevicePixelRatio(1);
  if (img.width() != sz) {
    img = img.scaled(sz, sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }
  return img;
}

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  QTextStream out(stdout);
  const QString png = argv[1], svg = argv[2], truth = argv[3];

  QImage ref(truth);  // inkscape-rendered 64px = ground truth
  out << "ground truth: " << truth << " (" << ref.width() << "x" << ref.height() << ")\n\n";

  // logical 32px at devicePixelRatio 2 -> needs 64 real pixels
  const QPixmap pSvg = QIcon(svg).pixmap(QSize(32, 32), 2.0);
  const QPixmap pPng = QIcon(png).pixmap(QSize(32, 32), 2.0);

  out << "requesting logical 32px @ dpr=2 (needs 64 device px):\n";
  out << QString("  SVG source -> pixmap %1x%2\n").arg(pSvg.width()).arg(pSvg.height());
  out << QString("  32px PNG   -> pixmap %1x%2\n\n").arg(pPng.width()).arg(pPng.height());

  out << "deviation from ground truth (0 = perfect):\n";
  out << QString("  SVG      %1\n").arg(diff(toImg(pSvg, 64), ref), 0, 'f', 2);
  out << QString("  32px PNG %1\n").arg(diff(toImg(pPng, 64), ref), 0, 'f', 2);
  return 0;
}
