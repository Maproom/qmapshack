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

#include "ui_ITextEditWidget.h"
#include <QDialog>
#include <QDir>
#include <QTextCharFormat>


class CTextEditWidget : public QDialog, private Ui::ITextEditWidget
{
    Q_OBJECT
public:
    CTextEditWidget(const QString& html, QWidget* parent);
    virtual ~CTextEditWidget();

    QString getHtml();

protected:
    void moveEvent(QMoveEvent* event) override;
    bool event(QEvent* event) override;

private slots:
    void textBold();
    void textUnderline();
    void textItalic();
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction* a);
    void resetFont();
    void resetLayout();

    void currentCharFormatChanged(const QTextCharFormat& format);
    void cursorPositionChanged();
    void selectionChanged();
    void clipboardDataChanged();

    void customContextMenuRequested();
    void pasteMode(QAction* action);
    void deleteSelected();

    void textEditScrolled();

    void insertFromTemplate();
private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat& format);
    void fontChanged(const QFont& f);
    void colorChanged(const QColor& c);
    void alignmentChanged(Qt::Alignment a);
    void updateSelectionWindow();

    QAction* actionTextColor;
    QMenu* menuTextEdit;
    QWidget* selectionWindow;
    QMenu* removeFormat;

    QFont defaultFont;
};
#endif // CTEXTEDITWIDGET_H
