/*
 * Drives the static recolour engine exactly the way QMapShack would.
 *
 * The QIcon is built with a bare addFile() -- byte for byte what uic emits into
 * setupUi() for a .ui <iconset>. That is the whole point: if this works, no .ui
 * file and no widget code has to change.
 *
 * Expected output (nothing deployed, no QT_PLUGIN_PATH):
 *   ### RecolorPlugin::create FIRED for ":/icons/Foo.svgt"
 *   theme=blue  -> #0000ff
 *   theme=red   -> #ff0000
 *   theme=green -> #22aa22
 *   semantic red fill, theme=green -> #ff0000   (untouched)
 */
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QIcon>
#include <QImage>
#include <QtPlugin>

Q_IMPORT_PLUGIN(RecolorPlugin)  // compiled in -- nothing to deploy

static void setTheme(const char* c) {
  QFile f("/tmp/themecolor");
  f.open(QIODevice::WriteOnly);
  f.write(c);
}

static QString px(const QIcon& i) { return QColor::fromRgba(i.pixmap(20, 20).toImage().pixel(10, 10)).name(); }

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);

  QIcon icon;  // built ONCE, as uic does in setupUi()
  icon.addFile(QStringLiteral(":/icons/Foo.svgt"), QSize(), QIcon::Normal, QIcon::Off);

  setTheme("#0000ff");
  qDebug().noquote() << "theme=blue  ->" << px(icon);
  setTheme("#ff0000");
  qDebug().noquote() << "theme=red   ->" << px(icon) << " <-- SAME QIcon, no rebuild";
  setTheme("#22aa22");
  qDebug().noquote() << "theme=green ->" << px(icon);

  // A semantic fill must ignore the theme entirely.
  QIcon warn;
  warn.addFile(QStringLiteral(":/icons/Warn.svgt"), QSize(), QIcon::Normal, QIcon::Off);
  qDebug().noquote() << "semantic red fill, theme=green ->" << px(warn) << " <-- must stay #ff0000";

  return 0;
}
