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

#include "helpers/COverviewAdvisoryDialog.h"

#include <gdal.h>
#include <gdal_priv.h>

#include <QDomDocument>
#include <QtWidgets>

#include "setup/IAppSetup.h"

COverviewAdvisoryDialog::COverviewAdvisoryDialog(const QString& filename, const CGdalVrtUtil::overview_advice_t& advice,
                                                 QWidget* parent)
    : QDialog(parent), filename_(filename), advice_(advice) {
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Fix it"));
  buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Later"));
  disconnect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &COverviewAdvisoryDialog::slotFixIt);

  const qint32 suggestedMax = advice_.suggestedLevels.isEmpty() ? 1 : advice_.suggestedLevels.last();
  const QString suggestedStr = formatFactors(advice_.suggestedLevels);

  // ---- current situation ---- (every referenced file is shown, none truncated)
  QString currentHtml = "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border-collapse:collapse\">";
  currentHtml += "<tr>" + htmlTh(tr("File")) + htmlTh(tr("Existing overview levels")) + htmlTh(tr("Status")) + "</tr>";

  // the container always leads the table, graded by the same rule as every source file
  {
    QString bg;
    const QString status = rowStatus(/*checked=*/true, advice_.containerFactors, suggestedMax, bg);
    const QString levels =
        formatFactors(advice_.containerFactors) +
        containerOvrSourceSuffix(hasSourceFiles(), advice_.containerHasOwnOvr, advice_.containerFactors);
    currentHtml +=
        "<tr>" + htmlTd(QFileInfo(filename_).fileName(), bg) + htmlTd(levels, bg) + htmlTd(status, bg) + "</tr>";
  }

  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    QString bg;
    const QString status = rowStatus(info.checked, info.factors, suggestedMax, bg);
    currentHtml += "<tr>" + htmlTd(QFileInfo(info.path).fileName(), bg) + htmlTd(formatFactors(info.factors), bg) +
                   htmlTd(status, bg) + "</tr>";
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
  const qint32 replaceCount =
      static_cast<qint32>(
          std::count_if(toFix.begin(), toFix.end(), [this](const QString& p) { return hasExistingOverviews(p); })) +
      (hasSourceFiles() && hasExistingOverviews(filename_) ? 1 : 0);
  const qint32 buildCount = static_cast<qint32>(toFix.size()) + (hasSourceFiles() ? 1 : 0) - replaceCount;

  QStringList summaryParts;
  if (buildCount > 0) {
    summaryParts << tr("%1 file(s) will have new overviews built").arg(buildCount);
  }
  if (replaceCount > 0) {
    summaryParts << tr("%1 file(s) will have existing overviews cleaned and rebuilt").arg(replaceCount);
  }

  const QString sizeEstimate = QLocale::system().formattedDataSize(advice_.estimatedOverviewBytes);
  labelSummary->setText(summaryParts.join(", ") % tr(". Estimated disk usage: %1.").arg(sizeEstimate));

  shell_ = new CShell(this);
  shell_->setVisible(false);
  verticalLayout->insertWidget(verticalLayout->indexOf(labelSummary), shell_);
  connect(shell_, &CShell::sigFinishedJob, this, &COverviewAdvisoryDialog::slotFixItDone);

  // Opened on demand (context menu) for a file with nothing to fix: show only the
  // current-situation table, not the fix-it machinery that would otherwise invite
  // rebuilding overviews that are already fine.
  if (!advice_.needsAttention()) {
    setWindowTitle(tr("Overview info"));
    labelAfterFixTitle->setVisible(false);
    textAfterFix->setVisible(false);
    labelSummary->setVisible(false);
    checkSuppressAdvisory->setVisible(false);
    buttonBox->button(QDialogButtonBox::Ok)->setVisible(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));
  }

  adjustSize();
}

QString COverviewAdvisoryDialog::formatFactors(const QVector<qint32>& factors) {
  if (factors.isEmpty()) {
    return tr("none");
  }
  QStringList parts;
  for (qint32 f : factors) {
    parts << QString::number(f);
  }
  return parts.join(", ");
}

QString COverviewAdvisoryDialog::htmlTh(const QString& text) {
  return QString("<th style=\"text-align:left; padding:4px 6px; background-color:#e0e0e0\">%1</th>")
      .arg(text.toHtmlEscaped());
}

QString COverviewAdvisoryDialog::htmlTd(const QString& text, const QString& bg) {
  return QString("<td style=\"padding:4px 6px; background-color:%1\">%2</td>").arg(bg, text.toHtmlEscaped());
}

QString COverviewAdvisoryDialog::rowStatus(bool checked, const QVector<qint32>& factors, qint32 suggestedMax,
                                           QString& bg) {
  if (!checked) {
    bg = "#d4edda";
    return tr("✓ covered by .ovr");
  }
  if (factors.isEmpty()) {
    bg = "#f8d7da";
    return tr("✗ None");
  }
  if (factors.last() < suggestedMax) {
    bg = "#fff3cd";
    return tr("⚠ Shallow (max %1)").arg(factors.last());
  }
  bg = "#d4edda";
  return tr("✓ OK");
}

