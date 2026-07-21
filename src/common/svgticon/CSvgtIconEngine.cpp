/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "CSvgtIconEngine.h"

#include <QApplication>
#include <QFile>
#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPixmapCache>
#include <QRegularExpression>
#include <QStyle>
#include <QStyleOption>
#include <QSvgRenderer>

QColor CSvgtIconEngine::roleColor(Role role) {
  // A fixed light/dark scheme, chosen by whether the palette is dark. The light arm is exactly
  // the colours the SVGs are authored in, so light mode renders as drawn; the dark arm is the
  // hand-picked counterpart. Tune here, not per icon.
  //
  // Following the palette instead was tried and rejected: no palette role carries navy, so ink
  // had to land on QPalette::Link -- #0000ff on a stock light theme, and an arbitrary
  // theme-defined blue elsewhere. That made the icon set's brand colour whatever the desktop
  // happened to call a hyperlink.
  //
  // The role colours are deliberately independent of QIcon::Mode: Disabled/Selected are not a
  // different colour scheme but a grey/tint applied to the rendered Normal pixmap. renderPixmap()
  // does that via QStyle::generatedIconPixmap(), the same way Qt's own QSvgIconEngine does.
  const bool dark = QGuiApplication::palette().color(QPalette::Window).lightness() < 128;
  switch (role) {
    case Role::Ink:
      return dark ? QColor(0xcc, 0xcc, 0xff) : QColor(0x00, 0x00, 0x80);
    case Role::Paper:
      return dark ? QColor(0x35, 0x35, 0x35) : QColor(0xff, 0xff, 0xff);
    case Role::Lead:
      return dark ? QColor(0xe0, 0xe0, 0xe0) : QColor(0x00, 0x00, 0x00);
    case Role::Mark:
      return dark ? QColor(0x66, 0xaa, 0xff) : QColor(0x00, 0x00, 0xff);
  }
  return QColor();
}

