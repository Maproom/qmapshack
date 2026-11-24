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

#include "CHelpBlock.h"

#include <QIcon>
#include <QPainter>
#include <QPainterPath>

namespace {
constexpr int kBarWidth = 3;
constexpr int kIconSize = 20;
constexpr int kPad = 6;
constexpr qreal kRadius = 4;
}  // namespace

CHelpBlock::CHelpBlock(QWidget* parent) : QFrame(parent) {
  setFrameShape(QFrame::NoFrame);
  setContentsMargins(kBarWidth + kPad + kIconSize + kPad, kPad, kPad, kPad);
}

void CHelpBlock::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  QPainterPath card;
  card.addRoundedRect(QRectF(rect()), kRadius, kRadius);
  p.fillPath(card, palette().color(QPalette::AlternateBase));

  p.save();
  p.setClipPath(card);
  p.fillRect(QRect(0, 0, kBarWidth, height()), palette().color(QPalette::Highlight));
  p.restore();

  QIcon(":/icons/HelpHint.svgt").paint(&p, QRect(kBarWidth + kPad, kPad, kIconSize, kIconSize));
}
