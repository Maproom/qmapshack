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

#include "mouse/CMouseEditLine.h"
#include "mouse/CScrOptPoint.h"
#include "mouse/CScrOptEditLine.h"
#include "mouse/CScrOptRange.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "canvas/CCanvas.h"


#include <QtWidgets>

CMouseEditLine::CMouseEditLine(CGisItemTrk &trk, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , state(eStateIdle)
    , idxOfFocus(-1)
    , idxStart(-1)
    , idxStop(-1)
{
    cursor  = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
    key     = trk.getKey();

    const CGisItemTrk::trk_t& data = trk.getTrackData();
    foreach (const CGisItemTrk::trkseg_t& seg, data.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& pt, seg.pts)
        {
            if(pt.flags & CGisItemTrk::trkpt_t::eDeleted)
            {
                continue;
            }
            coords << QPointF(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD);
        }
    }

    line = coords;
    gis->convertRad2Px(line);

    scrOptEditLine = new CScrOptEditLine(canvas);
    scrOptEditLine->show();

    connect(scrOptEditLine->pushSaveOrig, SIGNAL(clicked()), this, SLOT(slotCopyToOrig()));
    connect(scrOptEditLine->pushSaveNew, SIGNAL(clicked()), this, SLOT(slotCopyToNew()));
    connect(scrOptEditLine->pushAbort, SIGNAL(clicked()), this, SLOT(slotAbort()));
}

CMouseEditLine::~CMouseEditLine()
{
    delete scrOptPoint;
    delete scrOptEditLine;
    delete scrOptRange;
}

void CMouseEditLine::drawPointOfFocus(QPainter& p)
{
    if(idxOfFocus < 0)
    {
        return;
    }

    const QPointF& pt = line[idxOfFocus];

    p.setPen(Qt::darkBlue);
    p.setBrush(QColor(255,255,255,200));
    p.drawEllipse(pt, 6, 6);

    QPixmap bullet("://icons/8x8/bullet_magenta.png");
    p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
}

void CMouseEditLine::drawBullets(QPainter& p)
{
    QPixmap bullet("://icons/8x8/bullet_magenta.png");
    foreach(const QPointF& pt, line)
    {
        p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
    }
}

