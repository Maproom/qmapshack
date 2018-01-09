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

#ifndef CSELECTDBFOLDER_H
#define CSELECTDBFOLDER_H

#include "ui_ISelectDBFolder.h"
#include <QDialog>

class CSelectDBFolder : public QDialog, private Ui::ISelectDBFolder
{
    Q_OBJECT
public:
    CSelectDBFolder(quint64& id, QString& db, QString& host, QWidget * parent);
    virtual ~CSelectDBFolder();

private slots:
    void slotItemExpanded(QTreeWidgetItem * item);
    void slotItemSelectionChanged();

private:
    quint64& id;
    QString& db;
    QString& host;
};

#endif //CSELECTDBFOLDER_H

