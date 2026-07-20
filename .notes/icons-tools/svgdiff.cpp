// Throwaway: render every icon SVG with QSvgRenderer and diff against the Inkscape PNG.
#include <QGuiApplication>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QTextStream>
#include <QStringList>
#include <QRect>

static QStringList g_msgs;

static void handler(QtMsgType, const QMessageLogContext&, const QString& m) { g_msgs << m; }

// Bounding box of non-transparent pixels.
static QRect inkBox(const QImage& img) {
  int x0 = img.width(), y0 = img.height(), x1 = -1, y1 = -1;
  for (int y = 0; y < img.height(); ++y) {
    for (int x = 0; x < img.width(); ++x) {
      if (qAlpha(img.pixel(x, y)) > 8) {
        x0 = qMin(x0, x); y0 = qMin(y0, y); x1 = qMax(x1, x); y1 = qMax(y1, y);
      }
    }
  }
  return (x1 < 0) ? QRect() : QRect(QPoint(x0, y0), QPoint(x1, y1));
}

int main(int argc, char** argv) {
  qInstallMessageHandler(handler);
  QGuiApplication app(argc, argv);

  const QString dir = argv[1];
  const int SZ = 32;
  QTextStream out(stdout);
  out << "file\tstatus\tmeanDiff\tpctBadPx\tqtInk\tpngInk\twarnings\n";

  QDir d(dir);
  const QStringList svgs = d.entryList({"*.svg"}, QDir::Files, QDir::Name);
  for (const QString& s : svgs) {
    g_msgs.clear();
    const QString pngPath = dir + "/32x32/" + QString(s).replace(".svg", ".png");
    QImage png(pngPath);

    QSvgRenderer r(dir + "/" + s);
    QString status = r.isValid() ? "ok" : "PARSE_FAIL";

    QImage qt(SZ, SZ, QImage::Format_ARGB32_Premultiplied);
    qt.fill(Qt::transparent);
    if (r.isValid()) {
      QPainter p(&qt);
      p.setRenderHint(QPainter::Antialiasing, true);
      r.render(&p, QRectF(0, 0, SZ, SZ));
    }

    const QRect qtBox = inkBox(qt);
    if (qtBox.isNull() && status == "ok") status = "BLANK";

    double meanDiff = -1, pctBad = -1;
    QRect pngBox;
    if (!png.isNull()) {
      QImage ref = png.convertToFormat(QImage::Format_ARGB32_Premultiplied)
                       .scaled(SZ, SZ, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      pngBox = inkBox(ref);
      double sum = 0; long bad = 0;
      for (int y = 0; y < SZ; ++y) {
        for (int x = 0; x < SZ; ++x) {
          const QRgb a = qt.pixel(x, y), b = ref.pixel(x, y);
          const int dr = qAbs(qRed(a) - qRed(b)), dg = qAbs(qGreen(a) - qGreen(b));
          const int db = qAbs(qBlue(a) - qBlue(b)), da = qAbs(qAlpha(a) - qAlpha(b));
          const int mx = qMax(qMax(dr, dg), qMax(db, da));
          sum += (dr + dg + db + da) / 4.0;
          if (mx > 48) ++bad;
        }
      }
      meanDiff = sum / (SZ * SZ);
      pctBad = 100.0 * bad / (SZ * SZ);
    } else {
      status = "NO_PNG";
    }

    auto bs = [](const QRect& r) {
      return r.isNull() ? QString("-") : QString("%1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());
    };
    out << s << "\t" << status << "\t" << QString::number(meanDiff, 'f', 1) << "\t"
        << QString::number(pctBad, 'f', 1) << "\t" << bs(qtBox) << "\t" << bs(pngBox) << "\t"
        << g_msgs.join(" | ").left(200) << "\n";
  }
  return 0;
}
