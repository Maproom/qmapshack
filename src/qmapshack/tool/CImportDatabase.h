/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CIMPORTDATABASE_H
#define CIMPORTDATABASE_H

#include "ui_IImportDatabase.h"
#include <QPointer>
#include <QWidget>

class CQlgtDb;

class CImportDatabase : public QWidget, private Ui::IImportDatabase
{
    Q_OBJECT
public:
    CImportDatabase(QWidget* parent);
    virtual ~CImportDatabase();

    void stdOut(const QString& str);
    void stdErr(const QString& str);

private slots:
    void slotSelectSource();
    void slotSelectTarget();
    void slotStart();

private:
    QPointer<CQlgtDb> dbQlgt;
};

#endif //CIMPORTDATABASE_H