QByteArray CSvgtIconEngine::recolored(QByteArray data, const QColor& ink, const QColor& paper, const QColor& lead,
                                      const QColor& mark) {
  QString text = QString::fromUtf8(data);

  // The themable markup is a <style id="current-color-scheme"> block of .ink*/.paper*/.lead*/.mark* rules;
  // an icon without one is not themed and is returned as authored.
  static const QRegularExpression styleRe(R"(<style\b[^>]*\bid="current-color-scheme"[^>]*>(.*?)</style>)",
                                          QRegularExpression::DotMatchesEverythingOption);
  const QRegularExpressionMatch sm = styleRe.match(text);
  if (!sm.hasMatch()) {
    return data;
  }

  // Rewrite every colour declaration of every rule. The class name is a '-'-separated list of
  // role tokens, one per declaration, in the order the declarations appear:
  //
  //   .ink        { color:#000080 }                  -> color takes ink
  //   .paper-ink  { fill:#ffffff; color:#000080 }    -> fill takes paper, color takes ink
  //
  // The two-declaration form is how an element gets a themed fill and a DIFFERENTLY themed
  // stroke: an element has only one `color`, so `color` feeds an inline stroke="currentColor"
  // while the fill is named by the class. But Qt's QSvgRenderer IGNORES a class `fill:` (it
  // honours only inline fill and currentColor), so a class-filled shape renders black. The
  // resolved fill is therefore also inlined onto the elements that use the class -- see the
  // injection pass below. Strokes stay inline for the same reason (whether Qt honours a
  // class-supplied stroke has measured differently across versions; inkscape honours both).
  //
  // A token names the role by prefix: paper*, lead*, mark*, anything else (ink, ink2, ...) -> ink.
  static const QRegularExpression ruleRe(R"(\.([A-Za-z0-9_-]+)\s*\{([^}]*)\})");
  static const QRegularExpression declRe(R"(\s*([A-Za-z-]+)\s*:\s*[^;]*)");
  const QString rules = sm.captured(1);
  QString out;
  QHash<QString, QString> classFill;  // class name -> resolved fill colour, to inline for Qt
  qsizetype last = 0;
  QRegularExpressionMatchIterator it = ruleRe.globalMatch(rules);
  while (it.hasNext()) {
    const QRegularExpressionMatch m = it.next();
    out += rules.mid(last, m.capturedStart() - last);
    const QString cls = m.captured(1);
    const QStringList roles = cls.split('-');

    // Rebuild the rule declaration by declaration, so a property we do not theme is preserved
    // rather than dropped. Roles are positional; a rule with more declarations than the name
    // has tokens keeps the surplus untouched.
    QStringList decls;
    int i = 0;
    const QStringList parts = m.captured(2).split(';', Qt::SkipEmptyParts);
    for (const QString& part : parts) {
      const QRegularExpressionMatch dm = declRe.match(part);
      if (!dm.hasMatch() || i >= roles.size()) {
        decls += part.trimmed();
        continue;
      }
      const QString prop = dm.captured(1);
      const QString& token = roles.at(i);
      const QColor& c = token.startsWith(QLatin1String("paper"))  ? paper
                        : token.startsWith(QLatin1String("lead")) ? lead
                        : token.startsWith(QLatin1String("mark")) ? mark
                                                                  : ink;
      decls += prop + ":" + c.name();
      if (prop == QLatin1String("fill")) {
        classFill.insert(cls, c.name());
      }
      ++i;
    }
    out += "." + cls + "{" + decls.join(';') + "}";
    last = m.capturedEnd();
  }
  out += rules.mid(last);

  text.replace(sm.capturedStart(1), sm.capturedLength(1), out);

  // Inline each class's fill onto the elements that reference it: Qt's QSvgRenderer drops a class
  // `fill:`, so without this a shape whose only fill is class-supplied (the .paper-ink form) paints
  // black. A `fill="..."` presentation attribute is honoured by Qt (nothing overrides it there once
  // the class fill is dropped) and, in a renderer that DOES apply class fill, is overridden by the
  // identical class value -- so it changes nothing anywhere except Qt, where it is the fix. Elements
  // that already carry an inline fill (fill:... in a style, or a fill= attribute) are left untouched.
  if (!classFill.isEmpty()) {
    static const QRegularExpression tagRe(R"(<(?:path|rect|circle|ellipse|polygon|polyline|line|g|use)\b[^>]*?>)");
    static const QRegularExpression clsRe(R"RX(\bclass="([^"]*)")RX");
    static const QRegularExpression inlineFillRe(R"((?:^|[;"\s])fill\s*[:=])");
    QString result;
    qsizetype pos = 0;
    QRegularExpressionMatchIterator ti = tagRe.globalMatch(text);
    while (ti.hasNext()) {
      const QRegularExpressionMatch tm = ti.next();
      result += text.mid(pos, tm.capturedStart() - pos);
      pos = tm.capturedEnd();
      QString tag = tm.captured(0);

      const QRegularExpressionMatch cm = clsRe.match(tag);
      const auto fit = cm.hasMatch() ? classFill.constFind(cm.captured(1)) : classFill.constEnd();
      if (fit != classFill.constEnd() && !tag.contains(inlineFillRe)) {
        const qsizetype at = tag.endsWith(QLatin1String("/>")) ? tag.size() - 2 : tag.size() - 1;
        tag.insert(at, QStringLiteral(" fill=\"%1\"").arg(fit.value()));
      }
      result += tag;
    }
    result += text.mid(pos);
    text = result;
  }

  return text.toUtf8();
}

