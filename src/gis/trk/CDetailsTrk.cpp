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
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CGraphTrk.h"
#include "gis/trk/filter/CFilterDelete.h"
#include "gis/trk/filter/CFilterDouglasPeuker.h"
#include "gis/trk/filter/CFilterMedian.h"
#include "gis/trk/filter/CFilterNewDate.h"
#include "gis/trk/filter/CFilterObscureDate.h"
#include "gis/trk/filter/CFilterOffsetElevation.h"
#include "gis/trk/filter/CFilterReplaceElevation.h"
#include "gis/trk/filter/CFilterReset.h"
#include "gis/trk/filter/CFilterSpeed.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CSettings.h"
#include "plot/CPlotProfile.h"
#include "units/IUnit.h"
#include "widgets/CTextEditWidget.h"

#include <QtWidgets>
#include <proj_api.h>

CDetailsTrk::CDetailsTrk(CGisItemTrk& trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    QPixmap icon(16,8);

    for(int i=0; i < TRK_N_COLORS; ++i)
    {
        icon.fill(IGisItem::colorMap[i].color);
        comboColor->addItem(icon, IGisItem::colorMap[i].name, IGisItem::colorMap[i].color);
    }

    widgetColorLayout->setAlignment(Qt::AlignTop);

    int i = 0;
    const CActivityTrk::desc_t* actDesc = CActivityTrk::getActivityDescriptors();
    while(!actDesc[i].name.isEmpty())
    {
        const CActivityTrk::desc_t& desc = actDesc[i];
        QCheckBox * check = new QCheckBox(this);
        check->setText(desc.name);
        check->setIcon(QIcon(desc.iconLarge));
        check->setProperty("flag",   desc.flag);
        check->setProperty("name",   desc.name);
        check->setProperty("symbol", desc.iconLarge);
        check->setObjectName("check" + desc.objName);

        connect(check, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));

        layoutActivities->addWidget(check);

        i++;
    }
    layoutActivities->addItem(new QSpacerItem(0,0,QSizePolicy::Maximum, QSizePolicy::MinimumExpanding));

    // the first entry `solid color`, it is always available
    comboColorSource->addItem(QIcon("://icons/32x32/CSrcSolid.png"), "Solid color");
    foreach(const QString &key, trk.getExistingColorizeSources())
    {
        const CKnownExtension &ext = CKnownExtension::get(key);
        QIcon icon(ext.icon);
        comboColorSource->addItem(icon, ext.known ? ext.name : key, key);
    }

    setupGui();

    plot1 = new CPlotProfile(&trk, IPlot::eModeNormal, this);
    plot1->setMinimumSize(QSize(0, 100));
    plot1->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    plot1->show();
    layoutPlot->addWidget(plot1);

    plot2 = new CPlot(&trk, CPlotData::eAxisLinear
                        , tr("distance [%1]").arg(IUnit::self().baseunit)
                        , tr("speed. [%1]").arg(IUnit::self().speedunit)
                        , IUnit::self().speedfactor
                        , [](const CGisItemTrk::trkpt_t &p) {return p.distance; }
                        , [](const CGisItemTrk::trkpt_t &p) {return p.speed; }
                        , this);
    plot2->setLimits(0, NOFLOAT);
    plot2->setMinimumSize(QSize(0, 100));
    plot2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    plot2->show();
    layoutPlot->addWidget(plot2);

    plot3 = new CPlot(&trk, CPlotData::eAxisTime
                        , tr("time")
                        , tr("distance. [%1]").arg(IUnit::self().baseunit)
                        , IUnit::self().basefactor
                        , [](const CGisItemTrk::trkpt_t &p) {return p.time.toTime_t(); }
                        , [](const CGisItemTrk::trkpt_t &p) {return p.distance; }
                        , this);
    plot3->setMinimumSize(QSize(0, 100));
    plot3->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    plot3->show();
    layoutPlot->addWidget(plot3);

    const CGraphTrk * graphProperties = trk.getGraphProperties();
    graphProperties->fillComboBox(comboGraph2);
    graphProperties->fillComboBox(comboGraph3);


    if(trk.isOnDevice())
    {
        toolLock->setDisabled(true);
    }

    QTreeWidgetItem * item, * item0;
    item0 = new QTreeWidgetItem(treeFilter);
    item0->setIcon(0, QIcon("://icons/48x48/PointHide.png"));
    item0->setText(0, tr("Reduce visible track points"));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterDouglasPeuker(trk, treeFilter));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterReset(trk, treeFilter));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterDelete(trk, treeFilter));

    item0 = new QTreeWidgetItem(treeFilter);
    item0->setIcon(0, QIcon("://icons/48x48/SetEle.png"));
    item0->setText(0, tr("Change elevation of track points"));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterMedian(trk, treeFilter));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterReplaceElevation(trk, treeFilter));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterOffsetElevation(trk, treeFilter));

    item0 = new QTreeWidgetItem(treeFilter);
    item0->setIcon(0, QIcon("://icons/48x48/Time.png"));
    item0->setText(0, tr("Change timestamp of track points"));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterNewDate(trk, treeFilter));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterObscureDate(trk, treeFilter));

    item = new QTreeWidgetItem(item0);
    treeFilter->setItemWidget(item,0, new CFilterSpeed(trk, treeFilter));

    item0 = new QTreeWidgetItem(treeFilter);
    item0->setIcon(0, QIcon("://icons/48x48/TrkCut.png"));
    item0->setText(0, tr("Cut track into pieces"));

    SETTINGS;
    cfg.beginGroup("TrackDetails");
    checkGraph1->setChecked(cfg.value("showGraph1", true).toBool());
    checkGraph2->setChecked(cfg.value("showGraph2", true).toBool());
    checkGraph3->setChecked(cfg.value("showGraph3", true).toBool());
    splitter->restoreState   (cfg.value("splitterSizes").toByteArray());
    treeWidget->header()->restoreState(cfg.value("trackPointListState").toByteArray());
    cfg.endGroup();

    connect(checkGraph1, SIGNAL(clicked()), this, SLOT(slotShowPlots()));
    connect(checkGraph2, SIGNAL(clicked()), this, SLOT(slotShowPlots()));
    connect(checkGraph3, SIGNAL(clicked()), this, SLOT(slotShowPlots()));
    connect(comboColor,       SIGNAL(currentIndexChanged(int)),   this, SLOT(slotColorChanged(int)));
    connect(toolLock,         SIGNAL(toggled(bool)),              this, SLOT(slotChangeReadOnlyMode(bool)));
    connect(treeWidget,       SIGNAL(itemSelectionChanged()),     this, SLOT(slotItemSelectionChanged()));
    connect(textCmtDesc,      SIGNAL(anchorClicked(QUrl)),        this, SLOT(slotLinkActivated(QUrl)));
    connect(labelInfo,        SIGNAL(linkActivated(QString)),     this, SLOT(slotLinkActivated(QString)));

    connect(plot3, SIGNAL(sigMouseClickState(int)), this, SLOT(slotMouseClickState(int)));
    connect(plot1, SIGNAL(sigMouseClickState(int)), this, SLOT(slotMouseClickState(int)));
    connect(plot2, SIGNAL(sigMouseClickState(int)), this, SLOT(slotMouseClickState(int)));
    connect(comboColorSource, SIGNAL(currentIndexChanged(int)),   this, SLOT(slotColorSourceChanged(int)));
    connect(spinLimitHigh,    SIGNAL(valueChangedByStep(double)), this, SLOT(slotColorLimitHighChanged()));
    connect(spinLimitHigh,    SIGNAL(editingFinished()),          this, SLOT(slotColorLimitHighChanged()));
    connect(spinLimitLow,     SIGNAL(valueChangedByStep(double)), this, SLOT(slotColorLimitLowChanged()));
    connect(spinLimitLow,     SIGNAL(editingFinished()),          this, SLOT(slotColorLimitLowChanged()));

    connect(btnMaxFromData,   SIGNAL(clicked()),                  this, SLOT(slotLimitHighFromData()));
    connect(btnMinFromData,   SIGNAL(clicked()),                  this, SLOT(slotLimitLowFromData()));

    connect(listHistory,      SIGNAL(sigChanged()),               this, SLOT(setupGui()));

    slotShowPlots();
}

