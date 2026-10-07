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

#include "CTextEditWidget.h"

#include <QtWidgets>

#include "helpers/CSettings.h"
#include "helpers/Signals.h"
#include "widgets/CTemplateWidget.h"
#include "widgets/CTextEditWidgetSelMenu.h"

/// edge length of the colour button's swatch, in logical pixels
static constexpr qint32 kSwatchSize = 16;

CTextEditWidget::CTextEditWidget(const QString& html, QWidget* parent) : QDialog(parent) {
  SETTINGS;

  bool pastePlain = cfg.value("TextEditWidget/pastePlain", false).toBool();

  setupUi(this);

  selectionWindow =
      new CTextEditWidgetSelMenu(this,
                                 /* font style actions */ actionTextBold, actionTextItalic, actionTextUnderline,
                                 /* copy/paste actions */ actionCut, actionCopy, actionPaste);

  QScrollBar* vbar = textEdit->verticalScrollBar();
  connect(vbar, &QAbstractSlider::valueChanged, this, &CTextEditWidget::slotTextEditScrolled);

  toolBold->setDefaultAction(actionTextBold);
  toolItalic->setDefaultAction(actionTextItalic);
  toolUnder->setDefaultAction(actionTextUnderline);

  connect(actionTextBold, &QAction::triggered, this, &CTextEditWidget::slotTextBold);
  connect(actionTextItalic, &QAction::triggered, this, &CTextEditWidget::slotTextItalic);
  connect(actionTextUnderline, &QAction::triggered, this, &CTextEditWidget::slotTextUnderline);

  QActionGroup* grp = new QActionGroup(this);
  grp->addAction(actionAlignLeft);
  grp->addAction(actionAlignRight);
  grp->addAction(actionAlignCenter);
  grp->addAction(actionAlignJustify);
  connect(grp, &QActionGroup::triggered, this, &CTextEditWidget::slotTextAlign);

  toolLeft->setDefaultAction(actionAlignLeft);
  toolCenter->setDefaultAction(actionAlignCenter);
  toolRight->setDefaultAction(actionAlignRight);
  toolBlock->setDefaultAction(actionAlignJustify);

  defaultFont = textEdit->font();

  actionTextColor = new QAction(tr("&Color..."), this);
  actionTextColor->setObjectName("actionTextColor");
  connect(actionTextColor, &QAction::triggered, this, &CTextEditWidget::slotTextColor);
  toolColor->setDefaultAction(actionTextColor);

  // Reset sits next to the button that sets the colour; a plain click still picks one.
  QMenu* menuColor = new QMenu(this);
  menuColor->addAction(actionTextColor);
  menuColor->addAction(actionResetColor);
  toolColor->setMenu(menuColor);
  toolColor->setPopupMode(QToolButton::MenuButtonPopup);

  connect(comboStyle, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this,
          &CTextEditWidget::slotTextStyle);

  connect(comboFont, &QFontComboBox::currentFontChanged, textEdit, &QTextEdit::setCurrentFont);
  connect(spinFontSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), textEdit,
          &QTextEdit::setFontPointSize);

  connect(textEdit, &QTextEdit::currentCharFormatChanged, this, &CTextEditWidget::slotCurrentCharFormatChanged);
  connect(textEdit, &QTextEdit::cursorPositionChanged, this, &CTextEditWidget::slotCursorPositionChanged);
  connect(textEdit, &QTextEdit::textChanged, this, &CTextEditWidget::slotCursorPositionChanged);
  connect(textEdit, &QTextEdit::selectionChanged, this, &CTextEditWidget::slotSelectionChanged);

  textEdit->setHtml(html);
  textEdit->setFocus();

  fontChanged(textEdit->font());
  colorChanged(explicitColor(textEdit->currentCharFormat()));
  alignmentChanged(textEdit->alignment());

  toolInsertFromTemplate->setDefaultAction(actionInsertFromTemplate);
  toolUndo->setDefaultAction(actionUndo);
  toolRedo->setDefaultAction(actionRedo);
  toolCut->setDefaultAction(actionCut);
  toolCopy->setDefaultAction(actionCopy);
  toolPaste->setDefaultAction(actionPaste);

  QMenu* menu = new QMenu(this);
  menu->addAction(actionPastePlain);
  menu->addAction(actionPasteNormal);
  toolPaste->setMenu(menu);

  /* Setup contextmenu for textEdit */
  menuTextEdit = new QMenu(this);
  menuTextEdit->addAction(actionInsertFromTemplate);
  menuTextEdit->addSeparator();
  menuTextEdit->addAction(actionUndo);
  menuTextEdit->addAction(actionRedo);
  menuTextEdit->addSeparator();
  menuTextEdit->addAction(actionCut);
  menuTextEdit->addAction(actionCopy);
  menuTextEdit->addAction(actionPaste);
  menuTextEdit->addAction(actionDelete);
  menuTextEdit->addSeparator();

  removeFormat = new QMenu(tr("Reset format"), this);
  {
    menuTextEdit->addMenu(removeFormat);
    removeFormat->addAction(actionResetFont);
    removeFormat->addAction(actionResetColor);
    removeFormat->addAction(actionResetLayout);
  }

  connect(actionResetFont, &QAction::triggered, this, &CTextEditWidget::slotResetFont);
  connect(actionResetColor, &QAction::triggered, this, &CTextEditWidget::slotResetColor);
  connect(actionResetLayout, &QAction::triggered, this, &CTextEditWidget::slotResetLayout);

  menuTextEdit->addAction(actionSelectAll);

  actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
  actionPastePlain->setEnabled(!QApplication::clipboard()->text().isEmpty());
  actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
  actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

  connect(textEdit->document(), &QTextDocument::undoAvailable, actionUndo, &QAction::setEnabled);
  connect(textEdit->document(), &QTextDocument::redoAvailable, actionRedo, &QAction::setEnabled);

  connect(actionInsertFromTemplate, &QAction::triggered, this, &CTextEditWidget::slotInsertFromTemplate);
  connect(actionUndo, &QAction::triggered, textEdit, &QTextEdit::undo);
  connect(actionRedo, &QAction::triggered, textEdit, &QTextEdit::redo);

  actionCut->setEnabled(false);
  actionCopy->setEnabled(false);

  QActionGroup* pasteGroup = new QActionGroup(this);
  actionPastePlain->setChecked(pastePlain);
  actionPasteNormal->setChecked(!pastePlain);
  pasteGroup->addAction(actionPastePlain);
  pasteGroup->addAction(actionPasteNormal);
  connect(pasteGroup, &QActionGroup::triggered, this, &CTextEditWidget::slotPasteMode);

  slotPasteMode(pastePlain ? actionPastePlain : actionPasteNormal);

  connect(actionCut, &QAction::triggered, textEdit, &QTextEdit::cut);
  connect(actionCopy, &QAction::triggered, textEdit, &QTextEdit::copy);
  connect(actionSelectAll, &QAction::triggered, textEdit, &QTextEdit::selectAll);
  connect(actionPaste, &QAction::triggered, textEdit, &CTextEdit::paste);
  connect(actionDelete, &QAction::triggered, this, &CTextEditWidget::slotDeleteSelected);
  connect(textEdit, &QTextEdit::customContextMenuRequested, this, &CTextEditWidget::slotCustomContextMenuRequested);
  connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
  connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

  connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &CTextEditWidget::slotClipboardDataChanged);
}

