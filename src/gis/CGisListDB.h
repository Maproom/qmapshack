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

#ifndef CGISLISTDB_H
#define CGISLISTDB_H

#include <QSqlDatabase>
#include <QTreeWidget>

struct action_t;
class QMenu;
class IDBFolderSql;
class QUdpSocket;

class CGisListDB : public QTreeWidget
{
    Q_OBJECT
public:
    CGisListDB(QWidget * parent);
    virtual ~CGisListDB();

    enum column_e
    {
        eColumnCheckbox = 0
        ,eColumnName = 1
        ,eColumnTime = 2
    };

    bool hasDatabase(const QString& name);
    virtual bool event(QEvent * e) override;

signals:
    void sigChanged();

private slots:
    void slotContextMenu(const QPoint& point);
    void slotAddFolder();
    void slotDelFolder();
    void slotDelLostFound();
    void slotDelLostFoundItem();
    void slotItemExpanded(QTreeWidgetItem * item);
    void slotItemChanged(QTreeWidgetItem * item, int column);
    void slotAddDatabase();
    void slotDelDatabase();
    void slotDelItem();
    void slotUpdateDatabase();

    void slotReadyRead();

private:
    friend class CGisListDBEditLock;

    IDBFolderSql *getDataBase(const QString& name, const QString& host);
    void saveDatabaseConfiguration();

    int isInternalEdit = 0;

    QMenu * menuNone;
    QAction * actionAddDatabase;

    QMenu * menuFolder;
    QAction * actionAddFolder;
    QAction * actionDelFolder;

    QMenu * menuDatabase;
    QAction * actionDelDatabase;
    QAction * actionUpdate;

    QMenu * menuItem;
    QAction * actionDelItem;

    QMenu * menuLostFound;
    QAction * actionDelLostFound;

    QMenu * menuLostFoundItem;
    QAction * actionDelLostFoundItem;

    QUdpSocket * socket;

    quint32 lastTan;
};

#endif //CGISLISTDB_H