CDetailsTrk::~CDetailsTrk()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    cfg.setValue("showGraph1", checkGraph1->isChecked());
    cfg.setValue("showGraph2", checkGraph2->isChecked());
    cfg.setValue("showGraph3", checkGraph3->isChecked());
    cfg.setValue("splitterSizes",       splitter->saveState());
    cfg.setValue("trackPointListState", treeWidget->header()->saveState());
    cfg.endGroup();
}

void CDetailsTrk::slotLimitLowFromData()
{
    qreal min, max;
    trk.getExtrema(min, max, trk.getColorizeSource());
    spinLimitLow->setValue(min);
    slotColorLimitLowChanged();
}

void CDetailsTrk::slotLimitHighFromData()
{
    qreal min, max;
    trk.getExtrema(min, max, trk.getColorizeSource());
    spinLimitHigh->setValue(max);
    slotColorLimitHighChanged();
}

void CDetailsTrk::setupGui()
{
    if(originator)
    {
        return;
    }
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CDetailsTrk::setupGui");
    originator = true;

    QString str, val, unit;
    bool isReadOnly = trk.isReadOnly();

    tabWidget->widget(eTabFilter)->setEnabled(!isReadOnly);
    tabWidget->widget(eTabActivity)->setEnabled(!isReadOnly);
    tabWidget->widget(eTabStyle)->setEnabled(!isReadOnly);

    labelTainted->setVisible(trk.isTainted());

    labelInfo->setText(trk.getInfo(true));
    comboColor->setCurrentIndex(trk.getColorIdx());
    comboColor->setEnabled(!isReadOnly);
    toolLock->setChecked(isReadOnly);

    QList<QTreeWidgetItem*> items;
    const CGisItemTrk::trk_t& t = trk.getTrackData();
    foreach (const CGisItemTrk::trkseg_t& seg, t.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
        {
            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setTextAlignment(eColNum,     Qt::AlignLeft);
            item->setTextAlignment(eColEle,     Qt::AlignRight);
            item->setTextAlignment(eColDelta,   Qt::AlignRight);
            item->setTextAlignment(eColDist,    Qt::AlignRight);
            item->setTextAlignment(eColAscend,  Qt::AlignRight);
            item->setTextAlignment(eColDescend, Qt::AlignRight);
            item->setTextAlignment(eColSpeed,   Qt::AlignRight);

            QBrush b( trkpt.flags & CGisItemTrk::trkpt_t::eHidden ? Qt::gray : Qt::black );
            for(int i = 0; i < eColMax; i++)
            {
                item->setForeground(i, b);
            }

            item->setText(eColNum,QString::number(trkpt.idxTotal));
            if(trkpt.time.isValid())
            {
                item->setText(eColTime, IUnit::self().datetime2string(trkpt.time, true, QPointF(trkpt.lon, trkpt.lat)*DEG_TO_RAD));
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

            if(trkpt.slope1 != NOFLOAT)
            {
                str = QString("%1°(%2%)").arg(trkpt.slope1,2,'f',0).arg(trkpt.slope2,2,'f',0);
            }
            else
            {
                str = "-";
            }
            item->setText(eColSlope,str);

            IUnit::self().meter2elevation(trkpt.ascend, val, unit);
            item->setText(eColAscend, tr("%1 %2").arg(val).arg(unit));
            IUnit::self().meter2elevation(trkpt.descend, val, unit);
            item->setText(eColDescend, tr("%1 %2").arg(val).arg(unit));

            // position
            IUnit::degToStr(trkpt.lon, trkpt.lat, str);
            item->setText(eColPosition,str);

            items << item;
        }
    }

    treeWidget->clear();
    treeWidget->addTopLevelItems(items);
    treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, trk.getComment(), trk.getDescription(), trk.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start);
    textCmtDesc->ensureCursorVisible();

    quint32 flags = trk.getActivities().getAllFlags();

    int i = 0;
    const CActivityTrk::desc_t* actDesc = CActivityTrk::getActivityDescriptors();
    while(!actDesc[i].objName.isEmpty())
    {
        const CActivityTrk::desc_t& desc = actDesc[i];

        QCheckBox * check = findChild<QCheckBox*>("check" + desc.objName);
        if(check)
        {
            check->setChecked((flags & desc.flag) == desc.flag);
        }

        i++;
    }

    str.clear();
    trk.getActivities().printSummary(str);
    labelActivityInfo->setText(str);

    bool hasActivity = 0 != (flags & CGisItemTrk::trkpt_t::eActMask);
    labelActivityHelp->setVisible(!hasActivity);
    labelActivityInfo->setVisible(hasActivity);

    plotTrack->setTrack(&trk);
    listHistory->setupHistory(trk);

    QTabWidget * tabWidget = dynamic_cast<QTabWidget*>(parentWidget() ? parentWidget()->parentWidget() : 0);
    if(tabWidget)
    {
        int idx = tabWidget->indexOf(this);
        if(idx != NOIDX)
        {
            setObjectName(trk.getName());
            tabWidget->setTabText(idx, trk.getName());
        }
    }

    int currentIdx = comboColorSource->findData(trk.getColorizeSource());
    if(-1 == currentIdx)
    {
        currentIdx = 0;
    }

    bool enabled = (0 < currentIdx);

    spinLimitLow->setEnabled    (enabled);
    spinLimitHigh->setEnabled   (enabled);
    widgetColorLabel->setEnabled(enabled);
    btnMinFromData->setEnabled  (enabled);
    btnMaxFromData->setEnabled  (enabled);

    comboColorSource->blockSignals(true);
    comboColorSource->setCurrentIndex(currentIdx);
    comboColorSource->blockSignals(false);

    if(enabled)
    {
        const CKnownExtension &ext = CKnownExtension::get(comboColorSource->itemData(currentIdx).toString());

        spinLimitLow->blockSignals(true);
        spinLimitLow->setMinimum(ext.minimum);
        spinLimitLow->setMaximum(ext.maximum);
        spinLimitLow->setSuffix (ext.unit);
        spinLimitLow->setValue  (trk.getColorizeLimitLow());
        spinLimitLow->blockSignals(false);

        spinLimitHigh->blockSignals(true);
        spinLimitHigh->setMinimum(ext.minimum);
        spinLimitHigh->setMaximum(ext.maximum);
        spinLimitHigh->setSuffix (ext.unit);
        spinLimitHigh->setValue  (trk.getColorizeLimitHigh());
        spinLimitHigh->blockSignals(false);

        widgetColorLabel->setMinimum(spinLimitLow->value());
        widgetColorLabel->setMaximum(spinLimitHigh->value());
        widgetColorLabel->setUnit(ext.unit);
    }

    originator = false;
    CCanvas::restoreOverrideCursor("CDetailsTrk::setupGui");
}

