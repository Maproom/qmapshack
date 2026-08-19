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

#ifndef CVRTADVISORYDIALOG_H
#define CVRTADVISORYDIALOG_H

#include <QDialog>

#include "helpers/COverviewAdvisory.h"
#include "helpers/CVrtCombiner.h"
#include "shell/CShell.h"
#include "shell/CShellCmd.h"
#include "theme/CUiTheme.h"
#include "ui_IVrtAdvisoryDialog.h"

class QPushButton;
class QTextBrowser;

/**
   @brief Advisory dialog for slow-rendering VRTs: missing/inadequate overviews, or too
          many source files.

   - Shown when CDemVRT/CMapVRT's render times out, or on demand via the tree's
     "Overview Info..." context menu.
   - Always shows a "current situation" table with per-file overview state.
   - If advice.needsOverviewFix() is true: also shows an "after fix" table and a "Fix
     overviews" button (runs gdaladdo, edits <OverviewList> in place).
   - If false: only the situation table remains, with a plain "Close" button.
   - If advice.hasTooManySubfiles() is true: an extra warning line with its own adjacent
     "Combine files..." button - kept separate from "Fix overviews" since the two fix
     unrelated problems - lets the user merge the source files into a handful of large
     tiles, regardless of the above.

   Application-modal (setModal(true)): while it is open the map/DEM it is about must not
   be read - a draw() racing an external gdaladdo/gdalbuildvrt rewrite crashes GDAL.
   Shown with show(), not exec(); deletes itself on close (Qt::WA_DeleteOnClose).
 */
class CVrtAdvisoryDialog : public QDialog, private Ui::IVrtAdvisoryDialog {
  Q_OBJECT
 public:
  /**
     @param filename   the slow file's path
     @param advice     the probed advice for filename
     @param geometry   filename's dimensions/pixel size, shown as an informational line
     @param suppressed filename's current "don't show again" state; seeds the checkbox, so
                       opening the dialog just to look and closing it round-trips
     @param parent     passed straight to QDialog
   */
  CVrtAdvisoryDialog(const QString& filename, const COverviewAdvisory::advice_t& advice,
                     const COverviewAdvisory::geometry_t& geometry, bool suppressed, QWidget* parent);

  /// @brief True if the user checked "don't show this again for this file."
  bool suppressChecked() const { return checkSuppressAdvisory->isChecked(); }

  /// @brief The file this dialog is showing advisory info for, as passed to the constructor.
  const QString& filename() const { return filename_; }

 signals:
  /// @brief Emitted after Fix overviews or Combine files successfully changed filename_'s
  ///        on-disk representation.
  void sigContainerRebuilt();

 protected:
  void changeEvent(QEvent* e) override;
  void closeEvent(QCloseEvent* e) override;
  void reject() override;

 private slots:
  void slotFixOverviews();
  void slotCombineFiles();
  void slotJobFinished(qint32 id);

 private:
  enum class JobKind { None, FixOverviews, Combine };

  bool isJobRunning() const { return jobId_ != 0 && shell_->isVisible() && !canceling_; }

  /// @brief Build the two status tables and the summary. Re-run on a colour scheme change.
  void renderThemedContent();

  QString resampleAlgorithm() const { return advice_.isPaletteIndexed ? "nearest" : "average"; }

  /// @brief True if filename_ is a VRT backed by source files - only then is there an
  ///        <OverviewList> to keep. False is a degenerate VRT, handled but not expected.
  bool hasSourceFiles() const { return !advice_.perFileInfo.isEmpty(); }

  /// @brief The one directory holding every source file, or empty if they are spread over
  ///        several. Combine writes its tiles beside the sources, which has no answer once
  ///        that names more than one place - an empty result withdraws the offer.
  QString sharedSourceDir() const;

  /// @brief A list of ints (decimation factors or overview pixel sizes) as a
  ///        comma-separated string, or "none" if empty.
  static QString formatFactors(const QVector<qint32>& factors);
  static QString htmlTh(const QString& text);
  static QString htmlTd(const QString& text, CUiTheme::Role role);

  /// @brief One entry of the fix plan; see fixPlan().
  struct fix_step_t {
    /// Build/CleanRebuild run gdaladdo on a source raster; Add/UpdateList only record in
    /// the container's XML which levels its sources now provide.
    enum Action { BuildNew, CleanRebuild, AddList, UpdateList };
    QString path;
    Action action;

