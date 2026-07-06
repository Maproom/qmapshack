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

#ifndef COVERVIEWADVISORYDIALOG_H
#define COVERVIEWADVISORYDIALOG_H

#include <QDialog>

#include "helpers/CGdalVrtUtil.h"
#include "shell/CShell.h"
#include "shell/CShellCmd.h"
#include "ui_IOverviewAdvisoryDialog.h"

/**
   @brief Advisory dialog for slow-rendering VRTs due to missing/inadequate overviews.

   - Shown when CDemVRT/CMapVRT's render times out, or on demand via the tree's
     "Overview Info..." context menu.
   - Always shows a "current situation" table with per-file overview state.
   - If advice.needsOverviewFix() is true: also shows an "after fix" table and a "Fix
     it" button (runs gdaladdo, edits <OverviewList> in place).
   - If false: only the situation table remains, with a plain "Close" button.
   - If advice.hasTooManySubfiles() is true: an extra warning line recommends combining
     source files, regardless of the above - this app can't do that automatically.

   Non-modal: shown with show(), not exec(); deletes itself on close
   (Qt::WA_DeleteOnClose).
 */
class COverviewAdvisoryDialog : public QDialog, private Ui::IOverviewAdvisoryDialog {
  Q_OBJECT
 public:
  /**
     @param filename the slow file's path
     @param advice   buildOverviewAdvice()'s result for filename
     @param geometry filename's dimensions/pixel size, shown as an informational line
     @param parent   passed straight to QDialog
   */
  COverviewAdvisoryDialog(const QString& filename, const CGdalVrtUtil::overview_advice_t& advice,
                          const CGdalVrtUtil::raster_geometry_t& geometry, QWidget* parent);

  /// @brief True if the user checked "don't show this again for this file."
  bool suppressChecked() const { return checkSuppressAdvisory->isChecked(); }

  /// @brief The file this dialog is showing advisory info for, as passed to the constructor.
  const QString& filename() const { return filename_; }

 signals:
  /// @brief Emitted after Fix it completes successfully (gdaladdo on filesToFix(), plus
  ///        fixContainerOverviewList() if the container is a VRT with source files).
  void sigFixItDone();

 protected:
  void closeEvent(QCloseEvent* e) override;
  void reject() override;

 private slots:
  void slotFixIt();
  void slotFixItDone(qint32 id);

 private:
  bool isJobRunning() const { return jobId_ != 0 && shell_->isVisible() && !canceling_; }
  QString resampleAlgorithm() const { return advice_.isPaletteIndexed ? "nearest" : "average"; }
  bool hasExistingOverviews(const QString& filePath) const;

  /// @brief True if filename_ is a VRT backed by source files - only then does the
  ///        container need separate <OverviewList> bookkeeping. False is a degenerate
  ///        edge case (a VRT declaring no source files at all, e.g. constant-data
  ///        bands) - not a realistic QMapShack workflow, just handled gracefully.
  bool hasSourceFiles() const { return !advice_.perFileInfo.isEmpty(); }

  /// @brief A list of ints (decimation factors or overview pixel sizes) as a
  ///        comma-separated string, or "none" if empty.
  static QString formatFactors(const QVector<qint32>& factors);
  static QString htmlTh(const QString& text);
  static QString htmlTd(const QString& text, const QString& bg);

  /**
     @brief Status text/background for one current-situation row.

     Shared by the container row and every source-file row, so both use the same rule
     and can't disagree. `sufficient` is the precise, size-based verdict
     (CGdalVrtUtil::buildOverviewAdvice()); `sizes` is only used for the "Shallow
     (coarsest %1px)" label, never to derive the verdict itself.
   */
  static QString rowStatus(bool checked, const QVector<qint32>& sizes, bool sufficient, QString& bg);

  /**
     @brief Suffix for the container row: distinguishes a real .ovr file from a bare
            <OverviewList> declaration. A bare declaration only has read-speed value
            once every source file backs it up (see CGdalVrtUtil::buildOverviewAdvice()).
            Shown only for a VRT with source files (hasSourceFiles) and at least one
            verified level (sizes non-empty).
   */
  static QString containerOvrSourceSuffix(bool hasSourceFiles, bool hasOwnOvr, const QVector<qint32>& sizes);

  /**
     @brief The source files gdaladdo will run on:
     - every entry in advice_.perFileInfo that falls short of advice_.suggestedLevels
       (already-adequate ones are left alone)
     - or filename_ itself, if perFileInfo is empty (a VRT with no source files)
   */
  QStringList filesToFix() const;

  /**
     @brief Rewrite filename_'s <OverviewList> to declare advice_.suggestedLevels, without
            touching anything else in the VRT (SRS, extents, resampling, band setup).

     Safe to declare the target directly (not re-probe/intersect each source's actual
     factors): every file in filesToFix() was just rebuilt by slotFixIt() with exactly
     advice_.suggestedLevels, and every other source was excluded from the fix because it
     already had that many levels in its own native pyramid (see filesToFix()). So every
     source file supports at least suggestedLevels once the fix completes.
     fixContainerOverviewList() is only reached after CShell reports the whole gdaladdo
     job succeeded (slotFixItDone()), so a mid-queue failure never gets here.

     Deletes any stale <filename_>.ovr file only after the rewritten XML is safely
     committed: GDAL trusts a real .ovr file over anything the source files/OverviewList
     offer, so an old one left in place would keep shadowing the fix (see
     CGdalVrtUtil::buildOverviewAdvice()), but deleting it before a successful commit
     would leave filename_ worse off than before the fix on any failure below.
     @return false if filename_ couldn't be (re)opened/parsed, or the rewritten XML
             couldn't be saved
   */
  bool fixContainerOverviewList();

  QString filename_;
  CGdalVrtUtil::overview_advice_t advice_;
  CShell* shell_ = nullptr;
  qint32 jobId_ = 0;
  bool canceling_ = false;
};

#endif  // COVERVIEWADVISORYDIALOG_H
