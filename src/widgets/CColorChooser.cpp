/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/IGisItem.h"
#include "widgets/CColorChooser.h"

#include <QtWidgets>
#include <functional>

using std::bind;

CColorChooser::CColorChooser(QToolButton *parent)
    : QDialog(parent)
    , parentButton(parent)
{
    setupUi(this);
    setWindowFlags(Qt::ToolTip);

    for(quint32 i = 0; i < IGisItem::colorMapSize; i++)
    {
        QPixmap pixmap(16,16);
        pixmap.fill(IGisItem::colorMap[i].color);

        QToolButton * button = new QToolButton(this);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setAutoRaise(true);
        button->setIcon(QIcon(pixmap));
        button->setProperty("color", IGisItem::colorMap[i].color.name());
        horizontalLayout->addWidget(button);

        auto selectFunc = bind(&CColorChooser::slotSelect, this, button, std::placeholders::_1);
        connect(button, &QToolButton::clicked, this, selectFunc);
    }

    adjustSize();
}

void CColorChooser::slotSelect(QToolButton * button, bool)
{
    parentButton->setIcon(button->icon());
    parentButton->setProperty("color", button->property("color"));
    accept();
}
