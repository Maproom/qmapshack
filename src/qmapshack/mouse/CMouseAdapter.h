#ifndef CMOUSEADAPTER_H
#define CMOUSEADAPTER_H

#include <QObject>
#include <QPoint>
#include <QTime>
#include <QPointer>

class CCanvas;
class QMouseEvent;
class QWheelEvent;
class QPinchGesture;
class QKeyEvent;
class IMouse;

class CMouseAdapter : public QObject
{
    Q_OBJECT
public:
    explicit CMouseAdapter(CCanvas *canvas);
    virtual ~CMouseAdapter();

    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void pinchGestureEvent(QPinchGesture *e);
    void afterMouseLostEvent(QMouseEvent *e);

    void startMouseMove(const QPoint& pos);

    void setDelegate(IMouse * delegate);

    QPoint getPoint() { return lastPos; }

    operator const QCursor&() const;

    const static int longButtonPressTimeout = 400;
    const static int minimalMouseMovingDistance = 4;

private:

    CCanvas * canvas { nullptr };
    IMouse * delegate { nullptr };

    QTime buttonPressTime;

    QPoint lastPos;
    QPoint firstPos;

    bool ignoreClick = false;
    bool mouseDown = false;
    bool mouseDidMove = false;

};

#endif // CMOUSEADAPTER_H
