/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "CTextEditWidget.h"

#include <QtWidgets>

CTextEditWidget::CTextEditWidget(QWidget * parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(actionTextBold,      &QAction::triggered, this, &CTextEditWidget::textBold);
    connect(actionTextItalic,    &QAction::triggered, this, &CTextEditWidget::textItalic);
    connect(actionTextUnderline, &QAction::triggered, this, &CTextEditWidget::textUnderline);
    toolBold->setDefaultAction  (actionTextBold);
    toolItalic->setDefaultAction(actionTextItalic);
    toolUnder->setDefaultAction (actionTextUnderline);

    QActionGroup *grp = new QActionGroup(this);
    grp->addAction(actionAlignLeft);
    grp->addAction(actionAlignRight);
    grp->addAction(actionAlignCenter);
    grp->addAction(actionAlignJustify);
    connect(grp, &QActionGroup::triggered, this, &CTextEditWidget::textAlign);

    toolLeft->setDefaultAction(actionAlignLeft);
    toolCenter->setDefaultAction(actionAlignCenter);
    toolRight->setDefaultAction(actionAlignRight);
    toolBlock->setDefaultAction(actionAlignJustify);


    QPixmap pix(24, 24);
    pix.fill(Qt::black);
    actionTextColor = new QAction(pix, tr("&Color..."), this);
    connect(actionTextColor, &QAction::triggered, this, &CTextEditWidget::textColor);
    toolColor->setDefaultAction(actionTextColor);

    comboStyle->addItem("standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    connect(comboStyle, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CTextEditWidget::textStyle);

    connect(textEdit, &QTextEdit::currentCharFormatChanged, this, &CTextEditWidget::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,    this, &CTextEditWidget::cursorPositionChanged);
    connect(textEdit, &QTextEdit::selectionChanged,         this, &CTextEditWidget::selectionChanged);

    textEdit->setFocus();

    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    toolUndo->setDefaultAction(actionUndo);
    toolRedo->setDefaultAction(actionRedo);
    toolCut->setDefaultAction(actionCut);
    toolCopy->setDefaultAction(actionCopy);
    toolPaste->setDefaultAction(actionPaste);

    QMenu *menu = new QMenu(this);
    menu->addAction(actionPastePlain);
    toolPaste->setMenu(menu);

    /* Setup contextmenu for textEdit */
    menuTextEdit = new QMenu(this);
    menuTextEdit->addAction(actionUndo);
    menuTextEdit->addAction(actionRedo);
    menuTextEdit->addSeparator();
    menuTextEdit->addAction(actionCut);
    menuTextEdit->addAction(actionCopy);
    menuTextEdit->addAction(actionPaste);
    menuTextEdit->addAction(actionPastePlain);
    menuTextEdit->addAction(actionDelete);
    menuTextEdit->addSeparator();
    menuTextEdit->addAction(actionSelectAll);

    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
    actionPastePlain->setEnabled(!QApplication::clipboard()->text().isEmpty());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

    connect(textEdit->document(), &QTextDocument::undoAvailable, actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable, actionRedo, &QAction::setEnabled);

    connect(actionUndo, &QAction::triggered, textEdit, &QTextEdit::undo);
    connect(actionRedo, &QAction::triggered, textEdit, &QTextEdit::redo);

    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);

    connect(actionCut,       &QAction::triggered, textEdit, &QTextEdit::cut);
    connect(actionCopy,      &QAction::triggered, textEdit, &QTextEdit::copy);
    connect(actionPaste,     &QAction::triggered, textEdit, &QTextEdit::paste);
    connect(actionSelectAll, &QAction::triggered, textEdit, &QTextEdit::selectAll);

    connect(actionPastePlain, &QAction::triggered, textEdit, [this]() {
        QClipboard *clip = QApplication::clipboard();
        textEdit->insertPlainText( clip->text() );
    });

    connect(actionDelete, &QAction::triggered, textEdit, [this]() {
        textEdit->insertPlainText(QString());
    });

    connect(textEdit, &QTextEdit::customContextMenuRequested, textEdit, [this]() {
        menuTextEdit->exec(QCursor::pos());
    });

    connect(textEdit,                  &QTextEdit::copyAvailable, actionCut,  &QAction::setEnabled);
    connect(textEdit,                  &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    connect(QApplication::clipboard(), &QClipboard::dataChanged,  this,       &CTextEditWidget::clipboardDataChanged);
}

QString CTextEditWidget::getHtml()
{
    QString str = textEdit->toHtml();
    QRegExp re(".*(\\<body.*body\\>).*");
    if(re.exactMatch(str))
    {
        str = re.cap(1);

        QRegExp re1("<body.*>");
        re1.setMinimal(true);
        str = str.replace("body>","div>").replace(re1,"<div>");
    }

    return str;
}

void CTextEditWidget::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}


void CTextEditWidget::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}


void CTextEditWidget::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}


void CTextEditWidget::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
    {
        textEdit->setAlignment(Qt::AlignLeft);
    }
    else if (a == actionAlignCenter)
    {
        textEdit->setAlignment(Qt::AlignHCenter);
    }
    else if (a == actionAlignRight)
    {
        textEdit->setAlignment(Qt::AlignRight);
    }
    else if (a == actionAlignJustify)
    {
        textEdit->setAlignment(Qt::AlignJustify);
    }
}



void CTextEditWidget::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();

    if (styleIndex != 0)
    {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex)
        {
        default:
        case 1:
            style = QTextListFormat::ListDisc;
            break;

        case 2:
            style = QTextListFormat::ListCircle;
            break;

        case 3:
            style = QTextListFormat::ListSquare;
            break;

        case 4:
            style = QTextListFormat::ListDecimal;
            break;

        case 5:
            style = QTextListFormat::ListLowerAlpha;
            break;

        case 6:
            style = QTextListFormat::ListUpperAlpha;
            break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList())
        {
            listFmt = cursor.currentList()->format();
        }
        else
        {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    }
    else
    {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}


void CTextEditWidget::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
    {
        return;
    }
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}


void CTextEditWidget::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
    {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}


void CTextEditWidget::fontChanged(const QFont &f)
{
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}


void CTextEditWidget::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}


void CTextEditWidget::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
    {
        actionAlignLeft->setChecked(true);
    }
    else if (a & Qt::AlignHCenter)
    {
        actionAlignCenter->setChecked(true);
    }
    else if (a & Qt::AlignRight)
    {
        actionAlignRight->setChecked(true);
    }
    else if (a & Qt::AlignJustify)
    {
        actionAlignJustify->setChecked(true);
    }
}


void CTextEditWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}


void CTextEditWidget::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
}


void CTextEditWidget::clipboardDataChanged()
{
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
    actionPastePlain->setEnabled(!QApplication::clipboard()->text().isEmpty());
}


void CTextEditWidget::selectionChanged()
{
    const QTextCursor cursor = textEdit->textCursor();
    actionDelete->setEnabled(cursor.selectionStart() != cursor.selectionEnd());
}
