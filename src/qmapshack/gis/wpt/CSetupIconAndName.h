/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSETUPICONANDNAME_H
#define CSETUPICONANDNAME_H

#include "ui_ISetupIconAndName.h"

class CSetupIconAndName : public QDialog, private Ui::ISetupIconAndName
{
    Q_OBJECT
public:
    CSetupIconAndName(QString& icon, QString& name, QWidget * parent);
    virtual ~CSetupIconAndName() = default;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void slotEditName(const QString& str);
    void slotChangeIcon();

private:
    void checkInput();
    QString &icon;
    QString &name;
};

#endif //CSETUPICONANDNAME_H

