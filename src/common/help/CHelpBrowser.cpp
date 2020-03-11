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

#include "help/CHelpBrowser.h"
#include <QtHelp>

CHelpBrowser::CHelpBrowser(QHelpEngine *helpEngine, QWidget *parent)
    : QTextBrowser(parent)
    , engine(helpEngine)
{
    connect(engine->contentWidget(), &QHelpContentWidget::linkActivated, this, &CHelpBrowser::setSource);
    connect(engine->indexWidget(), &QHelpIndexWidget::linkActivated, this, &CHelpBrowser::setSource);
    connect(engine->searchEngine()->resultWidget(), &QHelpSearchResultWidget::requestShowLink, this, &CHelpBrowser::setSource);
}

void CHelpBrowser::setSource(const QUrl& url)
{
    if(url.scheme().startsWith("http"))
    {
        QDesktopServices::openUrl(url);
        return;
    }
    QTextBrowser::setSource(url);
}

QVariant CHelpBrowser::loadResource(int type, const QUrl &name)
{
    qDebug() << name;
    if (name.scheme() == "qthelp")
    {
        return QVariant(engine->fileData(name));
    }
    else
    {
        return QTextBrowser::loadResource(type, name);
    }
}
