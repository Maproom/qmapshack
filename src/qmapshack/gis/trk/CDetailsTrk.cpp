/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/proj_x.h"
#include "gis/trk/CDetailsTrk.h"
#include "gis/trk/CEnergyCyclingDialog.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CPropertyTrk.h"
#include "gis/trk/filter/CFilterChangeStartPoint.h"
#include "gis/trk/filter/CFilterDelete.h"
#include "gis/trk/filter/CFilterDeleteExtension.h"
#include "gis/trk/filter/CFilterDouglasPeuker.h"
#include "gis/trk/filter/CFilterInterpolateElevation.h"
#include "gis/trk/filter/CFilterInvalid.h"
#include "gis/trk/filter/CFilterLoopsCut.h"
#include "gis/trk/filter/CFilterMedian.h"
#include "gis/trk/filter/CFilterNewDate.h"
#include "gis/trk/filter/CFilterObscureDate.h"
#include "gis/trk/filter/CFilterOffsetElevation.h"
#include "gis/trk/filter/CFilterReplaceElevation.h"
#include "gis/trk/filter/CFilterReset.h"
#include "gis/trk/filter/CFilterSpeed.h"
#include "gis/trk/filter/CFilterSplitSegment.h"
#include "gis/trk/filter/CFilterSplitTrack.h"
#include "gis/trk/filter/CFilterSubPt2Pt.h"
#include "gis/trk/filter/CFilterTerrainSlope.h"
#include "gis/trk/filter/CFilterZeroSpeedDriftCleaner.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CSettings.h"
#include "helpers/Signals.h"
#include "plot/CPlot.h"
#include "plot/CPlotProfile.h"
#include "units/IUnit.h"
#include "widgets/CTextEditWidget.h"

#include <QtWidgets>

/* base case: add the filter specified in template parameter */
template<typename filter>
static void addFilters(QTreeWidgetItem* itemGroup, CGisItemTrk& trk, qint32& minWidth)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(itemGroup);
    itemGroup->treeWidget()->setItemWidget(item, /* column = */ 0, new filter(trk, itemGroup->treeWidget()));
    minWidth = qMax(minWidth, itemGroup->treeWidget()->itemWidget(item, 0)->sizeHint().width());
}

template<typename filter1, typename filter2, typename ... remainingFilters>
static void addFilters(QTreeWidgetItem* itemGroup, CGisItemTrk& trk, qint32& minWidth)
{
    addFilters<filter1>(itemGroup, trk, minWidth);
    addFilters<filter2, remainingFilters ...>(itemGroup, trk, minWidth);
}

template<typename ... filters>
static void addFilterGroup(QTreeWidget* widget, CGisItemTrk& trk, const QString& groupText, const QString& groupIcon, qint32& minWidth)
{
    QTreeWidgetItem* itemGroup = new QTreeWidgetItem(widget);
    itemGroup->setIcon(/* column = */ 0, QIcon(groupIcon));
    itemGroup->setText(/* column = */ 0, groupText);

    addFilters<filters ...>(itemGroup, trk, minWidth);
}

