/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/IDrawContext.h"
#include "CMainWindow.h"
#include "helpers/CSettings.h"
#include "items/CItemCutMap.h"
#include "setup/IAppSetup.h"
#include "tool/CToolCutMap.h"

CToolCutMap::CToolCutMap(QWidget* parent)
    : IToolGui(parent)
{
    setupUi(this);
    setObjectName(tr("Cut Map"));

    labelHelp->setText(tr("Paper maps usually have a border you don't want to have. To combine "
                          "maps seamlessly you have to cut that border, replacing it by transparent "
                          "pixel. This tool allows you to define a cut line and it will add an alpha "
                          "channel for transparency to your map."));

    labelHelp->setVisible(CMainWindow::self().showToolHelp()->isChecked());
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);

    connect(itemList, &CItemListWidget::sigAddItem, this, &CToolCutMap::slotAddItem);
    connect(itemList, &CItemListWidget::sigSelectionChanged, this, &CToolCutMap::slotMapSelectionChanged);
    connect(itemList, &CItemListWidget::sigChanged, this, &CToolCutMap::slotSomethingChanged);

    connect(lineSuffix, &QLineEdit::textChanged, this, &CToolCutMap::slotSomethingChanged);

    connect(pushStart, &QPushButton::clicked, this, &CToolCutMap::slotStart);
    connect(pushCancel, &QPushButton::clicked, &CShell::self(), &CShell::slotCancel);
    connect(&CShell::self(), &CShell::sigFinishedJob, this, &CToolCutMap::slotFinished);

    CToolCutMap::setupChanged();

    SETTINGS;
    cfg.beginGroup("ToolCutMap");
    itemList->loadSettings(cfg);
    groupOverviews->loadSettings(cfg);
    groupGDALParameters->loadSettings(cfg);
    checkAllFiles->setChecked(cfg.value("allFiles", false).toBool());
    lineSuffix->setText(cfg.value("suffix", "_cut").toString());
    cfg.endGroup();

    slotSomethingChanged();
}

CToolCutMap::~CToolCutMap()
{
    SETTINGS;
    cfg.beginGroup("ToolCutMap");
    cfg.remove("");
    itemList->saveSettings(cfg);
    groupOverviews->saveSettings(cfg);
    groupGDALParameters->saveSettings(cfg);
    cfg.setValue("allFiles", checkAllFiles->isChecked());
    cfg.setValue("suffix", lineSuffix->text());
    cfg.endGroup();
}

void CToolCutMap::setupChanged()
{
    bool hasGdalwarp = !IAppSetup::self().getGdalwarp().isEmpty();
    labelNoGdalwarp->setVisible(!hasGdalwarp);

    bool hasGdaladdo = !IAppSetup::self().getGdaladdo().isEmpty();
    labelNoGdaladdo->setVisible(!hasGdaladdo);

    frame->setVisible(hasGdalwarp && hasGdaladdo);
}

void CToolCutMap::slotAddItem(const QString& filename, QListWidget* list)
{
    CItemCutMap* item = new CItemCutMap(filename, stackedWidget, list);
    connect(item, &CItemFile::sigChanged, itemList, &CItemListWidget::sigChanged);
}

void CToolCutMap::slotMapSelectionChanged()
{
    CMainWindow::self().getCanvas()->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
    slotSomethingChanged();
}

void CToolCutMap::slotSomethingChanged()
{
    IItem* item = itemList->currentItem();
    if(item != nullptr)
    {
        bool ok = item->isOk();
        if(lineSuffix->text().isEmpty())
        {
            ok = false;
        }
        pushStart->setEnabled(ok);
    }
    else
    {
        pushStart->setEnabled(false);
    }
}


void CToolCutMap::buildCmd(QList<CShellCmd>& cmds, const IItem* iitem)
{
    const CItemCutMap* item = dynamic_cast<const CItemCutMap*>(iitem);
    if(nullptr == item)
    {
        return;
    }

    // ---- command 1 ----------------------
    const IDrawContext* context = item->getDrawContext();
    QStringList args;
    // --- overwrite last output file ---
    args << "-overwrite";
    // --- use all CPU cores when possible ---
    args << "-multi";
    args << "-wo";
    args << "NUM_THREADS=ALL_CPUS";

    if(context->getProjection().isEmpty())
    {
        // --- if the source in not referenced ---
        args << "-to";
        args << "SRC_METHOD=NO_GEOTRANSFORM";
        args << "-to";
        args << "DST_METHOD=NO_GEOTRANSFORM";
    }
    else
    {
        // --- this only works for referenced sources ---
        args << "-crop_to_cutline";
    }

    // --- define transparent color ---
    if(context->getRasterBandCount() == 1)
    {
        if(!context->getNoData())
        {
            // --- use no data value for destination, too ---
            args << "-dstnodata" << "255";
        }
    }
    else if(context->getRasterBandCount() == 3)
    {
        // --- add alpha channel to files with just RGB ---
        args << "-dstalpha";
    }

    args.append(groupGDALParameters->getArgs());

    QString wktFilename = createTempFile("csv");
    item->saveShape(wktFilename);
    QString inFilename = item->getFilename();
    QFileInfo fi(inFilename);
    QString outFilename = fi.absoluteDir().absoluteFilePath(fi.completeBaseName() + lineSuffix->text() + "." + fi.suffix());

    args << "-cutline";
    args << wktFilename;
    args << inFilename;
    args << outFilename;

    cmds << CShellCmd(IAppSetup::self().getGdalwarp(), args);

    // ---- command 2 ----------------------
    groupOverviews->buildCmd(cmds, outFilename, context->is32BitRgb() ? "cubic" : "nearest");
}

void CToolCutMap::slotStart()
{
    start(itemList, checkAllFiles->isChecked());
    if(jobId > 0)
    {
        itemList->setEnabled(false);
        frameInput->setEnabled(false);
        pushStart->setEnabled(false);
        pushCancel->setEnabled(true);
    }
}

void CToolCutMap::slotFinished(qint32 id)
{
    if(finished(id))
    {
        itemList->setEnabled(true);
        frameInput->setEnabled(true);
        pushStart->setEnabled(true);
        pushCancel->setEnabled(false);
    }
}
