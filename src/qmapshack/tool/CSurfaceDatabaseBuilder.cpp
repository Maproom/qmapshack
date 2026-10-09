/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#include "tool/CSurfaceDatabaseBuilder.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QtConcurrentRun>

#include "gis/trk/surface/CSurfaceDbBuilder.h"
#include "gis/trk/surface/CSurfaceDbManager.h"
#include "helpers/CSettings.h"
#include "theme/CUiTheme.h"

CSurfaceDatabaseBuilder::CSurfaceDatabaseBuilder(QWidget* parent) : QWidget(parent) {
  setObjectName(tr("Create Surface Database"));

  QVBoxLayout* layout = new QVBoxLayout(this);

  QLabel* help = new QLabel(this);
  help->setWordWrap(true);
  help->setOpenExternalLinks(true);
  help->setText(
      "<b>" + tr("Surface Database") + "</b><br/>" +
      tr("A surface database holds the roads and paths of an OpenStreetMap extract with their surface and type. "
         "With it, QMapShack shows the surfaces and way types of every track in the track's Info tab and can color "
         "tracks by surface (Style tab). Download an extract (.osm.pbf) e.g. from %1, the same file as for a Routino "
         "database. For a small country building takes about a minute and some 300 MB of memory.")
          .arg("<a href='https://download.geofabrik.de'>download.geofabrik.de</a>"));
  layout->addWidget(help);

  QGridLayout* grid = new QGridLayout();
  layout->addLayout(grid);

  grid->addWidget(new QLabel(tr("OSM extract"), this), 0, 0);
  lineSource = new QLineEdit(this);
  lineSource->setPlaceholderText(tr("e.g. belgium-latest.osm.pbf"));
  grid->addWidget(lineSource, 0, 1);
  QToolButton* toolSource = new QToolButton(this);
  toolSource->setIcon(QIcon("://icons/FileLoad.svgt"));
  toolSource->setToolTip(tr("Select the .osm.pbf file."));
  grid->addWidget(toolSource, 0, 2);

  grid->addWidget(new QLabel(tr("Name"), this), 1, 0);
  lineName = new QLineEdit(this);
  lineName->setPlaceholderText(tr("e.g. Belgium"));
  grid->addWidget(lineName, 1, 1);

  grid->addWidget(new QLabel(tr("Directory"), this), 2, 0);
  labelPath = new QLabel(this);
  labelPath->setTextInteractionFlags(Qt::TextSelectableByMouse);
  grid->addWidget(labelPath, 2, 1);
  QToolButton* toolPath = new QToolButton(this);
  toolPath->setIcon(QIcon("://icons/PathBlue.svgt"));
  toolPath->setToolTip(tr("Select the directory with the surface databases."));
  grid->addWidget(toolPath, 2, 2);

  grid->addWidget(new QLabel(tr("File"), this), 3, 0);
  labelTarget = new QLabel(this);
  grid->addWidget(labelTarget, 3, 1);

  QHBoxLayout* buttons = new QHBoxLayout();
  pushStart = new QPushButton(tr("Build"), this);
  pushCancel = new QPushButton(tr("Cancel"), this);
  progress = new QProgressBar(this);
  progress->setRange(0, 1000);
  progress->setTextVisible(true);
  buttons->addWidget(pushStart);
  buttons->addWidget(pushCancel);
  buttons->addWidget(progress, 1);
  layout->addLayout(buttons);

  labelStatus = new QLabel(this);
  labelStatus->setWordWrap(true);
  layout->addWidget(labelStatus);

  layout->addWidget(new QLabel("<b>" + tr("Surface databases (checked ones are used)") + "</b>", this));
  treeDatabases = new QTreeWidget(this);
  treeDatabases->setRootIsDecorated(false);
  treeDatabases->setHeaderLabels({tr("Name"), tr("Area"), tr("OSM data of"), tr("File")});
  layout->addWidget(treeDatabases, 1);

  SETTINGS;
  lineSource->setText(cfg.value("SurfaceDatabaseBuilder/source", QString()).toString());
  lineName->setText(cfg.value("SurfaceDatabaseBuilder/name", QString()).toString());

  connect(toolSource, &QToolButton::clicked, this, &CSurfaceDatabaseBuilder::slotSelectSource);
  connect(toolPath, &QToolButton::clicked, this, &CSurfaceDatabaseBuilder::slotSelectPath);
  connect(pushStart, &QPushButton::clicked, this, &CSurfaceDatabaseBuilder::slotStart);
  connect(pushCancel, &QPushButton::clicked, this, &CSurfaceDatabaseBuilder::slotCancel);
  connect(lineSource, &QLineEdit::textChanged, this, &CSurfaceDatabaseBuilder::updateButtons);
  connect(lineName, &QLineEdit::textChanged, this, &CSurfaceDatabaseBuilder::updateButtons);
  connect(&watcher, &QFutureWatcher<QString>::finished, this, &CSurfaceDatabaseBuilder::slotFinished);
  connect(treeDatabases, &QTreeWidget::itemChanged, this, &CSurfaceDatabaseBuilder::slotItemChanged);
  connect(&CSurfaceDbManager::self(), &CSurfaceDbManager::sigChanged, this,
          &CSurfaceDatabaseBuilder::slotDatabasesChanged);

  CSurfaceDbManager::self().rescan();
  slotDatabasesChanged();
  updateButtons();
}

