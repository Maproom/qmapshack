/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CEXPORTDATABASETHREAD_H
#define CEXPORTDATABASETHREAD_H

#include <QMutex>
#include <QSqlDatabase>
#include <QThread>

class CExportDatabaseThread : public QThread
{
    Q_OBJECT
public:
    CExportDatabaseThread(quint64 id, QSqlDatabase& db, QObject * parent);
    virtual ~CExportDatabaseThread() = default;

    void start(const QString& path, bool saveAsGpx11);

public slots:
    void slotAbort();

signals:
    void sigOut(const QString& msg);
    void sigErr(const QString& msg);

protected:
    void run() override;
    bool getKeepGoing() const;
    void dumpFolder(quint64 id, const QString &parentName, const QString& path, QSqlDatabase& db);

private:
    QString simplifyString(const QString &str) const;

    mutable QMutex mutex;
    bool keepGoing = false;

    quint64 parentFolderId;
    /// database connection from the main thread
    QSqlDatabase& dbParent;
    QString exportPath;
    bool asGpx11 = false;
};

#endif //CEXPORTDATABASETHREAD_H

