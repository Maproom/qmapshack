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

#ifndef CACTIONSELECT_H
#define CACTIONSELECT_H

#include "mouse/range/CScrOptRangeTool.h"
#include "ui_IActionSelect.h"

class CActionSelect : public QWidget, private Ui::IActionSelect
{
    Q_OBJECT
public:
    CActionSelect(QWidget * parent);
    virtual ~CActionSelect() = default;

    void setAction(CScrOptRangeTool::eAction action);
    CScrOptRangeTool::eAction getAction() const;

private:
    QVector<QRadioButton *> buttons;
};

#endif //CACTIONSELECT_H

