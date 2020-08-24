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

#include "CMainWindow.h"
#include "helpers/CSettings.h"
#include "tool/CMapVrtBuilder.h"

#include <QtWidgets>

CMapVrtBuilder::CMapVrtBuilder(QWidget *parent)
    : IToolShell(parent)
{
    setupUi(this);
    setTextBrowser(textBrowser);
    setObjectName(tr("Build GDAL VRT"));

    connect(toolSourceFiles, &QToolButton::clicked, this, &CMapVrtBuilder::slotSelectSourceFiles);
    connect(toolTargetFile,  &QToolButton::clicked, this, &CMapVrtBuilder::slotSelectTargetFile);
    connect(pushStart,       &QPushButton::clicked, this, &CMapVrtBuilder::slotStart);
    connect(labelHelpGDAL,   &QLabel::linkActivated, this, &CMapVrtBuilder::slotLinkActivated);

    pushStart->setDisabled(true);

    SETTINGS;
    cfg.beginGroup("VrtBuilder");
    groupAdvancedOptions->setChecked(cfg.value("AdvancedOptions", false).toBool());
    lineASrs->setText(cfg.value("a_srs", "").toString());
    lineSrcNoData->setText(cfg.value("srcndata", "").toString());
    lineVrtNoData->setText(cfg.value("vrtndata", "").toString());
    lineUserOptions->setText(cfg.value("useropt", "").toString());
    groupOverviews->setChecked(cfg.value("Overviews", false).toBool());
    checkBy2->setChecked(cfg.value("by2", false).toBool());
    checkBy4->setChecked(cfg.value("by4", false).toBool());
    checkBy8->setChecked(cfg.value("by8", false).toBool());
    checkBy16->setChecked(cfg.value("by16", false).toBool());
    checkBy32->setChecked(cfg.value("by32", false).toBool());
    checkBy64->setChecked(cfg.value("by64", false).toBool());
    cfg.endGroup();

    tempFile = new QTemporaryFile(this);
}

CMapVrtBuilder::~CMapVrtBuilder()
{
    SETTINGS;
    cfg.beginGroup("VrtBuilder");
    cfg.setValue("AdvancedOptions", groupAdvancedOptions->isChecked());
    cfg.setValue("a_srs", lineASrs->text());
    cfg.setValue("srcndata", lineSrcNoData->text());
    cfg.setValue("vrtndata", lineVrtNoData->text());
    cfg.setValue("useropt", lineUserOptions->text());
    cfg.setValue("Overviews", groupOverviews->isChecked());
    cfg.setValue("by2", checkBy2->isChecked());
    cfg.setValue("by4", checkBy4->isChecked());
    cfg.setValue("by8", checkBy8->isChecked());
    cfg.setValue("by16", checkBy16->isChecked());
    cfg.setValue("by32", checkBy32->isChecked());
    cfg.setValue("by64", checkBy64->isChecked());
    cfg.endGroup();
}

void CMapVrtBuilder::slotSelectSourceFiles()
{
    SETTINGS;
    QString path = cfg.value("VrtBuilder/sourcePath", QDir::homePath()).toString();

    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files..."), path);
    if(files.isEmpty())
    {
        return;
    }

    QFileInfo fi(files.first());
    path = fi.absolutePath();
    cfg.setValue("VrtBuilder/sourcePath", path);

    listWidget->clear();
    for(const QString &file : files)
    {
        new QListWidgetItem(QIcon("://icons/32x32/Map.png"), file, listWidget);
    }

    enableStartButton();
}

void CMapVrtBuilder::slotSelectTargetFile()
{
    SETTINGS;
    QString path = CMainWindow::self().getMapsPath();
    if(path.isEmpty())
    {
        path = QDir::homePath();
    }
    path = cfg.value("VrtBuilder/targetPath", path).toString();


    QString file = QFileDialog::getSaveFileName(this, tr("Select target file..."), path, "GDAL vrt (*.vrt)");
    if(file.isEmpty())
    {
        return;
    }

    QFileInfo fi(file);
    path = fi.absolutePath();
    cfg.setValue("VrtBuilder/targetPath", path);

    if(fi.suffix().toLower() != "vrt")
    {
        file += ".vrt";
    }

    labelTargetFilename->setText(file);

    enableStartButton();
}

void CMapVrtBuilder::enableStartButton()
{
    pushStart->setEnabled(listWidget->count() > 0 && labelTargetFilename->text() != "-");
}

void CMapVrtBuilder::slotStart()
{
    pushStart->setDisabled(true);
    last = false;

    QStringList args;

    if(groupAdvancedOptions->isChecked())
    {
        if(!lineASrs->text().isEmpty())
        {
            args << "-a_srs" << lineASrs->text();
        }

        if(!lineSrcNoData->text().isEmpty())
        {
            args << "-srcnodata" << lineSrcNoData->text();
        }

        if(!lineVrtNoData->text().isEmpty())
        {
            args << "-vrtnodata" << lineVrtNoData->text();
        }

        if(!lineUserOptions->text().isEmpty())
        {
            args << lineUserOptions->text().split(" ", QString::SkipEmptyParts);
        }
    }

    args << labelTargetFilename->text();
    if(QFile::exists(labelTargetFilename->text()))
    {
        QFile::remove(labelTargetFilename->text());
    }

    tempFile->open();
    tempFile->resize(0);
    QTextStream stream(tempFile);
    const int N = listWidget->count();
    for(int n = 0; n < N; n++)
    {
        stream << listWidget->item(n)->text() << endl;
    }
    tempFile->close();
    args << "-input_file_list" << tempFile->fileName();

    stdOut("gdalbuildvrt " +  args.join(" ") + "\n");
    cmd.start("gdalbuildvrt", args);
}

void CMapVrtBuilder::finished(int exitCode, QProcess::ExitStatus status)
{
    if(!last && groupOverviews->isChecked())
    {
        QStringList args;
        args << labelTargetFilename->text();
        if(checkBy2->isChecked())
        {
            args << "2";
        }
        if(checkBy4->isChecked())
        {
            args << "4";
        }
        if(checkBy8->isChecked())
        {
            args << "8";
        }
        if(checkBy16->isChecked())
        {
            args << "16";
        }
        if(checkBy32->isChecked())
        {
            args << "32";
        }
        if(checkBy64->isChecked())
        {
            args << "64";
        }
        stdOut("gdaladdo " +  args.join(" ") + "\n");
        cmd.start("gdaladdo", args);

        last = true;
        return;
    }
    textBrowser->setTextColor(Qt::darkGreen);
    textBrowser->append(tr("!!! done !!!\n"));
    pushStart->setEnabled(true);
}

void CMapVrtBuilder::slotLinkActivated(const QUrl& url)
{
    QDesktopServices::openUrl(url);
}
