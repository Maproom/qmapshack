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

#include <QtGui>
#include <QtHelp>

#include "help/CHelpBrowser.h"
#include "help/CHelpIndex.h"
#include "help/CHelpSearch.h"
#include "helpers/CSettings.h"

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

  CHelpBrowser* browser = new CHelpBrowser(engine, this);
  browser->setSource(QUrl(homepage));
  splitter->insertWidget(1, browser);

  setWidget(splitter);

  SETTINGS;
  cfg.beginGroup("Help");
  if (cfg.contains("geometry")) {
    restoreGeometry(cfg.value("geometry").toByteArray());
    splitter->restoreState(cfg.value("splitter").toByteArray());
  }
  cfg.endGroup();
}

CHelp::~CHelp() {
  SETTINGS;
  cfg.beginGroup("Help");
  cfg.setValue("geometry", saveGeometry());
  cfg.setValue("splitter", splitter->saveState());
  cfg.endGroup();
}
