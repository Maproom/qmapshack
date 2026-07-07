/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CVrtAdvisoryDialog.h"

#include <QDomDocument>
#include <QtWidgets>

#include "helpers/CVrtCombiner.h"
#include "setup/IAppSetup.h"
#include "units/IUnit.h"

CVrtAdvisoryDialog::CVrtAdvisoryDialog(const QString& filename, const CGdalVrtUtil::overview_advice_t& advice,
                                       const CGdalVrtUtil::raster_geometry_t& geometry, QWidget* parent)
    : QDialog(parent), filename_(filename), advice_(advice) {
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  // Application-modal: the map/DEM this dialog is about stays live otherwise, so panning
  // (draw()) or a mouse-move (getElevationAt()) would read the very files a Fix/Combine job
  // is rewriting - a concurrent-access crash. There is no sense working with QMapShack while
  // its files are being processed, so block the app and let the user wait.
  setModal(true);

  // More breathing room than the style default, so two tables plus several label rows
  // don't read as touching lines.
  verticalLayout->setSpacing(12);

  buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Fix overviews"));
  buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Later"));
  // Clear the platform-default role icons so no button in this dialog has one -
  // buttonCombine_ (added below) never gets one either.
  buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon());
  buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon());
  disconnect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &CVrtAdvisoryDialog::slotFixOverviews);

  // Respect the user's configured unit system (metric/imperial/nautical) rather than
  // hard-coding meters - same helper the rest of the app uses for any user-facing distance.
  const auto distStr = [](qreal meters) {
    QString val, unit;
    IUnit::self().meter2distance(meters, val, unit);
    return val + " " + unit;
  };
  const qreal widthMeters = geometry.xsizePx * geometry.pixelSizeX;
  const qreal heightMeters = geometry.ysizePx * geometry.pixelSizeY;
  labelRasterInfo->setText(tr("%1 × %2 px  ·  %3/px × %4/px  ·  %5 × %6")
                               .arg(geometry.xsizePx)
                               .arg(geometry.ysizePx)
                               .arg(distStr(geometry.pixelSizeX))
                               .arg(distStr(geometry.pixelSizeY))
                               .arg(distStr(widthMeters))
                               .arg(distStr(heightMeters)));

  if (advice_.hasTooManySubfiles()) {
    labelSubfileWarning->setText(tr("⚠ This file references %1 source files. Reading gets inefficient past %2 - "
                                    "consider combining them into fewer files.")
                                     .arg(advice_.perFileInfo.size())
                                     .arg(CGdalVrtUtil::overview_advice_t::kMaxSubfileCount));
    if (hasSourceFiles()) {
      // Placed under its own warning, not in buttonBox with "Fix overviews" - the two
      // actions fix unrelated problems.
      buttonCombine_ = new QPushButton(tr("Combine files..."));
      connect(buttonCombine_, &QPushButton::clicked, this, &CVrtAdvisoryDialog::slotCombineFiles);
      auto* combineRow = new QHBoxLayout();
      combineRow->addWidget(buttonCombine_);
      combineRow->addStretch();
      verticalLayout->insertLayout(verticalLayout->indexOf(labelSubfileWarning) + 1, combineRow);
    }
  } else {
    labelSubfileWarning->setVisible(false);
  }

  const QString suggestedStr = formatFactors(advice_.suggestedLevels);

  // ---- current situation ---- (every referenced file is shown, none truncated)
  QString currentHtml = "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border-collapse:collapse\">";
  currentHtml +=
      "<tr>" + htmlTh(tr("File")) + htmlTh(tr("Existing overview sizes (px)")) + htmlTh(tr("Status")) + "</tr>";

  // the container always leads the table, graded by the same rule as every source file
  {
    QString bg;
    const QString status = rowStatus(/*checked=*/true, advice_.containerOverviewSizes, advice_.containerSufficient, bg);
    const QString levels =
        formatFactors(advice_.containerOverviewSizes) +
        containerOvrSourceSuffix(hasSourceFiles(), advice_.containerHasOwnOvr, advice_.containerOverviewSizes);
    currentHtml +=
        "<tr>" + htmlTd(QFileInfo(filename_).fileName(), bg) + htmlTd(levels, bg) + htmlTd(status, bg) + "</tr>";
  }

  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    QString bg;
    const QString status = rowStatus(info.checked, info.overviewSizes, info.sufficient, bg);
    currentHtml += "<tr>" + htmlTd(QFileInfo(info.path).fileName(), bg) +
                   htmlTd(formatFactors(info.overviewSizes), bg) + htmlTd(status, bg) + "</tr>";
  }

  currentHtml += "</table>";

  // ---- after fix ----
  QString afterFixHtml = "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border-collapse:collapse\">";
  afterFixHtml += "<tr>" + htmlTh(tr("File")) + htmlTh(tr("New overview levels")) + htmlTh(tr("Action")) + "</tr>";

  const QStringList toFix = filesToFix();
  for (const QString& path : toFix) {
    const QString name = QFileInfo(path).fileName();
    const bool replacing = hasExistingOverviews(path);
    const QString action = replacing ? tr("Clean + rebuild") : tr("Build new");
    const QString bg = replacing ? "#fff3cd" : "#d4edda";
    afterFixHtml += "<tr>" + htmlTd(name, bg) + htmlTd(suggestedStr, bg) + htmlTd(action, bg) + "</tr>";
  }

  // Only a VRT with source files needs this extra row (see hasSourceFiles() - the
  // false case is already covered above). Unlike the rows above:
  // - this never builds overview pixel data on filename_ itself
  // - it only records, in the VRT's XML, which levels the source files provide
  // so the wording below says "Update"/"Add", not "Build new"/"Clean + rebuild".
  if (hasSourceFiles()) {
    const QString vrtName = QFileInfo(filename_).fileName();
    const bool replacing = hasExistingOverviews(filename_);
    // plain "<"/">" here - htmlTd() escapes the whole string itself, so a pre-escaped
    // entity would show up literally as "&lt;OverviewList&gt;"
    const QString action = replacing ? tr("Update <OverviewList>") : tr("Add <OverviewList>");
    const QString bg = replacing ? "#fff3cd" : "#d4edda";
    afterFixHtml += "<tr>" + htmlTd(vrtName, bg) + htmlTd(suggestedStr, bg) + htmlTd(action, bg) + "</tr>";
  }

  afterFixHtml += "</table>";

  // Measure each table's natural width using a temporary document (before any width="100%"),
  // then set the browsers to that width so the table fills them without white space.
  const auto idealWidth = [](const QString& html) {
    QTextDocument doc;
    doc.setHtml(html);
    return qRound(doc.idealWidth());
  };
  const int bw = qMax(idealWidth(currentHtml), idealWidth(afterFixHtml));

  const QString narrowTag = "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border-collapse:collapse\">";
  const QString fullTag =
      "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"border-collapse:collapse\">";
  textCurrent->setHtml(QString(currentHtml).replace(narrowTag, fullTag));
  textAfterFix->setHtml(QString(afterFixHtml).replace(narrowTag, fullTag));

  const int browserW = bw + textCurrent->frameWidth() * 2 + style()->pixelMetric(QStyle::PM_ScrollBarExtent);
  textCurrent->setMinimumWidth(browserW);
  textAfterFix->setMinimumWidth(browserW);

  // row height: cell padding (4px top + 4px bottom) + font height + 2px for borders.
  // Never truncated - tall tables scroll within the fixed-height browser below instead
  // of growing the dialog.
  const int rowH = fontMetrics().height() + 10;
  const qsizetype currentDataRows = 1 + advice_.perFileInfo.size();  // +1 for the container's own row
  textCurrent->setFixedHeight(qMin(static_cast<int>(1 + currentDataRows) * rowH + 8, 250));

  const qsizetype afterDataRows = static_cast<qsizetype>(toFix.size()) + (hasSourceFiles() ? 1 : 0);
  textAfterFix->setFixedHeight(qMin(static_cast<int>(1 + afterDataRows) * rowH + 8, 250));

  // ---- summary ----
  // Only describe build/replace work when overviews actually need fixing. A fully
  // qualified VRT has nothing to build (filesToFix() still names filename_ for the
  // no-source case), so the label then carries the disk-usage figure alone.
  QStringList summaryParts;
  if (advice_.needsOverviewFix()) {
    const qint32 replaceCount =
        static_cast<qint32>(
            std::count_if(toFix.begin(), toFix.end(), [this](const QString& p) { return hasExistingOverviews(p); })) +
        (hasSourceFiles() && hasExistingOverviews(filename_) ? 1 : 0);
    const qint32 buildCount = static_cast<qint32>(toFix.size()) + (hasSourceFiles() ? 1 : 0) - replaceCount;

    if (buildCount > 0) {
      summaryParts << tr("%1 file(s) will have new overviews built").arg(buildCount);
    }
    if (replaceCount > 0) {
      summaryParts << tr("%1 file(s) will have existing overviews cleaned and rebuilt").arg(replaceCount);
    }
  }

  // SI (base-1000) so the figure matches `du --si`, not the 1024-based IEC default.
  const QString sizeStr = QLocale::system().formattedDataSize(advice_.diskUsageBytes, 2, QLocale::DataSizeSIFormat);
  const QString usage =
      advice_.diskUsageIsEstimate ? tr("Estimated disk usage: %1.").arg(sizeStr) : tr("Disk usage: %1.").arg(sizeStr);
  // With nothing to fix there are no build/replace parts, so state the usage on its own.
  labelSummary->setText(summaryParts.isEmpty() ? usage : summaryParts.join(", ") % ". " % usage);

  // QLabel::sizeHint() reports the unwrapped single-line size; relying on the layout to
  // query heightForWidth() before adjustSize() isn't reliable, so compute the wrapped
  // height ourselves at browserW and set it as an explicit minimum.
  const auto ensureWrappedHeight = [this, browserW](QLabel* label) {
    const QRect bounds = fontMetrics().boundingRect(QRect(0, 0, browserW, 0), Qt::TextWordWrap, label->text());
    label->setMinimumHeight(bounds.height());
  };
  ensureWrappedHeight(labelRasterInfo);
  ensureWrappedHeight(labelSubfileWarning);
  ensureWrappedHeight(labelSummary);

  shell_ = new CShell(this, /*isSingleton=*/false);
  shell_->setVisible(false);
  verticalLayout->insertWidget(verticalLayout->indexOf(labelSummary), shell_);
  connect(shell_, &CShell::sigFinishedJob, this, &CVrtAdvisoryDialog::slotJobFinished);

  // Nothing to fix: hide the fix-overviews machinery regardless of hasTooManySubfiles().
  // labelSummary stays visible - its disk-usage estimate is informational at any time.
  if (!advice_.needsOverviewFix()) {
    labelAfterFixTitle->setVisible(false);
    textAfterFix->setVisible(false);
    buttonBox->button(QDialogButtonBox::Ok)->setVisible(false);
  }

  // Nothing to do at all (no overview fix, no subfile problem): plain "Overview info" +
  // "Close" treatment. A VRT with only the subfile problem keeps the normal wording since
  // it still has the Combine button to offer.
  if (!advice_.needsOverviewFix() && !advice_.hasTooManySubfiles()) {
    setWindowTitle(tr("Overview info"));
    checkSuppressAdvisory->setVisible(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));
  }

  // Force the deferred layout update to run before adjustSize() queries sizeHint.
  verticalLayout->activate();
  adjustSize();
}

