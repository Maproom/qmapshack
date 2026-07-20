/*
 * Reference implementation: a STATIC QIconEnginePlugin that recolours svg at
 * paint time via currentColor. Proves phase 7a's mechanism end to end.
 *
 * Two things make it work, and both are load-bearing:
 *
 *  - STATIC (qt_add_plugin STATIC + Q_IMPORT_PLUGIN). Compiled into the binary,
 *    so there is nothing to deploy and a from-source build cannot get it wrong.
 *  - Key "svgt", NOT "svg". Qt's own qsvgicon plugin claims "svg" and beats a
 *    static plugin to it (dynamic plugins are indexed first and first match
 *    wins). With "svg" this engine is never consulted at all.
 *
 * The theme colour here is read from a file so the test can change it between
 * renders. The real thing takes it from QPalette.
 */
#include <QColor>
#include <QFile>
#include <QIconEngine>
#include <QIconEnginePlugin>
#include <QPainter>
#include <QPixmap>
#include <QRegularExpression>
#include <QSvgRenderer>

static QColor liveColor() {
  QFile f("/tmp/themecolor");
  if (f.open(QIODevice::ReadOnly)) {
    return QColor(QString::fromUtf8(f.readAll()).trimmed());
  }
  return QColor("#0000ff");
}

class RecolorEngine : public QIconEngine {
 public:
  void addFile(const QString& f, const QSize&, QIcon::Mode, QIcon::State) override { file = f; }

  void paint(QPainter* p, const QRect& r, QIcon::Mode, QIcon::State) override {
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
      return;
    }
    QSvgRenderer renderer(recolored(f.readAll(), liveColor()));
    renderer.render(p, r);
  }

  /*
   * Sets the root <svg> color= to `ink`. Every fill="currentColor" resolves to
   * it; every other fill is untouched, which is how semantic red/green survive.
   *
   * REPLACES the attribute -- never inserts a second one. The sources carry a
   * default color="#000080" (so inkscape/mkicon still export navy PNGs and an
   * unthemed render still looks like today), and a duplicate color= makes the
   * document INVALID: QSvgRenderer then draws nothing at all. Measured, not
   * assumed -- see ccheck.cpp.
   */
  static QByteArray recolored(QByteArray data, const QColor& ink) {
    const int open = data.indexOf("<svg");
    if (open < 0) {
      return data;
    }
    const int close = data.indexOf('>', open);
    if (close < 0) {
      return data;
    }

    static const QRegularExpression re(R"(\scolor\s*=\s*"[^"]*")");
    const QString tag = QString::fromUtf8(data.mid(open, close - open));
    const QString attr = " color=\"" + ink.name() + "\"";

    QString fixed = tag;
    const QRegularExpressionMatch m = re.match(tag);
    if (m.hasMatch()) {
      fixed.replace(m.capturedStart(), m.capturedLength(), attr);
    } else {
      fixed += attr;
    }
    data.replace(open, close - open, fixed.toUtf8());
    return data;
  }

  QPixmap pixmap(const QSize& s, QIcon::Mode m, QIcon::State st) override {
    QPixmap pm(s);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    paint(&p, QRect(QPoint(0, 0), s), m, st);
    return pm;
  }

  QIconEngine* clone() const override { return new RecolorEngine(*this); }

 private:
  QString file;
};

class RecolorPlugin : public QIconEnginePlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QIconEngineFactoryInterface_iid FILE "recolor.json")

 public:
  QIconEngine* create(const QString& file) override {
    RecolorEngine* e = new RecolorEngine;
    if (!file.isEmpty()) {
      e->addFile(file, QSize(), QIcon::Normal, QIcon::Off);
    }
    return e;
  }
};

#include "plugin.moc"
