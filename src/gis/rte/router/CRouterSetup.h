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

#ifndef CROUTERSETUP_H
#define CROUTERSETUP_H

#include "ui_IRouterSetup.h"
#include "gis/IGisItem.h"
#include <QWidget>

class CRouterSetup : public QWidget, private Ui::IRouterSetup
{
    Q_OBJECT
public:    
    static CRouterSetup& self(){return *pSelf;}
    virtual ~CRouterSetup();

    void calcRoute(const IGisItem::key_t &key);

private slots:
    void slotSelectRouter(int i);

private:
    friend class Ui_IMainWindow;
    CRouterSetup(QWidget * parent);

    static CRouterSetup * pSelf;
};

#endif //CROUTERSETUP_H