CDetailsTrk::CDetailsTrk(CGisItemTrk& trk)
    : INotifyTrk(CGisItemTrk::eVisualDetails)
    , trk(trk)
{
    setupUi(this);
    QPixmap icon(14, 14);
    const int N = IGisItem::getColorMap().count();
    for(int i = 0; i < N; ++i)
    {
        const IGisItem::color_t& color = IGisItem::getColorMap()[i];
        icon.fill(color.color);
        comboColor->addItem(icon, color.label, color.color);
    }

    labelHelpTrackPointInfo->setText(tr( "<b>Track Point Information</b><br/>"
                                         "To document your tracks you can add a short description to a track "
                                         "point. All points with description will be marked along the track "
                                         "with auto-numbered bullets.<br/>"
                                         "You can add a description by a right click on a track point in the graphs. "
                                         "Or you do a left click on a track point in the map view.<br/>"
                                         "With the first track point you add information to this text will be replaced "
                                         "by a table. Use it to edit and delete track point information.<br/>"
                                         ));

    widgetColorLayout->setAlignment(Qt::AlignTop);

    widgetColorActivity->setTrack(&trk);

    connect(treeTrackPointInfo, &CTableTrkInfo::sigHasTrkPtInfo, this, &CDetailsTrk::slotHasTrkPtInfo);
    treeTrackPoint->setTrack(&trk);
    treeTrackPointInfo->setTrack(&trk);

    plot1 = new CPlotProfile(&trk, trk.limitsGraph1, IPlot::eModeNormal, this);
    plot2 = new CPlot(&trk, trk.limitsGraph2, IPlot::eModeNormal, this);
    plot3 = new CPlot(&trk, trk.limitsGraph3, IPlot::eModeNormal, this);

    for(IPlot* plot : { static_cast<IPlot*>(plot1), static_cast<IPlot*>(plot2), static_cast<IPlot*>(plot3) })
    {
        plot->setMinimumSize(QSize(0, 100));
        plot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        plot->show();
        framePlot->layout()->addWidget(plot);

        const CTrackData::trkpt_t* ptRange1 = nullptr, * ptRange2 = nullptr;
        trk.getMouseRange(ptRange1, ptRange2);
        plot->setMouseRangeFocus(ptRange1, ptRange2);
    }

    if(trk.isOnDevice())
    {
        toolLock->setDisabled(true);
    }

    SETTINGS;
    cfg.beginGroup("TrackDetails");
    checkGraph1->setChecked(cfg.value("showGraph1", true).toBool());
    checkGraph2->setChecked(cfg.value("showGraph2", true).toBool());
    checkGraph3->setChecked(cfg.value("showGraph3", true).toBool());
    splitter1->restoreState (cfg.value("splitterSizes1").toByteArray());
    splitter2->restoreState (cfg.value("splitterSizes2").toByteArray());
    splitter3->restoreState (cfg.value("splitterSizes3").toByteArray());
    tabWidget->setCurrentIndex(cfg.value("visibleTab", 0).toInt());
    cfg.endGroup();

    connect(checkGraph1, &QCheckBox::clicked, this, &CDetailsTrk::slotShowPlots);
    connect(checkGraph2, &QCheckBox::clicked, this, &CDetailsTrk::slotShowPlots);
    connect(checkGraph3, &QCheckBox::clicked, this, &CDetailsTrk::slotShowPlots);

    connect(toolLock, &QToolButton::toggled, this, &CDetailsTrk::slotChangeReadOnlyMode);
    connect(textCmtDesc, &QTextBrowser::anchorClicked, this, &CDetailsTrk::slotLinkActivated);

    connect(pushSetActivities, &QPushButton::clicked, this, &CDetailsTrk::slotSetActivities);
    connect(toolSetEnergyCycling, &QPushButton::clicked, this, &CDetailsTrk::slotSetEnergyCycling);

    connect(lineName, &QLineEdit::textEdited, this, &CDetailsTrk::slotNameChanged);
    connect(lineName, &QLineEdit::editingFinished, this, &CDetailsTrk::slotNameChangeFinished);

    connect(plot1, &CPlot::sigMouseClickState, this, &CDetailsTrk::slotMouseClickState);
    connect(plot2, &CPlot::sigMouseClickState, this, &CDetailsTrk::slotMouseClickState);
    connect(plot3, &CPlot::sigMouseClickState, this, &CDetailsTrk::slotMouseClickState);

    connect(spinLineWidth, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CDetailsTrk::slotLineWidth);
    connect(checkWithArrows, &QCheckBox::toggled, this, &CDetailsTrk::slotWithArrows);

    setupStyleLimits(trk.colorSourceLimit, toolLimitAutoStyle, toolLimitUsrStyle, toolLimitSysStyle, spinLimitLow, spinLimitHigh);

    connect(toolUserLineWith, &QToolButton::toggled, this, &CDetailsTrk::slotLineWidthMode);
    connect(toolUserArrow, &QToolButton::toggled, this, &CDetailsTrk::slotWithArrowsMode);

    connect(listHistory, &CHistoryListWidget::sigChanged, this, &CDetailsTrk::updateData);

    connect(comboColor, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsTrk::slotColorChanged);
    connect(comboColorSource, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsTrk::slotColorSourceChanged);
    connect(comboGraph2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsTrk::slotSetupGraph);
    connect(comboGraph3, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsTrk::slotSetupGraph);

    setupGraphLimits(trk.limitsGraph1, toolLimitAutoGraph1, toolLimitUsrGraph1, toolLimitSysGraph1, spinMinGraph1, spinMaxGraph1);
    setupGraphLimits(trk.limitsGraph2, toolLimitAutoGraph2, toolLimitUsrGraph2, toolLimitSysGraph2, spinMinGraph2, spinMaxGraph2);
    setupGraphLimits(trk.limitsGraph3, toolLimitAutoGraph3, toolLimitUsrGraph3, toolLimitSysGraph3, spinMinGraph3, spinMaxGraph3);

    // setting up the graph properties will trigger the signals
    // this is good because the signals are connected at this point
    // invoking the slots
    loadGraphSource(comboGraph2, 2, CKnownExtension::internalSpeedDist);
    loadGraphSource(comboGraph3, 3, CKnownExtension::internalProgress);

    // this will the largest minimum width of all filter widgets
    qint32 minWidth = 0;
    addFilterGroup<CFilterDouglasPeuker, CFilterZeroSpeedDriftCleaner, CFilterInvalid, CFilterReset, CFilterDelete>
        (treeFilter, trk, tr("Reduce visible track points"), "://icons/48x48/PointHide.png", minWidth);

    addFilterGroup<CFilterMedian, CFilterInterpolateElevation, CFilterReplaceElevation, CFilterOffsetElevation>
        (treeFilter, trk, tr("Change elevation of track points"), "://icons/48x48/SetEle.png", minWidth);

    addFilterGroup<CFilterNewDate, CFilterObscureDate, CFilterSpeed>
        (treeFilter, trk, tr("Change timestamp of track points"), "://icons/48x48/Time.png", minWidth);

    addFilterGroup<CFilterDeleteExtension, CFilterSplitSegment, CFilterSubPt2Pt, CFilterTerrainSlope, CFilterChangeStartPoint, CFilterLoopsCut, CFilterSplitTrack>
        (treeFilter, trk, tr("Miscellaneous"), "://icons/48x48/CSrcUnknown.png", minWidth);

    // limit tree widget horizontal size to the filter widget with the largest minimum size
    treeFilter->setMinimumWidth(minWidth + treeFilter->indentation());

    slotShowPlots();
}