QString COverviewAdvisoryDialog::containerOvrSourceSuffix(bool hasSourceFiles, bool hasOwnOvr,
                                                          const QVector<qint32>& factors) {
  if (!hasSourceFiles || factors.isEmpty()) {
    return QString();
  }
  return hasOwnOvr ? tr(" (own .ovr)") : tr(" (via <OverviewList>)");
}

bool COverviewAdvisoryDialog::hasExistingOverviews(const QString& filePath) const {
  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    if (info.path == filePath) {
      return info.checked && !info.factors.isEmpty();
    }
  }
  // No per-file entry: filePath is either the container's own row, or (see
  // hasSourceFiles()) filesToFix()'s fallback target. Either way, fall back to the
  // container's claim.
  return filePath == filename_ && advice_.containerFactor > 0;
}

QStringList COverviewAdvisoryDialog::filesToFix() const {
  if (!hasSourceFiles()) {
    // no source files - fix filename_ directly, no <OverviewList> bookkeeping
    return {filename_};
  }
  const qint32 suggestedMax = advice_.suggestedLevels.isEmpty() ? 1 : advice_.suggestedLevels.last();
  QStringList result;
  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    if (info.factors.isEmpty() || info.factors.last() < suggestedMax) {
      result << info.path;
    }
  }
  return result;
}

void COverviewAdvisoryDialog::slotFixIt() {
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
    // orange only when a real .ovr file exists and will be deleted - containerFactor > 0
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

  QMessageBox msgBox(this);
  msgBox.setWindowTitle(tr("Confirm fix"));
  msgBox.setText(confirmHtml);
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  if (msgBox.exec() != QMessageBox::Yes) {
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
                        "ALL_CPUS", "--config", "COMPRESS_OVERVIEW", "DEFLATE",  path};
    args += levelArgs;
    cmds << CShellCmd(gdaladdo, args);
  }

  // switch UI to progress mode
  labelAfterFixTitle->setVisible(false);
  textAfterFix->setVisible(false);
  shell_->setVisible(true);
  shell_->setMinimumHeight(150);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));
  adjustSize();

  jobId_ = shell_->execute(cmds);
}

void COverviewAdvisoryDialog::reject() {
  if (isJobRunning()) {
    close();  // routes through closeEvent(), which shows the abort confirmation
    return;
  }
  QDialog::reject();
}

void COverviewAdvisoryDialog::closeEvent(QCloseEvent* e) {
  if (isJobRunning()) {
    const int ret = QMessageBox::question(
        this, tr("Abort fix"), tr("Overview creation is in progress.\n\nAbort and remove any partially written files?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) {
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

void COverviewAdvisoryDialog::slotFixItDone(qint32 id) {
  if (id != jobId_) {
    return;
  }
  jobId_ = 0;

  if (canceling_ || !shell_->lastJobSucceeded()) {
    for (const QString& path : filesToFix()) {
      const QString ovrPath = path + ".ovr";
      if (QFileInfo::exists(ovrPath)) {
        QFile::remove(ovrPath);
        shell_->stdOut(tr("Removed %1.\n").arg(QFileInfo(ovrPath).fileName()));
      }
    }
    if (canceling_) {
      close();
    }
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
    emit sigFixItDone();
  }
}

bool COverviewAdvisoryDialog::fixContainerOverviewList() {
  // delete any stale .ovr file first - GDAL trusts a real .ovr file over anything the
  // source files offer, so an old one left behind would keep shadowing the fix (see
  // CGdalVrtUtil::buildOverviewAdvice())
  const QString ovrPath = filename_ + ".ovr";
  if (QFileInfo::exists(ovrPath)) {
    QFile::remove(ovrPath);
  }

  qreal pixelSizeX = 0.0;
  {
    GDALDatasetUniquePtr dataset(GDALDataset::FromHandle(GDALOpen(filename_.toUtf8(), GA_ReadOnly)));
    if (!dataset) {
      return false;
    }
    qreal geoTransform[6];
    if (dataset->GetGeoTransform(geoTransform) == CE_None) {
      pixelSizeX = qAbs(geoTransform[1]);
    }
    // dataset closes here, before filename_ is reopened for writing below
  }

  QStringList sourcePaths;
  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    sourcePaths << info.path;
  }
  // what gdalbuildvrt would declare for a fresh mosaic of these sources right now. Not
  // advice_.suggestedLevels directly - a skipped source, or an already-adequate one
  // with different levels, could make that claim untrue.
  const QVector<qint32> factors = CGdalVrtUtil::intersectSourceOverviewFactors(sourcePaths, pixelSizeX);

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
  return saveFile.commit();
}