CTextEditWidget::~CTextEditWidget() {
  SETTINGS;
  cfg.setValue("TextEditWidget/pastePlain", actionPastePlain->isChecked());
}

QString CTextEditWidget::getHtml() {
  QString str = textEdit->toHtml();
  static const QRegularExpression re(QRegularExpression::anchoredPattern(".*(\\<body.*body\\>).*"));
  const QRegularExpressionMatch& match = re.match(str);
  if (match.hasMatch()) {
    str = match.captured(1);

    static const QRegularExpression re1("<body.*>", QRegularExpression::PatternOption::InvertedGreedinessOption);
    str = str.replace("body>", "div>").replace(re1, "<div>");
  }
  return str;
}

void CTextEditWidget::slotTextBold() {
  QTextCharFormat fmt;
  fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
  mergeFormatOnWordOrSelection(fmt);
}

void CTextEditWidget::slotTextUnderline() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(actionTextUnderline->isChecked());
  mergeFormatOnWordOrSelection(fmt);
}

void CTextEditWidget::slotTextItalic() {
  QTextCharFormat fmt;
  fmt.setFontItalic(actionTextItalic->isChecked());
  mergeFormatOnWordOrSelection(fmt);
}

void CTextEditWidget::slotTextAlign(QAction* a) {
  if (a == actionAlignLeft) {
    textEdit->setAlignment(Qt::AlignLeft);
  } else if (a == actionAlignCenter) {
    textEdit->setAlignment(Qt::AlignHCenter);
  } else if (a == actionAlignRight) {
    textEdit->setAlignment(Qt::AlignRight);
  } else if (a == actionAlignJustify) {
    textEdit->setAlignment(Qt::AlignJustify);
  }
}

