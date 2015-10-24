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

#ifndef ISCROPT_H
#define ISCROPT_H

#include <QPixmap>
#include <QPointer>
#include <QRect>
#include <QWidget>

class QMouseEvent;
class IMouse;

#define SCR_OPT_OFFSET 15

class IScrOpt : public QWidget
{
public:
    IScrOpt(IMouse *mouse);
    virtual ~IScrOpt();


    void setOrigin(const QPoint& pos)
    {
        origin = pos;
    }
    const QPoint& getOrigin()
    {
        return origin;
    }

    virtual void draw(QPainter& p) = 0;
    void mouseMoveEvent(QMouseEvent *);

protected:
    void enterEvent(QEvent * e);
    void leaveEvent(QEvent * e);

    void drawBubble2(const QPointF &pt, QPainter& p);

    QPoint origin;
    QPoint mousePos;

    QPointer<IMouse> mouse;
};

#endif //ISCROPT_H

