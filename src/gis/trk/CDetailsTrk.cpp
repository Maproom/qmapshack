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

#include "gis/trk/CDetailsTrk.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"
#include "GeoMath.h"

#include <QtWidgets>
#include <proj_api.h>

CDetailsTrk::CDetailsTrk(CGisItemTrk& trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);
    setupGui();

    QPixmap icon(16,8);
    for(int i=0; i < 17; ++i)
    {
        icon.fill(CGisItemTrk::lineColors[i]);
        comboColor->addItem(icon,"",CGisItemTrk::lineColors[i]);
    }

    lineName->setText(trk.getName());
    comboColor->setCurrentIndex(trk.getColorIdx());

    plotElevation->setTrack(&trk);
    plotDistance->setTrack(&trk);
    plotSpeed->setTrack(&trk);
    plotTrack->setTrack(&trk);

    SETTINGS;
    cfg.beginGroup("TrackDetails");
    checkProfile->setChecked(cfg.value("showProfile", true).toBool());
    checkSpeed->setChecked(cfg.value("showSpeed", true).toBool());
    checkProgress->setChecked(cfg.value("showProgress", true).toBool());
    cfg.endGroup();

    connect(checkProfile, SIGNAL(clicked()), this, SLOT(slotShowPlots()));
    connect(checkSpeed, SIGNAL(clicked()), this, SLOT(slotShowPlots()));
    connect(checkProgress, SIGNAL(clicked()), this, SLOT(slotShowPlots()));
    connect(comboColor, SIGNAL(currentIndexChanged(int)), this, SLOT(slotColorChanged(int)));
    connect(lineName, SIGNAL(returnPressed()), this, SLOT(slotNameChanged()));
    connect(lineName, SIGNAL(textChanged(QString)), this, SLOT(slotNameChanged(QString)));
    connect(toolLock, SIGNAL(toggled(bool)), this, SLOT(slotChangeReadOnlyMode(bool)));
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));


    slotShowPlots();

}

CDetailsTrk::~CDetailsTrk()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    cfg.setValue("showProfile", checkProfile->isChecked());
    cfg.setValue("showSpeed", checkSpeed->isChecked());
    cfg.setValue("showProgress", checkProgress->isChecked());
    cfg.endGroup();
}

void CDetailsTrk::setupGui()
{
    QString str, val, unit;

    bool isReadOnly = trk.isReadOnly();

    if(trk.isTainted())
    {
        labelTainted->show();
    }
    else
    {
        labelTainted->hide();
    }

    labelInfo->setText(trk.getInfo());
    lineName->setEnabled(!isReadOnly);
    comboColor->setEnabled(!isReadOnly);
    toolLock->setChecked(isReadOnly);

    treeWidget->clear();

    QList<QTreeWidgetItem*> items;
    const CGisItemTrk::trk_t& t = trk.getTrackData();
    foreach (const CGisItemTrk::trkseg_t& seg, t.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
        {

            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setTextAlignment(eColNum,Qt::AlignLeft);
            item->setTextAlignment(eColEle,Qt::AlignRight);
            item->setTextAlignment(eColDelta,Qt::AlignRight);
            item->setTextAlignment(eColDist,Qt::AlignRight);
            item->setTextAlignment(eColAscend,Qt::AlignRight);
            item->setTextAlignment(eColDescend,Qt::AlignRight);
            item->setTextAlignment(eColSpeed,Qt::AlignRight);


            if(trkpt.flags & CGisItemTrk::trkpt_t::eDeleted)
            {
                for(int i = 0; i < eColMax; i++)
                {
                    item->setForeground(i,QBrush(Qt::gray));
                }
            }
            else
            {
                for(int i = 0; i < eColMax; i++)
                {
                    item->setForeground(i,QBrush(Qt::black));
                }
            }

            item->setText(eColNum,QString::number(trkpt.idx));
            if(trkpt.time.isValid())
            {
                item->setText(eColTime, IUnit::self().datetime2string(trkpt.time, QPointF(trkpt.lon, trkpt.lat)*DEG_TO_RAD));
            }
            else
            {
                item->setText(eColTime, "-");
            }

            if(trkpt.ele != NOINT)
            {
                IUnit::self().meter2elevation(trkpt.ele, val, unit);
                str = tr("%1 %2").arg(val).arg(unit);
            }
            else
            {
                str = "-";
            }
            item->setText(eColEle,str);

            IUnit::self().meter2distance(trkpt.deltaDistance, val, unit);
            item->setText(eColDelta, tr("%1 %2").arg(val).arg(unit));

            IUnit::self().meter2distance(trkpt.distance, val, unit);
            item->setText(eColDist, tr("%1 %2").arg(val).arg(unit));

            // speed
            if(trkpt.speed != NOFLOAT)
            {
                IUnit::self().meter2speed(trkpt.speed, val, unit);
                str = tr("%1 %2").arg(val).arg(unit);
            }
            else
            {
                str = "-";
            }
            item->setText(eColSpeed,str);

            item->setText(eColSlope, QString("%1°(%2%)").arg(trkpt.slope1,2,'f',0).arg(trkpt.slope2,2,'f',0));

            IUnit::self().meter2elevation(trkpt.ascend, val, unit);
            item->setText(eColAscend, tr("%1 %2").arg(val).arg(unit));
            IUnit::self().meter2elevation(trkpt.descend, val, unit);
            item->setText(eColDescend, tr("%1 %2").arg(val).arg(unit));

            // position
            GPS_Math_Deg_To_Str(trkpt.lon, trkpt.lat, str);
            item->setText(eColPosition,str);

            items << item;

        }
    }
    treeWidget->addTopLevelItems(items);

    treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

}

