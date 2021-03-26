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
#include "items/CItemFile.h"
#include "tool/CToolPalettize.h"

#include <QtWidgets>

CToolPalettize::CToolPalettize(QWidget *parent)
    : IToolGui(parent)
{
    setupUi(this);
    setObjectName(tr("Add Color Palette"));

    lineFilename->addAction(actionFilename, QLineEdit::TrailingPosition);

    labelHelp->setText(tr("Usually you use RGBA color while referencing a map because the large "
                          "color space allows you to scale and rotate the map without any loss "
                          "of quality. But it results into rather large files. The file size can "
                          "be optimized by using a color palette instead of the RGBA color space. "
                          "The impact on quality is low as long as you do not want to scale or "
                          "rotate the map. If you want to combine files with a color palette all "
                          "files need to have the same palette."
                          ));

    labelHelp->setVisible(CMainWindow::self().showToolHelp()->isChecked());
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);

    labelNote->setText(tr("Note: This tool will use all files in the list as a combined input "
                          "to derive an optimal palette. This will only work if all files have "
                          "the same projection and scale."
                          ));
    labelNote->setVisible(CMainWindow::self().showToolHelp()->isChecked());
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelNote, &QLabel::setVisible);

    connect(itemList, &CItemListWidget::sigAddItem, this, &CToolPalettize::slotAddItem);
    connect(itemList, &CItemListWidget::sigSelectionChanged, this, &CToolPalettize::slotMapSelectionChanged);
    connect(itemList, &CItemListWidget::sigChanged, this, &CToolPalettize::slotSomethingChanged);

    connect(radioSingle, &QRadioButton::toggled, lineSuffix, &QLineEdit::setEnabled);
    connect(radioSingle, &QRadioButton::toggled, this, &CToolPalettize::slotSomethingChanged);
    connect(radioCombined, &QRadioButton::toggled, lineFilename, &QLineEdit::setEnabled);
    connect(radioCombined, &QRadioButton::toggled, this, &CToolPalettize::slotSomethingChanged);

    connect(actionFilename, &QAction::triggered, this, &CToolPalettize::slotSelectFilename);
    connect(lineSuffix, &QLineEdit::textChanged, this, &CToolPalettize::slotSomethingChanged);
    connect(lineFilename, &QLineEdit::textChanged, this, &CToolPalettize::slotSomethingChanged);

    connect(pushStart, &QPushButton::clicked, this, &CToolPalettize::slotStart);
    connect(pushCancel, &QPushButton::clicked, &CShell::self(), &CShell::slotCancel);
    connect(&CShell::self(), &CShell::sigFinishedJob, this, &CToolPalettize::slotFinished);

    CToolPalettize::setupChanged();

    SETTINGS;
    cfg.beginGroup("ToolPalettize");
    itemList->loadSettings(cfg);
    checkCreateVrt->setChecked(cfg.value("createVrt", false).toBool());
    groupOverviews->loadSettings(cfg);
    radioSingle->setChecked(cfg.value("singleFiles", true).toBool());
    radioCombined->setChecked(cfg.value("combinedFile", false).toBool());
    lineFilename->setText(cfg.value("filename", "").toString());
    lineSuffix->setText(cfg.value("suffix", "_8bit").toString());
    cfg.endGroup();

    lineFilename->setEnabled(radioCombined->isChecked());
    lineSuffix->setEnabled(radioSingle->isChecked());

    inputFileList1 = new QTemporaryFile(this);
    inputFileList2 = new QTemporaryFile(this);
}

CToolPalettize::~CToolPalettize()
{
    SETTINGS;
    cfg.beginGroup("ToolPalettize");
    cfg.remove("");
    itemList->saveSettings(cfg);
    cfg.setValue("createVrt", checkCreateVrt->isChecked());
    groupOverviews->saveSettings(cfg);
    cfg.setValue("singleFiles", radioSingle->isChecked());
    cfg.setValue("combinedFile", radioCombined->isChecked());
    cfg.setValue("filename", lineFilename->text());
    cfg.setValue("suffix", lineSuffix->text());
    cfg.endGroup();
}

