/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CMOUSEADAPTER_H
#define CMOUSEADAPTER_H

#include <QObject>
#include <QPoint>
#include <QPointer>
#include <QTime>

class CCanvas;
class QMouseEvent;
class QWheelEvent;
class QPinchGesture;
class QKeyEvent;
class QTimer;
class IMouse;

class CMouseAdapter : public QObject
{
    Q_OBJECT
public:
    explicit CMouseAdapter(CCanvas* canvas);
    virtual ~CMouseAdapter();

    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect& rect);

    void unfocus();

    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void pinchGestureEvent(QPinchGesture* e);
    void afterMouseLostEvent(QMouseEvent* e);

    void startMouseMove(const QPoint& pos);

    void setDelegate(IMouse* delegate);

    QPoint getPoint() { return lastPos; }

    operator const QCursor&() const;

    const static int clickTimeout = 400;
    const static int longButtonPressTimeout = 800;
    const static int minimalMouseMovingDistance = 4;

private slots:
    void slotLongPressTimeout();

private:

    CCanvas* canvas { nullptr };
    IMouse* delegate { nullptr };

    QTime buttonPressTime;
    QTimer* longPressTimer;

    QPoint lastPos;
    QPoint firstPos;

    bool ignoreClick = false;
    bool mouseDidMove = false;
};

#endif // CMOUSEADAPTER_H
