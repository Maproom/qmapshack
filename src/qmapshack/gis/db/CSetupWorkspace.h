/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2020 Henri Hornburg hrnbg@t-online.de


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

#ifndef CSETUPWORKSPACE_H
#define CSETUPWORKSPACE_H

#include "ui_ISetupWorkspace.h"
#include <QDialog>

class CGisWorkspace;

class CSetupWorkspace : public QDialog, private Ui::ISetupWorkspace
{
    Q_OBJECT
public:
    CSetupWorkspace(CGisWorkspace *workspace, QWidget * parent);
    virtual ~CSetupWorkspace();

public slots:
    void accept() override;

private:
    CGisWorkspace * workspace;
};

#endif //CSETUPWORKSPACE_H

