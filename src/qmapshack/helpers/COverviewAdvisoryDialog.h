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
#include "ui_IOverviewAdvisoryDialog.h"

/**
   @brief Advisory dialog shown when CDemVRT/CMapVRT render times out due to missing or
          inadequate overview pyramids. Shows a "current situation" table with per-file
          overview state and an "after fix" table describing what will be done. A "Fix it"
          button will run the fix in-app (currently placeholder debug output).

   Non-modal: the caller shows it with show(), not exec(), and it deletes itself on close
   (Qt::WA_DeleteOnClose).
 */
class COverviewAdvisoryDialog : public QDialog, private Ui::IOverviewAdvisoryDialog {
  Q_OBJECT
 public:
  /**
     @param filename the slow file's path
     @param advice   buildOverviewAdvice()'s result for filename
     @param parent   passed straight to QDialog
   */
  COverviewAdvisoryDialog(const QString& filename, const CGdalVrtUtil::overview_advice_t& advice, QWidget* parent);

  /// @brief True if the user checked "don't show this again for this file."
  bool suppressChecked() const { return checkSuppressAdvisory->isChecked(); }

 private slots:
  void slotFixIt();

 private:
  bool hasExistingOverviews(const QString& filePath) const;

  QString filename_;
  CGdalVrtUtil::overview_advice_t advice_;
};

#endif  // COVERVIEWADVISORYDIALOG_H
