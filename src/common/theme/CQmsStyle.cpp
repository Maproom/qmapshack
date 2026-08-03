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

#include "theme/CQmsStyle.h"

#include <QApplication>
#include <QPainter>
#include <QStyleFactory>
#include <QStyleOptionMenuItem>

namespace {
constexpr qint32 kButtonBorderWidth = 2;
constexpr qint32 kMenuBarWidth = 4;
/** @brief Tint of a checked menu row. Painted under the content, so it does not dim the text. */
constexpr qint32 kMenuTintAlpha = 90;
}  // namespace

CQmsStyle::CQmsStyle(QStyle* base) : QProxyStyle(base) {}

void CQmsStyle::install() {
  // Re-create the active style by name: setStyle() deletes the style it replaces, so the running
  // one cannot be handed over as the base. Doing it this way keeps the user's -style choice.
  QApplication::setStyle(new CQmsStyle(QStyleFactory::create(QApplication::style()->objectName())));
}

void CQmsStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                              const QWidget* widget) const {
  QProxyStyle::drawPrimitive(element, option, painter, widget);

  if (element != PE_PanelButtonTool || !(option->state & State_On)) {
    return;
  }

  QPen pen(option->palette.color(QPalette::Highlight));
  pen.setWidth(kButtonBorderWidth);

  painter->save();
  painter->setPen(pen);
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(option->rect.adjusted(1, 1, -1, -1));
  painter->restore();
}

void CQmsStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                            const QWidget* widget) const {
  const QStyleOptionMenuItem* item = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
  const bool checked = element == CE_MenuItem && item != nullptr &&
                       item->checkType != QStyleOptionMenuItem::NotCheckable && item->checked;
  if (!checked) {
    QProxyStyle::drawControl(element, option, painter, widget);
    return;
  }

  const QColor highlight = item->palette.color(QPalette::Highlight);

  painter->save();

  // The tint goes under the content - painted over it, it dims the very text it marks. A row that
  // is already selected is filled by the base style, so it needs none.
  if (!(item->state & State_Selected)) {
    QColor tint = highlight;
    tint.setAlpha(kMenuTintAlpha);
    painter->fillRect(item->rect, tint);
  }

  // Weight, not only colour, carries the state - the same rule the icons follow.
  QStyleOptionMenuItem bolder(*item);
  bolder.font.setBold(true);
  QProxyStyle::drawControl(element, &bolder, painter, widget);

  QRect bar = item->rect;
  bar.setWidth(kMenuBarWidth);
  painter->fillRect(bar, highlight);

  painter->restore();
}
