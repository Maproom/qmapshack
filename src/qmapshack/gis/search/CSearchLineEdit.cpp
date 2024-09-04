/**********************************************************************************************
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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
#include "CSearchLineEdit.h"

#include <canvas/CCanvas.h>

#include <QActionGroup>
#include <QMenu>
#include <QTimer>

#include "CSearchExplanationDialog.h"
#include "gis/prj/IGisProject.h"
#include "gis/search/CSearch.h"

CSearchExplanationDialog* CSearchLineEdit::explanationDlg = nullptr;

CSearchLineEdit::CSearchLineEdit(QWidget* parent) : QLineEdit(parent) {
  searchCreationTimer = new QTimer(this);
  searchCreationTimer->setSingleShot(true);
  connect(searchCreationTimer, &QTimer::timeout, this, [this] { slotCreateSearch(text()); });

  actionClearFilter = new QAction(QIcon(":/icons/32x32/Filter.png"), tr("Clear Filter"), this);
  actionHelp = new QAction(QIcon(":/icons/32x32/CSrcUnknown.png"), tr("Open Help Window"), this);
  actionSetupFilter = new QAction(QIcon(":/icons/32x32/Apply.png"), tr("Setup Filter"), this);
  actionError = new QAction(QIcon(":/icons/32x32/Attention.png"), tr("Error parsing search"), this);
  actionAutoProperty = new QAction(QIcon(":/icons/32x32/Hint.png"), tr("Auto selected property"), this);

  actionNameOnly = new QAction(tr("Name Only"), this);
  actionNameOnly->setCheckable(true);
  actionCompleteText = new QAction(tr("Complete Text"), this);
  actionCompleteText->setCheckable(true);
  actionCaseSensitive = new QAction(tr("Case Sensitive"), this);
  actionCaseSensitive->setCheckable(true);

  addAction(actionClearFilter, QLineEdit::TrailingPosition);
  addAction(actionHelp, QLineEdit::TrailingPosition);
  addAction(actionSetupFilter, QLineEdit::LeadingPosition);

  connect(actionSetupFilter, &QAction::triggered, this, &CSearchLineEdit::slotSetupSearch);
  connect(actionNameOnly, &QAction::triggered, this, &CSearchLineEdit::slotSearchNameOnly);
  connect(actionCompleteText, &QAction::triggered, this, &CSearchLineEdit::slotSearchCompleteText);
  connect(actionCaseSensitive, &QAction::triggered, this, &CSearchLineEdit::slotCaseSensitive);
  connect(actionHelp, &QAction::triggered, this, &CSearchLineEdit::slotSearchHelp);
  connect(actionClearFilter, &QAction::triggered, this, &CSearchLineEdit::slotClearFilter);

  // make sure that the search is not conducted after every letter typed, but only if the user stops typing.
  // One second is pretty long, but it may allow the user a second of thinking before the lineEdit blocks for filtering
  connect(this, &CSearchLineEdit::textChanged, this, [this] { searchCreationTimer->start(1000); });

  setPlaceholderText(tr("start typing..."));
  setToolTip(
      tr("Filter: Start to type and the list will be reduced to matching items. An example would be \"date between "
         "2010 and 2012\""));
}

CSearchLineEdit::CSearchLineEdit(QWidget* parent, IGisProject* project, CSearch* search) : CSearchLineEdit(parent) {
  connectedProject = project;
  if (search != nullptr) {
    setText(search->getSearchText());
  }
}
void CSearchLineEdit::slotSetupSearch() {
  QMenu* menu = new QMenu(this);
  menu->addSection(tr("Apply filter to"));
  menu->addAction(actionNameOnly);
  menu->addAction(actionCompleteText);
  menu->addSection(tr("Case sensitivity"));
  menu->addAction(actionCaseSensitive);

  QActionGroup* actionGroup = new QActionGroup(menu);
  actionGroup->addAction(actionNameOnly);
  actionGroup->addAction(actionCompleteText);

  switch (CSearch::getSearchMode()) {
    case CSearch::eSearchModeName:
      actionNameOnly->setChecked(true);
      break;

    case CSearch::eSearchModeText:
      actionCompleteText->setChecked(true);
      break;
  }

  actionCaseSensitive->setChecked(CSearch::getCaseSensitivity() == Qt::CaseSensitive);
  menu->move(parentWidget()->mapToGlobal(geometry().topLeft()));
  menu->exec();
}

void CSearchLineEdit::slotSearchHelp() {
  if (explanationDlg == nullptr) {
    explanationDlg = new CSearchExplanationDialog(this);
  }

  explanationDlg->setVisible(true);
}

void CSearchLineEdit::slotClearFilter() {
  setText("");
  if (connectedProject != nullptr) {
    connectedProject->setProjectFilter(CSearch(""));
    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
  } else {
    emit sigSearchCleared(connectedProject);
  }
}

void CSearchLineEdit::slotSearchNameOnly(bool yes) {
  if (yes) {
    CSearch::setSearchMode(CSearch::eSearchModeName);
    // Trigger reprocessing of search
    slotCreateSearch(text());
  }
}

void CSearchLineEdit::slotSearchCompleteText(bool yes) {
  if (yes) {
    CSearch::setSearchMode(CSearch::eSearchModeText);
    // Trigger reprocessing of search
    slotCreateSearch(text());
  }
}

void CSearchLineEdit::slotCaseSensitive(bool yes) {
  if (yes) {
    CSearch::setCaseSensitivity(Qt::CaseSensitive);
  } else {
    CSearch::setCaseSensitivity(Qt::CaseInsensitive);
  }
  // Trigger reprocessing of search
  slotCreateSearch(text());
}

void CSearchLineEdit::slotCreateSearch(const QString& str) {
  actionClearFilter->setIcon(str.isEmpty() ? QIcon("://icons/32x32/Filter.png") : QIcon("://icons/32x32/Cancel.png"));
  CSearch currentSearch(str);

  // test whether syntax errors occured and show error
  if (currentSearch.getSyntaxError()) {
    addAction(actionError, QLineEdit::TrailingPosition);
    if (currentSearch.getSearchMode() == CSearch::eSearchModeName) {
      actionError->setToolTip(tr("Error parsing search.") + " " + tr("Continuing with search for match in names"));
    } else {
      actionError->setToolTip(tr("Error parsing search.") + " " + tr("Continuing with search for match in full text"));
    }
  } else {
    removeAction(actionError);
  }

  if (currentSearch.isAutodetectedProperty()) {
    addAction(actionAutoProperty, QLineEdit::TrailingPosition);
    actionAutoProperty->setToolTip(tr("Automatically set the property, please make sure the results are correct."));
  } else {
    removeAction(actionAutoProperty);
  }

  if (connectedProject != nullptr) {
    connectedProject->setProjectFilter(currentSearch);
    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
  } else {
    emit sigWorkspaceSearchChanged(currentSearch);
  }
}