CDetailsTrk::~CDetailsTrk()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    cfg.setValue("showGraph1", checkGraph1->isChecked());
    cfg.setValue("showGraph2", checkGraph2->isChecked());
    cfg.setValue("showGraph3", checkGraph3->isChecked());
    cfg.setValue("splitterSizes1", splitter1->saveState());
    cfg.setValue("splitterSizes2", splitter2->saveState());
    cfg.setValue("splitterSizes3", splitter3->saveState());
    cfg.setValue("visibleTab", tabWidget->currentIndex());
    cfg.endGroup();

    saveGraphSource(comboGraph2, 2);
    saveGraphSource(comboGraph3, 3);

    // Note: Maybe we have a race condition here.
    // Usually the QPointer to "dlgDetails" should be cleared if the object is destroyed.
    trk.clearDlgDetails();
}

void CDetailsTrk::slotHasTrkPtInfo(bool yes)
{
    labelHelpTrackPointInfo->setVisible(!yes);
    treeTrackPointInfo->setVisible(yes);
}

void CDetailsTrk::slotSetLimitModeStyle(CLimit::mode_e mode, bool on)
{
    if(!on)
    {
        return;
    }

    CLimit& limit = trk.colorSourceLimit;
    limit.setMode(mode);

    widgetColorLabel->setMinimum(limit.getMin());
    widgetColorLabel->setMaximum(limit.getMax());

    trk.updateHistory(CGisItemTrk::eVisualColorLegend | CGisItemTrk::eVisualDetails);
}

