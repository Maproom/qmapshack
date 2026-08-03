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

#ifndef CSVGTICONENGINE_H
#define CSVGTICONENGINE_H

#include <QColor>
#include <QHash>
#include <QIconEngine>
#include <QString>

/**
   @brief Icon engine that recolours a themable SVG (".svgt") from the application palette.

   A themed icon carries a <style id="current-color-scheme"> block with .ink/.paper classes
   (two roles, because a navy body and a light label move in opposite directions on a dark
   ground). At paint time this rewrites those class colours from the theme; fill="currentColor"
   parts follow. Qt's QSvgRenderer ignores a class `fill:` (unlike inkscape), so the resolved
   class fill is also inlined onto the elements that use it -- otherwise those shapes render
   black. An icon with no such block renders as authored.

   Roles map to a fixed light/dark pair, chosen by whether the palette is dark. The light arm is
   the colours the SVGs are authored in, so light mode renders as drawn. Reading the colours from
   the palette instead was tried and rejected -- see roleColor().

   Reached only through the ".svgt" suffix via CSvgtIconEnginePlugin, never referenced directly.
 */
class CSvgtIconEngine : public QIconEngine {
 public:
  CSvgtIconEngine() = default;

  void addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state) override;

  void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
  QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
  QPixmap scaledPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state, qreal scale) override;

  QSize actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

  QString key() const override;
  QIconEngine* clone() const override;

  /**
     @brief The three themed roles.

     Black is its own role, not more ink: 142 icons draw with both navy and black, and
     collapsing them onto one colour loses the drawing's detail on a dark ground.
   */
  enum class Role {
    Ink,    ///< the brand drawing colour -- navy #000080 today
    Paper,  ///< what ink sits on -- white and the light shades
    Lead,   ///< neutral detail: outlines, arrowheads -- black #000000 today
    Mark,   ///< marks the element being acted on -- the brighter blue #0000ff today
  };

  /**
     @brief Return @p data with the current-color-scheme classes recoloured.

     Rewrites each declaration of every rule in the <style id="current-color-scheme"> block
     to the colour of its role. Returns @p data unchanged when there is no such block.
   */
  static QByteArray recolored(QByteArray data, const QColor& ink, const QColor& paper, const QColor& lead,
                              const QColor& mark);

  /** @brief Theme colour for a role, from the fixed light/dark scheme; see the class doc. */
  static QColor roleColor(Role role);

 private:
  /**
     @brief The source registered for @p mode / @p state, with a fallback.

     A toggle button registers two files -- ".../UnLock.svgt" for Off and ".../Lock.svgt"
     for On -- so the engine must keep BOTH and pick per request. Falls back towards
     (Normal, Off) when a combination was never registered, which is what Qt itself does.
   */
  QString sourceFor(QIcon::Mode mode, QIcon::State state) const;

  QPixmap renderPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state, qreal scale);

  static int slot(QIcon::Mode mode, QIcon::State state) { return int(mode) * 2 + int(state); }

  QHash<int, QString> sources;  ///< slot() -> file; see sourceFor()
  QString firstSource;          ///< last-resort fallback: whatever was registered first
};

#endif  // CSVGTICONENGINE_H
