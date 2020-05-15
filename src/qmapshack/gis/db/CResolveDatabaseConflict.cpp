/**********************************************************************************************
    Copyright (C) 2020 Henri Hornburg <hrnbg@t-online.de>

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

#include "gis/db/CResolveDatabaseConflict.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"

CResolveDatabaseConflict::CResolveDatabaseConflict(QString msg, IGisItem* item, CDBProject::action_e& actionForAll, QWidget *parent)
    : QDialog (parent),
    actionForAll (actionForAll)
{
    setupUi(this);

    labelMsg->setText(msg);
    connect(pButAbort, &QPushButton::clicked, this, [this] { slotButttonClicked(CDBProject::eActionNone); });
    connect(pButRemote, &QPushButton::clicked, this, [this] { slotButttonClicked(CDBProject::eActionReload); });
    connect(pButLocal, &QPushButton::clicked, this, [this] { slotButttonClicked(CDBProject::eActionUpdate); });

    //Cloning a geocache leads to an error, as they are identified by their GCCode
    CGisItemWpt* wpt = static_cast<CGisItemWpt*>(item);
    if(wpt != nullptr && wpt->isGeocache())
    {
        pButClone->hide();
    }
    else
    {
        connect(pButClone, &QPushButton::clicked, this, [this] { slotButttonClicked(CDBProject::eActionClone); });
    }
}

CDBProject::action_e CResolveDatabaseConflict::getAction()
{
    //If the clone button is hidden, thus the item being a geocache, and the action for all is clone,
    //the user will have to make a new selection for this item
    if(actionForAll != CDBProject::eActionNone && !(actionForAll == CDBProject::eActionClone && pButClone->isHidden()))
    {
        return actionForAll;
    }
    else
    {
        CProgressDialog::setAllVisible(false);
        exec();
        CProgressDialog::setAllVisible(true);
        return actionSelected;
    }
}

void CResolveDatabaseConflict::slotButttonClicked(CDBProject::action_e actionConnected)
{
    actionSelected = actionConnected;
    if(checkBoxAll->isChecked())
    {
        actionForAll = actionSelected;
    }

    if(actionConnected == CDBProject::eActionNone)
    {
        done(QDialog::Rejected);
    }
    else
    {
        done(QDialog::Accepted);
    }
}
