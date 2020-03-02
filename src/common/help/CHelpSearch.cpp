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

#include "help/CHelpSearch.h"

#include <QtGui>
#include <QtWidgets>
#include <QtHelp>

CHelpSearch::CHelpSearch(QHelpEngine *engine, QWidget *parent)
    : QWidget(parent)
    , searchEngine(engine->searchEngine())
    , query(searchEngine->queryWidget())
    , result(searchEngine->resultWidget())
{
    labelSearch = new QLabel(tr("Search:"), this);

    QVBoxLayout * l = new QVBoxLayout(this);
    l->addWidget(labelSearch);
    l->addWidget(query);
    l->addWidget(result);

    connect(searchEngine, &QHelpSearchEngine::indexingStarted, this, [](){qDebug() << "indexing started...";});
    connect(searchEngine, &QHelpSearchEngine::indexingFinished, this, [](){qDebug() << "indexing finished...";});

    connect(searchEngine, &QHelpSearchEngine::searchingStarted, this, [](){qDebug() << "searching started...";});
    connect(searchEngine, &QHelpSearchEngine::searchingFinished, this, [](){qDebug() << "searching finished...";});

    searchEngine->reindexDocumentation();

    connect(query, &QHelpSearchQueryWidget::search, this, &CHelpSearch::slotSearch);

}

void CHelpSearch::slotSearch()
{
    searchEngine->search(query->searchInput());
}
