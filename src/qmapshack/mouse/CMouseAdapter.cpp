#include "canvas/CCanvas.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/CMouseEditArea.h"
#include "mouse/CMouseEditRte.h"
#include "mouse/CMouseEditTrk.h"
#include "mouse/CMouseMoveWpt.h"
#include "mouse/CMouseNormal.h"
#include "mouse/CMousePrint.h"
#include "mouse/CMouseRadiusWpt.h"
#include "mouse/CMouseRangeTrk.h"
#include "mouse/IMouse.h"
#include <QMouseEvent>
#include <QPinchGesture>

CMouseAdapter::CMouseAdapter(CCanvas *canvas) : QObject(canvas),
    canvas(canvas)
{
}

CMouseAdapter::~CMouseAdapter()
{
}

void CMouseAdapter::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
    delegate->draw(p, needsRedraw, rect);
}

void CMouseAdapter::mousePressEvent(QMouseEvent *e)
{
    // set firstPos and mouseDidMove to suppress small movements when clicking
    startMouseMove(e->pos());
    if(e->button() == Qt::LeftButton)
    {
        lastPos    = firstPos;
        mouseDown  = true;
        delegate->leftButtonDown(firstPos);
    }
    else if (e->button() == Qt::RightButton)
    {
        delegate->rightButtonDown(firstPos);
    }
    // make sure a click is actually shorter than longButtonPressTimeout
    buttonPressTime.start();
    ignoreClick = false;
}

void CMouseAdapter::mouseMoveEvent(QMouseEvent *e)
{
    const QPoint& pos = e->pos();

    // do not take the mouse as moving unless it has been moved
    // by significant distance away from starting point.
    // this helps doing clicks with the finger on a touchscreen
    // and suppresses routing triggered by very small movements.
    if (!mouseDidMove && (pos - firstPos).manhattanLength() >= minimalMouseMovingDistance)
    {
        mouseDidMove = true;
    }

    if (mouseDidMove)
    {
        if(mouseDown)
        {
            delegate->mouseDraged(firstPos, lastPos, pos);
        }
        else
        {
            delegate->mouseMoved(pos);
        }
    }

    lastPos = pos;
}

void CMouseAdapter::mouseReleaseEvent(QMouseEvent *e)
{
    if (mouseDown)
    {
        // suppress clicks when mouse was moved for more a few pixel
        if (mouseDidMove)
        {
            delegate->dragFinished(e->pos());
        }
        //suppress clicks when pressing too long or after zooming or display of CProgressDialog
        else if (!ignoreClick && buttonPressTime.elapsed() < longButtonPressTimeout)
        {
            delegate->leftClicked(e->pos());
        }
    }

    mouseDown    = false;
    mouseDidMove = false;
}

void CMouseAdapter::mouseDoubleClickEvent(QMouseEvent *e)
{
    delegate->doubleClicked(e->pos());
}

void CMouseAdapter::wheelEvent(QWheelEvent *e)
{
    // suppress little mouse-movements that are likely to happen when scrolling the mousewheel.
    startMouseMove(e->pos());
    if (e->buttons() != Qt::NoButton)
    {
        // no shortclick by releasing button right after scrolling the wheel
        ignoreClick = true;
    }
}

void CMouseAdapter::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Escape:
    {
        delegate->abortStep();
        break;
    }
    default:
    {
        delegate->scaleChanged();
    }
    }
}

void CMouseAdapter::pinchGestureEvent(QPinchGesture *e)
{
    // consider finger being down (equivalent to button pressed) during pinch
    mouseDown = true;
    // no shortclick by lifting the finger right after a pinch
    ignoreClick = true;
    // no on-the-fly-routing during pinch
    // timerRouting->stop();

    delegate->scaleChanged();
}

void CMouseAdapter::afterMouseLostEvent(QMouseEvent *e)
{
    // pinch or modal dialog interrupt tracking of mouse. As result the mouse
    // is at an arbitrary position.
    if (e->type() == QEvent::MouseMove)
    {
        // suppress jump of map when touching screen right afterwards
        lastPos    = e->pos();
        // consider the move starting at this position
        startMouseMove(e->pos());
    }
    mouseDown = e->buttons() & Qt::LeftButton;
}

void CMouseAdapter::startMouseMove(const QPoint& pos)
{
    // the mouse is not considered as moving
    // as long it has not been moved away from firstPos
    // by at least a few pixels.
    firstPos = pos;
    mouseDidMove = false;
}

void CMouseAdapter::setDelegate(IMouse *delegate)
{
    if (this->delegate != nullptr)
    {
        this->delegate->deleteLater();
    }
    this->delegate = delegate;

    if(canvas->underMouse())
    {
        const QString& src = QString("CMouseAdapter::setDelegate(%1)").arg(delegate->metaObject()->className());
        CCanvas::restoreOverrideCursor(src);
        CCanvas::setOverrideCursor(*delegate, src);
    }
}

CMouseAdapter::operator const QCursor&() const
{
    return *delegate;
}