QString CVrtAdvisoryDialog::formatFactors(const QVector<qint32>& factors) {
  if (factors.isEmpty()) {
    return tr("none");
  }
  QStringList parts;
  for (qint32 f : factors) {
    parts << QString::number(f);
  }
  return parts.join(", ");
}

QString CVrtAdvisoryDialog::htmlTh(const QString& text) {
  return QString("<th style=\"text-align:left; padding:4px 6px; background-color:#e0e0e0\">%1</th>")
      .arg(text.toHtmlEscaped());
}

QString CVrtAdvisoryDialog::htmlTd(const QString& text, const QString& bg) {
  return QString("<td style=\"padding:4px 6px; background-color:%1\">%2</td>").arg(bg, text.toHtmlEscaped());
}

QString CVrtAdvisoryDialog::rowStatus(bool checked, const QVector<qint32>& sizes, bool sufficient, QString& bg) {
  if (!checked) {
    bg = "#d4edda";
    return tr("✓ covered by .ovr");
  }
  if (sizes.isEmpty()) {
    bg = "#f8d7da";
    return tr("✗ None");
  }
  if (!sufficient) {
    bg = "#fff3cd";
    return tr("⚠ Shallow (coarsest %1px)").arg(sizes.last());
  }
  bg = "#d4edda";
  return tr("✓ OK");
}

