/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CMainWindow.h"
#include "helpers/CSettings.h"
#include "items/CItemMap.h"
#include "tool/CToolExport.h"

#include <QtWidgets>

CToolExport::CToolExport(QWidget *parent)
    : IToolGui(parent)
{
    setupUi(this);
    setObjectName(tr("Export Maps"));
    lineTragetFile->addAction(actionTargetFilename, QLineEdit::TrailingPosition);

    labelHelp->setText(tr("To use the maps on your device you have to export them to the proprietary "
                          "format supported by the device. Depending on the device this can vary from "
                          "a single layer map to a map stack with maps of different scale."
                          ));

    labelHelp->setVisible(CMainWindow::self().showToolHelp()->isChecked());
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);

    labelNote->setText(tr("Note: This tool will use all files in the list as a input. "
                          "This will only work if all files have the same projection."
                          ));
    labelNote->setVisible(CMainWindow::self().showToolHelp()->isChecked());
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelNote, &QLabel::setVisible);

    connect(itemTree, &CItemTreeWidget::sigSelectionChanged, this, &CToolExport::slotMapSelectionChanged);
    connect(itemTree, &CItemTreeWidget::sigChanged, this, &CToolExport::slotSomethingChanged);
    connect(lineTragetFile, &QLineEdit::textChanged, this, &CToolExport::slotSomethingChanged);
    connect(actionTargetFilename, &QAction::triggered, this, &CToolExport::slotSelectFilename);
    connect(comboExport, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(pushStart, &QPushButton::clicked, this, &CToolExport::slotStart);
    connect(pushCancel, &QPushButton::clicked, &CShell::self(), &CShell::slotCancel);
    connect(&CShell::self(), &CShell::sigFinishedJob, this, &CToolExport::slotFinished);


    setupChanged();

    SETTINGS;
    cfg.beginGroup("ToolExport");
    itemTree->loadSettings(cfg);

    comboExport->setCurrentIndex(cfg.value("current", 0).toInt());
    lineTragetFile->setText(cfg.value("targetFile", "").toString());
    cfg.endGroup();
}

CToolExport::~CToolExport()
{
    SETTINGS;
    cfg.beginGroup("ToolExport");
    cfg.remove("");
    itemTree->saveSettings(cfg);
    cfg.setValue("current", comboExport->currentIndex());
    cfg.setValue("targetFile", lineTragetFile->text());
    cfg.endGroup();
}


void CToolExport::setupChanged()
{
    bool hasMap2jnx = !IAppSetup::self().getQmtmap2jnx().isEmpty();
    labelNoMap2jnx->setVisible(!hasMap2jnx);

    frame->setVisible(hasMap2jnx);
}

void CToolExport::slotMapSelectionChanged()
{
    CMainWindow::self().getCanvas()->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
    slotSomethingChanged();
}


void CToolExport::slotSomethingChanged()
{
    bool ok = true;
    if(itemTree->topLevelItemCount() == 0)
    {
        ok = false;
    }
    if(lineTragetFile->text().isEmpty())
    {
        ok = false;
    }
    pushStart->setEnabled(ok);
}

void CToolExport::slotSelectFilename()
{
    SETTINGS;
    QString path = cfg.value("Path/mapInput", QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName(this, tr("Select filename..."), path);
    if(filename.isEmpty())
    {
        return;
    }
    cfg.setValue("Path/mapInput", QFileInfo(filename).absolutePath());

    lineTragetFile->setText(filename);
}


void CToolExport::buildCmd(QList<CShellCmd>& cmds, const IItem * iitem)
{
    inputFiles << iitem->getFilename();
}

void CToolExport::buildCmdFinal(QList<CShellCmd>& cmds)
{
    switch(comboExport->currentIndex())
    {
    case eTypeJnx:
        buildCmdFinalJnx(cmds);
        break;

    case eTypeRmap:
        buildCmdFinalRmap(cmds);
        break;
    }
}

void CToolExport::buildCmdFinalJnx(QList<CShellCmd>& cmds)
{
    QStringList args;
    args << "-q" << pageBirdsEyeJnx->getJpegQuality();
    args << "-s" << pageBirdsEyeJnx->getJpegSubsampling();
    args << "-p" << pageBirdsEyeJnx->getProductId();
    args << "-m" << pageBirdsEyeJnx->getProductName();
    args << "-n" << pageBirdsEyeJnx->getDescription();
    args << "-c" << pageBirdsEyeJnx->getCopyright();
    args << "-z" << pageBirdsEyeJnx->getZOrder();
    args += inputFiles;

    QString targetFile = lineTragetFile->text();
    QFileInfo fi(targetFile);
    if(fi.suffix().toUpper() != "JNX")
    {
        targetFile += ".jnx";
    }
    args << targetFile;
    cmds << CShellCmd(IAppSetup::self().getQmtmap2jnx(), args);
}

void CToolExport::buildCmdFinalRmap(QList<CShellCmd>& cmds)
{
}


void CToolExport::slotStart()
{
    inputFiles.clear();

    start(itemTree);
    if(jobId > 0)
    {
        itemTree->setEnabled(false);
        pushStart->setEnabled(false);
        pushCancel->setEnabled(true);
    }
}

void CToolExport::slotFinished(qint32 id)
{
    if(finished(id))
    {
        itemTree->setEnabled(true);
        pushStart->setEnabled(true);
        pushCancel->setEnabled(false);
    }
}

