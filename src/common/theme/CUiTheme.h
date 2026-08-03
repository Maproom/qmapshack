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

#ifndef CUITHEME_H
#define CUITHEME_H

#include <QColor>
#include <QGuiApplication>
#include <QPalette>
#include <QString>

class QEvent;
class QLabel;
class QWidget;

/**
   @brief True when the application palette is a dark one.

   Inline so the icon engine can share it: that plugin is a separate static target and links
   nothing of CUiTheme.cpp. Every consumer of the light/dark split has to agree on this test.
 */
inline bool paletteIsDark() { return QGuiApplication::palette().color(QPalette::Window).lightness() < 128; }

/**
   @brief Status colours for rich text, from a fixed light/dark scheme.

   Every role is a foreground/background PAIR. Rich text that sets only one of the two takes the
   other from the palette, which inverts with the colour scheme: a hardcoded light background
   ends up carrying the palette's light text. Use css() for anything that fills a background and
   cssForeground()/foreground() for text that sits on the widget's own background.

   The light arm keeps the colours the markup was authored in, so light mode renders unchanged;
   the dark arm is the hand-picked counterpart. Tune here, not per site.

   The trailing widget helpers are the other half: they repair widgets that resolve the palette
   once and would otherwise keep the scheme they were built in.
 */
class CUiTheme {
 public:
  /** @brief What a piece of text means, not what colour it is. */
  enum class Role {
    eNeutral,  ///< table header, plain emphasis
    eOk,       ///< success, "nothing to do"
    eWarn,     ///< degraded, destructive-but-intended
    eError,    ///< failure
    eInfo,     ///< progress, echoed commands
    eCode,     ///< inline code and preformatted blocks in the help pages
  };

  /**
     @brief Resolve every role to its light arm while the object lives, if @p active.

     For rendering onto paper, where the screen's colour scheme is irrelevant. Does not touch
     QPalette - a colour taken from the palette needs its own paper branch. GUI thread only, and
     does not nest: the override is a plain flag, not a counter and not thread-local.
   */
  class CForceLight {
   public:
    explicit CForceLight(bool active = true);
    ~CForceLight();

    CForceLight(const CForceLight&) = delete;
    CForceLight& operator=(const CForceLight&) = delete;

   private:
    const bool active;
  };

  /** @brief True when the application palette is a dark one, unless a CForceLight is active. */
  static bool isDark();

  /** @brief Text colour of @p role. Legible on the role's own background and on the palette's. */
  static QColor foreground(Role role);

  /** @brief Background colour of @p role. Always pair it with foreground(). */
  static QColor background(Role role);

  /**
     @brief CSS declarations for both colours, e.g. for a table cell that fills its background.

     No trailing ';' - append one before adding further declarations.
   */
  static QString css(Role role);

  /** @brief CSS colour declaration alone, for text on the widget's own background. No trailing ';'. */
  static QString cssForeground(Role role);

  /** @brief @p text wrapped in a span carrying cssForeground(). Inserted as-is - escape user data. */
  static QString span(Role role, const QString& text);

  /** @brief span() with @p text in bold, the usual shape for a status message. */
  static QString spanBold(Role role, const QString& text);

  /**
     @brief Style @p label as a permanent status message of @p role, in bold.

     For a label that is always a status message and only gets shown or hidden. A label that
     merely shows a status value sometimes has to carry the colour in its text - use span().

     Call it once; the label records @p role and installThemeRefresh() re-applies it on a colour
     scheme change, so the caller needs no changeEvent() of its own.
   */
  static void markLabel(QLabel* label, Role role);

  /**
     @brief True when @p event announces a palette change. Test a changeEvent() with this.

     Both event types have to be tested: which one arrives depends on the Qt version. Qt delivers
     PaletteChange to every widget, at every nesting depth, exactly once - so a widget that holds
     content derived from the scheme rebuilds it here, and needs nothing central to drive it.
   */
  static bool isPaletteChange(const QEvent* event);

  /**
     @brief Repair what a QLabel cannot repair for itself. Call once from main(), per application.

     Covers the two things a QLabel bakes in and never revisits: a markLabel() role, and the anchor
     colour of rich text. Qt re-resolves unstyled document text against the palette when it draws,
     but an anchor gets an explicit colour at parse time and QLabel does not re-parse; re-applying
     the label's own text is the repair.

     A QTextBrowser cannot be repaired this way - toHtml() returns the baked colour rather than the
     source - so whoever produced its content rebuilds it from its own changeEvent(). The checked
     state of buttons and menu items is drawn by CQmsStyle and needs nothing here.
   */
  static void installThemeRefresh();
};

#endif  // CUITHEME_H