QString CVrtAdvisoryDialog::containerOvrSourceSuffix(bool hasSourceFiles, bool hasOwnOvr,
                                                     const QVector<qint32>& sizes) {
  if (!hasSourceFiles || sizes.isEmpty()) {
    return QString();
  }
  return hasOwnOvr ? tr(" (own .ovr)") : tr(" (via <OverviewList>)");
}

bool CVrtAdvisoryDialog::hasExistingOverviews(const QString& filePath) const {
  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    if (info.path == filePath) {
      return info.checked && !info.overviewSizes.isEmpty();
    }
  }
  // No per-file entry: filePath is either the container's own row, or (see
  // hasSourceFiles()) filesToFix()'s fallback target. Either way, fall back to the
  // container's claim - containerHasOwnOvr, not just containerSufficient: a
  // fallback-trusted <OverviewList> (see buildOverviewAdvice()'s step 3) makes
  // containerSufficient true without there being anything physical to clean/replace, and
  // this result also drives the "Update"/"Add <OverviewList>" wording in the after-fix
  // preview table, which must agree with slotFixOverviews()'s confirmation dialog - that
  // one already keys off containerHasOwnOvr specifically because only a real .ovr file
  // gets deleted.
  return filePath == filename_ && advice_.containerHasOwnOvr;
}