void CDetailsTrk::setMouseFocus(const CGisItemTrk::trkpt_t * pt)
{
    if(pt != 0)
    {
        plotTrack->setMouseFocus(pt->lon, pt->lat);
        labelInfoTrkPt->setText(trk.getInfoTrkPt(*pt));
        labelInfoProgress->setText(trk.getInfoProgress(*pt));
    }
    else
    {
        labelInfoTrkPt->setText("-\n-");
        labelInfoProgress->setText("-\n-");
    }
}

void CDetailsTrk::setMouseRangeFocus(const CGisItemTrk::trkpt_t * pt1, const CGisItemTrk::trkpt_t * pt2)
{
    if(pt1 && pt2)
    {
        labelInfoRange->setText(trk.getInfoRange(*pt1, *pt2));
    }
    else
    {
        labelInfoRange->setText("-\n-");
    }
}

void CDetailsTrk::setMouseClickFocus(const CGisItemTrk::trkpt_t * pt)
{
    if(pt != 0)
    {
        treeWidget->blockSignals(true);
        treeWidget->setCurrentItem(treeWidget->topLevelItem(pt->idxTotal));
        treeWidget->blockSignals(false);
    }
}

void CDetailsTrk::slotMouseClickState(int s)
{
    if(s == IPlot::eMouseClickIdle)
    {
        labelInfoRange->setText("-\n-");
        plot3->setMouseRangeFocus(0,0);
        plot1->setMouseRangeFocus(0,0);
        plot2->setMouseRangeFocus(0,0);
    }
}

