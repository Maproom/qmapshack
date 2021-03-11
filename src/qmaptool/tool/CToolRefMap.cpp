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
#include "CToolRefMap.h"
#include "gis/proj_x.h"
#include "helpers/CSettings.h"
#include "items/CItemRefMap.h"
#include "overlay/refmap/COverlayRefMapPoint.h"
#include "setup/IAppSetup.h"
#include "shell/CShell.h"

CToolRefMap::CToolRefMap(QWidget *parent)
    : IToolGui(parent)
{
    setupUi(this);
    setObjectName(tr("Reference Map"));

    labelHelp->setText(tr("A scan of a paper map can be converted to a referenced raster map if "
                          "you place at least three reference points on the map. The more points "
                          "the better the result. If your map has a grid you can place points on "
                          "that grid with the grid tool."));

    labelHelp->setVisible(CMainWindow::self().showToolHelp()->isChecked());
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);

    connect(itemList, &CItemListWidget::sigAddItem, this, &CToolRefMap::slotAddItem);
    connect(itemList, &CItemListWidget::sigSelectionChanged, this, &CToolRefMap::slotMapSelectionChanged);
    connect(itemList, &CItemListWidget::sigChanged, this, &CToolRefMap::slotSomethingChanged);

    connect(lineSuffix, &QLineEdit::textChanged, this, &CToolRefMap::slotSomethingChanged);

    connect(pushStart, &QPushButton::clicked, this, &CToolRefMap::slotStart);
    connect(pushCancel, &QPushButton::clicked, &CShell::self(), &CShell::slotCancel);
    connect(&CShell::self(), &CShell::sigFinishedJob, this, &CToolRefMap::slotFinished);

    setupChanged();

    SETTINGS;
    cfg.beginGroup("ToolRefMap");
    itemList->loadSettings(cfg);
    checkCreateVrt->setChecked(cfg.value("createVrt", false).toBool());
    groupOverviews->loadSettings(cfg);
    checkAllFiles->setChecked(cfg.value("allFiles", false).toBool());
    lineSuffix->setText(cfg.value("suffix", "_ref").toString());
    cfg.endGroup();

    slotSomethingChanged();

    adjustSize();
}

CToolRefMap::~CToolRefMap()
{
    SETTINGS;
    cfg.beginGroup("ToolRefMap");
    cfg.remove("");
    itemList->saveSettings(cfg);
    cfg.setValue("createVrt", checkCreateVrt->isChecked());
    groupOverviews->saveSettings(cfg);
    cfg.setValue("allFiles", checkAllFiles->isChecked());
    cfg.setValue("suffix", lineSuffix->text());
    cfg.endGroup();
}


void CToolRefMap::setupChanged()
{
    bool hasGdalwarp = !IAppSetup::self().getGdalwarp().isEmpty();
    labelNoGdalwarp->setVisible(!hasGdalwarp);

    bool hasGdaltranslate = !IAppSetup::self().getGdaltranslate().isEmpty();
    labelNoGdalTranslate->setVisible(!hasGdaltranslate);

    bool hasGdaladdo = !IAppSetup::self().getGdaladdo().isEmpty();
    labelNoGdaladdo->setVisible(!hasGdaladdo);

    frame->setVisible(hasGdalwarp && hasGdaltranslate && hasGdaladdo);

    checkCreateVrt->setVisible(!IAppSetup::self().getGdalbuildvrt().isEmpty());
}

void CToolRefMap::slotAddItem(const QString& filename, QListWidget * list)
{
    CItemRefMap * item = new CItemRefMap(filename, stackedWidget, list);
    connect(item, &CItemFile::sigChanged, itemList, &CItemListWidget::sigChanged);
}

void CToolRefMap::slotMapSelectionChanged()
{
    CMainWindow::self().getCanvas()->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
    slotSomethingChanged();
}

void CToolRefMap::slotSomethingChanged()
{
    IItem * item = itemList->currentItem();
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

void CToolRefMap::buildCmd(QList<CShellCmd>& cmds, const IItem *iitem)
{
    const CItemRefMap * item = dynamic_cast<const CItemRefMap*>(iitem);
    if(nullptr == item)
    {
        return;
    }

    CProj proj;
    proj.init("EPSG:4326", item->getMapProjection().toLatin1());
    if(!proj.isValid())
    {
        return;
    }

    // ---- command 1 ----------------------
    QStringList args;
    args << "-a_srs" << item->getMapProjection();

    const QList<COverlayRefMapPoint*>& points = item->getRefPoints();
    for(const COverlayRefMapPoint* pt : points)
    {
        qreal lon = pt->getPtRef().x() * DEG_TO_RAD;
        qreal lat = pt->getPtRef().y() * DEG_TO_RAD;
        proj.transform(lon, lat, PJ_FWD);

        args << "-gcp";
        args << QString::number(pt->getPtPtx().x());
        args << QString::number(pt->getPtPtx().y());

        args << QString::number(lon, 'f', 6);
        args << QString::number(lat, 'f', 6);
        args << "0";
    }

    QString tmpname1    = createTempFile("tif");
    QString inFilename  = item->getFilename();
    args << inFilename << tmpname1;
    cmds << CShellCmd(IAppSetup::self().getGdaltranslate(), args);

    // ---- command 2 ----------------------
    IDrawContext * context = item->getDrawContext();
    args.clear();
    // --- set re-sampling method ---
    args << "-r";
    args << (context->is32BitRgb() ? "cubic" : "near");
    // --- overwrite last output file ---
    args << "-overwrite";
    // --- use all CPU cores when possible ---
    args << "-multi";
    args << "-wo";
    args << "NUM_THREADS=ALL_CPUS";
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

    QString tmpname2 = createTempFile("tif");
    args << tmpname1 << tmpname2;
    cmds << CShellCmd(IAppSetup::self().getGdalwarp(), args);

    // ---- command 3 ----------------------
    QFileInfo fi(inFilename);
    QString outFilename = fi.absoluteDir().absoluteFilePath(fi.completeBaseName() + lineSuffix->text() + "." + fi.suffix());

    args.clear();
    args << "-co" << "tiled=yes" << "-co" << "compress=deflate";
    args << tmpname2 << outFilename;
    cmds << CShellCmd(IAppSetup::self().getGdaltranslate(), args);

    QString lastOutFilname = outFilename;
    // ---- command 4 ----------------------
    if(checkCreateVrt->isChecked())
    {
        QFileInfo fi(outFilename);
        QString vrtFilename = fi.absoluteDir().absoluteFilePath(fi.completeBaseName() + ".vrt");
        args.clear();
        args << vrtFilename << outFilename;
        cmds << CShellCmd(IAppSetup::self().getGdalbuildvrt(), args);
        lastOutFilname = vrtFilename;
    }

    // ---- command 5 ----------------------
    groupOverviews->buildCmd(cmds, lastOutFilname, context->is32BitRgb() ? "cubic" : "nearest");
}

void CToolRefMap::slotStart()
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

void CToolRefMap::slotFinished(qint32 id)
{
    if(finished(id))
    {
        itemList->setEnabled(true);
        frameInput->setEnabled(true);
        pushStart->setEnabled(true);
        pushCancel->setEnabled(false);
    }
}