QStringList CVrtAdvisoryDialog::filesToFix() const {
  if (!hasSourceFiles()) {
    // no source files - fix filename_ directly, no <OverviewList> bookkeeping
    return {filename_};
  }
  QStringList result;
  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    if (!info.sufficient) {
      result << info.path;
    }
  }
  return result;
}

void CVrtAdvisoryDialog::slotFixOverviews() {
  const QString gdaladdo = IAppSetup::getPlatformInstance()->findExecutable("gdaladdo");
  if (gdaladdo.isEmpty()) {
    QMessageBox::warning(this, tr("gdaladdo not found"),
                         tr("Could not locate gdaladdo. Make sure the GDAL tools are installed and on PATH."));
    return;
  }

  const QString resampleAlg = resampleAlgorithm();
  QStringList levelArgs;
  for (qint32 level : advice_.suggestedLevels) {
    levelArgs << QString::number(level);
  }

  const QStringList toFix = filesToFix();

  // confirmation dialog — list all operations; highlight destructive ones in orange
  QString confirmHtml = "<p>" + tr("The following operations will be performed:") + "</p><table>";
  for (const QString& path : toFix) {
    const QString name = QFileInfo(path).fileName().toHtmlEscaped();
    if (hasExistingOverviews(path)) {
      confirmHtml += "<tr><td>" + name +
                     "&nbsp;&nbsp;</td>"
                     "<td><span style=\"color:darkorange\">" +
                     tr("Clean + rebuild") + "</span></td></tr>";
    } else {
      confirmHtml += "<tr><td>" + name + "&nbsp;&nbsp;</td><td>" + tr("Build new") + "</td></tr>";
    }
  }
  // !hasSourceFiles() has no separate <OverviewList> step - gdaladdo above is the whole fix
  if (hasSourceFiles()) {
    const QString vrtName = QFileInfo(filename_).fileName().toHtmlEscaped();
    // orange only when a real .ovr file exists and will be deleted - containerSufficient
    // alone isn't enough: a bare <OverviewList> verified via source files (see
    // CGdalVrtUtil::buildOverviewAdvice()) has no file to delete
    if (advice_.containerHasOwnOvr) {
      confirmHtml += "<tr><td>" + vrtName +
                     "&nbsp;&nbsp;</td>"
                     "<td><span style=\"color:darkorange\">" +
                     tr("Update &lt;OverviewList&gt;") + "</span></td></tr>";
    } else {
      confirmHtml += "<tr><td>" + vrtName + "&nbsp;&nbsp;</td><td>" + tr("Add &lt;OverviewList&gt;") + "</td></tr>";
    }
  }
  confirmHtml += "</table>";

  if (!confirmYesNo(tr("Confirm fix"), confirmHtml, Qt::RichText)) {
    return;
  }

  // build command queue — only files that actually fall short get touched; a source file
  // that's already adequate on its own is left alone
  QList<CShellCmd> cmds;
  for (const QString& path : toFix) {
    if (hasExistingOverviews(path)) {
      cmds << CShellCmd(gdaladdo, {"-clean", path});
    }
    QStringList args = {"-ro",      "-r",       resampleAlg,         "--config", "GDAL_NUM_THREADS",
                        "ALL_CPUS", "--config", "COMPRESS_OVERVIEW", "DEFLATE"};
    // Match the source's horizontal predictor so DEFLATE can difference continuous data
    // (DEM/RGB), shrinking overviews ~2-3x. Skip it for palette/index rasters, where
    // differencing index values hurts compression.
    if (!advice_.isPaletteIndexed) {
      args << "--config" << "PREDICTOR_OVERVIEW" << "2";
    }
    args << path;
    args += levelArgs;
    cmds << CShellCmd(gdaladdo, args);
  }

  jobKind_ = JobKind::FixOverviews;
  switchToProgressMode();
  jobId_ = shell_->execute(cmds);
}

