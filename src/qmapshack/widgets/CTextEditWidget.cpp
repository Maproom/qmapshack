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

CTextEditWidget::CTextEditWidget(const QString& html, QWidget* parent) : QDialog(parent) {
  SETTINGS;

  bool pastePlain = cfg.value("TextEditWidget/pastePlain", false).toBool();

  setupUi(this);

  selectionWindow =
      new CTextEditWidgetSelMenu(this,
                                 /* font style actions */ actionTextBold, actionTextItalic, actionTextUnderline,
                                 /* copy/paste actions */ actionCut, actionCopy, actionPaste);

  QScrollBar* vbar = textEdit->verticalScrollBar();
  connect(vbar, &QAbstractSlider::valueChanged, this, &CTextEditWidget::textEditScrolled);

  toolBold->setDefaultAction(actionTextBold);
  toolItalic->setDefaultAction(actionTextItalic);
  toolUnder->setDefaultAction(actionTextUnderline);

  connect(actionTextBold, &QAction::triggered, this, &CTextEditWidget::textBold);
  connect(actionTextItalic, &QAction::triggered, this, &CTextEditWidget::textItalic);
  connect(actionTextUnderline, &QAction::triggered, this, &CTextEditWidget::textUnderline);

  QActionGroup* grp = new QActionGroup(this);
  grp->addAction(actionAlignLeft);
  grp->addAction(actionAlignRight);
  grp->addAction(actionAlignCenter);
  grp->addAction(actionAlignJustify);
  connect(grp, &QActionGroup::triggered, this, &CTextEditWidget::textAlign);

  toolLeft->setDefaultAction(actionAlignLeft);
  toolCenter->setDefaultAction(actionAlignCenter);
  toolRight->setDefaultAction(actionAlignRight);
  toolBlock->setDefaultAction(actionAlignJustify);

  defaultFont = textEdit->font();

  QPixmap pix(24, 24);
  pix.fill(Qt::black);
  actionTextColor = new QAction(pix, tr("&Color..."), this);
  connect(actionTextColor, &QAction::triggered, this, &CTextEditWidget::textColor);
  toolColor->setDefaultAction(actionTextColor);

  connect(comboStyle, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CTextEditWidget::textStyle);

  connect(comboFont, &QFontComboBox::currentFontChanged, textEdit, &QTextEdit::setCurrentFont);
  connect(spinFontSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), textEdit,
          &QTextEdit::setFontPointSize);

  connect(textEdit, &QTextEdit::currentCharFormatChanged, this, &CTextEditWidget::currentCharFormatChanged);
  connect(textEdit, &QTextEdit::cursorPositionChanged, this, &CTextEditWidget::cursorPositionChanged);
  connect(textEdit, &QTextEdit::textChanged, this, &CTextEditWidget::cursorPositionChanged);
  connect(textEdit, &QTextEdit::selectionChanged, this, &CTextEditWidget::selectionChanged);

  textEdit->setHtml(html);
  textEdit->setFocus();

  fontChanged(textEdit->font());
  colorChanged(textEdit->textColor());
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
    removeFormat->addAction(actionResetLayout);
  }

  connect(actionResetFont, &QAction::triggered, this, &CTextEditWidget::resetFont);
  connect(actionResetLayout, &QAction::triggered, this, &CTextEditWidget::resetLayout);

  menuTextEdit->addAction(actionSelectAll);

  actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
  actionPastePlain->setEnabled(!QApplication::clipboard()->text().isEmpty());
  actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
  actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

  connect(textEdit->document(), &QTextDocument::undoAvailable, actionUndo, &QAction::setEnabled);
  connect(textEdit->document(), &QTextDocument::redoAvailable, actionRedo, &QAction::setEnabled);

  connect(actionInsertFromTemplate, &QAction::triggered, this, &CTextEditWidget::insertFromTemplate);
  connect(actionUndo, &QAction::triggered, textEdit, &QTextEdit::undo);
  connect(actionRedo, &QAction::triggered, textEdit, &QTextEdit::redo);

  actionCut->setEnabled(false);
  actionCopy->setEnabled(false);

  QActionGroup* pasteGroup = new QActionGroup(this);
  actionPastePlain->setChecked(pastePlain);
  actionPasteNormal->setChecked(!pastePlain);
  pasteGroup->addAction(actionPastePlain);
  pasteGroup->addAction(actionPasteNormal);
  connect(pasteGroup, &QActionGroup::triggered, this, &CTextEditWidget::pasteMode);

  pasteMode(pastePlain ? actionPastePlain : actionPasteNormal);

  connect(actionCut, &QAction::triggered, textEdit, &QTextEdit::cut);
  connect(actionCopy, &QAction::triggered, textEdit, &QTextEdit::copy);
  connect(actionSelectAll, &QAction::triggered, textEdit, &QTextEdit::selectAll);
  connect(actionPaste, &QAction::triggered, textEdit, &CTextEdit::paste);
  connect(actionDelete, &QAction::triggered, this, &CTextEditWidget::deleteSelected);
  connect(textEdit, &QTextEdit::customContextMenuRequested, this, &CTextEditWidget::customContextMenuRequested);
  connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
  connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

  connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &CTextEditWidget::clipboardDataChanged);
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

