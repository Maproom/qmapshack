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
   @brief Advisory dialog shown when CDemVRT's/CMapVRT's render times out while overviews
          are missing or inadequate for the dataset - explains the problem and offers
          copy-pasteable gdaladdo commands (see CGdalVrtUtil::buildOverviewAdvice()) to
          fix it.

   Non-modal: the caller shows it with show(), not exec(), and it deletes itself on close
   (Qt::WA_DeleteOnClose).
 */
class COverviewAdvisoryDialog : public QDialog, private Ui::IOverviewAdvisoryDialog {
  Q_OBJECT
 public:
  /**
     @param filename the slow file's path, shown in the explanatory message
     @param advice   CDemVRT::getOverviewAdvice()/CMapVRT::getOverviewAdvice()'s cached
                     result for filename; overviewsMissing picks the wording of the
                     explanatory message, perFileInfo populates the table (only shown
                     when overviewsMissing is false - if true every entry is trivially
                     "no overview"), and vrtCommand/filesCommand fill the two command
                     sections (each hidden if empty)
     @param parent   passed straight to QDialog
   */
  COverviewAdvisoryDialog(const QString& filename, const CGdalVrtUtil::OverviewAdvice& advice, QWidget* parent);

  /// @brief True if the user checked "don't show this again for this file."
  bool suppressChecked() const { return checkSuppressAdvisory->isChecked(); }
};

#endif  // COVERVIEWADVISORYDIALOG_H
