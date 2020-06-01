/**********************************************************************************************
    Copyright (C) 2020 Henri Hornburg <hrnbg@t-online.de>

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

#ifndef CDATABASECONFLICT_H
#define CDATABASECONFLICT_H

#include "gis/db/CDBProject.h"
#include "ui_IResolveDatabaseConflict.h"
#include <QDialog>

class CResolveDatabaseConflict : public QDialog, private Ui::IResolveDatabaseConflict
{
public:
    CResolveDatabaseConflict(QString msg, IGisItem *item, CDBProject::action_e &actionForAll, QWidget *parent = nullptr);

    CDBProject::action_e getAction();

private slots:
    void slotButttonClicked(CDBProject::action_e actionConnected);

private:
    CDBProject::action_e actionSelected = CDBProject::eActionNone;
    CDBProject::action_e& actionForAll;
};

#endif // CDATABASECONFLICT_H
