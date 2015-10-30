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

#ifndef CFADINGICON_H
#define CFADINGICON_H

#include <QLabel>
#include <QPixmap>

class CFadingIcon : public QLabel
{
    Q_OBJECT
public:
    CFadingIcon(const QPoint &pt, const QString& resource, QWidget * parent);
    virtual ~CFadingIcon();

private slots:
    void slotTimeout();

private:
    qreal o;
    QPixmap icon;
};

#endif //CFADINGICON_H