bool CVrtAdvisoryDialog::confirmYesNo(const QString& title, const QString& text, Qt::TextFormat format) {
  QMessageBox msgBox(this);
  msgBox.setWindowTitle(title);
  msgBox.setText(text);
  msgBox.setTextFormat(format);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  return msgBox.exec() == QMessageBox::Yes;
}

void CVrtAdvisoryDialog::removeIfExists(const QStringList& paths) {
  for (const QString& path : paths) {
    if (QFileInfo::exists(path)) {
      QFile::remove(path);
      shell_->stdOut(tr("Removed %1.\n").arg(QFileInfo(path).fileName()));
    }
  }
}

void CVrtAdvisoryDialog::switchToProgressMode() {
  labelAfterFixTitle->setVisible(false);
  textAfterFix->setVisible(false);
  shell_->setVisible(true);
  shell_->setMinimumHeight(150);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  if (buttonCombine_ != nullptr) {
    buttonCombine_->setEnabled(false);
  }
  buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));
  verticalLayout->activate();
  adjustSize();
}

void CVrtAdvisoryDialog::slotCombineFiles() {
  if (IAppSetup::getPlatformInstance()->findExecutable("gdal_translate").isEmpty() ||
      IAppSetup::getPlatformInstance()->findExecutable("gdalbuildvrt").isEmpty()) {
    QMessageBox::warning(this, tr("GDAL tools not found"),
                         tr("Could not locate gdal_translate/gdalbuildvrt. Make sure the GDAL tools are installed "
                            "and on PATH."));
    return;
  }

  // Layout comes entirely from the VRT XML (raster grid + each source's <DstRect>) - no
  // pixel reads, resolves in ms, so it runs inline on the GUI thread.
  const CVrtCombiner::layout_t layout = CVrtCombiner::readVrtLayout(filename_);
  if (!layout.valid()) {
    QMessageBox::warning(this, tr("Combine failed"),
                         tr("Could not read the raster layout of %1.").arg(QFileInfo(filename_).fileName()));
    return;
  }

  const QVector<CVrtCombiner::pixel_window_t> grid = CVrtCombiner::computeGrid(layout.rasterWidth, layout.rasterHeight);
  QVector<CVrtCombiner::pixel_window_t> tiles;
  for (const CVrtCombiner::pixel_window_t& cell : grid) {
    const CVrtCombiner::pixel_window_t tightened = CVrtCombiner::tightenToFootprints(layout.footprints, cell);
    if (!tightened.empty()) {
      tiles << tightened;
    }
  }
  if (tiles.isEmpty()) {
    QMessageBox::warning(this, tr("Combine failed"), tr("No data found in %1.").arg(QFileInfo(filename_).fileName()));
    return;
  }

  const qint32 gridSize = static_cast<qint32>(grid.size());

  // New tiles belong in the source files' own directory, which can differ from the VRT's.
  // perFileInfo is never empty here - slotCombineFiles() requires hasSourceFiles().
  const QString sourceDir = QFileInfo(advice_.perFileInfo.first().path).absolutePath();

  const qint32 droppedCount = gridSize - static_cast<qint32>(tiles.size());
  const QString vrtName = QFileInfo(filename_).fileName();

  QString confirmText = tr("This will split %1 into %2 compressed GeoTIFF(s) in %3, then rewrite %1 to reference "
                           "them. The current file will be backed up as %4.")
                            .arg(vrtName, QString::number(tiles.size()), sourceDir, vrtName + ".bak");
  if (droppedCount > 0) {
    confirmText += " " + tr("%1 grid cell(s) contained no data and will be skipped.").arg(droppedCount);
  }

  if (!confirmYesNo(tr("Confirm combine"), confirmText)) {
    return;
  }

  const QString gdalTranslate = IAppSetup::getPlatformInstance()->findExecutable("gdal_translate");
  const QString gdalbuildvrt = IAppSetup::getPlatformInstance()->findExecutable("gdalbuildvrt");

  // Only the final gdalbuildvrt -overwrite touches filename_ itself, so backing up now
  // keeps it untouched if any earlier gdal_translate call fails.
  const QString bakPath = filename_ + ".bak";
  if (QFileInfo::exists(bakPath)) {
    QFile::remove(bakPath);
  }
  if (!QFile::copy(filename_, bakPath)) {
    QMessageBox::warning(this, tr("Combine failed"), tr("Could not back up %1.").arg(vrtName));
    return;
  }

  QList<CShellCmd> cmds;
  combineOutputPaths_.clear();
  for (const CVrtCombiner::pixel_window_t& tile : tiles) {
    const QString outPath = QString("%1/group_r%2_c%3.tif").arg(sourceDir).arg(tile.row).arg(tile.col);
    combineOutputPaths_ << outPath;
    cmds << CShellCmd(gdalTranslate, {"-srcwin",
                                      QString::number(tile.xoff),
                                      QString::number(tile.yoff),
                                      QString::number(tile.xsize),
                                      QString::number(tile.ysize),
                                      "-co",
                                      "COMPRESS=DEFLATE",
                                      "-co",
                                      "PREDICTOR=2",
                                      "-co",
                                      "TILED=YES",
                                      "-co",
                                      "BLOCKXSIZE=512",
                                      "-co",
                                      "BLOCKYSIZE=512",
                                      "-co",
                                      "NUM_THREADS=ALL_CPUS",
                                      "-co",
                                      "BIGTIFF=IF_SAFER",
                                      filename_,
                                      outPath});
  }
  QStringList buildVrtArgs = {"-overwrite", filename_};
  buildVrtArgs += combineOutputPaths_;
  cmds << CShellCmd(gdalbuildvrt, buildVrtArgs);

  jobKind_ = JobKind::Combine;
  switchToProgressMode();
  jobId_ = shell_->execute(cmds);
}

