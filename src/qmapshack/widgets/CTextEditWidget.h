/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler <code@christian-eichler.de>

    Copyright (C) 2012 Digia Plc and/or its subsidiaries <>
    Contact: http://www.qt-project.org/legal

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#ifndef CTEXTEDITWIDGET_H
#define CTEXTEDITWIDGET_H

#include <QDialog>
#include <QDir>
#include <QTextCharFormat>

#include "ui_ITextEditWidget.h"

class CTextEditWidget : public QDialog, private Ui::ITextEditWidget {
  Q_OBJECT
 public:
  CTextEditWidget(const QString& html, QWidget* parent);
  virtual ~CTextEditWidget();

  QString getHtml();

 protected:
  void moveEvent(QMoveEvent* event) override;
  bool event(QEvent* event) override;

 private slots:
  void slotTextBold();
  void slotTextUnderline();
  void slotTextItalic();
  void slotTextStyle(int styleIndex);
  void slotTextColor();
  void slotTextAlign(QAction* a);
  void slotResetFont();

  /// @brief Drop the text colour so it follows the palette again.
  void slotResetColor();
  void slotResetLayout();

  void slotCurrentCharFormatChanged(const QTextCharFormat& format);
  void slotCursorPositionChanged();
  void slotSelectionChanged();
  void slotClipboardDataChanged();

  void slotCustomContextMenuRequested();
  void slotPasteMode(QAction* action);
  void slotDeleteSelected();

  void slotTextEditScrolled();

  void slotInsertFromTemplate();

 private:
  /// @brief Apply @p format to the selection, or to the word under the cursor if there is none.
  void mergeFormatOnWordOrSelection(const QTextCharFormat& format);
  void fontChanged(const QFont& f);

  /// @brief The colour @p fmt sets explicitly, or an invalid colour when it sets none.
  static QColor explicitColor(const QTextCharFormat& fmt);

  /// @brief Update the colour button's swatch. An invalid @p c means "no colour set".
  void colorChanged(const QColor& c);
  void alignmentChanged(Qt::Alignment a);
  void updateSelectionWindow();

  QAction* actionTextColor;
  QMenu* menuTextEdit;
  QWidget* selectionWindow;
  QMenu* removeFormat;

  QFont defaultFont;
};
#endif  // CTEXTEDITWIDGET_H
