// Re-verify the three Qt behaviours the icon plans depend on.
//
//   g++ -O2 -std=c++17 -fPIC qtfacts.cpp -o qtfacts $(pkg-config --cflags --libs Qt6Svg Qt6Gui Qt6Core)
//   QT_QPA_PLATFORM=offscreen ./qtfacts <an-icon.svg> <a-32px.png>
//
// Each of these was measured, not assumed, and each one decided something. If a future Qt makes
// one of them false, the corresponding decision needs revisiting -- so re-run this rather than
// trusting the numbers written in .notes/icons-task1-ui-svg-plan.md.
#include <QGuiApplication>
#include <QIcon>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QTextStream>

// 1. QPixmap and QIcon are NOT interchangeable for SVG.
//    Decides: <pixmap> refs in .ui cannot be swapped without a viewBox, and Windows must ship
//    iconengines/qsvgicon or icons go blurry (not blank -- nobody reports blurry).
static void pixmapVsIcon(QTextStream& o, const QString& svg) {
  o << "1. QPixmap vs QIcon on the same SVG\n";
  const QPixmap viaImage(svg);  // imageformats/qsvg -> one raster at the SVG's natural size
  o << "     QPixmap(svg)          -> " << viaImage.width() << "x" << viaImage.height()
    << "   (natural size; this is the fallback if qsvgicon is missing)\n";
  const QIcon ic(svg);  // iconengines/qsvgicon -> rendered per request
  for (int s : {16, 32, 64, 256}) {
    o << "     QIcon(svg).pixmap(" << QString::number(s).rightJustified(3) << ") -> "
      << ic.pixmap(s, s).width() << "x" << ic.pixmap(s, s).height() << "\n";
  }
  o << "     svg image plugin present: "
    << (QImageReader::supportedImageFormats().contains("svg") ? "yes" : "NO") << "\n\n";
}

// 2. QIcon refuses to upscale a raster past its natural size.
//    Decides: IWksItem's QPixmap icon cannot become a QIcon while waypoint symbols stay raster --
//    the delegate stretches 32 -> row height today, a QIcon would centre it at 32 instead.
static void iconWontUpscale(QTextStream& o, const QString& png) {
  o << "2. QIcon will not upscale a raster\n";
  const QPixmap raster(png);
  const QIcon ic(raster);
  o << "     source raster         -> " << raster.width() << "x" << raster.height() << "\n";
  for (int s : {24, 32, 40, 64}) {
    const QPixmap p = ic.pixmap(s, s);
    o << "     QIcon(raster).pixmap(" << QString::number(s).rightJustified(2) << ") -> "
      << p.width() << "x" << p.height() << (p.width() < s ? "   <- refused to upscale" : "") << "\n";
  }
  o << "\n";
}

// 3. Qt renders <marker> but ignores markerUnits="strokeWidth" -- the SVG default, and what
//    Inkscape's stock arrows are authored against.
//    Decides: markers must be baked into geometry (object-stroke-to-path), because an arrowhead
//    is the one part of a drawing sized by stroke width, so it is the one part Qt draws wrong.
static void markerUnitsIgnored(QTextStream& o) {
  o << "3. markerUnits=\"strokeWidth\" (the default) is ignored\n";
  // Two identical lines sharing one marker; only stroke-width differs. Per spec the second
  // arrowhead must be 8x the first. Inkscape and browsers do that; Qt draws them the same.
  static const char* kSvg =
      "<svg xmlns='http://www.w3.org/2000/svg' width='200' height='100' viewBox='0 0 200 100'>"
      "<defs><marker id='a' orient='auto' refX='0' refY='0' style='overflow:visible'>"
      "<path d='M 0,0 5,-5 -12.5,0 5,5 Z' fill='#000' transform='scale(0.4)'/></marker></defs>"
      "<path d='M 40,30 L 160,30' stroke='#00f' stroke-width='1' marker-end='url(#a)'/>"
      "<path d='M 40,70 L 160,70' stroke='#00f' stroke-width='8' marker-end='url(#a)'/></svg>";
  QSvgRenderer r{QByteArray(kSvg)};
  QImage img(200, 100, QImage::Format_ARGB32);
  img.fill(Qt::white);
  QPainter p(&img);
  r.render(&p);
  p.end();
  // Black ink only (the arrows); the lines are blue. Measure each arrow's height.
  auto arrowHeight = [&img](int yFrom, int yTo) {
    int top = -1, bottom = -1;
    for (int y = yFrom; y < yTo; ++y) {
      for (int x = 160; x < img.width(); ++x) {
        const QColor c = img.pixelColor(x, y);
        if (c.red() < 100 && c.green() < 100 && c.blue() < 100) {
          if (top < 0) top = y;
          bottom = y;
          break;
        }
      }
    }
    return (top < 0) ? 0 : bottom - top + 1;
  };
  const int thin = arrowHeight(0, 50);
  const int thick = arrowHeight(50, 100);
  o << "     stroke-width 1 -> arrowhead " << thin << "px tall\n"
    << "     stroke-width 8 -> arrowhead " << thick << "px tall"
    << (thick >= thin * 4 ? "   (scaled: Qt now honours markerUnits -- markers need no baking)"
                          : "   <- NOT scaled; per spec it should be ~8x the first")
    << "\n\n";
}

int main(int argc, char** argv) {
  if (argc != 3) {
    QTextStream(stderr) << "usage: qtfacts <icon.svg> <icon32.png>\n";
    return 2;
  }
  QTextStream o(stdout);
  {
    QGuiApplication app(argc, argv);
    o << "Qt " << qVersion() << "\n\n";
    pixmapVsIcon(o, argv[1]);
    iconWontUpscale(o, argv[2]);
    markerUnitsIgnored(o);
  }
  return 0;
}