void CVrtAdvisoryDialog::reject() {
  if (isJobRunning()) {
    close();  // routes through closeEvent(), which shows the abort confirmation
    return;
  }
  QDialog::reject();
}

void CVrtAdvisoryDialog::closeEvent(QCloseEvent* e) {
  if (isJobRunning()) {
    if (!confirmYesNo(tr("Abort"),
                      tr("An operation is in progress.\n\nAbort and remove any partially written files?"))) {
      e->ignore();
      return;
    }
    canceling_ = true;
    shell_->slotCancel();
    e->ignore();
    return;
  }
  QDialog::closeEvent(e);
}

void CVrtAdvisoryDialog::slotJobFinished(qint32 id) {
  if (id != jobId_) {
    return;
  }
  jobId_ = 0;

  switch (jobKind_) {
    case JobKind::FixOverviews:
      finishFixOverviews();
      break;
    case JobKind::Combine:
      finishCombine();
      break;
    case JobKind::None:
      break;
  }
  jobKind_ = JobKind::None;

  if (canceling_) {
    close();
  }
}

void CVrtAdvisoryDialog::finishFixOverviews() {
  if (canceling_ || !shell_->lastJobSucceeded()) {
    QStringList ovrPaths;
    for (const QString& path : filesToFix()) {
      ovrPaths << path + ".ovr";
    }
    removeIfExists(ovrPaths);
    return;
  }

  bool allGood = true;
  if (hasSourceFiles()) {
    if (fixContainerOverviewList()) {
      shell_->stdOut(tr("Rebuilt %1's <OverviewList> from its source files.\n").arg(QFileInfo(filename_).fileName()));
    } else {
      shell_->stdErr(tr("Failed to update %1.\n").arg(QFileInfo(filename_).fileName()));
      allGood = false;
    }
  }

  if (allGood) {
    emit sigContainerRebuilt();
  }
}

