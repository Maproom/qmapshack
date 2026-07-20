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

#ifndef CSVGTICON_H
#define CSVGTICON_H

#include <QString>

class QSvgWidget;

/**
   @brief Load a themable ".svgt" into a QSvgWidget, themed like every other icon.

   QSvgWidget draws through QSvgRenderer directly and never constructs a QIcon, so
   CSvgtIconEngine -- which is an icon engine -- never runs for it. The static
   explanatory icons in the setup dialogs are all QSvgWidgets, which is why they kept
   their authored navy on a dark theme while the toolbars around them followed it.

   This applies the same recolouring the engine does, and keeps following the theme:
   the widget is re-themed on a palette change, so a live theme switch is not left
   showing stale colours.

   Use this instead of QSvgWidget::load() for anything under ":/icons".
 */
namespace CSvgtIcon {
/** @brief Themed equivalent of @c widget->load(path). @p path is a ":/icons/Foo.svgt" resource. */
void load(QSvgWidget* widget, const QString& path);

/**
   @brief A themed inline "data:image/png" source for an icon embedded in rich text.

   Rich-text @c <img> loads through QTextDocument/QImageReader, never through QIcon, so the
   ".svgt" engine never runs on it. This renders @p resource (a ":/icons/Foo.svgt") through the
   engine at the current scheme instead, and returns a self-contained data URI that themes the
   same in a QLabel, QTextBrowser/QTextEdit or a print. @p px is the logical box size.
 */
QString htmlImageSrc(const QString& resource, int px = 16);
}  // namespace CSvgtIcon

#endif  // CSVGTICON_H
