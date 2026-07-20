/* Does the two-colour mechanism actually work in Qt?
 *
 * An SVG element has ONE `color`, so a themed fill and a differently-themed
 * stroke cannot both be currentColor. The phase-7a plan proposes:
 *
 *     .paper-ink { fill:#ffffff; color:#000080 }
 *     <path class="paper-ink" style="stroke:currentColor"/>
 *
 * i.e. the fill comes from the class DIRECTLY, and the class's `color` feeds an
 * inline currentColor stroke. The plan records this as verified; it is about to
 * carry ~900 element edits across 57 icons, so verify it here rather than trust
 * the note.
 *
 * Checks, in the order they can fail:
 *   1. fill from a class renders at all
 *   2. inline stroke=currentColor resolves against the class's color
 *   3. the two are INDEPENDENT (changing color must not move the fill)
 *
 * 1-3 are the mechanism and they are what the exit code gates.
 *
 * Why class-fill is safe on our Qt 6.8 floor, though only 6.10.1 is installed
 * here to measure: qtsvg's cssStyleLookup() -> parseCSStoXMLAttrs() merges CSS
 * declarations into the element's attribute set, and it covers fill, stroke AND
 * color alike -- present in the 6.8 branch, not a 6.10 addition. An inline
 * style= then wins, but only for the properties it actually declares. So a class
 * `fill` survives beside an inline style="stroke:currentColor", which is check 1.
 *
 * Check 4 is INFORMATIONAL. The plan records "Qt honours fill from a class and
 * silently IGNORES stroke from one" (measured on 6.9.2); both 6.10.1 here and
 * the 6.8 source above say a class stroke is honoured, so that note is most
 * likely a measurement artifact rather than a version difference. It does not
 * change the design -- inline strokes work under BOTH behaviours and cost
 * nothing -- so do not "simplify" strokes into the class on the strength of it.
 *
 * g++ -O2 -std=c++17 -fPIC strokecheck.cpp -o strokecheck \
 *     $(pkg-config --cflags --libs Qt6Svg Qt6Gui Qt6Core)
 * QT_QPA_PLATFORM=offscreen ./strokecheck
 */

#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QtGlobal>

#include <cstdio>

static QImage render(const QByteArray& svg) {
  QSvgRenderer r(svg);
  QImage img(64, 64, QImage::Format_ARGB32_Premultiplied);
  img.fill(Qt::transparent);
  if (!r.isValid()) {
    return QImage();
  }
  QPainter p(&img);
  r.render(&p);
  return img;
}

// Centre = interior (fill). A point just inside the rect edge = the stroke.
static QRgb centre(const QImage& i) { return i.isNull() ? 0 : i.pixel(32, 32); }
static QRgb edge(const QImage& i) { return i.isNull() ? 0 : i.pixel(32, 8); }

static const char* fmt(QRgb c) {
  static char buf[32];
  snprintf(buf, sizeof(buf), "#%02x%02x%02x a%d", qRed(c), qGreen(c), qBlue(c), qAlpha(c));
  return buf;
}

static bool near(QRgb c, int r, int g, int b) {
  return qAbs(qRed(c) - r) <= 2 && qAbs(qGreen(c) - g) <= 2 && qAbs(qBlue(c) - b) <= 2;
}

static int fails = 0;
static void check(const char* what, bool ok, QRgb got) {
  printf("%-58s %-16s %s\n", what, fmt(got), ok ? "OK" : "FAIL");
  fails += !ok;
}

// A rect with a fat stroke, so interior and edge are separable at 64px.
static QByteArray doc(const QByteArray& rules, const QByteArray& inlineStyle) {
  return "<svg xmlns='http://www.w3.org/2000/svg' width='64' height='64' viewBox='0 0 64 64'>"
         "<style id='current-color-scheme'>" +
         rules +
         "</style>"
         "<rect class='two' x='8' y='8' width='48' height='48' stroke-width='8' style='" +
         inlineStyle + "'/></svg>";
}

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);

  // 1 + 2: white fill from the class, navy stroke via the class's color.
  {
    const QImage i = render(doc(".two{fill:#ffffff;color:#000080}", "stroke:currentColor"));
    check("1. fill from class -> white interior", near(centre(i), 255, 255, 255), centre(i));
    check("2. inline stroke:currentColor -> navy edge", near(edge(i), 0, 0, 128), edge(i));
  }

  // 3: independence -- retheme `color` only; the fill must not follow it.
  {
    const QImage i = render(doc(".two{fill:#ffffff;color:#ccccff}", "stroke:currentColor"));
    check("3a. fill unchanged when color changes", near(centre(i), 255, 255, 255), centre(i));
    check("3b. stroke followed color -> #ccccff", near(edge(i), 204, 204, 255), edge(i));
  }

  // 4: informational only -- see the header. Not gated either way.
  {
    const QImage i = render(doc(".two{fill:#ffffff;stroke:#ff0000}", ""));
    const bool honoured = near(edge(i), 255, 0, 0);
    printf("%-58s %-16s %s\n", "4. stroke from a class (informational)", fmt(edge(i)),
           honoured ? "HONOURED (plan says ignored on 6.9.2)" : "ignored, as the plan records");
  }

  printf("\n%s\n", fails ? "MECHANISM UNSOUND -- do not migrate" : "mechanism sound (checks 1-3)");
  return fails ? 1 : 0;
}
