/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "mouse/range/CRangeToolSetup.h"

#include <QtWidgets>

#include "mouse/range/CScrOptRangeTool.h"
#include "svgticon/CSvgtIcon.h"

CRangeToolSetup::CRangeToolSetup(CScrOptRangeTool& parent) : QDialog(&parent), tool(parent) {
  setupUi(this);
  CSvgtIcon::load(label_6, "://icons/SelectRange.svgt");
  CSvgtIcon::load(label_8, "://icons/SelectEndPoint.svgt");
  CSvgtIcon::load(label_9, "://icons/SelectStartPoint.svgt");
  CSvgtIcon::load(label_7, "://icons/SelectReset.svgt");
  CSvgtIcon::load(label_10, "://icons/PointHide.svgt");
  CSvgtIcon::load(label_11, "://icons/PointShow.svgt");
  CSvgtIcon::load(label_12, "://icons/Activity.svgt");
  CSvgtIcon::load(label_13, "://icons/Copy.svgt");
  CSvgtIcon::load(label_14, "://icons/DeleteMultiple.svgt");
  CSvgtIcon::load(label_17, "://icons/SelectRange.svgt");
  CSvgtIcon::load(label_18, "://icons/SelectReset.svgt");
  CSvgtIcon::load(label_20, "://icons/SelectEndPoint.svgt");
  CSvgtIcon::load(label_22, "://icons/SelectStartPoint.svgt");

  actionsHidePoints->setAction(tool.actionHidePoints);
  actionsShowPoints->setAction(tool.actionShowPoints);
  actionsActivity->setAction(tool.actionActivity);
  actionsCopy->setAction(tool.actionCopy);
  actionsDelete->setAction(tool.actionDelete);

  checkMoveMap->setChecked(tool.enableUpdateCavas);
}

void CRangeToolSetup::accept() {
  tool.actionHidePoints = actionsHidePoints->getAction();
  tool.actionShowPoints = actionsShowPoints->getAction();
  tool.actionActivity = actionsActivity->getAction();
  tool.actionCopy = actionsCopy->getAction();
  tool.actionDelete = actionsDelete->getAction();

  tool.enableUpdateCavas = checkMoveMap->isChecked();

  QDialog::accept();
}