void CMouseEditLine::drawHighlight1(QPainter& p)
{
    if(idxStart < 0 || idxOfFocus < 0)
    {
        return;
    }

    int pos = idxOfFocus < idxStart ? idxOfFocus : idxStart;
    int len = qAbs(idxOfFocus - idxStart) + 1;

    QPolygonF highlight = line.mid(pos,len);

    p.setPen(QPen(Qt::red, 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(highlight);
}

void CMouseEditLine::drawHighlight2(QPainter& p)
{
    if(idxStart < 0 || idxStop < 0)
    {
        return;
    }

    int pos = idxStop < idxStart ? idxStop : idxStart;
    int len = qAbs(idxStop - idxStart) + 1;

    QPolygonF highlight = line.mid(pos,len);

    p.setPen(QPen(Qt::red, 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(highlight);
}

void CMouseEditLine::draw(QPainter& p, bool needsRedraw, const QRect &rect)
{
    if(needsRedraw)
    {
        line = coords;
        gis->convertRad2Px(line);
    }

    p.setPen(QPen(Qt::magenta, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(line);

    switch(state)
    {
        case eStateIdle:
        case eStatePointSelected:
            drawBullets(p);
            drawPointOfFocus(p);
            break;

        case eStateSelectRange:
            drawHighlight1(p);
            drawBullets(p);
            drawPointOfFocus(p);
            break;

        case eStateRangeSelected:
            drawHighlight2(p);
            drawBullets(p);
            drawPointOfFocus(p);
            break;
        default:;
    }


    if(!scrOptPoint.isNull())
    {
        scrOptPoint->draw(p);
    }
    if(!scrOptRange.isNull())
    {
        scrOptRange->draw(p);
    }
}

void CMouseEditLine::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        switch(state)
        {

            case eStateSelectRange:
                cursor  = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
                QApplication::restoreOverrideCursor();
                QApplication::setOverrideCursor(cursor);
                //break; no break fall thru

            case eStateRangeSelected:
                state = eStateIdle;
                idxOfFocus  = -1;
                idxStart    = -1;
                idxStop     = -1;

                canvas->update();
                break;

            default:
                slotAbort();
        }
    }
    else if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
            case eStateIdle:
            {
                if(idxOfFocus >= 0)
                {
                    scrOptPoint = new CScrOptPoint(line[idxOfFocus], canvas);                    
                    scrOptPoint->show();
                    connect(scrOptPoint->toolDelete, SIGNAL(clicked()), this, SLOT(slotDeletePoint()));
                    connect(scrOptPoint->toolSelectRange, SIGNAL(clicked()), this, SLOT(slotSelectRange()));
                    canvas->update();

                    state = eStatePointSelected;
                }
                break;
            }
            case eStatePointSelected:
            {
                delete scrOptPoint;                
                canvas->update();
                idxOfFocus = -1;
                state = eStateIdle;
                break;
            }
            case eStateSelectRange:
            {
                state   = eStateRangeSelected;
                idxStop = idxOfFocus;

                scrOptRange = new CScrOptRange(line[idxStop], canvas);
                scrOptRange->show();
                connect(scrOptRange->toolDelete, SIGNAL(clicked()), this, SLOT(slotDeleteRange()));

                cursor  = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
                QApplication::restoreOverrideCursor();
                QApplication::setOverrideCursor(cursor);

                canvas->update();
                break;
            }
            case eStateRangeSelected:
            {
                delete scrOptRange;

                state = eStateIdle;
                idxOfFocus  = -1;
                idxStart    = -1;
                idxStop     = -1;

                canvas->update();
                break;
            }


            default:;
        }
    }

}

void CMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();    

    switch(state)
    {
        case eStateIdle:
        case eStateSelectRange:
        {
            if(!scrOptEditLine->rect().contains(point))
            {
                panCanvas(point);
            }

            int old = idxOfFocus;
            idxOfFocus = getPointCloseBy(point);
            if(old != idxOfFocus)
            {
                canvas->update();
            }
            break;
        }
        default:;
    }
}

void CMouseEditLine::mouseReleaseEvent(QMouseEvent *e)
{

}

void CMouseEditLine::wheelEvent(QWheelEvent * e)
{
    canvas->update();
}


int CMouseEditLine::getPointCloseBy(const QPoint& screenPos)
{
    qint32 i    = 0;
    qint32 idx  = -1;
    qint32  d   = NOINT;
    foreach(const QPointF& point, line)
    {
        int tmp = (screenPos - point).manhattanLength();
        if(tmp < d)
        {
            idx = i;
            d   = tmp;
        }
        i++;
    }

    if(d > 40) idx = -1;

    return idx;
}

void CMouseEditLine::slotDeletePoint()
{
    if(idxOfFocus < 0)
    {
        return;
    }
    scrOptPoint->deleteLater();

    coords.remove(idxOfFocus);
    line.remove(idxOfFocus);

    idxOfFocus  = -1;
    state       = eStateIdle;

    canvas->update();
}

void CMouseEditLine::slotSelectRange()
{
    if(idxOfFocus < 0)
    {
        return;
    }
    scrOptPoint->deleteLater();

    idxStart    = idxOfFocus;
    state       = eStateSelectRange;

    cursor  = QCursor(QPixmap(":/cursors/cursorSelectRange.png"),0,0);
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(cursor);

}

void CMouseEditLine::slotDeleteRange()
{
    if(idxStart < 0 || idxStop < 0)
    {
        return;
    }
    scrOptRange->deleteLater();

    int len = qAbs(idxStop - idxStart) + 1;
    int idx = idxStart < idxStop ? idxStart : idxStop;
    coords.remove(idx,len);
    line = coords;
    gis->convertRad2Px(line);

    state = eStateIdle;
    idxOfFocus  = -1;
    idxStart    = -1;
    idxStop     = -1;

    canvas->update();
}

void CMouseEditLine::slotAbort()
{
    canvas->resetMouse();
    canvas->update();
}

void CMouseEditLine::slotCopyToOrig()
{    
    int res = QMessageBox::warning(canvas, tr("Warning!"), tr("This will replace all data of the orignal by a simple line of coordinates. All other data will be lost permanently."), QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);

    if(res != QMessageBox::Ok)
    {
        return;
    }

    IGisLine * l = dynamic_cast<IGisLine*>(CGisWidget::self().getItemByKey(key));
    if(l != 0)
    {
        l->replaceData(coords);
    }
    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseEditLine::slotCopyToNew()
{
    CGisProject * project = CGisWidget::self().selectProject();
    if(project == 0)
    {
        return;
    }

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk == 0)
    {
        return;
    }

    new CGisItemTrk(coords, trk->getName(), project, -1);

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