void CDetailsTrk::setupGraphLimits(CLimit& limit, QToolButton* toolLimitAutoGraph, QToolButton* toolLimitUsrGraph, QToolButton* toolLimitSysGraph, QDoubleSpinBox* spinMinGraph, QDoubleSpinBox* spinMaxGraph)
{
    bool isAutoMode = (limit.getMode() == CLimit::eModeAuto);

    spinMinGraph->setDisabled(isAutoMode);
    spinMinGraph->setSuffix(limit.getUnit());
    spinMinGraph->setValue(limit.getMin());

    spinMaxGraph->setDisabled(isAutoMode);
    spinMaxGraph->setSuffix(limit.getUnit());
    spinMaxGraph->setValue(limit.getMax());

    switch(limit.getMode())
    {
    case CLimit::eModeUser:
        toolLimitUsrGraph->setChecked(true);
        break;

    case CLimit::eModeAuto:
        toolLimitAutoGraph->setChecked(true);
        break;

    case CLimit::eModeSys:
        toolLimitSysGraph->setChecked(true);
        break;
    }

    connect(toolLimitAutoGraph, &QToolButton::toggled, spinMinGraph, &QDoubleSpinBox::setDisabled);
    connect(toolLimitAutoGraph, &QToolButton::toggled, spinMaxGraph, &QDoubleSpinBox::setDisabled);

    auto limitAutoFunc = [this, &limit, spinMinGraph, spinMaxGraph](bool checked){slotSetLimitModeGraph(CLimit::eModeAuto, &limit, spinMinGraph, spinMaxGraph, checked);};
    auto limitUserFunc = [this, &limit, spinMinGraph, spinMaxGraph](bool checked){slotSetLimitModeGraph(CLimit::eModeUser, &limit, spinMinGraph, spinMaxGraph, checked);};
    auto limitSysFunc = [this, &limit, spinMinGraph, spinMaxGraph](bool checked){slotSetLimitModeGraph(CLimit::eModeSys, &limit, spinMinGraph, spinMaxGraph, checked);};

    connect(toolLimitAutoGraph, &QToolButton::toggled, this, limitAutoFunc);
    connect(toolLimitUsrGraph, &QToolButton::toggled, this, limitUserFunc);
    connect(toolLimitSysGraph, &QToolButton::toggled, this, limitSysFunc);

    connect(spinMinGraph, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), &limit, &CLimit::setMin);
    connect(spinMaxGraph, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), &limit, &CLimit::setMax);

    auto limitChangedFunc = [this, &limit, spinMinGraph, spinMaxGraph](){setupLimits(&limit, spinMinGraph, spinMaxGraph);};
    connect(&limit, &CLimit::sigChanged, this, limitChangedFunc);
}

void CDetailsTrk::setupStyleLimits(CLimit& limit, QToolButton* toolLimitAuto, QToolButton* toolLimitUsr, QToolButton* toolLimitSys, CDoubleSpinBox* spinMin, CDoubleSpinBox* spinMax)
{
    bool isAutoMode = (limit.getMode() == CLimit::eModeAuto);

    spinMin->setDisabled(isAutoMode);
    spinMin->setSuffix(limit.getUnit());
    spinMin->setValue(limit.getMin());

    spinMax->setDisabled(isAutoMode);
    spinMax->setSuffix(limit.getUnit());
    spinMax->setValue(limit.getMax());

    switch(limit.getMode())
    {
    case CLimit::eModeUser:
        toolLimitUsr->setChecked(true);
        break;

    case CLimit::eModeAuto:
        toolLimitAuto->setChecked(true);
        break;

    case CLimit::eModeSys:
        toolLimitSys->setChecked(true);
        break;
    }

    connect(toolLimitAuto, &QToolButton::toggled, spinMax, &QDoubleSpinBox::setDisabled);
    connect(toolLimitAuto, &QToolButton::toggled, spinMin, &QDoubleSpinBox::setDisabled);

    connect(toolLimitAuto, &QToolButton::toggled, this, [this](bool checked){slotSetLimitModeStyle(CLimit::eModeAuto, checked);});
    connect(toolLimitUsr, &QToolButton::toggled, this, [this](bool checked){slotSetLimitModeStyle(CLimit::eModeUser, checked);});
    connect(toolLimitSys, &QToolButton::toggled, this, [this](bool checked){slotSetLimitModeStyle(CLimit::eModeSys, checked);});

    connect(spinMax, &CDoubleSpinBox::valueChangedByStep, this, &CDetailsTrk::slotColorLimitHighChanged);
    connect(spinMax, &CDoubleSpinBox::editingFinished, this, &CDetailsTrk::slotColorLimitHighChanged);
    connect(spinMin, &CDoubleSpinBox::valueChangedByStep, this, &CDetailsTrk::slotColorLimitLowChanged);
    connect(spinMin, &CDoubleSpinBox::editingFinished, this, &CDetailsTrk::slotColorLimitLowChanged);

    connect(&limit, &CLimit::sigChanged, this, [this, &limit, spinMin, spinMax] {setupLimits(&limit, spinMin, spinMax);});
}

