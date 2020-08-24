/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "setup/IAppSetup.h"
#include "tool/CRoutinoDatabaseBuilder.h"

#include <QtWidgets>


CRoutinoDatabaseBuilder::CRoutinoDatabaseBuilder(QWidget * parent)
    : IToolShell(parent)
{
    setupUi(this);
    setTextBrowser(textBrowser);

    setObjectName(tr("Create Routino Database"));

    connect(toolSourceFiles,  &QToolButton::clicked,   this, &CRoutinoDatabaseBuilder::slotSelectSourceFiles);
    connect(toolTargetPath,   &QToolButton::clicked,   this, &CRoutinoDatabaseBuilder::slotSelectTargetPath);
    connect(pushStart,        &QPushButton::clicked,   this, &CRoutinoDatabaseBuilder::slotStart);
    connect(lineTargetPrefix, &QLineEdit::textChanged, this, &CRoutinoDatabaseBuilder::enabelStartButton);
    connect(labelHelp,        &QLabel::linkActivated,  this, &CRoutinoDatabaseBuilder::slotLinkActivated);

    pushStart->setDisabled(true);

    SETTINGS;
    QString path = CMainWindow::self().getRoutinoPath();
    if(path.isEmpty())
    {
        path = QDir::homePath();
    }
    path = cfg.value("RoutinoDatabaseBuilder/targetPath", path).toString();

    labelTargetPath->setText(path);

    enabelStartButton();
}

CRoutinoDatabaseBuilder::~CRoutinoDatabaseBuilder()
{
}

void CRoutinoDatabaseBuilder::slotSelectSourceFiles()
{
    SETTINGS;
    QString path = cfg.value("RoutinoDatabaseBuilder/sourcePath", QDir::homePath()).toString();

    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files..."), path, "OSM Database (*.pbf)");
    if(files.isEmpty())
    {
        return;
    }

    QFileInfo fi(files.first());
    path = fi.absolutePath();
    cfg.setValue("RoutinoDatabaseBuilder/sourcePath", path);

    listWidget->clear();
    for(const QString &file : files)
    {
        new QListWidgetItem(QIcon("://icons/32x32/Map.png"), file, listWidget);
    }

    enabelStartButton();
}

void CRoutinoDatabaseBuilder::slotSelectTargetPath()
{
    SETTINGS;
    QString path = cfg.value("RoutinoDatabaseBuilder/targetPath", QDir::homePath()).toString();

    path = QFileDialog::getExistingDirectory(this, tr("Select target path..."), path);
    if(path.isEmpty())
    {
        return;
    }

    cfg.setValue("RoutinoDatabaseBuilder/targetPath", path);


    labelTargetPath->setText(path);

    enabelStartButton();
}


void CRoutinoDatabaseBuilder::enabelStartButton()
{
    pushStart->setDisabled(true);

    planetsplitter = QStandardPaths::findExecutable("planetsplitter");
    if(planetsplitter.isEmpty())
    {
        planetsplitter = QStandardPaths::findExecutable("routino-planetsplitter");
    }
    labelError->setVisible(planetsplitter.isEmpty());

    if(planetsplitter.isEmpty())
    {
        return;
    }

    if(listWidget->count() == 0)
    {
        return;
    }
    if(labelTargetPath->text() == "-")
    {
        return;
    }

    if(lineTargetPrefix->text().isEmpty())
    {
        return;
    }

    pushStart->setEnabled(true);
}


void CRoutinoDatabaseBuilder::slotStart()
{
    pushStart->setDisabled(true);

    sourceFiles.clear();
    const int N = listWidget->count();
    for(int n = 0; n < N; n++)
    {
        sourceFiles << listWidget->item(n)->text();
    }

    targetPrefix    = lineTargetPrefix->text();
    targetPath      = labelTargetPath->text();
    first           = true;
    last            = false;

    textBrowser->clear();

    slotFinished(0, QProcess::NormalExit);
}

void CRoutinoDatabaseBuilder::finished(int exitCode, QProcess::ExitStatus status)
{
    if(last)
    {
        textBrowser->setTextColor(Qt::darkGreen);
        textBrowser->append(tr("!!! done !!!\n"));
        pushStart->setEnabled(true);
        return;
    }

    IAppSetup* instance = IAppSetup::getPlatformInstance();
    if(sourceFiles.isEmpty())
    {
        QStringList args;
        args << QString("--dir=%1").arg(targetPath);
        args << QString("--prefix=%1").arg(targetPrefix);
        args << QString("--tagging=%1").arg(instance->routinoPath("tagging.xml"));
        args << "--process-only";

        stdOut(planetsplitter + " " +  args.join(" ") + "\n");
        cmd.start(planetsplitter, args);

        last = true;
    }
    else
    {
        QStringList args;
        args << QString("--dir=%1").arg(targetPath);
        args << QString("--prefix=%1").arg(targetPrefix);
        args << QString("--tagging=%1").arg(instance->routinoPath("tagging.xml"));

        if(first)
        {
            first = false;
            args << "--parse-only";
        }
        else
        {
            args << "--parse-only" << "--append";
        }

        args << sourceFiles.first();
        sourceFiles.pop_front();


        stdOut(planetsplitter + " " +  args.join(" ") + "\n");
        cmd.start(planetsplitter, args);
    }
}

void CRoutinoDatabaseBuilder::slotLinkActivated(const QUrl& url)
{
    QDesktopServices::openUrl(url);
}
