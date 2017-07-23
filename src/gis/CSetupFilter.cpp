/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/CGisWidget.h"
#include "gis/CSetupFilter.h"
#include "gis/prj/IGisProject.h"

CSetupFilter::CSetupFilter(CGisWidget *parent)
    : QWidget(parent)
    , gisWidget(parent)
{
    setupUi(this);

    switch (IGisProject::filterMode)
    {
    case IGisProject::eFilterModeName:
        radioName->setChecked(true);
        break;

    case IGisProject::eFilterModeText:
        radioText->setChecked(true);
        break;
    }

    connect(radioName, &QRadioButton::clicked, this, &CSetupFilter::slotSelect);
    connect(radioText, &QRadioButton::clicked, this, &CSetupFilter::slotSelect);
}


void CSetupFilter::slotSelect()
{
    IGisProject::filterMode = radioName->isChecked() ? IGisProject::eFilterModeName : IGisProject::eFilterModeText;
    gisWidget->applyFilter();
    deleteLater();
}