void CDetailsTrk::loadGraphSource(QComboBox* comboBox, qint32 n, const QString cfgDefault)
{
    const CPropertyTrk* p = trk.getPropertyHandler();
    if(p == nullptr)
    {
        return;
    }

    comboBox->blockSignals(true);
    p->fillComboBox(comboBox);
    comboBox->blockSignals(false);

    // try to restore last graph setup
    // signals are unblocked by now changing the combobox will trigger a graph update
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    int i = comboBox->findData(cfg.value(QString("propGraph%1").arg(n), cfgDefault).toString());
    if(i != NOIDX)
    {
        comboBox->setCurrentIndex(i);
    }
    cfg.endGroup();
}

void CDetailsTrk::saveGraphSource(QComboBox* comboBox, qint32 n)
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    if(comboBox->currentIndex() != 0)
    {
        cfg.setValue(QString("propGraph%1").arg(n), comboBox->currentData().toString());
    }
    cfg.endGroup();
}


void CDetailsTrk::updateData()
{
    if(originator)
    {
        return;
    }
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
    originator = true;

    bool isReadOnly = trk.isReadOnly();
    bool isNogo = trk.isNogo();

    frameTrackInfo->setVisible(CMainWindow::self().isShowTrackInfoPoints());

    tabWidget->widget(eTabFilter)->setEnabled(!isReadOnly);

    labelTainted->setVisible(trk.isTainted());

    labelInfo->setText(trk.getInfo(IGisItem::eFeatureNone));
    comboColor->setCurrentIndex(trk.getColorIdx());
    toolLock->setChecked(isReadOnly);
    labelNogo->setVisible(isNogo);

    lineName->setText(trk.getName());
    lineName->setReadOnly(isReadOnly);

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, trk.getComment(), trk.getDescription(), trk.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start);
    textCmtDesc->ensureCursorVisible();

    QString str;
    trk.getActivities().printSummary(str);
    labelActivityInfo->setText(str);

    bool hasActivity = !trk.getActivities().getAllActivities().isEmpty();
    labelActivityHelp->setVisible(!hasActivity);
    labelActivityInfo->setVisible(hasActivity);
    pushSetActivities->setEnabled(!isReadOnly);

    plotTrack->setTrack(&trk);
    listHistory->setupHistory(trk);

    QTabWidget* tabWidget_ = dynamic_cast<QTabWidget*>(parentWidget() ? parentWidget()->parentWidget() : nullptr);
    if(nullptr != tabWidget_)
    {
        int idx = tabWidget_->indexOf(this);
        if(idx != NOIDX)
        {
            setObjectName(trk.getName());
            tabWidget_->setTabText(idx, trk.getName());
        }
    }

    plot1->updateData();
    plot2->updateData();
    plot3->updateData();

    X______________BlockAllSignals______________X(this);

    spinLineWidth->setValue(trk.lineScale.val().toDouble());
    toolUserLineWith->setChecked(trk.lineScale.getMode() == CValue::eModeUser);

    checkWithArrows->setEnabled(!isNogo);
    checkWithArrows->setChecked(!isNogo && trk.showArrows.val().toBool());
    toolUserArrow->setEnabled(!isNogo);
    toolUserArrow->setChecked(!isNogo && trk.showArrows.getMode() == CValue::eModeUser);
    toolDefArrow->setEnabled(!isNogo);

    comboColorSource->clear();
    // the first entry `solid color`, it is always available
    comboColorSource->addItem(QIcon("://icons/32x32/CSrcSolid.png"), tr("Color"));
    comboColorSource->addItem(QIcon("://icons/32x32/Activity.png"), tr("Activity"), "activity");
    const QList<QString>& keys = trk.getExistingDataSources();
    for(const QString& key : keys)
    {
        const CKnownExtension& ext = CKnownExtension::get(key);
        QIcon icon(ext.icon);
        comboColorSource->addItem(icon, ext.known ? ext.nameLongText : key, key);
    }
    int currentIdx = comboColorSource->findData(trk.getColorizeSource());
    if(-1 == currentIdx)
    {
        trk.setColorizeSource(QString());
        currentIdx = 0;
    }
    comboColorSource->setCurrentIndex(currentIdx);

    QString source = comboColorSource->currentData().toString();
    bool enabledColorize = !source.isEmpty() && (source != "activity");
    bool enabledActivity = source == "activity";

    comboColor->setVisible(!(enabledColorize || enabledActivity));
    widgetColorLabel->setVisible(enabledColorize);
    widgetColorLabel->setEnabled(enabledColorize);
    toolLimitAutoStyle->setEnabled(enabledColorize);
    toolLimitUsrStyle->setEnabled(enabledColorize);
    toolLimitSysStyle->setEnabled(enabledColorize);

    if(enabledColorize)
    {
        const CKnownExtension& ext = CKnownExtension::get(comboColorSource->itemData(currentIdx).toString());

        spinLimitLow->setMinimum(ext.minimum);
        spinLimitLow->setMaximum(ext.maximum);
        spinLimitLow->setSuffix (ext.unit);
        spinLimitLow->setValue  (trk.getColorizeLimitLow());

        spinLimitHigh->setMinimum(ext.minimum);
        spinLimitHigh->setMaximum(ext.maximum);
        spinLimitHigh->setSuffix (ext.unit);
        spinLimitHigh->setValue  (trk.getColorizeLimitHigh());

        widgetColorLabel->setMinimum(spinLimitLow->value());
        widgetColorLabel->setMaximum(spinLimitHigh->value());
        widgetColorLabel->setUnit(ext.unit);
    }

    widgetColorActivity->updateData();
    widgetColorActivity->setVisible(enabledActivity);
    widgetColorActivity->setEnabled(enabledActivity);

    X_____________UnBlockAllSignals_____________X(this);

    // refill comboboxes to select track property to be displayed by graphs
    loadGraphSource(comboGraph2, 2, CKnownExtension::internalSpeedDist);
    loadGraphSource(comboGraph3, 3, CKnownExtension::internalProgress);

    CFilterDeleteExtension* filterDeleteExtension = treeFilter->findChild<CFilterDeleteExtension*>();
    if(nullptr != filterDeleteExtension)
    {
        filterDeleteExtension->updateUi();
    }

    CFilterReplaceElevation* filterReplaceElevation = treeFilter->findChild<CFilterReplaceElevation*>();
    if(nullptr != filterReplaceElevation)
    {
        filterReplaceElevation->updateUi();
    }

    CFilterSpeed* filterSpeed = tabWidget->findChild<CFilterSpeed*>("IFilterSpeed");
    if(nullptr != filterSpeed)
    {
        filterSpeed->updateUi();
    }

    CFilterChangeStartPoint* filterChangeStartPoint = tabWidget->findChild<CFilterChangeStartPoint*>("IFilterChangeStartPoint");
    if(nullptr != filterChangeStartPoint)
    {
        filterChangeStartPoint->updateUi();
    }


    QString tooltip = tr("Set parameters to compute \"Energy Use Cycling\" for a cycling tour");
    if(trk.getEnergyCycling().isValid())
    {
        toolSetEnergyCycling->setEnabled(true);
    }
    else
    {
        toolSetEnergyCycling->setEnabled(false);
        tooltip += "<b style='color: red;'>" + tr(" - Computation needs valid time, elevation and slope data.") + "</b>";
    }
    toolSetEnergyCycling->setToolTip(tooltip);

    enableTabFilter();
    originator = false;
}

