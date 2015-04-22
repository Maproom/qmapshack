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

#ifndef CSETUPFOLDER_H
#define CSETUPFOLDER_H

#include "gis/db/IDBFolder.h"
#include "ui_ISetupFolder.h"
#include <QDialog>

class CSetupFolder : public QDialog, private Ui::ISetupFolder
{
    Q_OBJECT
public:
    CSetupFolder(IDBFolder::type_e& type, QString& name, bool groupAllowed, QWidget * parent);
    virtual ~CSetupFolder();

public slots:
    void accept();

private slots:
    void slotNameChanged(const QString& text);

private:
    IDBFolder::type_e& type;
    QString& name;
};

#endif //CSETUPFOLDER_H

