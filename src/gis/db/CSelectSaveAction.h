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

#ifndef CSELECTSAVEACTION_H
#define CSELECTSAVEACTION_H

#include "ui_ISelectSaveAction.h"
#include <QDialog>

class IGisItem;

class CSelectSaveAction : public QDialog, private Ui::ISelectSaveAction
{
    Q_OBJECT
public:
    CSelectSaveAction(const IGisItem * src, const IGisItem * tar, QWidget * parent);
    virtual ~CSelectSaveAction();

    enum result_e
    {
        eResultNone,
        eResultSave,
        eResultSkip,
    };

    result_e getResult()
    {
        return result;
    }
    bool allOthersToo();

private slots:
    void slotSelectResult();

private:
    result_e result = eResultNone;
};

#endif //CSELECTSAVEACTION_H

