// Throwaway: cost of constructing/decoding an icon per paint, PNG vs SVG.
#include <QGuiApplication>
#include <QElapsedTimer>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include <QSvgRenderer>
#include <QTextStream>

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  QTextStream out(stdout);

  const QString png = argv[1];
  const QString svg = argv[2];
  const int N = 2000;
  const int SZ = 32;

  QImage canvas(64, 64, QImage::Format_ARGB32_Premultiplied);
  QElapsedTimer t;

  // 1. fresh QIcon(path) per iteration -> what the delegates do today
  t.start();
  for (int i = 0; i < N; ++i) {
    QIcon ic(png);
    QPainter p(&canvas);
    ic.paint(&p, QRect(0, 0, SZ, SZ));
  }
  const double freshPng = t.nsecsElapsed() / 1e6;

  // 2. static QIcon, painted per iteration -> what a trivial fix does
  {
    static QIcon cached(png);
    t.restart();
    for (int i = 0; i < N; ++i) {
      QPainter p(&canvas);
      cached.paint(&p, QRect(0, 0, SZ, SZ));
    }
  }
  const double cachedPng = t.nsecsElapsed() / 1e6;

  // 3. fresh QIcon(svg) per iteration -> naive SVG swap in a delegate
  t.restart();
  for (int i = 0; i < N; ++i) {
    QIcon ic(svg);
    QPainter p(&canvas);
    ic.paint(&p, QRect(0, 0, SZ, SZ));
  }
  const double freshSvg = t.nsecsElapsed() / 1e6;

  // 4. static QIcon(svg)
  {
    static QIcon cachedS(svg);
    t.restart();
    for (int i = 0; i < N; ++i) {
      QPainter p(&canvas);
      cachedS.paint(&p, QRect(0, 0, SZ, SZ));
    }
  }
  const double cachedSvg = t.nsecsElapsed() / 1e6;

  // 5. raw QSvgRenderer reparse + render each time -> worst case
  t.restart();
  for (int i = 0; i < N; ++i) {
    QSvgRenderer r(svg);
    QPainter p(&canvas);
    r.render(&p, QRectF(0, 0, SZ, SZ));
  }
  const double rawSvg = t.nsecsElapsed() / 1e6;

  // 6. pre-rasterized QPixmap blit -> the theoretical floor
  const QPixmap pm = QIcon(png).pixmap(SZ, SZ);
  t.restart();
  for (int i = 0; i < N; ++i) {
    QPainter p(&canvas);
    p.drawPixmap(0, 0, pm);
  }
  const double blit = t.nsecsElapsed() / 1e6;

  auto row = [&](const char* label, double ms) {
    out << QString("  %1 %2 ms total  %3 us/paint\n")
               .arg(QString(label).leftJustified(34))
               .arg(QString::number(ms, 'f', 1).rightJustified(7))
               .arg(QString::number(ms * 1000.0 / N, 'f', 2).rightJustified(8));
  };
  out << "N = " << N << " paints at " << SZ << "px\n";
  row("fresh QIcon(png) per paint", freshPng);
  row("static QIcon(png), paint only", cachedPng);
  row("fresh QIcon(svg) per paint", freshSvg);
  row("static QIcon(svg), paint only", cachedSvg);
  row("raw QSvgRenderer reparse+render", rawSvg);
  row("pre-rasterized pixmap blit", blit);
  return 0;
}
