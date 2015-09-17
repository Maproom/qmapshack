/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CSELECTACTIVITY_H
#define CSELECTACTIVITY_H

#include "ui_ISelectActivity.h"
#include <QDialog>

class CSelectActivity : public QDialog, private Ui::ISelectActivity
{
    Q_OBJECT
public:
    CSelectActivity(quint32& flag, QString& name, QString& icon, QWidget * parent);
    virtual ~CSelectActivity();


private slots:
    void slotActivitySelected(bool);

private:
    quint32& flag;
    QString& name;
    QString& icon;
};

#endif //CSELECTACTIVITY_H

