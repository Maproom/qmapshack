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
    : QWidget(parent)
    , tainted(false)
{
    setupUi(this);
    setObjectName(tr("Build GDAL VRT"));

    connect(toolSourceFiles, SIGNAL(clicked()), this, SLOT(slotSelectSourceFiles()));
    connect(toolTargetFile, SIGNAL(clicked()), this, SLOT(slotSelectTargetFile()));
    connect(pushStart, SIGNAL(clicked()), this, SLOT(slotStart()));

    connect(&cmd, SIGNAL(readyReadStandardError()), this, SLOT(slotStderr()));
    connect(&cmd, SIGNAL(readyReadStandardOutput()), this, SLOT(slotStdout()));
    connect(&cmd, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(slotFinished(int,QProcess::ExitStatus)));

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
    foreach(const QString& file, files)
    {
        new QListWidgetItem(QIcon("://icons/32x32/Map.png"), file, listWidget);
    }

    enabelStartButton();
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

    enabelStartButton();
}


void CMapVrtBuilder::enabelStartButton()
{
    pushStart->setDisabled(true);
    if(listWidget->count() == 0)
    {
        return;
    }
    if(labelTargetFilename->text() == "-")
    {
        return;
    }
    pushStart->setEnabled(true);
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

void CMapVrtBuilder::progress(const QString& str)
{
    QRegExp re("^(0[0-9\\.]*).*$");

    output += str;
    QStringList lines = output.split("\n");

    if(re.exactMatch(lines.last()))
    {
        QString prog    = re.cap(1);
        int points      = prog.count('.');
        int zeros       = prog.count('0');
        int p = (zeros - 1) * 10 + (points%3) * 2.5 + ((points/3) == zeros ? 7.5 : 0);
        if(p > 100) p = 100;

        progressBar->setValue(p);
    }

}

void CMapVrtBuilder::slotStderr()
{
    QString str;
    textBrowser->setTextColor(Qt::red);

    str = cmd.readAllStandardError();

#ifndef WIN32
    if(str[0] == '\r')
    {
        textBrowser->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
        textBrowser->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
        textBrowser->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
        textBrowser->textCursor().removeSelectedText();

        str = str.split("\r").last();
    }
#endif

    textBrowser->insertPlainText(str);
    textBrowser->verticalScrollBar()->setValue(textBrowser->verticalScrollBar()->maximum());

    tainted = true;
}

void CMapVrtBuilder::slotStdout()
{
    QString str;
    textBrowser->setTextColor(Qt::blue);
    str = cmd.readAllStandardOutput();

#ifndef WIN32
    if(str[0] == '\r')
    {
        textBrowser->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
        textBrowser->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
        textBrowser->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
        textBrowser->textCursor().removeSelectedText();

        str = str.split("\r").last();
    }
#endif

    progress(str);

    textBrowser->insertPlainText(str);
    textBrowser->verticalScrollBar()->setValue(textBrowser->verticalScrollBar()->maximum());
}

void CMapVrtBuilder::stdOut(const QString& str, bool gui)
{
    textBrowser->setTextColor(Qt::black);
    textBrowser->append(str);
}


void CMapVrtBuilder::stdErr(const QString& str, bool gui)
{
    textBrowser->setTextColor(Qt::red);
    textBrowser->append(str);
}


void CMapVrtBuilder::slotFinished(int exitCode, QProcess::ExitStatus status)
{
    output.clear();

    if(exitCode || status)
    {

        textBrowser->setTextColor(Qt::red);
        textBrowser->append(tr("!!! failed !!!\n"));
    }
}