void CTextEditWidget::slotTextStyle(int styleIndex) {
  if (styleIndex > 0) {
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListDisc;

    static QTextListFormat::Style indexToFormat[] = {QTextListFormat::ListDisc,       QTextListFormat::ListCircle,
                                                     QTextListFormat::ListSquare,     QTextListFormat::ListDecimal,
                                                     QTextListFormat::ListLowerAlpha, QTextListFormat::ListUpperAlpha,
                                                     QTextListFormat::ListLowerRoman, QTextListFormat::ListUpperRoman};

    if ((unsigned)styleIndex <= sizeof(indexToFormat) / sizeof(QTextListFormat::Style)) {
      style = indexToFormat[styleIndex - 1];
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    QTextListFormat listFmt;

    if (cursor.currentList()) {
      listFmt = cursor.currentList()->format();
    } else {
      listFmt.setIndent(blockFmt.indent() + 1);
      blockFmt.setIndent(0);
      cursor.setBlockFormat(blockFmt);
    }

    listFmt.setStyle(style);

    cursor.createList(listFmt);

    cursor.endEditBlock();
  } else {
    slotResetLayout();
  }
}

void CTextEditWidget::slotResetLayout() { textEdit->textCursor().setBlockFormat(QTextBlockFormat()); }

void CTextEditWidget::slotResetFont() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(false);
  fmt.setFontWeight(QFont::Normal);
  fmt.setFontItalic(false);
  // setCharFormat() replaces the format, so omitting setForeground() is what drops the colour.
  fmt.setFont(defaultFont);
  fmt.setFontPointSize(defaultFont.pointSizeF());

  QTextCursor cursor = textEdit->textCursor();
  if (!cursor.hasSelection()) {
    cursor.select(QTextCursor::WordUnderCursor);
  }
  cursor.setCharFormat(fmt);

  fontChanged(defaultFont);
  colorChanged(QColor());
}