CSurfaceDatabaseBuilder::~CSurfaceDatabaseBuilder() {
  // the worker reports to this widget: stop it before going away
  cancel = true;
  watcher.waitForFinished();
}

QString CSurfaceDatabaseBuilder::targetFile() const {
  QString name = lineName->text().trimmed();
  static const QRegularExpression unsafe("[^\\w.-]+");
  name.replace(unsafe, "_");
  if (name.isEmpty()) {
    return QString();
  }
  return QDir(CSurfaceDbManager::self().getPath()).absoluteFilePath(name + "." + CSurfaceDbBuilder::suffix());
}

void CSurfaceDatabaseBuilder::updateButtons() {
  const bool running = watcher.isRunning();
  const QString target = targetFile();
  labelPath->setText(CSurfaceDbManager::self().getPath());
  labelTarget->setText(target.isEmpty() ? "-" : QFileInfo(target).fileName());
  pushStart->setEnabled(!running && !target.isEmpty() && QFileInfo(lineSource->text()).isFile());
  pushCancel->setEnabled(running);
  lineSource->setEnabled(!running);
  lineName->setEnabled(!running);
}

void CSurfaceDatabaseBuilder::slotSelectSource() {
  QString path = lineSource->text();
  if (path.isEmpty()) {
    SETTINGS;
    const QStringList routino = cfg.value("Route/routino/paths", QStringList()).toStringList();
    path = routino.isEmpty() ? QDir::homePath() : routino.first();
  }
  const QString file =
      QFileDialog::getOpenFileName(this, tr("Select OSM extract..."), path, tr("OSM extract (*.osm.pbf *.pbf)"));
  if (file.isEmpty()) {
    return;
  }
  lineSource->setText(file);
  if (lineName->text().isEmpty()) {
    // belgium-latest.osm.pbf -> Belgium
    QString name = QFileInfo(file).fileName().section('.', 0, 0).remove("-latest");
    if (!name.isEmpty()) {
      name[0] = name[0].toUpper();
    }
    lineName->setText(name);
  }
}

void CSurfaceDatabaseBuilder::slotSelectPath() {
  const QString path =
      QFileDialog::getExistingDirectory(this, tr("Select directory..."), CSurfaceDbManager::self().getPath());
  if (path.isEmpty()) {
    return;
  }
  CSurfaceDbManager::self().setPath(path);
  updateButtons();
}

