/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CGEOSEARCHWEBCONFIGDIALOG_H
#define CGEOSEARCHWEBCONFIGDIALOG_H

#include "gis/search/CGeoSearchWeb.h"
#include "ui_IGeoSearchWebConfigDialog.h"

class CGeoSearchWebConfigDialog : public QDialog, private Ui::IGeoSearchWebConfigDialog
{
    Q_OBJECT
public:
    CGeoSearchWebConfigDialog(QList<CGeoSearchWeb::service_t>& services, QWidget* parent);
    virtual ~CGeoSearchWebConfigDialog() = default;

public slots:
    void accept() override;

private slots:
    void slotAddNew();
    void slotDelSelected();
    void slotSelectionChanged();
    void slotReset();

private:
    void setupTreeWidget();
    QList<CGeoSearchWeb::service_t>& services;
};

#endif //CGEOSEARCHWEBCONFIGDIALOG_H

