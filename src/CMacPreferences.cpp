/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg

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

#include "CMacPreferences.h"

#include "canvas/CCanvas.h"
#include "dem/CDemDraw.h"
#include "dem/CDemPathSetup.h"
#include "gis/db/CSetupWorkspace.h"
#include "gis/trk/CKnownExtension.h"
#include "helpers/CSettings.h"
#include "map/CMapDraw.h"
#include "map/CMapPathSetup.h"
#include "units/CCoordFormatSetup.h"
#include "units/CTimeZoneSetup.h"
#include "units/CUnitsSetup.h"

#include <QtGui>
#include <QtWidgets>


CMacPreferences::CMacPreferences(QWidget * parent)
    : parent(parent), mapPaths((QStringList&)CMapDraw::getMapPaths()), cache((QString&)CMapDraw::getCacheRoot()),
    demPaths((QStringList&)CDemDraw::getDemPaths())
{
    setupUi(this);

    // remove default status bar
    this->setStatusBar(nullptr);

    connect(actionSetupMapPaths,         &QAction::triggered,            this,      &CMacPreferences::slotSetupMapPath);
    connect(actionSetupDEMPaths,         &QAction::triggered,            this,      &CMacPreferences::slotSetupDemPath);
    connect(actionSetupUnits,            &QAction::triggered,            this,      &CMacPreferences::slotSetupUnits);
    connect(actionSetupTimeZone,         &QAction::triggered,            this,      &CMacPreferences::slotSetupTimeZone);
    connect(actionSetupCoordFormat,      &QAction::triggered,            this,      &CMacPreferences::slotSetupCoordFormat);
    connect(actionSetupWorkspace,        &QAction::triggered,            this,      &CMacPreferences::slotSetupWorkspace);

    addSettingsDialog(new CMapPathSetup(mapPaths, cache));
    addSettingsDialog(new CDemPathSetup(demPaths));
    addSettingsDialog(new CUnitsSetup(this));
    addSettingsDialog(new CTimeZoneSetup(this));
    addSettingsDialog(new CCoordFormatSetup(this));
    addSettingsDialog(new CSetupWorkspace(this));
    this->setCentralWidget(stackedWidget);
    // setup first dialog
    switchSettingsDialog(0);
}


void CMacPreferences::addSettingsDialog(QDialog* settingDialog)
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(settingDialog->sizePolicy().hasHeightForWidth());
    settingDialog->setSizePolicy(sizePolicy);

    settingDialog->setWindowFlags(Qt::Widget);
    QDialogButtonBox* buttonBox = settingDialog->findChild<QDialogButtonBox*>();
    if(buttonBox != nullptr)
    {
        buttonBox->hide();
    }
    stackedWidget->addWidget(settingDialog);
}

void CMacPreferences::switchSettingsDialog(int idx)
{
    stackedWidget->setCurrentIndex(idx);
    parent->setWindowTitle(stackedWidget->currentWidget()->windowTitle());

    // resize to setting content (300 min width to make sure all buttons on toolbar are visible)
    stackedWidget->widget(idx)->adjustSize();
    QSize sw = stackedWidget->widget(idx)->size();
    QSize st = toolBar->size();
    parent->resize(qMax(sw.width(), 300), sw.height()+st.height());
}


void CMacPreferences::saveSettings()
{
    // all settings dialogs do some stuff in the accept()
    for(int i = 0; i < stackedWidget->count(); i++)
    {
        QDialog* dlg = (QDialog*)stackedWidget->widget(i);
        dlg->accept();
    }
    // some setting dialogs needs some more stuff
    CMapDraw::setupMapPath(mapPaths);
    CDemDraw::setupDemPath(demPaths);
    CKnownExtension::init(IUnit::self());
}


void CMacPreferences::slotSetupMapPath()
{
    switchSettingsDialog(0);
}

void CMacPreferences::slotSetupDemPath()
{
    switchSettingsDialog(1);
}

void CMacPreferences::slotSetupUnits()
{
    switchSettingsDialog(2);
}

void CMacPreferences::slotSetupTimeZone()
{
    switchSettingsDialog(3);
}

void CMacPreferences::slotSetupCoordFormat()
{
    switchSettingsDialog(4);
}

void CMacPreferences::slotSetupWorkspace()
{
    switchSettingsDialog(5);
}
