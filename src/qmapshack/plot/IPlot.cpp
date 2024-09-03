/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2015 Christian Eichler <code@christian-eichler.de>

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

#include "plot/IPlot.h"

#include <QKeyEvent>
#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/trk/CActivityTrk.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "misc.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/range/CScrOptRangeTrk.h"
#include "plot/CPlotAxis.h"
#include "widgets/CFadingIcon.h"

const QPen IPlot::pens[] = {QPen(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                            QPen(QColor(0xFFC00000), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                            QPen(Qt::yellow, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                            QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)};

const QPen IPlot::pensThin[] = {QPen(Qt::darkBlue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                                QPen(Qt::darkRed, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                                QPen(Qt::darkYellow, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                                QPen(Qt::darkGreen, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)};

const QColor IPlot::colors[] = {QColor(Qt::blue), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(Qt::darkGreen)};

int IPlot::cnt = 0;

IPlot::IPlot(CGisItemTrk* trk, CPlotData::axistype_e type, mode_e mode, QWidget* parent)
    : QWidget(parent), INotifyTrk(CGisItemTrk::eVisualPlot), mode(mode), trk(trk), fm(font()) {
  cnt++;
  setObjectName(QString("IPlot%1").arg(cnt));

  setContextMenuPolicy(Qt::CustomContextMenu);
  setMouseTracking(true);

  if (trk) {
    trk->registerVisual(this);
  }

  data = new CPlotData(type, this);

  if ((mode == eModeIcon) || (mode == eModeSimple)) {
    showScale = false;
    thinLine = true;
  }

  if (mode == eModeWindow) {
    setWindowFlags(Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose, true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
  }

  menu = new QMenu(this);
  actionResetZoom = menu->addAction(QIcon("://icons/32x32/Zoom.png"), tr("Reset Zoom"), this, &IPlot::slotResetZoom);
  actionStopRange =
      menu->addAction(QIcon("://icons/32x32/SelectReset.png"), tr("Reset Range"), this, &IPlot::slotStopRange);
  actionPrint = menu->addAction(QIcon("://icons/32x32/Save.png"), tr("Save..."), this, &IPlot::slotSave);
  menu->addSeparator();
  actionAddWpt = menu->addAction(QIcon("://icons/32x32/AddWpt.png"), tr("Add Waypoint"), this, &IPlot::slotAddWpt);
  actionAddTrkPtInfo = menu->addAction(QIcon("://icons/32x32/AddPointInfo.png"), tr("Add Trackpoint Info"), this,
                                       &IPlot::slotAddTrkPtInfo);
  actionCutTrk = menu->addAction(QIcon("://icons/32x32/TrkCut.png"), tr("Cut Track..."), this, &IPlot::slotCutTrk);

  connect(this, &IPlot::customContextMenuRequested, this, &IPlot::slotContextMenu);
}

IPlot::~IPlot() {
  cnt--;

  if (trk) {
    trk->unregisterVisual(this);
    /*
        Always set the mode to normal. If the object is not owner
        of the current mode, the request will be ignored.
     */
    trk->setMode(CGisItemTrk::eModeNormal, objectName());

    /*
        As having the user focus will always display an on screen plot, closing
        the plot has to result into the track loosing the focus.
     */
    if (mode == eModeWindow) {
      trk->looseUserFocus();
      CCanvas* canvas = dynamic_cast<CCanvas*>(parent());
      if (canvas) {
        canvas->saveSizeTrackProfile();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
      }
    }
  }
}

void IPlot::clear() {
  needsRedraw = true;
  data->lines.clear();
  data->tags.clear();
  data->badData = true;
  update();
}

void IPlot::setXTicScale(qreal scale) {
  data->x().setTicScale(scale);
  setSizes();
  update();
}

void IPlot::setYLabel(const QString& str) {
  if (mode == eModeSimple) {
    return;
  }
  data->ylabel = str;
  setSizes();
  update();
}

void IPlot::setXLabel(const QString& str) {
  if (mode == eModeSimple) {
    return;
  }
  data->xlabel = str;
  setSizes();
  update();
}

void IPlot::newLine(const QPolygonF& line, const QString& label) {
  data->lines.clear();

  QRectF r = line.boundingRect();
  if ((r.height() < 0) || (r.width() < 0) || line.isEmpty()) {
    data->badData = true;
    return;
  }

  CPlotData::line_t l;
  l.points = line;
  l.label = label;

  data->badData = false;
  data->lines << l;
  setSizes();
  data->x().setScale(rectGraphArea.width());
  data->y().setScale(rectGraphArea.height());

  needsRedraw = true;
  update();
}

void IPlot::addLine(const QPolygonF& line, const QString& label) {
  QRectF r = line.boundingRect();
  if (!r.isValid() || line.isEmpty()) {
    return;
  }

  CPlotData::line_t l;
  l.points = line;
  l.label = label;

  data->lines << l;
  setSizes();
  data->x().setScale(rectGraphArea.width());
  data->y().setScale(rectGraphArea.height());

  needsRedraw = true;
  update();
}

void IPlot::setLimits() { data->setLimits(); }

void IPlot::resetZoom() {
  data->x().resetZoom();
  data->y().resetZoom();
  setSizes();

  needsRedraw = true;
  update();
}

void IPlot::paintEvent(QPaintEvent* /*e*/) {
  QPainter p(this);
  draw(p);
}

void IPlot::resizeEvent(QResizeEvent* e) {
  setSizes();

  buffer = QImage(e->size(), QImage::Format_ARGB32);

  needsRedraw = true;
  update();
}

void IPlot::leaveEvent(QEvent* /*e*/) {
  needsRedraw = true;
  posMouse1 = NOPOINT;

  CCanvas::restoreOverrideCursor("IPlot::leaveEvent");
  update();
}

void IPlot::enterEvent(QEvent* /*e*/) {
  needsRedraw = true;
  QCursor cursor = QCursor(QPixmap(":/cursors/cursorArrow.png"), 0, 0);
  CCanvas::setOverrideCursor(cursor, "IPlot::enterEvent");
  update();
}

void IPlot::draw(QPainter& p) {
  if (needsRedraw) {
    draw();
    needsRedraw = false;
  }

  p.drawImage(0, 0, buffer);
  drawDecoration(p);
}

void IPlot::keyPressEvent(QKeyEvent* e) {
  // close the current window if `Esc` was pressed
  if (Qt::Key_Escape == e->key()) {
    e->accept();
    deleteLater();
  } else {
    QWidget::keyPressEvent(e);
  }
}

bool IPlot::graphAreaContainsMousePos(QPoint& pos) {
  if (rectGraphArea.contains(pos)) {
    return true;
  }

  if ((pos.y() < rectGraphArea.bottom()) && (pos.y() > rectGraphArea.top())) {
    if (pos.x() < rectGraphArea.left()) {
      pos.rx() = rectGraphArea.left();
    }

    if (pos.x() > rectGraphArea.right()) {
      pos.rx() = rectGraphArea.right();
    }

    return true;
  }

  return false;
}

void IPlot::mouseMoveEvent(QMouseEvent* e) {
  if (data->lines.isEmpty() || data->badData || !data->x().isValid() || !data->y().isValid()) {
    return;
  }

  QPoint pos = e->pos();
  if (!mouseDidMove && (e->buttons() == Qt::LeftButton) &&
      ((pos - posLast).manhattanLength() >= CMouseAdapter::minimalMouseMovingDistance)) {
    mouseDidMove = true;
  }

  if (mouseDidMove) {
    if (!scrOptRange.isNull()) {
      delete scrOptRange;
    }
    QPoint diff = pos - posLast;

    data->x().move(-diff.x());
    data->y().move(diff.y());
    needsRedraw = true;
    update();

    posLast = pos;
    return;
  }

  posMouse1 = NOPOINT;
  if (graphAreaContainsMousePos(pos)) {
    posMouse1 = pos;

    // set point of focus at track object
    qreal x = data->x().pt2val(posMouse1.x() - left);
    setMouseFocus(x, CGisItemTrk::eFocusMouseMove);

    // update canvas if visible
    CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
    if (canvas) {
      canvas->update();
    }
    e->accept();
  }

  update();
}

bool IPlot::setMouseFocus(qreal pos, enum CGisItemTrk::focusmode_e fm) {
  if (nullptr == trk) {
    return false;
  }

  if (fm == CGisItemTrk::eFocusMouseMove && trk->getMode() == CGisItemTrk::eModeRange &&
      !is_in(trk->getRangState(), {
                                      CGisItemTrk::eRangeStateIdle,
                                      CGisItemTrk::eRangeStateClicked1st,
                                      CGisItemTrk::eRangeStateMove1st,
                                      CGisItemTrk::eRangeStateMove2nd,
                                  })) {
    return false;
  }

  if (data->axisType == CPlotData::eAxisLinear) {
    return trk->setMouseFocusByDistance(pos, fm, objectName());
  } else if (data->axisType == CPlotData::eAxisTime) {
    return trk->setMouseFocusByTime(pos, fm, objectName());
  }

  return false;
}

void IPlot::mousePressEvent(QMouseEvent* e) {
  if ((e->button() == Qt::LeftButton) && (mode == eModeIcon)) {
    trk->edit();
  }

  mouseDidMove = false;
  posLast = e->pos();
}

void IPlot::mouseReleaseEvent(QMouseEvent* e) {
  bool wasProcessed = false;

  if (e->button() == Qt::LeftButton) {
    if ((mode == eModeIcon) || mouseDidMove) {
      mouseDidMove = false;
      return;
    } else if (mode == eModeSimple) {
      wasProcessed = mouseReleaseEventSimple(e);
    } else {
      wasProcessed = mouseReleaseEventNormal(e);
    }
  }

  // Update canvas only if the object is the owner of the range selection
  if (wasProcessed) {
    emit sigMouseClickState(mouseClickState);

    // update canvas if visible
    CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
    if (canvas) {
      canvas->update();
    }
  }
  e->accept();
  update();
}

bool IPlot::mouseReleaseEventSimple(QMouseEvent* e) {
  QPoint pos = e->pos();
  posMouse1 = graphAreaContainsMousePos(pos) ? pos : NOPOINT;

  // set point of focus at track object
  qreal x = data->x().pt2val(posMouse1.x() - left);

  bool wasProcessed = setMouseFocus(x, CGisItemTrk::eFocusMouseClick);
  if (!wasProcessed) {
    new CFadingIcon(posMouse1, "://icons/48x48/NotPossible.png", this);
  }

  return wasProcessed;
}

bool IPlot::mouseReleaseEventNormal(QMouseEvent* e) {
  bool wasProcessed = true;

  QPoint pos = e->pos();
  posMouse1 = graphAreaContainsMousePos(pos) ? pos : NOPOINT;

  // set point of focus at track object
  qreal x = data->x().pt2val(posMouse1.x() - left);

  switch (mouseClickState) {
    case eMouseClickIdle: {
      // In idle state a mouse click will select the first point of a range
      if (trk->setMode(CGisItemTrk::eModeRange, objectName())) {
        setMouseFocus(x, CGisItemTrk::eFocusMouseClick);
        mouseClickState = eMouseClick1st;
      } else {
        /*
            If the object is not the owner of the range selection, no action has to be taken.
            However the user has to be informed, that he clicked on the wrong widget.
         */
        new CFadingIcon(posMouse1, "://icons/48x48/NotPossible.png", this);
        wasProcessed = false;
      }
      break;
    }

    case eMouseClick1st: {
      // In 1st click state a mouse click will select the second point of a range and display options
      setMouseFocus(x, CGisItemTrk::eFocusMouseClick);
      /*
          As the screen option is created on the fly it has to be connected to all slots,too.
          Later, when destroyed the slots will be disconnected automatically.
       */
      delete scrOptRange;
      scrOptRange = new CScrOptRangeTrk(pos, trk, nullptr, this);
      connect(scrOptRange->toolHidePoints, &QToolButton::clicked, this, &IPlot::slotHidePoints);
      connect(scrOptRange->toolShowPoints, &QToolButton::clicked, this, &IPlot::slotShowPoints);
      connect(scrOptRange->toolCopy, &QToolButton::clicked, this, &IPlot::slotCopy);
      connect(scrOptRange->toolActivity, &QToolButton::clicked, this, &IPlot::slotActivity);

      /* Adjust position of screen option widget if the widget is out of the visible area*/
      QRect r1 = scrOptRange->geometry();
      QRect r2 = geometry();
      r1.moveTopLeft(mapToParent(r1.topLeft()));
      if (!r2.contains(r1)) {
        // test if screen option is out of area on the right side
        if (!r2.contains(r1.topRight())) {
          QPoint pt = QPoint(r2.width(), r2.height()) - QPoint(r1.width(), r1.height());
          scrOptRange->move(pt);
        }
        // test if screen option is out of area on the left side
        else if (!r2.contains(r1.topLeft())) {
          QPoint pt = QPoint(0, r2.height()) - QPoint(0, r1.height());
          scrOptRange->move(pt);
        }
        // test if screen option is out of area on the bottom
        else if (!r2.contains(r1.bottomLeft())) {
          QPoint pt = QPoint(r1.left(), r2.height()) - QPoint(r2.left(), r1.height());
          scrOptRange->move(pt);
        }
      }

      mouseClickState = eMouseClick2nd;
      break;
    }

    case eMouseClick2nd: {
      // In second click state a mouse click will reset the range selection
      delete scrOptRange;
      trk->setMode(CGisItemTrk::eModeNormal, objectName());
      idxSel1 = idxSel2 = NOIDX;
      mouseClickState = eMouseClickIdle;
      break;
    }
  }

  return wasProcessed;
}

void IPlot::wheelEvent(QWheelEvent* e) {
  const auto& modifiers = QApplication::keyboardModifiers();
  const int delta = modifiers == Qt::AltModifier ? e->angleDelta().x() : e->angleDelta().y();
  bool in = CMainWindow::self().flipMouseWheel() ? (delta < 0) : (delta > 0);

  bool doHorizontalZoom = false;
  bool doVerticalZoom = false;

  switch (modifiers) {
    case Qt::AltModifier:
      doHorizontalZoom = true;
      break;

    case Qt::ControlModifier:
      doVerticalZoom = true;
      break;

    case Qt::NoModifier:
      doHorizontalZoom = true;
      doVerticalZoom = true;
      break;
  }

  if (doHorizontalZoom) {
    data->x().zoom(in, e->position().x() - left);
    setSizes();
    data->x().setScale(rectGraphArea.width());
  }

  if (doVerticalZoom) {
    data->y().zoom(in, bottom - e->position().y());
    setSizes();
    data->y().setScale(rectGraphArea.height());
  }

  QPoint p = mapToGlobal(e->position().toPoint() + QPoint(32, 0));
  QToolTip::showText(p, tr("Hold CTRL key for vertical zoom, only.\nHold ALT key for horizontal zoom, only."), this,
                     QRect(), 500);
  needsRedraw = true;
  update();

  e->accept();
}

void IPlot::setSizes() {
  fm = QFontMetrics(CMainWindow::self().getMapFont());
  left = 0;

  scaleWidthX1 = showScale ? data->x().getScaleWidth(fm) : 0;
  scaleWidthY1 = showScale ? data->y().getScaleWidth(fm) : 0;

  scaleWidthY1 = (scaleWidthX1 / 2) > scaleWidthY1 ? scaleWidthX1 / 2 : scaleWidthY1;

  fontWidth = fm.maxWidth();
  fontHeight = fm.height();

  if (mode == eModeSimple) {
    deadAreaX = 0;
    deadAreaY = 0;
  } else {
    deadAreaX = fontWidth >> 1;
    deadAreaY = (fontHeight + 1) >> 1;
  }

  iconBarHeight = height() > 350 ? 21 : 9;

  setLRTB();
  setSizeIconArea();
  setSizeXLabel();
  setSizeYLabel();
  setSizeTrackInfo();
  setSizeDrawArea();
}

void IPlot::setLRTB() {
  left = 0;

  left += data->ylabel.isEmpty() ? 0 : fontHeight;
  left += scaleWidthY1;
  left += deadAreaX;

  right = size().width();
  right -= deadAreaX;
  right -= scaleWidthX1 / 2;

  top = 0;
  if (!data->tags.isEmpty()) {
    top += iconBarHeight;
  }
  top += deadAreaY;

  bottom = size().height();
  bottom -= data->xlabel.isEmpty() ? 0 : fontHeight;
  // tick marks
  if (scaleWidthX1) {
    bottom -= fontHeight;
  }
  bottom -= deadAreaY;

  if (!data->xlabel.isEmpty()) {
    bottom -= deadAreaY;
  }
}

void IPlot::setSizeIconArea() { rectIconArea = QRect(left, deadAreaY, right - left, 16 + fontHeight + deadAreaY); }

void IPlot::setSizeXLabel() {
  if (mode == IPlot::eModeSimple) {
    rectX1Label = {0, 0, 0, 0};
  }

  int y;
  if (data->xlabel.isEmpty()) {
    rectX1Label = {0, 0, 0, 0};
  } else {
    rectX1Label.setWidth(right - left);
    rectX1Label.setHeight(fontHeight);
    y = (size().height() - rectX1Label.height()) - deadAreaY;

    rectX1Label.moveTopLeft(QPoint(left, y));
  }
}

void IPlot::setSizeYLabel() {
  if (mode == IPlot::eModeSimple) {
    rectX1Label = {0, 0, 0, 0};
  }

  if (data->ylabel.isEmpty()) {
    rectY1Label = {0, 0, 0, 0};
  } else {
    rectY1Label.setWidth(bottom - top);
    rectY1Label.setHeight(fontHeight);
    rectY1Label.moveTopLeft(QPoint(size().height() - bottom, 0));
  }
}

void IPlot::setSizeTrackInfo() {
  if (data->tags.isEmpty() /*|| !CResources::self().showTrackProfileEleInfo()*/) {
    rectTrackInfo = QRect();
    return;
  }

  rectTrackInfo.setWidth(right - left);
  rectTrackInfo.setHeight(qMax(fontHeight, iconBarHeight));
  rectTrackInfo.moveLeft(left);
  rectTrackInfo.moveTop(size().height() - fontHeight);
}

void IPlot::setSizeDrawArea() {
  rectGraphArea.setWidth(right - left);
  rectGraphArea.setHeight(bottom - top);
  rectGraphArea.moveTopLeft(QPoint(left, top));

  data->x().setScale(rectGraphArea.width());
  data->y().setScale(rectGraphArea.height());
}

void IPlot::draw() {
  buffer.fill(Qt::transparent);
  QPainter p(&buffer);
  USE_ANTI_ALIASING(p, true);

  if ((mode == eModeNormal) || (mode == eModeSimple)) {
    p.fillRect(rect(), Qt::white);
  } else if (mode == eModeIcon) {
    QRect r = rect();
    r.adjust(2, 2, -2, -2);
    if (underMouse() || posMouse1 != NOPOINT || solid) {
      p.setPen(solid ? CDraw::penBorderBlack : CDraw::penBorderBlue);
      p.setOpacity(1.0);
    } else {
      p.setPen(CDraw::penBorderBlack);
      p.setOpacity(0.6);
    }
    p.setBrush(QColor(255, 255, 255, 255));

    PAINT_ROUNDED_RECT(p, r);
  }

  if (data->lines.isEmpty() || data->badData || !data->x().isValid() || !data->y().isValid()) {
    p.drawText(rect(), Qt::AlignCenter, tr("No or bad data."));
    return;
  }

  p.setFont(CMainWindow::self().getMapFont());
  drawTags(p);
  p.setClipping(true);
  p.setClipRect(rectGraphArea);
  drawData(p);
  p.setClipping(false);
  drawLabels(p);
  if (showScale) {
    drawXScale(p);
    drawYScale(p);
  }
  drawGridX(p);
  drawGridY(p);
  drawActivities(p);
  drawXTic(p);
  drawYTic(p);
  p.setPen(QPen(Qt::black, 2));
  p.drawRect(rectGraphArea);
  drawLegend(p);

  p.setClipping(false);
  drawTagLabels(p);
}

QPointF IPlot::getBasePoint(int ptx) const {
  CPlotAxis& yaxis = data->y();

  if (0 >= data->ymin && 0 <= data->ymax) {
    return QPointF(ptx, bottom - yaxis.val2pt(0));
  } else if (data->ymin >= 0) {
    return QPointF(ptx, bottom - yaxis.val2pt(data->ymin));
  } else if (data->ymax <= 0) {
    return QPointF(ptx, bottom - yaxis.val2pt(data->ymax));
  }

  qWarning() << "Requesting basePoint for ptx = " << ptx << "; data->ymin/max = {" << data->ymin << ",  " << data->ymax
             << "}";
  return QPointF(ptx, bottom);
}

QPolygonF IPlot::getVisiblePolygon(const QPolygonF& polyline, QPolygonF& line) const {
  const CPlotAxis& xaxis = data->x();
  const CPlotAxis& yaxis = data->y();

  int ptx = NOINT;
  int pty = NOINT;

  for (const QPointF& pt : polyline) {
    int oldPtx = ptx;
    int oldPty = pty;
    ptx = left + xaxis.val2pt(pt.x());
    pty = bottom - yaxis.val2pt(pt.y());

    if (ptx >= left && ptx <= right) {
      // if oldPtx is < left, then ptx is the first visible point
      if (NOINT == oldPtx || oldPtx < left) {
        // we may need to interpolate things if we just found the first visible point
        if (NOINT != oldPtx && ptx > left) {
          line << getBasePoint(left);

          int intPty = oldPty + ((oldPty - pty) * (left - oldPtx)) / (oldPtx - ptx);
          line << QPointF(left, intPty);
        } else {
          line << getBasePoint(ptx);
        }
      }

      line << QPointF(ptx, pty);
    } else if (ptx > right) {
      // handle the special case `no point in the visible interval`
      // -> add interpolated left point
      if (oldPtx < left) {
        oldPty = oldPty + (pty - oldPty) / (left - oldPtx);
        oldPtx = left;

        line << getBasePoint(oldPtx);
        line << QPointF(oldPtx, oldPty);
      }

      // interpolate the value at `right`
      pty = (ptx - oldPtx) == 0 ? NOINT : oldPty + ((pty - oldPty) * (right - oldPtx)) / (ptx - oldPtx);
      ptx = right;
      line << QPointF(ptx, pty);
    }

    if (ptx >= right) {
      break;
    }
  }
  line << getBasePoint(ptx);
  return line;
}

void IPlot::drawData(QPainter& p) {
  int penIdx = 0;
  const QList<CPlotData::line_t>& lines = data->lines;
  for (const CPlotData::line_t& line : lines) {
    QPolygonF poly;
    getVisiblePolygon(line.points, poly);

    p.setPen(Qt::NoPen);
    p.setBrush(colors[penIdx]);
    p.drawPolygon(poly);

    p.setPen(thinLine ? pensThin[penIdx++] : pens[penIdx++]);
    p.setBrush(Qt::NoBrush);
    poly.pop_front();
    poly.pop_back();
    p.drawPolyline(poly);
  }
}

void IPlot::drawLabels(QPainter& p) {
  if (mode == IPlot::eModeSimple) {
    return;
  }

  p.setPen(Qt::darkBlue);

  if (rectX1Label.isValid()) {
    p.drawText(rectX1Label, Qt::AlignCenter, data->xlabel);
  }

  p.save();
  QTransform t = p.transform();
  t.translate(0, size().height());
  t.rotate(-90);
  p.setTransform(t);

  if (rectY1Label.isValid()) {
    p.drawText(rectY1Label, Qt::AlignCenter, data->ylabel);
  }
  p.restore();
}

void IPlot::drawXScale(QPainter& p) {
  QRect recText;

  if (data->x().getTicType() == CPlotAxis::eNoTic) {
    return;
  }

  p.setPen(Qt::darkBlue);
  recText.setHeight(fontHeight);
  recText.setWidth(scaleWidthX1);

  int ix_ = -1;

  const int iy = bottom + deadAreaY;
  const CPlotAxis::tic_t* t = data->x().ticmark();
  while (t) {
    int ix = left + data->x().val2pt(t->val) - (scaleWidthX1 + 1) / 2;
    if (((ix_ < 0) || ((ix - ix_) > scaleWidthX1 + 5)) && !t->lbl.isEmpty()) {
      recText.moveTopLeft(QPoint(ix, iy));
      p.drawText(recText, Qt::AlignCenter, t->lbl);
      ix_ = ix;
    }
    t = data->x().ticmark(t);
  }

  qreal limMin, limMax, useMin, useMax;
  data->x().getLimits(limMin, limMax, useMin, useMax);

  if (!isZoomed()) {
    return;
  }

  qreal scale = (right - left) / (limMax - limMin);

  int x = left + (useMin - limMin) * scale;
  int y = bottom + 5;
  int w = (useMax - useMin) * scale;

  p.setPen(QPen(Qt::red, 3));
  p.drawLine(x, y, x + w, y);
}

void IPlot::drawYScale(QPainter& p) {
  QString format_single_prec;
  QRect recText;
  if (data->y().getTicType() == CPlotAxis::eNoTic) {
    return;
  }

  p.setPen(Qt::darkBlue);
  recText.setHeight(fontHeight);
  recText.setWidth(scaleWidthY1);

  int ix = left - scaleWidthY1 - deadAreaX;
  int iy;

  qreal limMin, limMax, useMin, useMax;
  data->y().getLimits(limMin, limMax, useMin, useMax);

  // draw min/max labels 1st;
  QRect recTextMin;
  QRect recTextMax;

  format_single_prec = data->y().fmtsgl(data->ymin);
  if (data->ymin >= useMin) {
    iy = bottom - data->y().val2pt(data->ymin) - fontHeight / 2;
    recText.moveTopLeft(QPoint(ix, iy));
    p.drawText(recText, Qt::AlignRight, QString::asprintf(format_single_prec.toLatin1().data(), data->ymin));
    recTextMin = recText;
  }
  format_single_prec = data->y().fmtsgl(data->ymax);
  if (data->ymax <= useMax) {
    iy = bottom - data->y().val2pt(data->ymax) - fontHeight / 2;
    recText.moveTopLeft(QPoint(ix, iy));
    p.drawText(recText, Qt::AlignRight, QString::asprintf(format_single_prec.toLatin1().data(), data->ymax));
    recTextMax = recText;
  }

  // draw tic marks
  const CPlotAxis::tic_t* t = data->y().ticmark();
  while (t) {
    iy = bottom - data->y().val2pt(t->val) - fontHeight / 2;

    recText.moveTopLeft(QPoint(ix, iy));

    if (!recTextMin.intersects(recText) && !recTextMax.intersects(recText)) {
      p.drawText(recText, Qt::AlignRight, t->lbl);
    }

    t = data->y().ticmark(t);
  }

  if (!isZoomed()) {
    return;
  }

  qreal scale = (top - bottom) / (limMax - limMin);

  int x = left - 5;
  int y = bottom + (useMin - limMin) * scale;
  int h = (useMax - useMin) * scale;

  p.setPen(QPen(Qt::red, 3));
  p.drawLine(x, y, x, y + h);
}

void IPlot::drawGridX(QPainter& p) {
  CPlotAxis::tictype_e oldtic = data->x().setTicType(CPlotAxis::eTicNorm);

  const int dy = rectGraphArea.height();
  const CPlotAxis::tic_t* t = data->x().ticmark();

  QPen oldpen = p.pen();
  p.setPen(QPen(QColor(0, 150, 0, 128), 1, Qt::DotLine));

  const int iy = rectGraphArea.top();
  while (t) {
    int ix = left + data->x().val2pt(t->val);
    p.drawLine(ix, iy, ix, iy + dy);
    t = data->x().ticmark(t);
  }
  p.setPen(oldpen);
  data->x().setTicType(oldtic);
}

void IPlot::drawGridY(QPainter& p) {
  CPlotAxis::tictype_e oldtic = data->y().setTicType(CPlotAxis::eTicNorm);
  const int dx = rectGraphArea.width();
  const CPlotAxis::tic_t* t = data->y().ticmark();

  QPen oldpen = p.pen();
  p.setPen(QPen(QColor(0, 150, 0, 128), 1, Qt::DotLine));

  const int ix = rectGraphArea.left();
  while (nullptr != t) {
    int iy = bottom - data->y().val2pt(t->val);
    p.drawLine(ix, iy, ix + dx, iy);
    t = data->y().ticmark(t);
  }

  // draw min/max lines
  qreal limMin, limMax, useMin, useMax;
  data->y().getLimits(limMin, limMax, useMin, useMax);

  if (data->ymin > useMin) {
    int iy = bottom - data->y().val2pt(data->ymin);
    p.drawLine(ix, iy, ix + dx, iy);
  }
  if (data->ymax < useMax) {
    int iy = bottom - data->y().val2pt(data->ymax);
    p.drawLine(ix, iy, ix + dx, iy);
  }

  p.setPen(oldpen);
  data->y().setTicType(oldtic);
}

void IPlot::drawXTic(QPainter& p) {
  const CPlotAxis::tic_t* t = data->x().ticmark();

  p.setPen(QPen(Qt::black, 2));
  const int iyb = rectGraphArea.bottom();
  const int iyt = rectGraphArea.top();
  while (nullptr != t) {
    const int ix = left + data->x().val2pt(t->val);
    p.drawLine(ix, iyb, ix, iyb - 5);
    p.drawLine(ix, iyt, ix, iyt + 5);
    t = data->x().ticmark(t);
  }
}

void IPlot::drawYTic(QPainter& p) {
  const CPlotAxis::tic_t* t = data->y().ticmark();

  p.setPen(QPen(Qt::black, 2));
  const int ixl = rectGraphArea.left();
  const int ixr = rectGraphArea.right();
  while (t) {
    const int iy = bottom - data->y().val2pt(t->val);
    p.drawLine(ixl, iy, ixl + 5, iy);
    p.drawLine(ixr, iy, ixr - 5, iy);
    t = data->y().ticmark(t);
  }
}

void IPlot::drawLegend(QPainter& p) {
  if ((data->lines.size() < 2) || (mode == eModeIcon) || (mode == eModeSimple)) {
    return;
  }

  int penIdx = 0;
  QFontMetrics fm(p.font());
  int h = fm.height();

  int x = rectGraphArea.left() + 10;
  int y = rectGraphArea.top() + 2 + h;

  const QList<CPlotData::line_t>& lines = data->lines;
  for (const CPlotData::line_t& line : lines) {
    p.setPen(Qt::black);
    p.drawText(x + 30, y, line.label);
    p.setPen(pens[penIdx++]);
    p.drawLine(x, y, x + 20, y);

    y += fm.height();
  }
}

void IPlot::drawDecoration(QPainter& p) {
  p.save();
  if (posMouse1 != NOPOINT) {
    // draw the vertical `you are here` line
    int x = posMouse1.x();
    p.setPen(QPen(Qt::red, 2));
    if (x >= left && x <= right) {
      p.drawLine(x, top, x, bottom);

      // check if the mouse is near a waypoint
      if (!showWptLabels) {
        for (const CPlotData::point_t& tag : std::as_const(data->tags)) {
          int ptx = left + data->x().val2pt(tag.point.x());

          if (qAbs(x - ptx) >= 10) {
            continue;
          }
          QFont f = CMainWindow::self().getMapFont();
          f.setBold(true);
          QFontMetrics fm(f);
          QRect r = fm.boundingRect(tag.label);
          r.moveCenter(QPoint(ptx, top - fm.height() / 2 - fm.descent()));
          r.adjust(-3, -2, 3, 0);

          p.setPen(Qt::NoPen);
          p.setBrush(Qt::white);
          p.drawRoundedRect(r, RECT_RADIUS, RECT_RADIUS);

          p.setFont(f);
          p.setPen(Qt::darkBlue);
          p.drawText(r, Qt::AlignCenter, tag.label);

          break;
        }
      }
    }
  }

  if ((idxSel1 != NOIDX) && (idxSel2 != NOIDX) && !data->badData) {
    p.setClipRect(rectGraphArea);

    int penIdx = 3;

    const QPolygonF& polyline = data->lines.first().points.mid(idxSel1, idxSel2 - idxSel1 + 1);
    QPolygonF line;
    getVisiblePolygon(polyline, line);

    // avoid drawing if the whole interval is outside the visible range
    if (!(line.first().x() >= right || line.last().x() <= left)) {
      // draw the background
      p.setPen(Qt::NoPen);
      p.setBrush(colors[penIdx]);
      p.drawPolygon(line);

      // draw the foreground
      p.setPen(thinLine ? pensThin[penIdx] : pens[penIdx]);
      p.setBrush(Qt::NoBrush);
      line.pop_front();
      line.pop_back();
      p.drawPolyline(line);

      p.setPen(QPen(Qt::darkBlue, 2));
      p.drawLine(line.first().x(), top, line.first().x(), bottom);
      p.drawLine(line.last().x(), top, line.last().x(), bottom);
    }
    p.setClipping(false);
  }

  if (!scrOptRange.isNull()) {
    scrOptRange->draw(p);
  }

  p.restore();
}

void IPlot::drawTags(QPainter& p) {
  if (data->tags.isEmpty()) {
    return;
  }

  p.save();

  QFont f = CMainWindow::self().getMapFont();
  f.setBold(true);
  p.setFont(f);

  CPlotAxis& xaxis = data->x();
  CPlotAxis& yaxis = data->y();

  for (const CPlotData::point_t& tag : std::as_const(data->tags)) {
    int ptx = left + xaxis.val2pt(tag.point.x());
    int pty = bottom - yaxis.val2pt(tag.point.y());

    if (!((left < ptx) && (ptx < right))) {
      continue;
    }

    if (iconBarHeight >= pty) {
      continue;
    }

    QPixmap icon = tag.icon.scaled(iconBarHeight, iconBarHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    p.drawPixmap(ptx - icon.width() / 2, 2, icon);

    if (pty > bottom) {
      pty = bottom;
    }

    p.setPen(QPen(Qt::white, 3));
    p.drawLine(ptx, top, ptx, pty);
    p.setPen(QPen(Qt::black, 1));
    p.drawLine(ptx, top, ptx, pty);
  }

  p.restore();
}

void IPlot::drawTagLabels(QPainter& p) {
  if (data->tags.isEmpty()) {
    return;
  }

  if (!showWptLabels) {
    return;
  }

  p.save();
  QFont f = CMainWindow::self().getMapFont();
  f.setBold(true);
  QFontMetrics fm(f);
  p.setFont(f);

  CPlotAxis& xaxis = data->x();

  for (const CPlotData::point_t& tag : std::as_const(data->tags)) {
    int ptx = left + xaxis.val2pt(tag.point.x());

    if (!((left < ptx) && (ptx < right))) {
      continue;
    }

    p.save();
    p.translate(ptx, top);
    p.rotate(90);
    p.translate(5, -3);
    CDraw::text(tag.label, p, fm.boundingRect(tag.label), Qt::black);
    p.restore();
  }
  p.restore();
}

void IPlot::drawActivities(QPainter& p) {
  if ((mode == eModeIcon) || (trk->getActivities().getAllActivities().isEmpty())) {
    return;
  }

  if ((trk->getActivities().getAllActivities().count() == 1) &&
      (trk->getActivities().getAllActivities().values().first() == CTrackData::trkpt_t::eAct20None)) {
    return;
  }

  const QList<CActivityTrk::range_t>& ranges = trk->getActivities().getActivityRanges();

  int bar_height = (mode == eModeSimple) ? 18 : 26;
  int color_width = (mode == eModeSimple) ? 3 : 3;
  int icon_frame = (mode == eModeSimple) ? 12 : 20;
  int icon_size = (mode == eModeSimple) ? 8 : 16;

  QRect rectClipping = QRect(0, 0, right - left, 27);
  p.save();
  p.setClipping(true);
  p.translate(left, bottom - bar_height);
  p.setBrush(QColor(0, 170, 0, 100));
  p.setPen(Qt::NoPen);
  p.drawRect(rectClipping);

  QRect rectIconFrame(0, 0, icon_frame, icon_frame);
  QRect rectIcon(0, 0, icon_size, icon_size);

  for (const CActivityTrk::range_t& range : ranges) {
    int x1, x2, y1 = 0;
    const CTrackData& trkData = trk->getTrackData();

    const CTrackData::trkpt_t* trkptBeg = trkData.getTrkPtByTotalIndex(range.idxTotalBeg);
    const CTrackData::trkpt_t* trkptEnd = trkData.getTrkPtByTotalIndex(range.idxTotalEnd);

    if (data->axisType == CPlotData::eAxisTime) {
      x1 = data->x().val2pt(trkptBeg->time.toSecsSinceEpoch());
      x2 = data->x().val2pt(trkptEnd->time.toSecsSinceEpoch());
    } else {
      x1 = data->x().val2pt(trkptBeg->distance);
      x2 = data->x().val2pt(trkptEnd->distance);
    }

    if (trkptBeg != nullptr && !data->lines.isEmpty()) {
      y1 = data->y().val2pt(data->lines[0].points[trkptBeg->idxVisible].y());
    }

    const CActivityTrk::desc_t& desc = CActivityTrk::getDescriptor(range.activity);

    int d = (x2 - x1);
    if (d >= 20) {
      int c = x1 + d / 2;

      p.setPen(QPen(desc.color, 1));
      rectIconFrame.moveCenter(QPoint(c, icon_frame / 2 + color_width));
      p.setBrush(QColor(255, 255, 255, 100));
      p.drawRoundedRect(rectIconFrame, RECT_RADIUS, RECT_RADIUS);

      rectIcon.moveCenter(QPoint(c, icon_frame / 2 + color_width));
      p.drawPixmap(rectIcon, QPixmap(desc.iconSmall));
    }

    p.setPen(QPen(Qt::darkGreen, 1));
    p.drawLine(x1, bar_height, x1, qMin(0, bar_height - y1));

    p.setPen(QPen(desc.color, color_width, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(x1, color_width / 2, x2, color_width / 2);
  }

  p.restore();
}

void IPlot::save(QImage& image, const CTrackData::trkpt_t* pTrkpt) {
  resize(image.size());
  setSizes();
  buffer = QImage(image.size(), QImage::Format_ARGB32);
  draw();
  if (pTrkpt != nullptr) {
    posMouse1.rx() = left + data->x().val2pt(pTrkpt->distance);
    posMouse1.ry() = top + data->y().val2pt(pTrkpt->ele);

    QPainter p(&buffer);
    drawDecoration(p);
  }
  image = buffer;
}

void IPlot::slotContextMenu(const QPoint& point) {
  QPoint p = mapToGlobal(point);

  if (mode == IPlot::eModeSimple) {
    actionResetZoom->setEnabled(isZoomed());
    actionStopRange->setEnabled(false);
    actionPrint->setEnabled(false);
    actionAddWpt->setEnabled(false);
    actionCutTrk->setEnabled(false);
    actionAddTrkPtInfo->setEnabled(false);
  } else {
    actionResetZoom->setEnabled(isZoomed());
    actionStopRange->setEnabled((mouseClickState != eMouseClickIdle) && !(idxSel1 == NOIDX || idxSel2 == NOIDX));
    actionPrint->setEnabled(mouseClickState != eMouseClick2nd);
    actionAddWpt->setDisabled(posMouse1 == NOPOINT);
    actionCutTrk->setDisabled(actionStopRange->isEnabled());
  }
  posMouse2 = posMouse1;

  menu->exec(p);

  posMouse2 = NOPOINT;
}

void IPlot::slotSave() {
  SETTINGS;
  QString path = cfg.value("Paths/lastGraphPath", QDir::homePath()).toString();
  QString filename = QFileDialog::getSaveFileName(this, tr("Select output file"), path, "PNG Image (*.png)");

  if (filename.isEmpty()) {
    return;
  }

  QFileInfo fi(filename);
  if (fi.suffix().toLower() != "png") {
    filename += ".png";
  }

  QImage img(size(), QImage::Format_ARGB32);
  QPainter p;
  p.begin(&img);
  p.fillRect(rect(), QBrush(Qt::white));
  draw(p);
  p.end();

  img.save(filename);

  path = fi.absolutePath();
  cfg.setValue("Paths/lastGraphPath", path);
}

void IPlot::slotHidePoints() {
  trk->hideSelectedPoints();
  slotStopRange();
}

void IPlot::slotShowPoints() {
  trk->showSelectedPoints();
  slotStopRange();
}

void IPlot::slotActivity() {
  CActivityTrk::getMenu(trk->getKey(), this, true);
  slotStopRange();
}

void IPlot::slotCopy() {
  trk->copySelectedPoints();
  slotStopRange();
}

void IPlot::slotStopRange() {
  scrOptRange->deleteLater();
  trk->setMode(CGisItemTrk::eModeNormal, objectName());
  idxSel1 = idxSel2 = NOIDX;
  mouseClickState = eMouseClickIdle;

  emit sigMouseClickState(mouseClickState);

  // update canvas if visible
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->update();
  }
}

void IPlot::slotResetZoom() {
  data->x().resetZoom();
  data->y().resetZoom();
  setSizes();

  needsRedraw = true;
  update();
}

void IPlot::slotAddWpt() {
  if (posMouse2 == NOPOINT) {
    return;
  }

  const CTrackData::trkpt_t* trkpt = trk->getMouseMoveFocusPoint();
  if (trkpt == nullptr) {
    return;
  }

  CGisWorkspace::self().addWptByPos({trkpt->lon, trkpt->lat});
  CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void IPlot::slotAddTrkPtInfo() {
  if (posMouse2 == NOPOINT) {
    return;
  }

  const CTrackData::trkpt_t* trkpt = trk->getMouseMoveFocusPoint();
  if (trkpt == nullptr) {
    return;
  }

  trk->setMouseFocusByTotalIndex(trkpt->idxTotal, CGisItemTrk::eFocusMouseClick, "IPlot");
  CGisWorkspace::self().addTrkInfoByKey(trk->getKey());
  CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void IPlot::slotCutTrk() {
  // set point of mouse click focus to position of context menu stored in
  // secondary mouse point
  qreal x = data->x().pt2val(posMouse2.x() - left);
  setMouseFocus(x, CGisItemTrk::eFocusMouseClick);

  /*
     Trigger cut by event not by direct call to API. This is because cutting the track
     might result into deleting the original one. The original one is the parent of this
     plot and needs to destroy it. This would be impossible if we are still in this method
     because the API call did not return yet.
   */
  CGisWorkspace::self().postEventForWks(new CEvtA2WCutTrk(trk->getKey()));
}

void IPlot::setMouseRangeFocus(const CTrackData::trkpt_t* ptRange1, const CTrackData::trkpt_t* ptRange2) {
  if (nullptr == ptRange1 || nullptr == ptRange2) {
    idxSel1 = NOIDX;
    idxSel2 = NOIDX;
  } else {
    if (ptRange1->idxTotal < ptRange2->idxTotal) {
      while (ptRange1->isHidden()) {
        ptRange1++;
      }
      while (ptRange2->isHidden()) {
        ptRange2--;
      }
      idxSel1 = ptRange1->idxVisible;
      idxSel2 = ptRange2->idxVisible;
    } else {
      while (ptRange1->isHidden()) {
        ptRange1--;
      }
      while (ptRange2->isHidden()) {
        ptRange2++;
      }
      idxSel1 = ptRange2->idxVisible;
      idxSel2 = ptRange1->idxVisible;
    }
  }
  update();
}

bool IPlot::isZoomed() const {
  bool zoomed = false;
  qreal limMin, limMax, useMin, useMax;
  data->x().getLimits(limMin, limMax, useMin, useMax);
  zoomed |= !((limMax - limMin) <= (useMax - useMin));
  data->y().getLimits(limMin, limMax, useMin, useMax);
  zoomed |= !((limMax - limMin) <= (useMax - useMin));

  return zoomed;
}
