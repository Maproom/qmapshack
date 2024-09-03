/**********************************************************************************************
    Copyright (C) 2015 Florian Pigorsch <mail@florian-pigorsch.de>

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

#include "widgets/CDateTimeEditor.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QTimeZone>

CDateTimeEditor::CDateTimeEditor(QWidget* parent) : QWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);

  addGroup(YEAR, 1, 9999, 'Y');
  addGroup(MONTH, 1, 12, 'M');
  addGroup(DAY, 1, 31, 'D');
  addGroup(HOUR, 0, 23, 'H');
  addGroup(MINUTE, 0, 59, 'M');
  addGroup(SECOND, 0, 59, 'S');

  addCharacter(YEAR, "0123456789");
  addCharacter(YEAR, "0123456789");
  addCharacter(YEAR, "0123456789");
  addCharacter(YEAR, "0123456789");
  addFixedCharacter('-');
  addCharacter(MONTH, "01");
  addCharacter(MONTH, "0123456789");
  addFixedCharacter('-');
  addCharacter(DAY, "0123");
  addCharacter(DAY, "0123456789");
  addFixedCharacter(' ');
  addCharacter(HOUR, "012");
  addCharacter(HOUR, "0123456789");
  addFixedCharacter(':');
  addCharacter(MINUTE, "012345");
  addCharacter(MINUTE, "0123456789");
  addFixedCharacter(':');
  addCharacter(SECOND, "012345");
  addCharacter(SECOND, "0123456789");

  m_selectedPosition = -1;
  m_selectedGroup = NONE;

  validate();
}

void CDateTimeEditor::setDateTime(const QDateTime& dateTime) {
  m_dateTime = dateTime;
  if (m_dateTime.isValid()) {
    setNumber(YEAR, m_dateTime.date().year());
    setNumber(MONTH, m_dateTime.date().month());
    setNumber(DAY, m_dateTime.date().day());
    setNumber(HOUR, m_dateTime.time().hour());
    setNumber(MINUTE, m_dateTime.time().minute());
    setNumber(SECOND, m_dateTime.time().second());
  } else {
    for (CharacterInfo& c : m_characters) {
      if (c.isEditable()) {
        c.m_character = m_groups[c.m_group].placeholder;
      }
    }
  }

  validate();
  update();
}

QSize CDateTimeEditor::sizeHint() const {
  const QSize charSize = getCharSize();
  const int h = qMax(charSize.height(), 14) + 2 * 5;
  const int w = m_characters.size() * charSize.width() + 2 * 5;
  return QSize(w, h);
}

QSize CDateTimeEditor::minimumSizeHint() const { return sizeHint(); }

void CDateTimeEditor::paintEvent(QPaintEvent* /*event*/) {
  QPainter painter(this);

  QStyleOptionFrame panel;
  initStyleOption(&panel);
  style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &painter, this);

  QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
  const int margin = 2;
  r.setX(r.x() + margin);
  r.setY(r.y() + margin);
  r.setRight(r.right() - margin);
  r.setBottom(r.bottom() - margin);
  painter.setClipRect(r);

  QPalette pal = style()->standardPalette();

  if (m_selectedPosition >= 0) {
    const GroupInfo& g = m_groups[m_selectedGroup];

    for (auto p : g.positions) {
      const QRect r = getCharacterRect(p);
      painter.fillRect(r, pal.alternateBase());
    }

    const QRect r = getCharacterRect(m_selectedPosition);
    if (g.invalid) {
      painter.fillRect(r, Qt::red);
    } else {
      painter.fillRect(r, pal.highlight());
    }
  }

  painter.setRenderHint(QPainter::Antialiasing);
  for (const CharacterInfo& c : std::as_const(m_characters)) {
    const QRect r = getCharacterRect(c.m_position);
    if (c.m_position == m_selectedPosition) {
      painter.setPen(pal.highlightedText().color());
    } else {
      const GroupInfo& g = m_groups[c.m_group];
      if (g.invalid) {
        painter.setPen(Qt::red);
      } else {
        painter.setPen(pal.text().color());
      }
    }

    painter.drawText(r, Qt::AlignCenter, c.m_character);
  }
}

void CDateTimeEditor::mousePressEvent(QMouseEvent* event) {
  const int p = getNearestEditablePosition(event->pos());
  if (p >= 0) {
    m_selectedPosition = p;
    m_selectedGroup = m_characters[m_selectedPosition].m_group;
  } else {
    m_selectedPosition = -1;
    m_selectedGroup = NONE;
  }
  update();
}

void CDateTimeEditor::wheelEvent(QWheelEvent* event) {
  if (m_selectedPosition == -1) {
    return;
  }

  if (event->angleDelta().y() > 0) {
    editSelectedCharacterDelta(+1);
  } else if (event->angleDelta().y() < 0) {
    editSelectedCharacterDelta(-1);
  }
}

