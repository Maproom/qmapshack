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

#include <QtWidgets>

namespace {
constexpr qsizetype cMaxRowsShown = 10;

QString formatFactors(const QVector<qint32>& factors) {
  if (factors.isEmpty()) {
    return QCoreApplication::translate("COverviewAdvisoryDialog", "none");
  }
  QStringList parts;
  for (qint32 f : factors) {
    parts << QString::number(f);
  }
  return parts.join(", ");
}

QString htmlTh(const QString& text) {
  return QString("<th style=\"text-align:left; padding:4px 6px; background-color:#e0e0e0\">%1</th>")
      .arg(text.toHtmlEscaped());
}

QString htmlTd(const QString& text, const QString& bg) {
  return QString("<td style=\"padding:4px 6px; background-color:%1\">%2</td>").arg(bg, text.toHtmlEscaped());
}

/// Returns the overview factors that the VRT dataset itself currently reports (Branch 1 only).
QVector<qint32> vrtOwnFactors(const QString& filename, const CGdalVrtUtil::overview_advice_t& advice) {
  for (const CGdalVrtUtil::file_overview_info_t& info : advice.perFileInfo) {
    if (info.path == filename) {
      return info.factors;
    }
  }
  return {};
}
}  // namespace

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

  // ---- current situation ----
  const qsizetype rowCount = qMin(advice_.perFileInfo.size(), cMaxRowsShown);

  if (advice_.perFileInfo.size() > cMaxRowsShown) {
    labelCurrentTitle->setText(tr("<b>Current overview situation</b> (%1 weakest files shown of %2):")
                                   .arg(cMaxRowsShown)
                                   .arg(advice_.perFileInfo.size()));
  }

  QString currentHtml = "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border-collapse:collapse\">";
  currentHtml += "<tr>" + htmlTh(tr("File")) + htmlTh(tr("Existing overview levels")) + htmlTh(tr("Status")) + "</tr>";

  for (qsizetype i = 0; i < rowCount; ++i) {
    const CGdalVrtUtil::file_overview_info_t& info = advice_.perFileInfo[i];
    const QString name = QFileInfo(info.path).fileName();
    const QString levels = formatFactors(info.factors);
    QString status;
    QString bg;
    if (info.factors.isEmpty()) {
      status = tr("✗ None");
      bg = "#f8d7da";
    } else if (info.factors.last() < suggestedMax) {
      status = tr("⚠ Shallow (max %1)").arg(info.factors.last());
      bg = "#fff3cd";
    } else {
      status = tr("✓ OK");
      bg = "#d4edda";
    }
    currentHtml += "<tr>" + htmlTd(name, bg) + htmlTd(levels, bg) + htmlTd(status, bg) + "</tr>";
  }

  if (advice_.perFileInfo.size() > cMaxRowsShown) {
    const qsizetype extra = advice_.perFileInfo.size() - cMaxRowsShown;
    currentHtml += QString("<tr><td colspan=\"3\" style=\"padding:4px 6px; font-style:italic\">%1</td></tr>")
                       .arg(tr("…and %1 more file(s) not shown").arg(extra).toHtmlEscaped());
  }

  if (advice_.vrtNeedsOverviewList) {
    const QString vrtName = QFileInfo(filename_).fileName();
    QString levels;
    QString status;
    QString bg;
    if (!advice_.vrtHasOverviewList) {
      levels = tr("none");
      status = tr("✗ None");
      bg = "#f8d7da";
    } else {
      const QVector<qint32> factors = vrtOwnFactors(filename_, advice_);
      levels = formatFactors(factors);
      status = tr("⚠ Shallow (max %1)").arg(factors.isEmpty() ? 0 : factors.last());
      bg = "#fff3cd";
    }
    currentHtml += "<tr>" + htmlTd(vrtName, bg) + htmlTd(levels, bg) + htmlTd(status, bg) + "</tr>";
  }

  currentHtml += "</table>";

  // ---- after fix ----
  QString afterFixHtml = "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border-collapse:collapse\">";
  afterFixHtml += "<tr>" + htmlTh(tr("File")) + htmlTh(tr("New overview levels")) + htmlTh(tr("Action")) + "</tr>";

  for (const QString& path : advice_.sourceFilePaths) {
    const QString name = QFileInfo(path).fileName();
    const bool replacing = hasExistingOverviews(path);
    const QString action = replacing ? tr("Clean + rebuild") : tr("Build new");
    const QString bg = replacing ? "#fff3cd" : "#d4edda";
    afterFixHtml += "<tr>" + htmlTd(name, bg) + htmlTd(suggestedStr, bg) + htmlTd(action, bg) + "</tr>";
  }

  if (advice_.vrtNeedsOverviewList) {
    const QString vrtName = QFileInfo(filename_).fileName();
    const bool replacing = advice_.vrtHasOverviewList;
    const QString action = replacing ? tr("Clean + rebuild") : tr("Build new");
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

  // row height: cell padding (4px top + 4px bottom) + font height + 2px for borders
  const int rowH = fontMetrics().height() + 10;
  const qsizetype currentDataRows =
      rowCount + (advice_.vrtNeedsOverviewList ? 1 : 0) + (advice_.perFileInfo.size() > cMaxRowsShown ? 1 : 0);
  textCurrent->setFixedHeight(qMin(static_cast<int>(1 + currentDataRows) * rowH + 8, 250));

  const qsizetype afterDataRows =
      static_cast<qsizetype>(advice_.sourceFilePaths.size()) + (advice_.vrtNeedsOverviewList ? 1 : 0);
  textAfterFix->setFixedHeight(qMin(static_cast<int>(1 + afterDataRows) * rowH + 8, 250));

  // ---- summary ----
  const qint32 replaceCount =
      static_cast<qint32>(std::count_if(advice_.sourceFilePaths.begin(), advice_.sourceFilePaths.end(),
                                        [this](const QString& p) { return hasExistingOverviews(p); })) +
      (advice_.vrtNeedsOverviewList && advice_.vrtHasOverviewList ? 1 : 0);
  const qint32 buildCount =
      static_cast<qint32>(advice_.sourceFilePaths.size()) + (advice_.vrtNeedsOverviewList ? 1 : 0) - replaceCount;

  QStringList summaryParts;
  if (buildCount > 0) {
    summaryParts << tr("%1 file(s) will have new overviews built").arg(buildCount);
  }
  if (replaceCount > 0) {
    summaryParts << tr("%1 file(s) will have existing overviews cleaned and rebuilt").arg(replaceCount);
  }

  const QString sizeEstimate = QLocale::system().formattedDataSize(advice_.estimatedOverviewBytes);
  labelSummary->setText(summaryParts.join(", ") % tr(". Estimated disk usage: %1.").arg(sizeEstimate));

  adjustSize();
}

