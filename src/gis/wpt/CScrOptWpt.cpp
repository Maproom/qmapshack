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


#include "gis/wpt/CScrOptWpt.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/CGisWidget.h"
#include "mouse/IMouse.h"
#include "canvas/CCanvas.h"
#include "CMainWindow.h"

#include <QtWidgets>

CScrOptWpt::CScrOptWpt(CGisItemWpt *wpt, const QPoint& origin, IMouse *parent)
    : IScrOpt(parent->getCanvas())
    , key(wpt->getKey())
{
    setupUi(this);
    setOrigin(origin);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(wpt->getInfo());

    anchor = wpt->getPointCloseBy(origin);
    move(anchor.toPoint() + QPoint(30,30));
    adjustSize();
    show();

    connect(toolDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
}

CScrOptWpt::~CScrOptWpt()
{

}

void CScrOptWpt::slotDelete()
{
    CGisWidget::self().delItemByKey(key);
}

void CScrOptWpt::draw(QPainter& p)
{
    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(item == 0)
    {
        QWidget::deleteLater();
        return;
    }
    item->drawHighlight(p);

    QRectF r = rect();
    r.moveTopLeft(QPoint(x(), y()));
    QPainterPath path1;
    path1.addRoundedRect(r,5,5);

    QPolygonF poly2;
    poly2 << anchor << (r.topLeft() + QPointF(10,0)) << (r.topLeft() + QPointF(0,10)) << anchor;
    QPainterPath path2;
    path2.addPolygon(poly2);

    path1 = path1.united(path2);

    p.setPen(CCanvas::penBorderGray);
    p.setBrush(CCanvas::brushBackWhite);
    p.drawPolygon(path1.toFillPolygon());

}
