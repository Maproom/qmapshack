/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/rte/router/brouter/CRouterBRouterSetup.h"
#include "gis/rte/router/brouter/CRouterBRouterSetupPage.h"
#include "gis/rte/router/brouter/CRouterBRouterSetupWizard.h"

CRouterBRouterSetupPage::CRouterBRouterSetupPage()
    : QWizardPage()
{
}

CRouterBRouterSetupPage::~CRouterBRouterSetupPage()
{
}

bool CRouterBRouterSetupPage::isComplete() const
{
    switch(wizard()->currentId())
    {
    case CRouterBRouterSetupWizard::ePageLocalDirectory:
    {
        return setup != nullptr
               && setup->isLocalBRouterInstalled()
               && QFile(setup->localJavaExecutable).exists()
               && QFileInfo(setup->localJavaExecutable).baseName().startsWith("java");
    }

    case CRouterBRouterSetupWizard::ePageLocalInstallation:
    {
        return setup != nullptr
               && setup->isLocalBRouterInstalled();
    }

    case CRouterBRouterSetupWizard::ePageProfiles:
    {
        const QListView *profilesListView = findChild<QListView*>("listProfiles");
        Q_ASSERT(profilesListView != nullptr);
        return profilesListView->model()->rowCount() > 0;
    }

    default:
    {
        return false;
    }
    }
}

void CRouterBRouterSetupPage::setComplete(bool newComplete)
{
    if (newComplete != complete)
    {
        complete = newComplete;
        emit completeChanged();
    }
}

void CRouterBRouterSetupPage::emitCompleteChanged()
{
    emit completeChanged();
}