void CVrtAdvisoryDialog::finishCombine() {
  if (canceling_ || !shell_->lastJobSucceeded()) {
    // Only the final gdalbuildvrt -overwrite touches filename_; a kill/failure there can
    // leave it truncated, so restore from the backup. Harmless no-op if that step was
    // never reached.
    const QString bakPath = filename_ + ".bak";
    if (QFileInfo::exists(bakPath)) {
      QFile::remove(filename_);
      if (QFile::copy(bakPath, filename_)) {
        shell_->stdOut(tr("Restored %1 from backup.\n").arg(QFileInfo(filename_).fileName()));
      } else {
        shell_->stdErr(tr("Failed to restore %1 from backup - %2 is still available for manual recovery.\n")
                           .arg(QFileInfo(filename_).fileName(), QFileInfo(bakPath).fileName()));
        QMessageBox::warning(this, tr("Restore failed"),
                             tr("Combine failed and %1 could not be automatically restored from its backup.\n\n"
                                "Rename %2 back to %1 yourself to recover the original file.")
                                 .arg(QFileInfo(filename_).fileName(), QFileInfo(bakPath).fileName()));
      }
    }
    removeIfExists(combineOutputPaths_);
    combineOutputPaths_.clear();
    return;
  }

  const qsizetype tileCount = combineOutputPaths_.size();
  shell_->stdOut(tr("Combined %1 source file(s) into %2 tile(s); %3 rewritten.\n")
                     .arg(advice_.perFileInfo.size())
                     .arg(tileCount)
                     .arg(QFileInfo(filename_).fileName()));
  combineOutputPaths_.clear();

  // Originals are never deleted - a modal reminder to clean them up, since a shell_ log
  // line is easy to miss once the dialog is closed.
  QMessageBox::information(this, tr("Combine finished"),
                           tr("%1 has been rewritten to reference %2 merged tile(s).\n\n"
                              "The %3 original source file(s) are still in %4 - delete them yourself once you've "
                              "confirmed the merged file works as expected.")
                               .arg(QFileInfo(filename_).fileName())
                               .arg(tileCount)
                               .arg(advice_.perFileInfo.size())
                               .arg(QFileInfo(advice_.perFileInfo.first().path).absolutePath()));

  emit sigContainerRebuilt();
}

bool CVrtAdvisoryDialog::fixContainerOverviewList() {
  // Safe to declare directly: every file in filesToFix() was just rebuilt with exactly
  // this list (see slotFixOverviews()), and every other source was excluded from the fix
  // because it already had it in its own native pyramid (see filesToFix()) - and this is
  // only reached after CShell reports the whole gdaladdo job succeeded.
  const QVector<qint32>& factors = advice_.suggestedLevels;

  QFile file(filename_);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }
  QDomDocument doc;
  if (!doc.setContent(&file)) {
    file.close();
    return false;
  }
  file.close();

  QDomElement root = doc.documentElement();

  // remove any existing <OverviewList>
  const QDomNodeList existing = root.elementsByTagName("OverviewList");
  for (int i = existing.count() - 1; i >= 0; --i) {
    root.removeChild(existing.at(i));
  }

  if (!factors.isEmpty()) {
    QStringList levelStrs;
    for (qint32 level : factors) {
      levelStrs << QString::number(level);
    }
    QDomElement ovr = doc.createElement("OverviewList");
    ovr.setAttribute("resampling", resampleAlgorithm());
    ovr.appendChild(doc.createTextNode(levelStrs.join(' ')));
    root.insertBefore(ovr, root.firstChild());
  }

  QSaveFile saveFile(filename_);
  if (!saveFile.open(QIODevice::WriteOnly)) {
    return false;
  }
  QTextStream stream(&saveFile);
  doc.save(stream, 2);
  if (!saveFile.commit()) {
    return false;
  }

  // Only now that the rewritten <OverviewList> is safely committed, drop any stale .ovr
  // file - GDAL trusts a real .ovr file over anything the source files/OverviewList offer,
  // so an old one left behind would keep shadowing the fix (see
  // CGdalVrtUtil::buildOverviewAdvice()). Deleting it only after a successful commit means
  // a failure anywhere above leaves the original, still-working state untouched instead of
  // stripping the container's overview acceleration with nothing to show for it.
  const QString ovrPath = filename_ + ".ovr";
  if (QFileInfo::exists(ovrPath)) {
    QFile::remove(ovrPath);
  }

  return true;
}