void CDateTimeEditor::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_Left:
      selectPrevPosition();
      break;

    case Qt::Key_Right:
      selectNextPosition();
      break;

    case Qt::Key_Up:
      editSelectedCharacterDelta(+1);
      break;

    case Qt::Key_Down:
      editSelectedCharacterDelta(-1);
      break;

    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
      if (editSelectedCharacter(event->text())) {
        selectNextPosition();
      }
      break;

    default:
      break;
  }
}

void CDateTimeEditor::focusInEvent(QFocusEvent*) { selectFirstPosition(); }

void CDateTimeEditor::focusOutEvent(QFocusEvent*) {
  m_selectedPosition = -1;
  m_selectedGroup = NONE;
  update();
}

bool CDateTimeEditor::focusNextPrevChild(bool next) {
  bool found = next ? selectNextPosition() : selectPrevPosition();

  if (!found) {
    found = QWidget::focusNextPrevChild(next);
  }
  return found;
}

const QSize& CDateTimeEditor::getCharSize() const {
  if (m_font != font() || m_charSize.isEmpty()) {
    m_font = font();
    const QFontMetrics fm = fontMetrics();

    m_charSize.setWidth(0);
    m_charSize.setHeight(0);

    const QString& chars = "0123456789-: YMDHS";
    for (const QChar& c : chars) {
      const QSize cSize = fm.size(Qt::TextSingleLine, c);
      m_charSize = m_charSize.expandedTo(cSize);
    }
  }

  return m_charSize;
}

QRect CDateTimeEditor::getCharacterRect(int position) const {
  const QSize& s = getCharSize();
  return QRect(QPoint(position * s.width() + 4, 5), s);
}

void CDateTimeEditor::addCharacter(CDateTimeEditor::Group group, const QString& availableCharacters) {
  CharacterInfo c;
  c.m_position = m_characters.size();
  c.m_group = group;
  c.m_character = m_groups[group].placeholder;
  c.m_availableCharacters = availableCharacters;
  m_characters << c;
  m_groups[c.m_group].positions << c.m_position;
}

void CDateTimeEditor::addFixedCharacter(const QChar& character) {
  CharacterInfo c;
  c.m_position = m_characters.size();
  c.m_group = NONE;
  c.m_character = character;
  m_characters << c;
  m_groups[c.m_group].positions << c.m_position;
}

void CDateTimeEditor::addGroup(CDateTimeEditor::Group group, int minValue, int maxValue, const QChar& placeholder) {
  GroupInfo g;
  g.group = group;
  g.value = -1;
  g.invalid = true;
  g.minValue = minValue;
  g.maxValue = maxValue;
  g.placeholder = placeholder;
  m_groups.insert(group, g);
}

int CDateTimeEditor::getNearestEditablePosition(const QPoint& pos) const {
  int minDist = -1;
  int bestPosition = -1;

  for (const CharacterInfo& c : m_characters) {
    if (c.isEditable()) {
      const QRect r = getCharacterRect(c.m_position);
      int dist = -1;
      if (r.contains(pos)) {
        dist = 0;
      } else {
        if (pos.x() <= r.left()) {
          const int d = r.left() - pos.x();
          if (dist < 0 || d < dist) {
            dist = d;
          }
        } else if (pos.x() >= r.right()) {
          const int d = pos.x() - r.right();
          if (dist < 0 || d < dist) {
            dist = d;
          }
        }
        if (pos.y() <= r.top()) {
          const int d = r.top() - pos.y();
          if (dist < 0 || d < dist) {
            dist = d;
          }
        } else if (pos.y() >= r.bottom()) {
          const int d = pos.y() - r.bottom();
          if (dist < 0 || d < dist) {
            dist = d;
          }
        }
      }

      if (minDist < 0 || dist < minDist) {
        minDist = dist;
        bestPosition = c.m_position;
      }
    }
  }

  return bestPosition;
}

bool CDateTimeEditor::selectFirstPosition() {
  bool found = false;

  m_selectedPosition = -1;
  m_selectedGroup = NONE;

  for (const CharacterInfo& c : std::as_const(m_characters)) {
    if (c.isEditable()) {
      found = true;
      m_selectedPosition = c.m_position;
      m_selectedGroup = c.m_group;
      break;
    }
  }

  update();

  return found;
}

bool CDateTimeEditor::selectPrevPosition() {
  bool found = false;

  if (m_selectedPosition >= 0) {
    for (int i = m_selectedPosition - 1; i >= 0; --i) {
      const CharacterInfo& c = m_characters[i];
      if (c.isEditable()) {
        found = true;
        m_selectedPosition = i;
        m_selectedGroup = c.m_group;
        break;
      }
    }
  } else {
    found = selectFirstPosition();
  }

  update();

  return found;
}

bool CDateTimeEditor::selectNextPosition() {
  bool found = false;
  if (m_selectedPosition >= 0) {
    for (int i = m_selectedPosition + 1; i < m_characters.size(); ++i) {
      const CharacterInfo& c = m_characters[i];
      if (c.isEditable()) {
        found = true;
        m_selectedPosition = i;
        m_selectedGroup = c.m_group;
        break;
      }
    }
  } else {
    found = selectFirstPosition();
  }

  update();

  return found;
}

