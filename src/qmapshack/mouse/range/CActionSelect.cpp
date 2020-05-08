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

#include "mouse/range/CActionSelect.h"

CActionSelect::CActionSelect(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    buttons = {radioButton1, radioButton2, radioButton3, radioButton4};
    radioButton1->setProperty("action", int(CScrOptRangeTool::eAction::Nothing));
    radioButton2->setProperty("action", int(CScrOptRangeTool::eAction::Reset));
    radioButton3->setProperty("action", int(CScrOptRangeTool::eAction::ToEnd));
    radioButton4->setProperty("action", int(CScrOptRangeTool::eAction::ToStart));
}

void CActionSelect::setAction(CScrOptRangeTool::eAction action)
{
    buttons[int(action)]->setChecked(true);
}

CScrOptRangeTool::eAction CActionSelect::getAction() const
{
    for(QRadioButton * button : buttons)
    {
        if(button->isChecked())
        {
            return CScrOptRangeTool::eAction(button->property("action").toInt());
        }
    }
    return CScrOptRangeTool::eAction::Default;
}