void CDetailsTrk::setMouseFocus(const CTrackData::trkpt_t* pt)
{
    if(nullptr != pt)
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

void CDetailsTrk::setMouseRangeFocus(const CTrackData::trkpt_t* pt1, const CTrackData::trkpt_t* pt2)
{
    labelInfoRange->setText( (pt1 && pt2) ? trk.getInfoRange(*pt1, *pt2) : "-\n-" );
}

void CDetailsTrk::enableTabFilter()
{
    if (trk.isReadOnly())
    {
        return;
    }

    switch(trk.getMode())
    {
    case CGisItemTrk::eModeNormal:
        tabWidget->widget(eTabFilter)->setEnabled(true);
        break;

    case CGisItemTrk::eModeRange:
        tabWidget->widget(eTabFilter)->setEnabled(false);
        break;
    }
}

void CDetailsTrk::setMouseClickFocus(const CTrackData::trkpt_t* pt)
{
    if(nullptr != pt)
    {
        treeTrackPoint->blockSignals(true);
        treeTrackPoint->setCurrentItem(treeTrackPoint->topLevelItem(pt->idxTotal));
        treeTrackPoint->blockSignals(false);
    }
}

void CDetailsTrk::slotMouseClickState(int s)
{
    if(s == IPlot::eMouseClickIdle)
    {
        labelInfoRange->setText("-\n-");
        plot3->setMouseRangeFocus(0, 0);
        plot1->setMouseRangeFocus(0, 0);
        plot2->setMouseRangeFocus(0, 0);
    }
}

void CDetailsTrk::slotNameChanged(const QString& name)
{
    QTabWidget* tabWidget_ = dynamic_cast<QTabWidget*>(parentWidget() ? parentWidget()->parentWidget() : nullptr);
    if(nullptr != tabWidget_)
    {
        int idx = tabWidget_->indexOf(this);
        if(idx != NOIDX)
        {
            const QString shownName = name.isEmpty() ? CGisItemTrk::noName : QString(name).replace('&', "&&");
            setObjectName(shownName);
            tabWidget_->setTabText(idx, shownName);
        }
    }
}

void CDetailsTrk::slotNameChangeFinished()
{
    lineName->clearFocus();

    const QString& name = lineName->text();
    slotNameChanged(name);

    if(name != trk.getName())
    {
        trk.setName(name);
        updateData();
    }
}


void CDetailsTrk::slotShowPlots()
{
    plot1->setVisible(checkGraph1->isChecked());
    plot2->setVisible(checkGraph2->isChecked());
    plot3->setVisible(checkGraph3->isChecked());
    CDetailsTrk::updateData();
}

void CDetailsTrk::slotColorChanged(int idx)
{
    if(trk.getColorIdx() != idx)
    {
        trk.setColor(idx);
    }
}

void CDetailsTrk::slotColorSourceChanged(int idx)
{
    trk.setColorizeSource(comboColorSource->itemData(idx).toString());
    updateData();
}

void CDetailsTrk::slotColorLimitHighChanged()
{
    const double val = spinLimitHigh->value();
    trk.setColorizeLimitHigh(val);
    widgetColorLabel->setMaximum(val);
}

void CDetailsTrk::slotColorLimitLowChanged()
{
    const double val = spinLimitLow->value();
    trk.setColorizeLimitLow(val);
    widgetColorLabel->setMinimum(val);
}

void CDetailsTrk::slotChangeReadOnlyMode(bool on)
{
    trk.setReadOnlyMode(on);
    // as setReadOnlyMode() is a method of IGisItem it will bypass updateHistory() of the track
    // Therefore we have to call updateVisuals() explicitly.
    trk.updateVisuals(CGisItemTrk::eVisualProject, "CDetailsTrk::slotChangeReadOnlyMode()");
    updateData();
}

void CDetailsTrk::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "comment")
    {
        CTextEditWidget dlg(trk.getComment(), this);
        if(dlg.exec() == QDialog::Accepted)
        {
            trk.setComment(dlg.getHtml());
        }
        updateData();
    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(trk.getDescription(), this);
        if(dlg.exec() == QDialog::Accepted)
        {
            trk.setDescription(dlg.getHtml());
        }
        updateData();
    }
    else if(url.toString() == "links")
    {
        QList<IGisItem::link_t> links = trk.getLinks();
        CLinksDialog dlg(links, this);
        if(dlg.exec() == QDialog::Accepted)
        {
            trk.setLinks(links);
        }
        updateData();
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}