void CDateTimeEditor::editSelectedCharacterDelta(int delta) {
  if (m_selectedPosition < 0 || m_selectedGroup == NONE) {
    return;
  }

  GroupInfo& g = m_groups[(int)m_selectedGroup];
  int value = getNumber(m_selectedGroup);

  if (value < 0) {
    setNumber(m_selectedGroup, g.minValue);
  } else {
    const int valuation = g.valuation(m_selectedPosition);
    value += delta * valuation;
    value = qMax(value, g.minValue);
    value = qMin(value, g.maxValue);
    setNumber(m_selectedGroup, value);
  }

  validate();
  update();
}

bool CDateTimeEditor::editSelectedCharacter(const QString& keyText) {
  if (m_selectedPosition < 0 || keyText.size() != 1) {
    return false;
  }

  CharacterInfo& c = m_characters[m_selectedPosition];
  const int index = c.m_availableCharacters.indexOf(keyText[0]);
  if (index < 0) {
    return false;
  }

  c.m_character = keyText[0];

  validate();
  update();

  return true;
}

void CDateTimeEditor::validate() {
  for (GroupInfo& g : m_groups) {
    g.invalid = false;
  }

  int hours = getNumber(HOUR);
  int minutes = getNumber(MINUTE);
  int seconds = getNumber(SECOND);
  if (hours < 0 || hours > 23) {
    hours = -1;
    markInvalid(HOUR);
  }
  if (minutes < 0 || minutes > 59) {
    minutes = -1;
    markInvalid(MINUTE);
  }
  if (seconds < 0 || seconds > 59) {
    seconds = -1;
    markInvalid(SECOND);
  }

  int years = getNumber(YEAR);
  int months = getNumber(MONTH);
  int days = getNumber(DAY);

  if (years < 0 || years == 0) {
    years = -1;
    markInvalid(YEAR);
  }
  if (months <= 0 || months > 12) {
    months = -1;
    markInvalid(MONTH);
  }
  if (days <= 0 || days > 31) {
    days = -1;
    markInvalid(DAY);
  }
  if (months != -1 && days != -1) {
    if ((months == 2) && days > 29) {
      days = -1;
      months = -1;
    } else if ((months == 4 || months == 6 || months == 9 || months == 11) && days > 30) {
      days = -1;
      months = -1;
    }

    if (days == -1) {
      markInvalid(DAY);
      markInvalid(MONTH);
    }
  }

  if (years != -1 && months != -1 && days != -1) {
    if (!QDate(years, months, days).isValid()) {
      years = -1;
      months = -1;
      days = -1;
      markInvalid(YEAR);
      markInvalid(MONTH);
      markInvalid(DAY);
    }
  }

  m_groups[YEAR].setValue(years);
  m_groups[MONTH].setValue(months);
  m_groups[DAY].setValue(days);
  m_groups[HOUR].setValue(hours);
  m_groups[MINUTE].setValue(minutes);
  m_groups[SECOND].setValue(seconds);

  if (years != -1 && months != -1 && days != -1 && hours != -1 && minutes != -1 && seconds != -1) {
    m_dateTime = QDateTime(QDate(years, months, days), QTime(hours, minutes, seconds), m_dateTime.timeZone());
  } else {
    m_dateTime = QDateTime();
  }
  emit dateTimeChanged(m_dateTime);

  update();
}

void CDateTimeEditor::setNumber(Group group, int value) {
  const auto& g = m_groups[group];

  auto positions = g.positions;
  if (value < 0) {
    for (auto p : positions) {
      CharacterInfo& c = m_characters[p];
      c.m_character = g.placeholder;
    }
  } else {
    QString s = QString::number(value);
    while (s.size() < positions.size()) {
      s.push_front('0');
    }
    while (s.size() > positions.size()) {
      s.remove(0, 1);
    }

    for (int i = 0; i != positions.size(); ++i) {
      m_characters[positions[i]].m_character = s[i];
    }
  }

  m_groups[group].setValue(getNumber(group));
}

int CDateTimeEditor::getNumber(Group group) const {
  const auto& g = m_groups[group];
  int value = 0;
  for (auto p : g.positions) {
    const CharacterInfo& c = m_characters[p];
    if (c.m_character.isDigit()) {
      value = 10 * value + c.m_character.digitValue();
    } else {
      return -1;
    }
  }
  return value;
}

void CDateTimeEditor::markInvalid(Group group) { m_groups[group].invalid = true; }

void CDateTimeEditor::initStyleOption(QStyleOptionFrame* option) const {
  if (!option) {
    return;
  }

  option->initFrom(this);
  option->rect = contentsRect();
  option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
  option->midLineWidth = 0;
  option->state |= QStyle::State_Sunken;
}
