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

#include "help/CHelpBrowser.h"

#include <QtHelp>

CHelpBrowser::CHelpBrowser(QHelpEngine* helpEngine, QWidget* parent) : QTextBrowser(parent), engine(helpEngine) {
  connect(engine->contentWidget(), &QHelpContentWidget::linkActivated, this, &CHelpBrowser::setSource);
  connect(engine->indexWidget(), &QHelpIndexWidget::documentActivated, this,
          [this](const QHelpLink& document, const QString& keyword) { setSource(document.url); });
  connect(engine->searchEngine()->resultWidget(), &QHelpSearchResultWidget::requestShowLink, this,
          &CHelpBrowser::setSource);

  QTextBrowser::setOpenLinks(false);
  connect(this, &QTextBrowser::anchorClicked, this, &CHelpBrowser::setSource);
}

void CHelpBrowser::setSource(const QUrl& url) {
  if (url.scheme().startsWith("http")) {
    QDesktopServices::openUrl(url);
  } else {
    QTextBrowser::setSource(url);
  }
}

QVariant CHelpBrowser::loadResource(int type, const QUrl& name) {
  if (name.scheme() == "qthelp") {
    return QVariant(engine->fileData(name));
  } else {
    return QTextBrowser::loadResource(type, name);
  }
}

void CHelpBrowser::contextMenuEvent(QContextMenuEvent* event) {
  QMenu* menu = createStandardContextMenu();

  menu->addSeparator();
  if (isBackwardAvailable()) {
    menu->addAction(QIcon(":/icons/32x32/Left.png"), tr("Go back one page"),
        Qt::CTRL | Qt::Key_Left, this, &CHelpBrowser::backward);
  }
  if (isForwardAvailable()) {
    menu->addAction(QIcon(":/icons/32x32/Right.png"), tr("Go forward one page"),
        Qt::CTRL | Qt::Key_Right, this, &CHelpBrowser::forward);
  }
  if (isBackwardAvailable()) {
    menu->addAction(QIcon(":/icons/32x32/ToTop.png"), tr("Go to initial page"),
        Qt::CTRL | Qt::Key_Up, this, &CHelpBrowser::home);
  }

  menu->exec(event->globalPos());

  delete menu;
}
