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
/// Cap the table at this many rows (the weakest, since perFileInfo is sorted weakest-first)
/// so a mosaic with very many source files can't blow up the dialog's size.
constexpr qsizetype cMaxRowsShown = 10;
}  // namespace

COverviewAdvisoryDialog::COverviewAdvisoryDialog(const QString& filename, const CGdalVrtUtil::overview_advice_t& advice,
                                                 QWidget* parent)
    : QDialog(parent) {
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  const QString sizeEstimate = QLocale::system().formattedDataSize(advice.estimatedOverviewBytes);

  if (advice.overviewsMissing) {
    labelMessage->setText(
        tr("<h3>Rendering is slow</h3>This file has no overviews (pyramids) built yet, so every redraw has to "
           "decode the data at full resolution:<br><b>%1</b><br><br>"
           "Building overviews once fixes this. Run one of the commands below in a terminal. This adds roughly "
           "<b>%2</b> of disk space (uncompressed estimate - actual size will likely be smaller thanks to "
           "compression); make sure that's available first.")
            .arg(filename, sizeEstimate));
  } else {
    labelMessage->setText(
        tr("<h3>Rendering is slow</h3>This file has overviews, but at least one of its source files' overviews "
           "isn't deep enough for the current view, so some redraws still have to decode much of the data at a "
           "finer resolution than necessary:<br><b>%1</b><br><br>"
           "Building deeper overviews once fixes this. Run one of the commands below in a terminal. This adds "
           "roughly <b>%2</b> of disk space (uncompressed estimate - actual size will likely be smaller thanks "
           "to compression); make sure that's available first.<br><br>"
           "<i>Note: since this file already has some overviews, the commands below add the new levels "
           "alongside the existing ones rather than replacing them, which can leave a mixed set of decimation "
           "levels. For a clean rebuild instead, run <code>gdaladdo -clean</code> on the affected file first.</i>")
            .arg(filename, sizeEstimate));
  }

  const bool showOverviews = !advice.overviewsMissing && !advice.perFileInfo.isEmpty();
  labelFileOverviews->setVisible(showOverviews);
  if (showOverviews) {
    QString html = "<table cellspacing=\"0\" cellpadding=\"4\">";
    html += QString("<tr style=\"font-weight:bold;border-bottom:1px solid gray;\"><td>%1</td><td>%2</td></tr>")
                .arg(tr("File"), tr("Overview factors"));

    const qsizetype rowCount = qMin(advice.perFileInfo.size(), cMaxRowsShown);
    for (qsizetype row = 0; row < rowCount; ++row) {
      const CGdalVrtUtil::file_overview_info_t& info = advice.perFileInfo[row];

      QStringList factorStrs;
      for (qint32 factor : info.factors) {
        factorStrs << QString::number(factor);
      }
      const QString factorsText = factorStrs.isEmpty() ? tr("none") : factorStrs.join(", ");

      // highlight the weakest file (first row) - it's the one the advisory is really about
      const QString rowStyle = (row == 0) ? " style=\"background-color:#fff3cd;\"" : "";
      html += QString("<tr%1><td>%2</td><td>%3</td></tr>")
                  .arg(rowStyle, QFileInfo(info.path).fileName().toHtmlEscaped(), factorsText);
    }
    html += "</table>";

    if (advice.perFileInfo.size() > rowCount) {
      html += tr("<p>...and %1 more file(s) not shown.</p>").arg(advice.perFileInfo.size() - rowCount);
    }

    labelFileOverviews->setText(html);
  }

  const bool hasVrtCommand = !advice.vrtCommand.isEmpty();
  const bool hasFilesCommand = !advice.filesCommand.isEmpty();
  // vrtCommand is only ever set alongside filesCommand (see buildOverviewAdvice()), so both
  // visible means there really are two alternatives to choose between - number them only
  // then; labelVrtCommand stays hidden otherwise, so its text doesn't matter in that case
  if (hasVrtCommand && hasFilesCommand) {
    labelVrtCommand->setText(tr("Option 1: build overviews on the VRT file:"));
    labelFilesCommand->setText(tr("Option 2: build overviews on the underlying source file(s):"));
  } else {
    labelFilesCommand->setText(tr("Build overviews on the underlying source file(s):"));
  }

  plainTextVrtCommand->setPlainText(advice.vrtCommand);
  labelVrtCommand->setVisible(hasVrtCommand);
  plainTextVrtCommand->setVisible(hasVrtCommand);

  plainTextFilesCommand->setPlainText(advice.filesCommand);
  labelFilesCommand->setVisible(hasFilesCommand);
  plainTextFilesCommand->setVisible(hasFilesCommand);
}
