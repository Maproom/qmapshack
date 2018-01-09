/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CEXPORTDATABASE_H
#define CEXPORTDATABASE_H

#include <QDialog>
#include <QThread>
#include <ui_IExportDatabase.h>


class CExportDatabaseThread;

class CExportDatabase : public QDialog, private Ui::IExportDatabase
{
    Q_OBJECT
public:
    CExportDatabase(quint64 id, QSqlDatabase& db, QWidget * parent);
    virtual ~CExportDatabase();

protected:
    void closeEvent(QCloseEvent * e) override;

private slots:
    void slotStdout(const QString& str);
    void slotStderr(const QString& str);
    void slotSetPath();
    void slotStart();
    void slotStarted();
    void slotFinished();

private:
    CExportDatabaseThread * thread;
};

#endif //CEXPORTDATABASE_H

