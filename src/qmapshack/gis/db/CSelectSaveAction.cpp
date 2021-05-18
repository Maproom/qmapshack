/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CCanvas.h"
#include "gis/db/CSelectSaveAction.h"
#include "gis/IGisItem.h"
#include "helpers/CProgressDialog.h"

CSelectSaveAction::CSelectSaveAction(const IGisItem* src, const IGisItem* tar, QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);

    labelIcon1->setPixmap(src->getDisplayIcon());
    labelInfo1->setText(src->getInfo(IGisItem::eFeatureShowName));
    labelIcon2->setPixmap(tar->getDisplayIcon());
    labelInfo2->setText(tar->getInfo(IGisItem::eFeatureShowName));

    adjustSize();

    connect(pushSave, &QPushButton::clicked, this, &CSelectSaveAction::slotSelectResult);
    connect(pushSkip, &QPushButton::clicked, this, &CSelectSaveAction::slotSelectResult);
    connect(pushClone, &QPushButton::clicked, this, &CSelectSaveAction::slotSelectResult);

    CCanvas::setOverrideCursor(Qt::ArrowCursor, "CSelectSaveAction");
    CProgressDialog::setAllVisible(false);
}

CSelectSaveAction::~CSelectSaveAction()
{
    CCanvas::restoreOverrideCursor("~CSelectSaveAction");
    CProgressDialog::setAllVisible(true);
}

bool CSelectSaveAction::allOthersToo()
{
    return checkAllOtherToo->isChecked();
}

void CSelectSaveAction::slotSelectResult()
{
    if(sender() == pushSave)
    {
        result = eResultSave;
    }
    else if(sender() == pushSkip)
    {
        result = eResultSkip;
    }
    else if(sender() == pushClone)
    {
        result = eResultClone;
    }

    accept();
}
