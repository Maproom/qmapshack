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

#include <QEvent>
#include <QtHelp>

#include "theme/CUiTheme.h"

CHelpBrowser::CHelpBrowser(QHelpEngine* helpEngine, QWidget* parent) : QTextBrowser(parent), engine(helpEngine) {
  connect(engine->contentWidget(), &QHelpContentWidget::linkActivated, this, &CHelpBrowser::setSource);
  connect(engine->indexWidget(), &QHelpIndexWidget::documentActivated, this,
          [this](const QHelpLink& document, const QString& keyword) { setSource(document.url); });
  connect(engine->searchEngine()->resultWidget(), &QHelpSearchResultWidget::requestShowLink, this,
          &CHelpBrowser::setSource);

  QTextBrowser::setOpenLinks(false);
  connect(this, &QTextBrowser::anchorClicked, this, &CHelpBrowser::setSource);
}

void CHelpBrowser::changeEvent(QEvent* e) {
  QTextBrowser::changeEvent(e);
  // loadResource() resolves the themed colours once per page load, so re-read the page.
  if (CUiTheme::isPaletteChange(e)) {
    reload();
  }
}

void CHelpBrowser::setSource(const QUrl& url) {
  if (url.scheme().startsWith("http")) {
    QDesktopServices::openUrl(url);
  } else {
    QTextBrowser::setSource(url);
  }
}

QVariant CHelpBrowser::loadResource(int type, const QUrl& name) {
  const QVariant data =
      (name.scheme() == "qthelp") ? QVariant(engine->fileData(name)) : QTextBrowser::loadResource(type, name);

  if (type == QTextDocument::StyleSheetResource) {
    // The packaged help CSS fills the code background but leaves the text colour to the palette,
    // which inverts on a dark scheme. Append the themed pair - a later rule wins. A miss becomes
    // a hit carrying just that rule, so code blocks stay themed even with no stylesheet shipped.
    return QString::fromUtf8(data.toByteArray()) + "\ncode, pre {" + CUiTheme::css(CUiTheme::Role::eCode) + "}\n";
  }

  return data;
}

void CHelpBrowser::contextMenuEvent(QContextMenuEvent* event) {
  QMenu* menu = createStandardContextMenu();

  menu->addSeparator();
  if (isBackwardAvailable()) {
    menu->addAction(QIcon(":/icons/Left.svgt"), tr("Go back one page"), Qt::CTRL | Qt::Key_Left, this,
                    &CHelpBrowser::backward);
  }
  if (isForwardAvailable()) {
    menu->addAction(QIcon(":/icons/Right.svgt"), tr("Go forward one page"), Qt::CTRL | Qt::Key_Right, this,
                    &CHelpBrowser::forward);
  }
  if (isBackwardAvailable()) {
    menu->addAction(QIcon(":/icons/ToTop.svgt"), tr("Go to initial page"), Qt::CTRL | Qt::Key_Up, this,
                    &CHelpBrowser::home);
  }

  menu->exec(event->globalPos());

  delete menu;
}