void CTextEditWidget::slotResetColor() {
  // Same reach as slotResetFont() next to it in the menu, and as picking a colour.
  QTextCursor cursor = textEdit->textCursor();
  if (!cursor.hasSelection()) {
    cursor.select(QTextCursor::WordUnderCursor);
  }

  const qint32 start = cursor.selectionStart();
  const qint32 end = cursor.selectionEnd();

  QTextDocument* doc = textEdit->document();

  // Collect first, edit afterwards: clearing a fragment's colour can merge it with its
  // neighbours, which would invalidate the iterator mid-walk.
  QList<QPair<qint32, qint32>> ranges;
  for (QTextBlock block = doc->findBlock(start); block.isValid() && block.position() < end; block = block.next()) {
    for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
      const QTextFragment fragment = it.fragment();
      if (!fragment.isValid() || !fragment.charFormat().hasProperty(QTextFormat::ForegroundBrush)) {
        continue;
      }
      const qint32 from = qMax(fragment.position(), start);
      const qint32 to = qMin(fragment.position() + fragment.length(), end);
      if (from < to) {
        ranges << qMakePair(from, to);
      }
    }
  }

  // Replace, not merge - a merge cannot remove a property - and per run, or the runs get flattened.
  QTextCursor edit(doc);
  edit.beginEditBlock();
  for (const QPair<qint32, qint32>& range : std::as_const(ranges)) {
    edit.setPosition(range.first);
    edit.setPosition(range.second, QTextCursor::KeepAnchor);
    QTextCharFormat fmt = edit.charFormat();
    fmt.clearForeground();
    edit.setCharFormat(fmt);
  }
  edit.endEditBlock();

  colorChanged(QColor());
}

void CTextEditWidget::slotTextColor() {
  // Open on what the text renders in, which for an unset foreground is the palette colour.
  const QColor& current = explicitColor(textEdit->currentCharFormat());
  const QColor col =
      QColorDialog::getColor(current.isValid() ? current : textEdit->palette().color(QPalette::Text), this);
  if (!col.isValid()) {
    return;
  }

  QTextCharFormat fmt;
  fmt.setForeground(col);
  mergeFormatOnWordOrSelection(fmt);

  colorChanged(col);
}

void CTextEditWidget::mergeFormatOnWordOrSelection(const QTextCharFormat& format) {
  QTextCursor cursor = textEdit->textCursor();
  if (!cursor.hasSelection()) {
    cursor.select(QTextCursor::WordUnderCursor);
  }
  cursor.mergeCharFormat(format);
  textEdit->mergeCurrentCharFormat(format);
}

void CTextEditWidget::fontChanged(const QFont& f) {
  actionTextBold->setChecked(f.bold());
  actionTextItalic->setChecked(f.italic());
  actionTextUnderline->setChecked(f.underline());
}

QColor CTextEditWidget::explicitColor(const QTextCharFormat& fmt) {
  return fmt.hasProperty(QTextFormat::ForegroundBrush) ? fmt.foreground().color() : QColor();
}

void CTextEditWidget::colorChanged(const QColor& c) {
  // An unset foreground renders in the palette colour, not black.
  const QColor& color = c.isValid() ? c : textEdit->palette().color(QPalette::Text);

  const qreal dpr = devicePixelRatioF();
  QPixmap pix(QSize(kSwatchSize, kSwatchSize) * dpr);
  pix.setDevicePixelRatio(dpr);
  pix.fill(color);

  // Outline, so a swatch close to the toolbar colour is still a swatch.
  QPainter p(&pix);
  p.setPen(palette().color(QPalette::WindowText));
  p.drawRect(QRect(0, 0, kSwatchSize - 1, kSwatchSize - 1));

  actionTextColor->setIcon(pix);
}

void CTextEditWidget::alignmentChanged(Qt::Alignment a) {
  if (a & Qt::AlignLeft) {
    actionAlignLeft->setChecked(true);
  } else if (a & Qt::AlignHCenter) {
    actionAlignCenter->setChecked(true);
  } else if (a & Qt::AlignRight) {
    actionAlignRight->setChecked(true);
  } else if (a & Qt::AlignJustify) {
    actionAlignJustify->setChecked(true);
  }
}

void CTextEditWidget::slotCurrentCharFormatChanged(const QTextCharFormat& format) {
  fontChanged(format.font());
  colorChanged(explicitColor(format));
}