void CDetailsTrk::slotSetActivities()
{
    CActivityTrk::getMenu(trk.getKey(), this, true);
}

void CDetailsTrk::slotSetEnergyCycling()
{
    if (!trk.getEnergyCycling().isValid())
    {
        return;
    }

    CEnergyCyclingDialog energyCyclingDlg(trk.getEnergyCycling(), this);

    qint32 ret = energyCyclingDlg.exec();

    if(ret == QDialog::Accepted || ret == QDialog::Rejected)
    {
        trk.updateVisuals(CGisItemTrk::eVisualDetails, "CDetailsTrk::slotSetEnergyCycling()");
    }
}

void CDetailsTrk::setupGraph(CPlot* plot, const CLimit& limit, const QString& source, QDoubleSpinBox* spinMin, QDoubleSpinBox* spinMax)
{
    trk.getPropertyHandler()->setupPlot(plot, source);
    spinMin->setSuffix(limit.getUnit());
    spinMax->setSuffix(limit.getUnit());
    spinMin->setValue(limit.getMin());
    spinMax->setValue(limit.getMax());
}

void CDetailsTrk::slotSetupGraph(int idx)
{
    QObject* s = sender();
    if(s == comboGraph2)
    {
        saveGraphSource(comboGraph2, 2);
        setupGraph(plot2, trk.limitsGraph2, comboGraph2->itemData(idx).toString(), spinMinGraph2, spinMaxGraph2);
    }
    else if(s == comboGraph3)
    {
        saveGraphSource(comboGraph3, 3);
        setupGraph(plot3, trk.limitsGraph3, comboGraph3->itemData(idx).toString(), spinMinGraph3, spinMaxGraph3);
    }
}

