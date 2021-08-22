/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CFadingIcon.h"
#include <QtWidgets>

CFadingIcon::CFadingIcon(const QPoint& pt, const QString& resource, QWidget* parent)
    : QLabel(parent)
    , icon(resource)
{
    setPixmap(icon);

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(100);
    timer->start();

    connect(timer, &QTimer::timeout, this, &CFadingIcon::slotTimeout);

    move(pt.x() - icon.width() / 2, pt.y() - icon.height() / 2);
    show();
}

CFadingIcon::~CFadingIcon()
{
}

void CFadingIcon::slotTimeout()
{
    o -= 0.1;
    if(o <= 0)
    {
        deleteLater();
    }
    else
    {
        QPixmap tmp(icon.size());
        tmp.fill(Qt::transparent);

        QPainter p(&tmp);
        p.setOpacity(o);
        p.drawPixmap(0, 0, icon);
        setPixmap(tmp);
    }
}
