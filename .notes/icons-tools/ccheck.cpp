// Independent check: does Qt's SVG renderer honour currentColor, and how?
// Renders 8x8 swatches and reports the centre pixel.
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QtGlobal>
#include <cstdio>

static QString px(const QString& svg) {
  QSvgRenderer r(svg.toUtf8());
  if (!r.isValid()) return "INVALID";
  QImage img(8, 8, QImage::Format_ARGB32);
  img.fill(Qt::transparent);
  QPainter p(&img);
  r.render(&p);
  p.end();
  const QColor c = img.pixelColor(4, 4);
  if (c.alpha() == 0) return "transparent";
  return c.name();
}

static void t(const char* what, const QString& svg, const char* expect) {
  const QString got = px(svg);
  std::printf("%-46s %-12s expect %s\n", what, qPrintable(got), expect);
}

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  std::printf("Qt %s\n\n", qVersion());

  const QString head = "<svg xmlns='http://www.w3.org/2000/svg' width='8' height='8'";
  const QString rect = "<rect width='8' height='8'";

  t("1 currentColor + root color attr", head + " color='#ff0000'>" + rect + " fill='currentColor'/></svg>", "#ff0000");

  t("2 currentColor + root style=color", head + " style='color:#0000ff'>" + rect + " fill='currentColor'/></svg>",
    "#0000ff");

  t("3 currentColor inherited via <g color>", head + "><g color='#00ff00'>" + rect + " fill='currentColor'/></g></svg>",
    "#00ff00");

  t("4 lowercase currentcolor (QTBUG-46947)", head + " color='#ff0000'>" + rect + " fill='currentcolor'/></svg>",
    "black if bug");

  t("5 currentColor, NO color set", head + ">" + rect + " fill='currentColor'/></svg>", "black");

  t("6 plain fill untouched by root color", head + " color='#ff0000'>" + rect + " fill='#00ff00'/></svg>",
    "#00ff00 (semantic safe)");

  t("7 stroke='currentColor'",
    head +
        " color='#ff0000'><rect x='1' y='1' width='6' height='6' fill='none' "
        "stroke='currentColor' stroke-width='6'/></svg>",
    "#ff0000");

  // KDE Breeze shape: a <style> block defines the class colour.
  t("8 KDE style-block class",
    head +
        "><style type='text/css' id='current-color-scheme'>"
        ".ColorScheme-Text{color:#232629;}</style>"
        "<g class='ColorScheme-Text' fill='currentColor'>" +
        rect + "/></g></svg>",
    "#232629");

  // Does a root color= attribute BEAT the style block? (the specificity trap)
  t("9 style-block class vs root color attr",
    head +
        " color='#ff0000'><style type='text/css' id='current-color-scheme'>"
        ".ColorScheme-Text{color:#232629;}</style>"
        "<g class='ColorScheme-Text' fill='currentColor'>" +
        rect + "/></g></svg>",
    "#232629 if class wins");

  return 0;
}
