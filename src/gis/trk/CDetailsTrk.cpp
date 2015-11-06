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
#include "units/IUnit.h"
#include "widgets/CTextEditWidget.h"
#include "widgets/CColorLegend.h"

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

    setupGui();

    plotElevation->setTrack(&trk);
    plotDistance->setTrack(&trk);
    plotSpeed->setTrack(&trk);

    // add all available slope sources to the corresponding combobox
    bool haveDisabledEntry = false;
    std::array<bool, TRK_N_COLORIZESOURCES> existingSources = trk.getExistingKnownColorizeSources();

    // never disable the first entry `static color`, it is always available
    comboColorSource->addItem(QIcon("://icons/32x32/Tainted.png"), "Static color");
    for(size_t i = 0; i < TRK_N_COLORIZESOURCES; i++)
    {
        QIcon icon(CGisItemTrk::colorizeSource[i].icon);
        comboColorSource->addItem(icon, CGisItemTrk::colorizeSource[i].name, CGisItemTrk::colorizeSource[i].intName);

        if(!existingSources[i])
        {
            comboColorSource->setItemData(i + 1, false, Qt::UserRole - 1);
            haveDisabledEntry = true;
        }
    }

    foreach(const QString &key, trk.getExistingUnknownColorizeSources())
    {
        QIcon icon("://icons/32x32/CSrcUnknown.png");
        comboColorSource->addItem(icon, key, key);
    }

    // show a message as soon as entries have been disabled
    if(haveDisabledEntry)
    {
        labelDisabledEntries->setText("Some sources are disabled due to missing data.");
    }

    int currentIdx = comboColorSource->findData(trk.getColorizeSource());
    if(-1 == currentIdx) currentIdx = 0;
    comboColorSource->setCurrentIndex(currentIdx);
    slotColorSourceChanged(currentIdx, trk.getColorizeLimitLow(), trk.getColorizeLimitHigh());

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
    checkProfile->setChecked (cfg.value("showProfile",  true).toBool());
    checkSpeed->setChecked   (cfg.value("showSpeed",    true).toBool());
    checkProgress->setChecked(cfg.value("showProgress", true).toBool());
    splitter->restoreState   (cfg.value("splitterSizes").toByteArray());
    treeWidget->header()->restoreState(cfg.value("trackPointListState").toByteArray());
    cfg.endGroup();

    connect(checkProfile,     SIGNAL(clicked()),                  this, SLOT(slotShowPlots()));
    connect(checkSpeed,       SIGNAL(clicked()),                  this, SLOT(slotShowPlots()));
    connect(checkProgress,    SIGNAL(clicked()),                  this, SLOT(slotShowPlots()));
    connect(comboColor,       SIGNAL(currentIndexChanged(int)),   this, SLOT(slotColorChanged(int)));
    connect(toolLock,         SIGNAL(toggled(bool)),              this, SLOT(slotChangeReadOnlyMode(bool)));
    connect(treeWidget,       SIGNAL(itemSelectionChanged()),     this, SLOT(slotItemSelectionChanged()));
    connect(textCmtDesc,      SIGNAL(anchorClicked(QUrl)),        this, SLOT(slotLinkActivated(QUrl)));
    connect(labelInfo,        SIGNAL(linkActivated(QString)),     this, SLOT(slotLinkActivated(QString)));

    connect(comboColorSource, SIGNAL(currentIndexChanged(int)),   this, SLOT(slotColorSourceChanged(int)));
    connect(spinLimitHigh,    SIGNAL(valueChangedByStep(double)), this, SLOT(slotColorLimitHighChanged()));
    connect(spinLimitHigh,    SIGNAL(editingFinished()),          this, SLOT(slotColorLimitHighChanged()));
    connect(spinLimitLow,     SIGNAL(valueChangedByStep(double)), this, SLOT(slotColorLimitLowChanged()));
    connect(spinLimitLow,     SIGNAL(editingFinished()),          this, SLOT(slotColorLimitLowChanged()));

    connect(btnMaxFromData,   SIGNAL(clicked()),                  this, SLOT(slotLimitHighFromData()));
    connect(btnMinFromData,   SIGNAL(clicked()),                  this, SLOT(slotLimitLowFromData()));

    connect(plotDistance,     SIGNAL(sigMouseClickState(int)),    this, SLOT(slotMouseClickState(int)));
    connect(plotElevation,    SIGNAL(sigMouseClickState(int)),    this, SLOT(slotMouseClickState(int)));
    connect(plotSpeed,        SIGNAL(sigMouseClickState(int)),    this, SLOT(slotMouseClickState(int)));

    connect(listHistory,      SIGNAL(sigChanged()),               this, SLOT(setupGui()));

    slotShowPlots();
}