bool COverviewAdvisoryDialog::hasExistingOverviews(const QString& filePath) const {
  if (advice_.vrtHasOverviewList) {
    return true;
  }
  for (const CGdalVrtUtil::file_overview_info_t& info : advice_.perFileInfo) {
    if (info.path == filePath) {
      return !info.factors.isEmpty();
    }
  }
  return false;
}

void COverviewAdvisoryDialog::slotFixIt() {
  QStringList toClean;
  for (const QString& path : advice_.sourceFilePaths) {
    if (hasExistingOverviews(path)) {
      toClean << QFileInfo(path).fileName();
    }
  }
  if (advice_.vrtNeedsOverviewList && advice_.vrtHasOverviewList) {
    toClean << QFileInfo(filename_).fileName();
  }

  QStringList confirmLines;
  if (!toClean.isEmpty()) {
    confirmLines << tr("Existing overviews will be removed and rebuilt for:");
    for (const QString& name : toClean) {
      confirmLines << "  • " + name;
    }
  }
  if (advice_.vrtNeedsOverviewList && !advice_.vrtHasOverviewList) {
    confirmLines << tr("The VRT file will be modified in place:");
    confirmLines << "  • " + QFileInfo(filename_).fileName();
  }

  if (!confirmLines.isEmpty()) {
    const int ret = QMessageBox::question(this, tr("Confirm fix"), confirmLines.join('\n') + "\n\n" + tr("Continue?"),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) {
      return;
    }
  }

  // placeholder — real implementation will run gdaladdo + edit VRT
  const QString resampleAlg = advice_.isCategorical ? "nearest" : "average";
  QStringList levelArgs;
  for (qint32 level : advice_.suggestedLevels) {
    levelArgs << QString::number(level);
  }
  const QString levelStr = levelArgs.join(' ');

  qDebug() << "OVR FIX: Fix it clicked for" << filename_;
  for (const QString& path : advice_.sourceFilePaths) {
    if (hasExistingOverviews(path)) {
      qDebug() << "OVR FIX:  would run: gdaladdo -clean" << path;
    }
    qDebug() << "OVR FIX:  would run: gdaladdo -ro -r" << resampleAlg << "--config COMPRESS_OVERVIEW DEFLATE" << path
             << levelStr;
  }
  if (advice_.vrtNeedsOverviewList) {
    const QString verb = advice_.vrtHasOverviewList ? "update" : "add";
    qDebug() << "OVR FIX:  would" << verb
             << "<OverviewList resampling=\"" % resampleAlg % "\">" % levelStr % "</OverviewList>" << "in" << filename_;
  }

  accept();
}