    bool isContainerStep() const { return action == AddList || action == UpdateList; }
    bool replacesExisting() const { return action == CleanRebuild || action == UpdateList; }
  };

  /// @brief What "Fix overviews" will do, in order. Single source for the after-fix
  ///        table, the summary counts, the confirmation dialog, the gdaladdo queue and the
  ///        failure cleanup.
  QVector<fix_step_t> fixPlan() const;

  /// @brief User-facing wording for one fix_step_t::Action.
  static QString actionText(fix_step_t::Action action);

  /// @brief Status text/role for one current-situation row; shared by the container row
  ///        and every source row so the two cannot disagree. `sizes` only feeds the
  ///        "Shallow (coarsest %1px)" label, never the verdict.
  static QString rowStatus(bool checked, const QVector<qint32>& sizes, bool sufficient, CUiTheme::Role& role);

  /// @brief Container-row suffix distinguishing a real .ovr from a bare <OverviewList>.
  ///        Shown only for a VRT with sources and at least one verified level.
  static QString containerOvrSourceSuffix(bool hasSourceFiles, bool hasOwnOvr, const QVector<qint32>& sizes);

  /**
     @brief Rewrite filename_'s <OverviewList> to declare advice_.suggestedLevels, leaving
            the rest of the VRT untouched.

     Declares the target directly rather than re-probing: every source either was just
     rebuilt with exactly those levels or already had them. Only reached once the whole
     gdaladdo queue succeeded. Any stale <filename_>.ovr is deleted after the XML is
     committed - GDAL trusts a real .ovr over the sources, so one left behind shadows the
     fix, but deleting it earlier would leave the file worse off on a failure.
     @return false if filename_ couldn't be parsed or the rewritten XML couldn't be saved
   */
  bool fixContainerOverviewList();

  /// @brief Shared UI transition for slotFixOverviews()/slotCombineFiles(): hide the
  ///        after-fix section, show shell_, disable the action buttons.
  void switchToProgressMode();

  /// @brief Size view to hold html's table of rowCount rows, capped at kMaxTableWidth and
  ///        kMaxTableHeight - past either the browser scrolls instead of the dialog growing.
  ///        Pass the html measured, not the width="100%" variant view may display.
  void sizeTableBrowser(QTextBrowser* view, const QString& html, qsizetype rowCount) const;

  /// @brief Yes/No confirmation dialog, defaulting to No.
  bool confirmYesNo(const QString& title, const QString& text);

  /// @brief Yes/No confirmation over a list of `rowCount` `<tr>` rows.
  ///        A QMessageBox cannot scroll, so a long list pushes its buttons off screen.
  bool confirmList(const QString& title, const QString& lead, const QString& rowsHtml, qsizetype rowCount);

  /// @brief Delete every path in paths that still exists, logging each removal to shell_.
  void removeIfExists(const QStringList& paths);

  /// @brief slotJobFinished()'s completion logic for a JobKind::FixOverviews job.
  void finishFixOverviews();

  /// @brief slotJobFinished()'s completion logic for a JobKind::Combine job: restores
  ///        filename_ from its ".bak" on cancel/failure, otherwise keeps the merged tiles
  ///        (originals are never deleted) and emits sigContainerRebuilt().
  void finishCombine();

  /// @brief Size caps for every table browser, so neither a long list nor one pathological
  ///        file name can blow up the dialog.
  static constexpr qint32 kMaxTableHeight = 250;
  static constexpr qint32 kMaxTableWidth = 900;

  QString filename_;
  COverviewAdvisory::advice_t advice_;
  CShell* shell_ = nullptr;
  qint32 jobId_ = 0;
  /// @brief Set by closeEvent()'s abort path: user confirmed closing mid-operation, finish
  ///        tearing down the shell_ job then close().
  bool canceling_ = false;
  JobKind jobKind_ = JobKind::None;
  QStringList combineOutputPaths_; /**< Group tile paths for the running/last Combine job - used to
                                        delete partial output on cancel/failure. */
  QPushButton* buttonCombine_ = nullptr;
};

#endif  // CVRTADVISORYDIALOG_H
