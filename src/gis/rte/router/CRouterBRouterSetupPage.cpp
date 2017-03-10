/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#include "CRouterBRouterSetupPage.h"
#include "CRouterBRouterSetup.h"
#include "CRouterBRouterSetupWizard.h"

CRouterBRouterSetupPage::CRouterBRouterSetupPage()
    : QWizardPage()
{
}

CRouterBRouterSetupPage::~CRouterBRouterSetupPage()
{
}

bool CRouterBRouterSetupPage::isComplete() const
{
    if (setup == nullptr)
    {
        return false;
    }
    switch(wizard()->currentId())
    {
    case CRouterBRouterSetupWizard::Page_LocalDirectory:
    {
        return setup->isLocalBRouterInstalled()
                && QFile(setup->localJavaExecutable).exists()
                && QFileInfo(setup->localJavaExecutable).baseName().startsWith("java");
    }
    case CRouterBRouterSetupWizard::Page_LocalInstallation:
    {
        return setup->isLocalBRouterInstalled();
    }
    case CRouterBRouterSetupWizard::Page_Profiles:
    {
        return findChild<QListView*>("listProfiles")->model()->rowCount() > 0;
    }
    default:
    {
        return false;
    }
    }
}

void CRouterBRouterSetupPage::setSetup(CRouterBRouterSetup * setup)
{
    this->setup = setup;
}

void CRouterBRouterSetupPage::emitCompleteChanged()
{
    emit completeChanged();
}
