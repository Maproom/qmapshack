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

#include "CHelp.h"

#include <QMainWindow>
#include <QtGui>
#include <QtHelp>

#include "help/CHelpBrowser.h"
#include "help/CHelpIndex.h"
#include "help/CHelpSearch.h"
#include "helpers/CSettings.h"

/** The size of the help window the first time it opens [px]. */
static constexpr QSize kDefaultSize(1100, 750);

static bool isFileNewer(const QString& originalPath, const QString& copyPath) {
  QFileInfo originalInfo(originalPath);
  QFileInfo copyInfo(copyPath);

  // Ensure both files actually exist before comparing
  if (!originalInfo.exists() || !copyInfo.exists()) {
    return originalInfo.exists();  // If only original exists, it's "newer"
  }

  // Direct comparison of QDateTime objects
  return originalInfo.lastModified() > copyInfo.lastModified();
}

CHelp::CHelp(const QString& sourceQhc, const QString& homepage, QWidget* parent) : QDockWidget(tr("Help"), parent) {
  setWindowFlag(Qt::Tool, true);
  setAttribute(Qt::WA_DeleteOnClose, true);

  splitter = new QSplitter(Qt::Horizontal, this);

  QFileInfo fi(sourceQhc);

  const QString& userDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  QDir().mkpath(userDir);

  const QString& userQhc = QDir(userDir).filePath(fi.baseName() + ".qhc");
  const QString& sourceQch = fi.dir().filePath(fi.baseName() + ".qch");

  // Copy once if it doesn't exist
  if (isFileNewer(sourceQhc, userQhc)) {
    QFile::copy(sourceQhc, userQhc);
  }

  qDebug() << "compressed help:" << sourceQch;
  qDebug() << "help collection:" << userQhc;
  engine = new QHelpEngine(userQhc, this);
  engine->setupData();

  for (const QString& ns : engine->registeredDocumentations()) {
    engine->unregisterDocumentation(ns);
  }

  if (!engine->registerDocumentation(sourceQch)) {
    qWarning() << "failed to register" << sourceQch << "Reason:" << engine->error();
  }

  qDebug() << "Registered docs:" << engine->registeredDocumentations();

  index = new CHelpIndex(engine, this);
  search = new CHelpSearch(engine, this);

  tabWidget = new QTabWidget(this);
  tabWidget->addTab(engine->contentWidget(), tr("Content"));
  tabWidget->addTab(index, tr("Index"));
  tabWidget->addTab(search, tr("Search"));

  splitter->insertWidget(0, tabWidget);

  browser = new CHelpBrowser(engine, this);
  browser->setSource(QUrl(homepage));
  splitter->insertWidget(1, browser);

  setWidget(splitter);
  restoreSplitter();
}

CHelp::CHelp(const QString& homepage, QWidget* parent) : QDockWidget(tr("Help"), parent) {
  setWindowFlag(Qt::Tool, true);
  setAttribute(Qt::WA_DeleteOnClose, true);

  browser = new CHelpBrowser(nullptr, this);
  browser->setSource(QUrl::fromLocalFile(homepage));
  setWidget(browser);
}

CHelp::~CHelp() {
  SETTINGS;
  cfg.beginGroup("Help");
  cfg.setValue("geometry", saveGeometry());
  cfg.setValue("floating", isFloating());
  if (splitter != nullptr) {
    cfg.setValue("splitter", splitter->saveState());
  }
  cfg.endGroup();
}

void CHelp::restoreSplitter() {
  SETTINGS;
  if (cfg.contains("Help/splitter")) {
    splitter->restoreState(cfg.value("Help/splitter").toByteArray());
  }
}

void CHelp::place(QMainWindow* main) {
  SETTINGS;
  cfg.beginGroup("Help");
  const bool floating = cfg.value("floating", true).toBool();
  const QByteArray& geometry = cfg.value("geometry").toByteArray();
  cfg.endGroup();

  setFloating(floating);
  if (!floating) {
    return;
  }
  if (!geometry.isEmpty() && restoreGeometry(geometry)) {
    return;
  }
  // Docked, it would take the width of the dock column; the pages need room.
  resize(kDefaultSize.boundedTo(main->size()));
  move(main->geometry().center() - rect().center());
}

void CHelp::setPage(const QString& file) { browser->setSource(QUrl::fromLocalFile(file)); }
