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

#include <QVBoxLayout>
#include <QMessageBox>
#include "CRouterBRouterTilesPage.h"
#include "CRouterBRouterSetup.h"
#include "CRouterBRouterTilesSelect.h"

CRouterBRouterTilesPage::CRouterBRouterTilesPage()
    : QWizardPage()
{
    layout = new QVBoxLayout(this);
    widgetLocalTilesSelect = new CRouterBRouterTilesSelect(this);
    widgetLocalTilesSelect->setObjectName(QStringLiteral("widgetLocalTilesSelect"));
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetLocalTilesSelect->sizePolicy().hasHeightForWidth());
    widgetLocalTilesSelect->setSizePolicy(sizePolicy);
    layout->addWidget(widgetLocalTilesSelect);
    connect(widgetLocalTilesSelect, &CRouterBRouterTilesSelect::sigCompleteChanged, this, &CRouterBRouterTilesPage::slotTileDownloadStatusChanged);
}

CRouterBRouterTilesPage::~CRouterBRouterTilesPage()
{
}

bool CRouterBRouterTilesPage::isComplete() const
{
    return !(widgetLocalTilesSelect->isDownloading() || widgetLocalTilesSelect->isDownloadSelected());
}

void CRouterBRouterTilesPage::slotTileDownloadStatusChanged()
{
    emit completeChanged();
}

void CRouterBRouterTilesPage::beginPage()
{
    widgetLocalTilesSelect->initialize();
}

void CRouterBRouterTilesPage::setSetup(CRouterBRouterSetup * setup)
{
    widgetLocalTilesSelect->setSetup(setup);
}

bool CRouterBRouterTilesPage::raiseWarning()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    if (widgetLocalTilesSelect->isDownloading())
    {
        msgBox.setText("Download of routing data is in progress.");
    }
    else if (widgetLocalTilesSelect->isDownloadSelected())
    {
        msgBox.setText("You did not yet download the selected routing data.");
    }
    else
    {
        return false;
    }
    msgBox.setInformativeText("Do you want to cancel or continue with setup");
    msgBox.setStandardButtons(QMessageBox::Cancel);
    QPushButton *continueButton = msgBox.addButton(tr("Continue with Setup"), QMessageBox::NoRole);
    msgBox.exec();
    if (msgBox.clickedButton() == continueButton)
    {
        return true;
    }
    else
    {
        widgetLocalTilesSelect->cancelDownload();
        return false;
    }
}
