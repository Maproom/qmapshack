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

#ifndef CSETUPDATABASE_H
#define CSETUPDATABASE_H

#include "ui_ISetupDatabase.h"
#include <QDialog>

class CGisListDB;

class CSetupDatabase : public QDialog, private Ui::ISetupDatabase
{
    Q_OBJECT
public:
    CSetupDatabase(CGisListDB& parent);
    virtual ~CSetupDatabase();

    bool isSqlite() const {return radioSqlite->isChecked(); }
    bool isMysql() const {return radioMysql->isChecked(); }
    bool noPasswd() const;

    QString getName() const {return lineName->text(); }
    QString getFilename() const {return labelFilename->text(); }
    QString getServer() const {return lineServer->text(); }
    QString getPort() const {return linePort->text(); }
    QString getUser() const {return lineUser->text(); }
    QString getPasswd() const {return linePasswd->text(); }


public slots:
    void accept() override;

private slots:
    void slotNewDB();
    void slotOpenDB();
    void slotUpdateButtonBox();

private:
    CGisListDB& list;
};

#endif //CSETUPDATABASE_H

