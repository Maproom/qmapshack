/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CGISDATABASE_H
#define CGISDATABASE_H

#include "ui_IGisDatabase.h"

class IDBFolder;

class CGisDatabase : public QWidget, private Ui::IGisDatabase
{
    Q_OBJECT
public:
    static CGisDatabase& self()
    {
        return *pSelf;
    }

    virtual ~CGisDatabase();

    void postEventForDb(QEvent * event);
    void sendEventForDb(QEvent * event);

    void createDatabase();

private slots:
    void slotHelpText();

private:
    friend class CMainWindow;
    CGisDatabase(QWidget * parent);

    static CGisDatabase * pSelf;
};

#endif //CGISDATABASE_H