void CDetailsTrk::slotSetLimitModeGraph(CLimit::mode_e mode, CLimit* limit, QDoubleSpinBox* spinMin, QDoubleSpinBox* spinMax, bool on)
{
    if(!on)
    {
        return;
    }

    limit->setMode(mode);
    spinMin->setValue(limit->getMin());
    spinMax->setValue(limit->getMax());
}

void CDetailsTrk::setupLimits(CLimit* limit, QDoubleSpinBox* spinMin, QDoubleSpinBox* spinMax)
{
    spinMin->setValue(limit->getMin());
    spinMax->setValue(limit->getMax());
}

void CDetailsTrk::slotLineWidthMode(bool isUser)
{
    trk.lineScale.setMode(isUser ? CValue::eModeUser : CValue::eModeSys);
    spinLineWidth->setValue(trk.lineScale.val().toDouble());
}

void CDetailsTrk::slotLineWidth(int f)
{
    trk.lineScale = f;
}

void CDetailsTrk::slotWithArrowsMode(bool isUser)
{
    trk.showArrows.setMode(isUser ? CValue::eModeUser : CValue::eModeSys);
    checkWithArrows->setChecked(trk.showArrows.val().toBool());
}

void CDetailsTrk::slotWithArrows(bool yes)
{
    trk.showArrows = yes;
}