void CSurfaceDatabaseBuilder::slotStart() {
  const QString target = targetFile();
  if (QFileInfo::exists(target)) {
    const int res = QMessageBox::question(this, tr("Replace database?"),
                                          tr("%1 already exists. Replace it?").arg(QFileInfo(target).fileName()),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (res != QMessageBox::Yes) {
      return;
    }
  }
  if (!QDir().mkpath(CSurfaceDbManager::self().getPath())) {
    labelStatus->setText(tr("Can't create %1.").arg(CSurfaceDbManager::self().getPath()));
    return;
  }

  SETTINGS;
  cfg.setValue("SurfaceDatabaseBuilder/source", lineSource->text());
  cfg.setValue("SurfaceDatabaseBuilder/name", lineName->text());

  CSurfaceDbBuilder::options_t opts;
  opts.pbf = lineSource->text();
  opts.target = target;
  opts.name = lineName->text().trimmed();

  cancel = false;
  progress->setValue(0);
  labelStatus->setText(tr("Starting..."));

  watcher.setFuture(QtConcurrent::run([opts, this]() {
    CSurfaceDbBuilder builder(opts);
    qint32 last = -1;
    const bool ok = builder.build(
        [&](qreal fraction, const QString& step) {
          const qint32 value = qRound(fraction * 1000);
          if (value == last) {
            return;
          }
          last = value;
          QMetaObject::invokeMethod(
              this,
              [this, value, step]() {
                progress->setValue(value);
                labelStatus->setText(step);
              },
              Qt::QueuedConnection);
        },
        &cancel);
    if (!ok) {
      return builder.errorString();
    }
    const CSurfaceDbBuilder::stats_t& stats = builder.statistics();
    return QString("ok:") + tr("Done: %1 roads and paths with %2 nodes.").arg(stats.ways).arg(stats.nodes);
  }));
  updateButtons();
}

void CSurfaceDatabaseBuilder::slotCancel() {
  cancel = true;
  labelStatus->setText(tr("Canceling..."));
}

void CSurfaceDatabaseBuilder::slotFinished() {
  const QString res = watcher.result();
  if (res.startsWith("ok:")) {
    progress->setValue(1000);
    labelStatus->setText(res.mid(3));
    CUiTheme::markLabel(labelStatus, CUiTheme::Role::eOk);
  } else {
    labelStatus->setText(tr("Failed: %1").arg(res));
    CUiTheme::markLabel(labelStatus, CUiTheme::Role::eError);
  }
  CSurfaceDbManager::self().rescan();
  updateButtons();
}

void CSurfaceDatabaseBuilder::slotDatabasesChanged() {
  treeDatabases->blockSignals(true);
  treeDatabases->clear();
  for (const CSurfaceDbManager::db_t& db : CSurfaceDbManager::self().getDatabases()) {
    QTreeWidgetItem* item = new QTreeWidgetItem(treeDatabases);
    item->setData(0, Qt::UserRole, db.file);
    item->setText(3, QFileInfo(db.file).fileName());
    if (!db.error.isEmpty()) {
      item->setText(0, tr("unusable"));
      item->setToolTip(0, db.error);
      continue;
    }
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(0, db.active ? Qt::Checked : Qt::Unchecked);
    item->setText(0, db.name);
    const QRectF& r = db.boundingBox;
    item->setText(1, QString("%1..%2 N, %3..%4 E")
                         .arg(r.top(), 0, 'f', 2)
                         .arg(r.bottom(), 0, 'f', 2)
                         .arg(r.left(), 0, 'f', 2)
                         .arg(r.right(), 0, 'f', 2));
    item->setText(2, db.osmTimestamp.isValid() ? QLocale().toString(db.osmTimestamp.toLocalTime(), QLocale::ShortFormat)
                                               : tr("unknown"));
    item->setToolTip(3,
                     tr("%1\nfrom %2, built %3")
                         .arg(db.file, db.source, QLocale().toString(db.created.toLocalTime(), QLocale::ShortFormat)));
  }
  for (qint32 i = 0; i < treeDatabases->columnCount(); i++) {
    treeDatabases->resizeColumnToContents(i);
  }
  treeDatabases->blockSignals(false);
  updateButtons();
}

void CSurfaceDatabaseBuilder::slotItemChanged(QTreeWidgetItem* item, int column) {
  if (column != 0) {
    return;
  }
  const QString file = item->data(0, Qt::UserRole).toString();
  const bool active = item->checkState(0) == Qt::Checked;
  // the manager emits sigChanged(), which rebuilds the list: do it after this signal
  QMetaObject::invokeMethod(
      this, [file, active]() { CSurfaceDbManager::self().setActive(file, active); }, Qt::QueuedConnection);
}
