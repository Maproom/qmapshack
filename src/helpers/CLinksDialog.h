/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CLINKSDIALOG_H
#define CLINKSDIALOG_H

#include "gis/IGisItem.h"
#include "ui_ILinksDialog.h"
#include <QDialog>

class CLinksDialog : public QDialog, private Ui::ILinksDialog
{
    Q_OBJECT
public:
    CLinksDialog(QList<IGisItem::link_t>& links, QWidget * parent);
    virtual ~CLinksDialog();

public slots:
    void accept();

private slots:
    void slotAddLink();
    void slotDelLink();
    void slotItemSelectionChanged();

private:
    QList<IGisItem::link_t>& links;
};

#endif //CLINKSDIALOG_H

