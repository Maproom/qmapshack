/*******************************************************************************
 * Copyright (c) 2015 Florian Pigorsch <mail@florian-pigorsch.de>               *
 *                                                                              *
 * Permission is hereby granted, free of charge, to any person obtaining a copy *
 * of this software and associated documentation files (the "Software"), to deal*
 * in the Software without restriction, including without limitation the rights *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    *
 * copies of the Software, and to permit persons to whom the Software is        *
 * furnished to do so, subject to the following conditions:                     *
 *                                                                              *
 * The above copyright notice and this permission notice shall be included in   *
 * all copies or substantial portions of the Software.                          *
 *                                                                              *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,*
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    *
 * THE SOFTWARE.                                                                *
 *******************************************************************************/

#ifndef QTDATETIMEEDITOR_H
#define QTDATETIMEEDITOR_H

#include <QDateTime>
#include <QList>
#include <QMap>
#include <QWidget>
class QStyleOptionFrame;

class CDateTimeEditor : public QWidget {
  Q_OBJECT

 public:
  CDateTimeEditor(QWidget* parent = nullptr);

  const QDateTime& getDateTime() const { return m_dateTime; }
  void setDateTime(const QDateTime& dateTime);

  virtual QSize sizeHint() const override;
  virtual QSize minimumSizeHint() const override;

 signals:
  void dateTimeChanged(const QDateTime&);

 protected:
  virtual void paintEvent(QPaintEvent* event) override;
  virtual void mousePressEvent(QMouseEvent* event) override;
  virtual void wheelEvent(QWheelEvent* event) override;
  virtual void keyPressEvent(QKeyEvent* event) override;
  virtual void focusInEvent(QFocusEvent* event) override;
  virtual void focusOutEvent(QFocusEvent* event) override;
  virtual bool focusNextPrevChild(bool next) override;

 private:
  enum Group { NONE = 0, YEAR, MONTH, DAY, HOUR, MINUTE, SECOND };

  const QSize& getCharSize() const;
  QRect getCharacterRect(int position) const;
  void addCharacter(Group group, const QString& availableCharacters);
  void addFixedCharacter(const QChar& character);
  void addGroup(Group group, int minValue, int maxValue, const QChar& placeholder);

  int getNearestEditablePosition(const QPoint& pos) const;
  bool selectFirstPosition();
  bool selectPrevPosition();
  bool selectNextPosition();
  void editSelectedCharacterDelta(int delta);
  bool editSelectedCharacter(const QString& keyText);
  void validate();

  void setNumber(Group group, int value);
  int getNumber(Group group) const;
  void markInvalid(Group group);

  void initStyleOption(QStyleOptionFrame* option) const;

 private:
  QDateTime m_dateTime;
  mutable QFont m_font;
  mutable QSize m_charSize;

  int m_selectedPosition = -1;
  struct CharacterInfo {
    int m_position;
    Group m_group;
    QChar m_character;
    QString m_availableCharacters;
    bool isEditable() const { return !m_availableCharacters.isEmpty(); }
  };

  Group m_selectedGroup = NONE;
  struct GroupInfo {
    Group group;
    QList<int> positions;
    bool invalid;
    int value;
    int minValue;
    int maxValue;
    QChar placeholder;

    void setValue(int v) {
      value = v;
      invalid = (value < minValue || value > maxValue);
    }

    int valuation(int pos) {
      int v = 1;
      for (int i = positions.size() - 1; i >= 0; --i) {
        if (positions[i] == pos) {
          return v;
        }

        v *= 10;
      }
      return 0;
    }
  };

  QList<CharacterInfo> m_characters;
  QMap<int, GroupInfo> m_groups;
};

#endif
