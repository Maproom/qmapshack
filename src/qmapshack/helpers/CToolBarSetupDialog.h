/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#ifndef CTOOLBARSETUPDIALOG_H
#define CTOOLBARSETUPDIALOG_H

#include "ui_IToolBarSetupDialog.h"

class CToolBarConfig;

class CToolBarSetupDialog : public QDialog, private Ui::IToolBarSetupDialog
{
    Q_OBJECT
public:
    CToolBarSetupDialog(QWidget* const& parent, CToolBarConfig* const& config);
    virtual ~CToolBarSetupDialog();

public slots:
    void accept() override;
    void slotButtonClicked(QAbstractButton* button) const;

private:
    class CDialogItem : public QListWidgetItem
    {
public:
        CDialogItem(QIcon icon, QString text, QString name)
            : QListWidgetItem(icon, text, nullptr, QListWidgetItem::UserType),
            actionName(name) {}
        ~CDialogItem() override {}
        QListWidgetItem* clone() const override { return new CDialogItem(this->icon(), this->text(), this->actionName); }
private:
        const QString actionName;
        friend class CToolBarSetupDialog;
    };

    class CItemFilter : public QObject, public CSelectDoubleListWidget::IItemFilter
    {
public:
        CItemFilter(QObject* parent) : QObject(parent) {}
        ~CItemFilter() override {}
        bool shouldBeMoved(QListWidgetItem* item) override;
    };

    void configure() const;
    CToolBarConfig* config;
};
#endif //CTOOLBARSETUPDIALOG_H