void CTextEditWidget::slotCursorPositionChanged() {
  static QHash<QTextListFormat::Style, int> styleToIndex(
      {std::make_pair(QTextListFormat::ListDisc, 1), std::make_pair(QTextListFormat::ListCircle, 2),
       std::make_pair(QTextListFormat::ListSquare, 3), std::make_pair(QTextListFormat::ListDecimal, 4),
       std::make_pair(QTextListFormat::ListLowerAlpha, 5), std::make_pair(QTextListFormat::ListUpperAlpha, 6),
       std::make_pair(QTextListFormat::ListLowerRoman, 7), std::make_pair(QTextListFormat::ListUpperRoman, 8)});

  alignmentChanged(textEdit->alignment());

  qint32 listStyleIndex = 0;

  QTextCursor cursor = textEdit->textCursor();
  if (cursor.currentList()) {
    QTextListFormat::Style style = cursor.currentList()->format().style();

    if (styleToIndex.contains(style)) {
      listStyleIndex = styleToIndex[cursor.currentList()->format().style()];
    }
  }

  X______________BlockAllSignals______________X(this);
  comboStyle->setCurrentIndex(listStyleIndex);

  const QFont& font = cursor.charFormat().font();
  comboFont->setCurrentFont(font);

  qint32 pointSize = font.pointSize();

  if (-1 == pointSize) {
    // some texts (if pasted from px. a browser) have their font size
    // specified in pixels instead of points, so we need to convert that
    QFontInfo info(font);
    pointSize = info.pointSize();
  }
  spinFontSize->setValue(pointSize);

  X_____________UnBlockAllSignals_____________X(this);
}

void CTextEditWidget::slotClipboardDataChanged() {
  actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
  actionPastePlain->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void CTextEditWidget::slotSelectionChanged() {
  bool hasSel = textEdit->textCursor().hasSelection();

  actionDelete->setEnabled(hasSel);
  removeFormat->setEnabled(hasSel);
  actionResetFont->setEnabled(hasSel);
  actionResetLayout->setEnabled(hasSel);

  updateSelectionWindow();
}

void CTextEditWidget::slotCustomContextMenuRequested() { menuTextEdit->exec(QCursor::pos()); }

void CTextEditWidget::slotDeleteSelected() { textEdit->insertPlainText(QString()); }

void CTextEditWidget::slotTextEditScrolled() { updateSelectionWindow(); }

void CTextEditWidget::moveEvent(QMoveEvent* event) { updateSelectionWindow(); }

void CTextEditWidget::slotPasteMode(QAction* action) {
  textEdit->setPastePlain(action == actionPastePlain);

  actionPaste->setIcon(action->icon());
}

bool CTextEditWidget::event(QEvent* event) {
  if (event->type() == QEvent::WindowActivate) {
    updateSelectionWindow();
  } else if (event->type() == QEvent::WindowDeactivate) {
    selectionWindow->hide();
  }
  return QDialog::event(event);
}

void CTextEditWidget::updateSelectionWindow() {
  const QTextCursor& cursor = textEdit->textCursor();
  const QRect& rect = textEdit->cursorRect();

  // don't show the selctionWindow, if there is no selection or
  // the cursor is not visible
  if (cursor.hasSelection() && rect.y() >= 0 && rect.y() <= textEdit->height()) {
    qint32 dy = cursor.anchor() < cursor.position() ? (6 + rect.height()) : (-6 - selectionWindow->height());

    qint32 dx = -selectionWindow->width() / 2;

    selectionWindow->move(textEdit->mapToGlobal(QPoint(rect.x(), rect.y())) + QPoint(dx, dy));
    selectionWindow->show();
  } else {
    selectionWindow->hide();
  }
}

void CTextEditWidget::slotInsertFromTemplate() {
  CTemplateWidget dlg(this);
  if (dlg.exec() == QDialog::Accepted) {
    textEdit->insertHtml(dlg.text());
  }
}
