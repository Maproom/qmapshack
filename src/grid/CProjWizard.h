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

#ifndef CPROJWIZARD_H
#define CPROJWIZARD_H

#include "ui_IProjWizard.h"
#include <QDialog>

class CProjWizard : public QDialog, private Ui::IProjWizard
{
    Q_OBJECT
public:
    CProjWizard(QLineEdit& line);
    virtual ~CProjWizard();
    static bool validProjStr(const QString projStr);

public slots:
    virtual void accept() override;
    void slotChange();

private:
    void findDatum(const QString& str);

    QLineEdit& line;
};

#endif //CPROJWIZARD_H