void CDetailsTrk::slotShowPlots()
{
    plot1->setVisible(checkGraph1->isChecked());
    plot2->setVisible(checkGraph2->isChecked());
    plot3->setVisible(checkGraph3->isChecked());
}

void CDetailsTrk::slotColorChanged(int idx)
{
    if(trk.getColorIdx() != idx)
    {
        trk.setColor(idx);
    }
}

void CDetailsTrk::slotColorSourceChanged(int idx, float valueLow, float valueHigh)
{
    trk.setColorizeSource(comboColorSource->itemData(idx).toString());
    setupGui();
}

void CDetailsTrk::slotColorLimitHighChanged()
{
    const double val = spinLimitHigh->value();
    trk.setColorizeLimitHigh(val);
    widgetColorLabel->setMaximum(val);

    if(spinLimitLow->value() >= val)
    {
        spinLimitLow->setValue(val - .1f);
    }
}

void CDetailsTrk::slotColorLimitLowChanged()
{
    const double val = spinLimitLow->value();
    trk.setColorizeLimitLow(val);
    widgetColorLabel->setMinimum(val);

    if(spinLimitHigh->value() <= val)
    {
        spinLimitHigh->setValue(val + .1f);
    }
}

void CDetailsTrk::slotChangeReadOnlyMode(bool on)
{
    trk.setReadOnlyMode(on);
    setupGui();
}