void CSvgtIconEngine::addFile(const QString& fileName, const QSize&, QIcon::Mode mode, QIcon::State state) {
  // One vector source per mode/state, rendered at any size. Size is ignored on purpose:
  // a vector serves every size, so there is nothing to select on.
  //
  // Keeping only the last file here is what made every toggle button show its On icon:
  // uic registers Off then On, so a lock button was always drawn shut.
  if (fileName.isEmpty()) {
    return;
  }
  if (firstSource.isEmpty()) {
    firstSource = fileName;
  }
  sources.insert(slot(mode, state), fileName);
}

QString CSvgtIconEngine::sourceFor(QIcon::Mode mode, QIcon::State state) const {
  // Exact match first, then widen: state matters more than mode, because state selects a
  // DIFFERENT drawing (Lock vs UnLock) while mode only asks for a variation of one.
  for (const int key : {slot(mode, state), slot(QIcon::Normal, state), slot(mode, QIcon::Off),
                        slot(QIcon::Normal, QIcon::Off)}) {
    const auto it = sources.constFind(key);
    if (it != sources.constEnd()) {
      return it.value();
    }
  }
  return firstSource;
}

QPixmap CSvgtIconEngine::renderPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state, qreal scale) {
  const QString fileName = sourceFor(mode, state);
  if (fileName.isEmpty() || size.isEmpty()) {
    return QPixmap();
  }

  const QColor ink = roleColor(Role::Ink);
  const QColor paper = roleColor(Role::Paper);
  const QColor lead = roleColor(Role::Lead);
  const QColor mark = roleColor(Role::Mark);
  const QSize deviceSize = size * scale;

  // Cache key names the resolved file, the mode, and every theme colour, so a palette change or a
  // Disabled/Normal difference misses and re-renders -- and On and Off never share an entry.
  const QString cacheKey = QStringLiteral("svgt:%1:%2x%3@%4:m%5:%6/%7/%8/%9")
                               .arg(fileName)
                               .arg(deviceSize.width())
                               .arg(deviceSize.height())
                               .arg(scale)
                               .arg(int(mode))
                               .arg(ink.name(), paper.name(), lead.name(), mark.name());

  QPixmap pm;
  if (QPixmapCache::find(cacheKey, &pm)) {
    return pm;
  }

  QFile f(fileName);
  if (!f.open(QIODevice::ReadOnly)) {
    return QPixmap();
  }

  QSvgRenderer renderer(recolored(f.readAll(), ink, paper, lead, mark));
  if (!renderer.isValid()) {
    return QPixmap();
  }

  pm = QPixmap(deviceSize);
  pm.fill(Qt::transparent);
  {
    QPainter p(&pm);
    renderer.render(&p);
  }
  pm.setDevicePixelRatio(scale);

  // Bake in the disabled/selected look. A custom QIconEngine must do this itself -- nothing above
  // the engine re-applies it; Qt's own QSvgIconEngine greys/tints here too, via the same call.
  if (mode != QIcon::Normal) {
    if (QStyle* style = QApplication::style()) {
      QStyleOption opt;
      opt.palette = QGuiApplication::palette();
      const QPixmap generated = style->generatedIconPixmap(mode, pm, &opt);
      if (!generated.isNull()) {
        pm = generated;
      }
    }
  }

  QPixmapCache::insert(cacheKey, pm);
  return pm;
}

QPixmap CSvgtIconEngine::scaledPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state, qreal scale) {
  return renderPixmap(size, mode, state, scale);
}

QPixmap CSvgtIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
  return scaledPixmap(size, mode, state, 1.0);
}

void CSvgtIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
  const qreal scale = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
  const QPixmap pm = scaledPixmap(rect.size(), mode, state, scale);
  if (!pm.isNull()) {
    painter->drawPixmap(rect, pm);
  }
}

QSize CSvgtIconEngine::actualSize(const QSize& size, QIcon::Mode, QIcon::State) {
  // A vector icon serves any requested size.
  return size;
}

QString CSvgtIconEngine::key() const { return QStringLiteral("svgt"); }

QIconEngine* CSvgtIconEngine::clone() const { return new CSvgtIconEngine(*this); }