CDetailsTrk::~CDetailsTrk()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    cfg.setValue("showProfile",         checkProfile->isChecked());
    cfg.setValue("showSpeed",           checkSpeed->isChecked());
    cfg.setValue("showProgress",        checkProgress->isChecked());
    cfg.setValue("splitterSizes",       splitter->saveState());
    cfg.setValue("trackPointListState", treeWidget->header()->saveState());
    cfg.endGroup();
}

void CDetailsTrk::slotLimitLowFromData()
{
    float min = trk.getExtremum(false);
    spinLimitLow->setValue(min);
    slotColorLimitLowChanged();
}

void CDetailsTrk::slotLimitHighFromData()
{
    float max = trk.getExtremum(true);
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

    tabWidget->widget(3)->setEnabled(!isReadOnly);
    tabWidget->widget(1)->setEnabled(!isReadOnly);

    if(trk.isTainted())
    {
        labelTainted->show();
    }
    else
    {
        labelTainted->hide();
    }

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
            item->setTextAlignment(eColNum,Qt::AlignLeft);
            item->setTextAlignment(eColEle,Qt::AlignRight);
            item->setTextAlignment(eColDelta,Qt::AlignRight);
            item->setTextAlignment(eColDist,Qt::AlignRight);
            item->setTextAlignment(eColAscend,Qt::AlignRight);
            item->setTextAlignment(eColDescend,Qt::AlignRight);
            item->setTextAlignment(eColSpeed,Qt::AlignRight);


            if(trkpt.flags & CGisItemTrk::trkpt_t::eHidden)
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

    if((flags & CGisItemTrk::trkpt_t::eActMask) == 0)
    {
        labelActivityHelp->show();
        labelActivityInfo->hide();
    }
    else
    {
        labelActivityHelp->hide();
        labelActivityInfo->show();
    }

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
        plotDistance->setMouseRangeFocus(0,0);
        plotElevation->setMouseRangeFocus(0,0);
        plotSpeed->setMouseRangeFocus(0,0);
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
}

void CDetailsTrk::slotColorSourceChanged(int idx, float valueLow, float valueHigh)
{
    // index 0 in the combobox: `static color`.
    // the static color is only used if the colorizeSource is set to -1
    // as soon as colorizeSource is set >= 0 the "real" source is used
    const int idxInSource = idx - 1;

    spinLimitLow->setEnabled    (-1 != idxInSource);
    spinLimitHigh->setEnabled   (-1 != idxInSource);
    widgetColorLabel->setEnabled(-1 != idxInSource);
    btnMinFromData->setEnabled  (-1 != idxInSource);
    btnMaxFromData->setEnabled  (-1 != idxInSource);

    const struct CGisItemTrk::ColorizeSource &source = (
        -1 != idxInSource && idxInSource < TRK_N_COLORIZESOURCES ? CGisItemTrk::colorizeSource[idxInSource] : CGisItemTrk::unknownColorizeSource
    );

    // update the limits
    spinLimitLow->setMinimum(source.minimum);
    spinLimitLow->setMaximum(source.maximum);
    spinLimitLow->setValue  (HUGE_VALF != valueLow  ? valueLow  : source.defLimitLow);
    spinLimitLow->setSuffix (source.unit);

    spinLimitHigh->setMinimum(source.minimum);
    spinLimitHigh->setMaximum(source.maximum);
    spinLimitHigh->setValue  (HUGE_VALF != valueHigh ? valueHigh : source.defLimitHigh);
    spinLimitHigh->setSuffix (source.unit);

    widgetColorLabel->setMinimum(spinLimitLow->value());
    widgetColorLabel->setMaximum(spinLimitHigh->value());
    widgetColorLabel->setUnit(source.unit);

    trk.setColorizeSource(comboColorSource->itemData(idx).toString());
}

void CDetailsTrk::slotColorLimitHighChanged()
{
    const double val = spinLimitHigh->value();
    trk.setColorizeLimitHigh(val);
    widgetColorLabel->setMaximum(val);

    if(spinLimitLow->value() >= val)
    {
        spinLimitLow->setValue(val - 1.f);
    }
}

void CDetailsTrk::slotColorLimitLowChanged()
{
    const double val = spinLimitLow->value();
    trk.setColorizeLimitLow(val);
    widgetColorLabel->setMinimum(val);

    if(spinLimitHigh->value() <= val)
    {
        spinLimitHigh->setValue(val + 1.f);
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