void CDetailsTrk::slotItemSelectionChanged()
{
    QTreeWidgetItem * item = treeWidget->currentItem();
    if(item != 0)
    {
        quint32 idx = item->text(eColNum).toUInt();
        trk.setMouseFocusByTotalIndex(idx, CGisItemTrk::eFocusMouseMove, "CDetailsTrk");
    }
}

void CDetailsTrk::slotLinkActivated(const QString& url)
{
    if(url == "name")
    {
        QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new track name."), QLineEdit::Normal, trk.getName());
        if(name.isEmpty())
        {
            return;
        }
        trk.setName(name);
        setupGui();
    }
}

void CDetailsTrk::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "comment")
    {
        CTextEditWidget dlg(this);
        dlg.setHtml(trk.getComment());
        if(dlg.exec() == QDialog::Accepted)
        {
            trk.setComment(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(this);
        dlg.setHtml(trk.getDescription());
        if(dlg.exec() == QDialog::Accepted)
        {
            trk.setDescription(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "links")
    {
        QList<IGisItem::link_t> links = trk.getLinks();
        CLinksDialog dlg(links, this);
        if(dlg.exec() == QDialog::Accepted)
        {
            trk.setLinks(links);
        }
        setupGui();
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}

void CDetailsTrk::slotActivitySelected(bool checked)
{
    if(!checked)
    {
        if(QMessageBox::warning(this, tr("Reset activities..."), tr("This will remove all activities from the track. Proceed?"), QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok) != QMessageBox::Ok)
        {
            setupGui();
            return;
        }

        trk.setActivity(CGisItemTrk::trkpt_t::eActNone, tr("None"), "://icons/48x48/ActNone.png");
        return;
    }

    QObject * s = sender();
    bool ok = false;
    quint32 flag = s->property("flag").toUInt(&ok);
    if(ok)
    {
        trk.setActivity(flag, s->property("name").toString(), s->property("symbol").toString());
    }
}