void CDetailsTrk::setMouseMoveFocus(const CGisItemTrk::trkpt_t * pt)
{
    if(pt != 0)
    {
        plotTrack->setMouseMoveFocus(pt->lon, pt->lat);
        labelInfoTrkPt->setText(trk.getInfoTrkPt(*pt));
        labelInfoProgress->setText(trk.getInfoProgress(*pt));
    }
    else
    {
        labelInfoTrkPt->setText("\n");
        labelInfoProgress->clear();
    }
}

void CDetailsTrk::setMouseClickFocus(const CGisItemTrk::trkpt_t * pt)
{
    if(pt != 0)
    {
        treeWidget->blockSignals(true);
        treeWidget->setCurrentItem(treeWidget->topLevelItem(pt->idx));
        treeWidget->blockSignals(false);
    }
}

void CDetailsTrk::slotShowPlots()
{
    if(checkProfile->isChecked())
    {
        plotElevation->show();
    }
    else
    {
        plotElevation->hide();
    }

    if(checkSpeed->isChecked())
    {
        plotSpeed->show();
    }
    else
    {
        plotSpeed->hide();
    }

    if(checkProgress->isChecked())
    {
        plotDistance->show();
    }
    else
    {
        plotDistance->hide();
    }
}

void CDetailsTrk::slotColorChanged(int idx)
{
    if(trk.getColorIdx() != idx)
    {
        trk.setColor(idx);
    }
    setupGui();
}

void CDetailsTrk::slotChangeReadOnlyMode(bool on)
{
    trk.setReadOnlyMode(on);
    setupGui();

}

void CDetailsTrk::slotNameChanged(const QString& name)
{
    QPalette palette = lineName->palette();
    if(trk.getName() != name)
    {
        palette.setColor(QPalette::Base, QColor(255, 128, 128));
    }
    else
    {
        palette.setColor(QPalette::Base, QColor(255, 255, 255));
    }
    lineName->setPalette(palette);
}


void CDetailsTrk::slotNameChanged()
{
    QString  name  = lineName->text();
    QString _name_ = trk.getName();

    QPalette palette = lineName->palette();

    if(_name_ != name)
    {
        trk.setName(name);
        palette.setColor(QPalette::Base, QColor(128, 255, 128));
    }

    lineName->setPalette(palette);
    setupGui();
}

void CDetailsTrk::slotItemSelectionChanged()
{
    QTreeWidgetItem * item = treeWidget->currentItem();
    if(item != 0)
    {
        quint32 idx = item->text(eColNum).toUInt();
        trk.setMouseFocusByIndex(idx, CGisItemTrk::eFocusMouseMove);
    }
}
