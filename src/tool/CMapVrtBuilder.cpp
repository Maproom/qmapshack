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

#include "CMapVrtBuilder.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CMapVrtBuilder::CMapVrtBuilder(QWidget *parent)
    : IToolShell(textBrowser, parent)
{
    setupUi(this);
    setObjectName(tr("Build GDAL VRT"));

    connect(toolSourceFiles, &QToolButton::clicked, this, &CMapVrtBuilder::slotSelectSourceFiles);
    connect(toolTargetFile,  &QToolButton::clicked, this, &CMapVrtBuilder::slotSelectTargetFile);
    connect(pushStart,       &QPushButton::clicked, this, &CMapVrtBuilder::slotStart);

    pushStart->setDisabled(true);
}

CMapVrtBuilder::~CMapVrtBuilder()
{
}

void CMapVrtBuilder::slotSelectSourceFiles()
{
    SETTINGS;
    QString path = cfg.value("VrtBuilder/sourcePath",QDir::homePath()).toString();

    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files..."), path);
    if(files.isEmpty())
    {
        return;
    }

    QFileInfo fi(files.first());
    path = fi.absolutePath();
    cfg.setValue("VrtBuilder/sourcePath", path);

    listWidget->clear();
    foreach(const QString &file, files)
    {
        new QListWidgetItem(QIcon("://icons/32x32/Map.png"), file, listWidget);
    }

    enableStartButton();
}

void CMapVrtBuilder::slotSelectTargetFile()
{
    SETTINGS;
    QString path = cfg.value("VrtBuilder/targetPath",QDir::homePath()).toString();

    QString file = QFileDialog::getSaveFileName(this, tr("Select target file..."), path);
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

    QStringList args;
    args << labelTargetFilename->text();

    foreach(const QListWidgetItem * item, listWidget->findItems("*", Qt::MatchWildcard))
    {
        args << item->text();
    }

    stdOut("gdalbuildvrt " +  args.join(" ") + "\n");
    cmd.start("gdalbuildvrt", args);
}

void CMapVrtBuilder::finished(int exitCode, QProcess::ExitStatus status)
{
    textBrowser->setTextColor(Qt::darkGreen);
    textBrowser->append(tr("!!! done !!!\n"));
    pushStart->setEnabled(true);
    return;
}