void CToolPalettize::slotSelectFilename()
{
    SETTINGS;
    QString path = cfg.value("Path/mapInput", QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName(this, tr("Select filename..."), path);
    if(filename.isEmpty())
    {
        return;
    }
    cfg.setValue("Path/mapInput", QFileInfo(filename).absolutePath());

    if(!filename.endsWith(".TIF", Qt::CaseInsensitive))
    {
        filename += ".tif";
    }

    lineFilename->setText(filename);
}

void CToolPalettize::setupChanged()
{
    bool hasGdaladdo = !IAppSetup::self().getGdaladdo().isEmpty();
    labelNoGdaladdo->setVisible(!hasGdaladdo);

    bool hasGdaltranslate = !IAppSetup::self().getGdaltranslate().isEmpty();
    labelNoGdaltranslate->setVisible(!hasGdaltranslate);

    bool hasQmtrgb2pct = !IAppSetup::self().getQmtrgb2pct().isEmpty();
    labelNoQmtrgb2pct->setVisible(!hasQmtrgb2pct);

    frame->setVisible(hasGdaladdo && hasQmtrgb2pct && hasGdaltranslate);
}

void CToolPalettize::slotAddItem(const QString& filename, QListWidget * list)
{
    CItemFile * item = new CItemFile(filename, list);
    connect(item, &CItemFile::sigChanged, itemList, &CItemListWidget::sigChanged);
}

void CToolPalettize::slotMapSelectionChanged()
{
    CMainWindow::self().getCanvas()->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
    slotSomethingChanged();
}

void CToolPalettize::slotSomethingChanged()
{
    bool ok = itemList->count() > 0;
    if(radioSingle->isChecked() && lineSuffix->text().isEmpty())
    {
        ok = false;
    }
    if(radioCombined->isChecked() && lineFilename->text().isEmpty())
    {
        ok = false;
    }

    pushStart->setEnabled(ok);
}

void CToolPalettize::buildCmd(QList<CShellCmd>& cmds, const IItem * iitem)
{
    inputFileList1->open();
    QTextStream stream(inputFileList1);
    stream << iitem->getFilename() << endl;
    inputFileList1->close();
}

void CToolPalettize::buildCmdFinal(QList<CShellCmd>& cmds)
{
    QStringList args;

    // ---- command 1 ----------------------
    QString vrtFilename = createTempFile("vrt");
    args.clear();
    args << vrtFilename;
    args << "-input_file_list" << inputFileList1->fileName();
    cmds << CShellCmd(IAppSetup::self().getGdalbuildvrt(), args);

    // ---- command 2 ----------------------
    QString pctFilename = createTempFile("vrt");
    args.clear();
    args << "--sct" << pctFilename;
    args << vrtFilename;
    cmds << CShellCmd(IAppSetup::self().getQmtrgb2pct(), args);

    // ---- command 2..2 + N ----------------------
    if(radioCombined->isChecked())
    {
        inputFileList2->open();
        QTextStream stream(inputFileList2);

        const int N = itemList->count();
        for(int n = 0; n < N; n++)
        {
            const IItem * item = dynamic_cast<const IItem*>(itemList->item(n));
            if(nullptr == item)
            {
                continue;
            }
            QString inFilename = item->getFilename();
            QString outFilename = createTempFile("tif");

            stream << outFilename << endl;

            args.clear();
            args << "--pct" << pctFilename;
            args << inFilename;
            args << outFilename;
            cmds << CShellCmd(IAppSetup::self().getQmtrgb2pct(), args);
        }

        inputFileList2->close();

        // ---- command 2 + N + 1 ----------------------
        QString vrtFilename = createTempFile("vrt");
        args.clear();
        args << vrtFilename;
        args << "-input_file_list" << inputFileList2->fileName();
        cmds << CShellCmd(IAppSetup::self().getGdalbuildvrt(), args);

        // ---- command 2 + N + 2 ----------------------
        QString outFilename = lineFilename->text();
        if(!outFilename.endsWith(".TIF", Qt::CaseInsensitive))
        {
            outFilename += ".tif";
        }

        args.clear();
        args << "-co" << "TILED=YES";
        args << "-co" << "COMPRESS=LZW";
        args << vrtFilename;
        args << outFilename;
        cmds << CShellCmd(IAppSetup::self().getGdaltranslate(), args);

        QString lastOutFilname = outFilename;
        // ---- command 2 + N + 3 ----------------------
        if(checkCreateVrt->isChecked())
        {
            QFileInfo fi(outFilename);
            QString vrtFilename = fi.absoluteDir().absoluteFilePath(fi.completeBaseName() + ".vrt");
            args.clear();
            args << vrtFilename << outFilename;
            cmds << CShellCmd(IAppSetup::self().getGdalbuildvrt(), args);
            lastOutFilname = vrtFilename;
        }

        // ---- command 2 + N + 4 ----------------------
        groupOverviews->buildCmd(cmds, lastOutFilname, "nearest");
    }
    else
    {
        // ---- command 3..3*N ----------------------
        const int N = itemList->count();
        for(int n = 0; n < N; n++)
        {
            const IItem * item = dynamic_cast<const IItem*>(itemList->item(n));
            if(nullptr == item)
            {
                continue;
            }
            QString inFilename = item->getFilename();
            QFileInfo fi(inFilename);
            QString outFilename = fi.absoluteDir().absoluteFilePath(fi.completeBaseName() + lineSuffix->text() + "." + fi.suffix());

            // ---- command n*3 ----------------------
            args.clear();
            args << "--pct" << pctFilename;
            args << inFilename;
            args << outFilename;
            cmds << CShellCmd(IAppSetup::self().getQmtrgb2pct(), args);

            QString lastOutFilname = outFilename;
            // ---- command n*3 + 1 ----------------------
            if(checkCreateVrt->isChecked())
            {
                QFileInfo fi(outFilename);
                QString vrtFilename = fi.absoluteDir().absoluteFilePath(fi.completeBaseName() + ".vrt");
                args.clear();
                args << vrtFilename << outFilename;
                cmds << CShellCmd(IAppSetup::self().getGdalbuildvrt(), args);
                lastOutFilname = vrtFilename;
            }

            // ---- command n*3 + 2 ----------------------
            groupOverviews->buildCmd(cmds, lastOutFilname, "nearest");
        }
    }
}

void CToolPalettize::slotStart()
{
    // reset file with list of input files
    inputFileList1->open();
    inputFileList1->resize(0);
    inputFileList1->close();

    start(itemList, true);
    if(jobId > 0)
    {
        itemList->setEnabled(false);
        pushStart->setEnabled(false);
        pushCancel->setEnabled(true);
    }
}

void CToolPalettize::slotFinished(qint32 id)
{
    if(finished(id))
    {
        itemList->setEnabled(true);
        pushStart->setEnabled(true);
        pushCancel->setEnabled(false);
    }
}
