/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#include "CHelp.h"
#include "help/CHelpIndex.h"
#include "helpers/CSettings.h"
#include "help/CHelpBrowser.h"
#include "help/CHelpSearch.h"


#include <QtGui>
#include <QtHelp>


CHelp::CHelp(const QString& helpfile, const QString& homepage, QWidget *parent)
    : QDockWidget(tr("Help"),parent)
{
    setWindowFlag(Qt::Tool, true);
    setAttribute(Qt::WA_DeleteOnClose, true);

    splitter = new QSplitter(Qt::Horizontal, this);

    qDebug() << "search help at:" << helpfile;
    engine = new QHelpEngine(helpfile, this);
    engine->setupData();

    index = new CHelpIndex(engine, this);
    search = new CHelpSearch(engine, this);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(engine->contentWidget(), tr("Content"));
    tabWidget->addTab(index, tr("Index"));
    tabWidget->addTab(search, tr("Search"));

    splitter->insertWidget(0, tabWidget);

    CHelpBrowser *browser = new CHelpBrowser(engine, this);
    browser->setSource(QUrl(homepage));
    splitter->insertWidget(1, browser);

    setWidget(splitter);

    SETTINGS;
    cfg.beginGroup("Help");
    if ( cfg.contains("geometry"))
    {
        restoreGeometry(cfg.value("geometry").toByteArray());
        splitter->restoreState(cfg.value("splitter").toByteArray());
    }
    cfg.endGroup();
}


CHelp::~CHelp()
{
    SETTINGS;
    cfg.beginGroup("Help");
    cfg.setValue("geometry", saveGeometry());
    cfg.setValue("splitter", splitter->saveState());
    cfg.endGroup();
}