void CTextEditWidget::textBold() {
  QTextCharFormat fmt;
  fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
  mergeFormatOnWordOrSelection(fmt);
}

void CTextEditWidget::textUnderline() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(actionTextUnderline->isChecked());
  mergeFormatOnWordOrSelection(fmt);
}

void CTextEditWidget::textItalic() {
  QTextCharFormat fmt;
  fmt.setFontItalic(actionTextItalic->isChecked());
  mergeFormatOnWordOrSelection(fmt);
}

void CTextEditWidget::textAlign(QAction* a) {
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

void CTextEditWidget::textStyle(int styleIndex) {
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
    resetLayout();
  }
}

void CTextEditWidget::resetLayout() { textEdit->textCursor().setBlockFormat(QTextBlockFormat()); }

void CTextEditWidget::resetFont() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(false);
  fmt.setFontWeight(QFont::Normal);
  fmt.setFontItalic(false);
  fmt.setForeground(QColor());

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

void CTextEditWidget::textColor() {
  QColor col = QColorDialog::getColor(textEdit->textColor(), this);
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

void CTextEditWidget::colorChanged(const QColor& c) {
  QPixmap pix(16, 16);
  pix.fill(c);
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

void CTextEditWidget::currentCharFormatChanged(const QTextCharFormat& format) {
  fontChanged(format.font());
  colorChanged(format.foreground().color());
}

void CTextEditWidget::cursorPositionChanged() {
  static QHash<QTextListFormat::Style, int> styleToIndex(
      {std::make_pair(QTextListFormat::ListDisc, 1), std::make_pair(QTextListFormat::ListCircle, 2),
       std::make_pair(QTextListFormat::ListSquare, 3), std::make_pair(QTextListFormat::ListDecimal, 4),
       std::make_pair(QTextListFormat::ListLowerAlpha, 5), std::make_pair(QTextListFormat::ListUpperAlpha, 6),
       std::make_pair(QTextListFormat::ListLowerRoman, 7), std::make_pair(QTextListFormat::ListUpperRoman, 8)});

  alignmentChanged(textEdit->alignment());

  int listStyleIndex = 0;

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

  int pointSize = font.pointSize();

  if (-1 == pointSize) {
    // some texts (if pasted from px. a browser) have their font size
    // specified in pixels instead of points, so we need to convert that
    QFontInfo info(font);
    pointSize = info.pointSize();
  }
  spinFontSize->setValue(pointSize);

  X_____________UnBlockAllSignals_____________X(this);
}

void CTextEditWidget::clipboardDataChanged() {
  actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
  actionPastePlain->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void CTextEditWidget::selectionChanged() {
  bool hasSel = textEdit->textCursor().hasSelection();

  actionDelete->setEnabled(hasSel);
  removeFormat->setEnabled(hasSel);
  actionResetFont->setEnabled(hasSel);
  actionResetLayout->setEnabled(hasSel);

  updateSelectionWindow();
}

void CTextEditWidget::customContextMenuRequested() { menuTextEdit->exec(QCursor::pos()); }

void CTextEditWidget::deleteSelected() { textEdit->insertPlainText(QString()); }

void CTextEditWidget::textEditScrolled() { updateSelectionWindow(); }

void CTextEditWidget::moveEvent(QMoveEvent* event) { updateSelectionWindow(); }

void CTextEditWidget::pasteMode(QAction* action) {
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
    int dy = cursor.anchor() < cursor.position() ? (6 + rect.height()) : (-6 - selectionWindow->height());

    int dx = -selectionWindow->width() / 2;

    selectionWindow->move(textEdit->mapToGlobal(QPoint(rect.x(), rect.y())) + QPoint(dx, dy));
    selectionWindow->show();
  } else {
    selectionWindow->hide();
  }
}

void CTextEditWidget::insertFromTemplate() {
  CTemplateWidget dlg(this);
  if (dlg.exec() == QDialog::Accepted) {
    textEdit->insertHtml(dlg.text());
  }
}
