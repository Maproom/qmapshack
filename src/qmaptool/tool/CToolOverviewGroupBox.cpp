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

#include "setup/IAppSetup.h"
#include "tool/CToolOverviewGroupBox.h"

#include <QtWidgets>

CToolOverviewGroupBox::CToolOverviewGroupBox(QWidget *parent)
    : QGroupBox(parent)
{
    setupUi(this);
}

void CToolOverviewGroupBox::saveSettings(QSettings& cfg)
{
    cfg.setValue("createOverviews", isChecked());
    cfg.setValue("by2", checkBy2->isChecked());
    cfg.setValue("by4", checkBy4->isChecked());
    cfg.setValue("by8", checkBy8->isChecked());
    cfg.setValue("by16", checkBy16->isChecked());
    cfg.setValue("by32", checkBy32->isChecked());
    cfg.setValue("by64", checkBy64->isChecked());
    cfg.setValue("useExternal", checkExternal->isChecked());
}

void CToolOverviewGroupBox::loadSettings(QSettings& cfg)
{
    setChecked(cfg.value("createOverviews", false).toBool());
    checkBy2->setChecked(cfg.value("by2", false).toBool());
    checkBy4->setChecked(cfg.value("by4", false).toBool());
    checkBy8->setChecked(cfg.value("by8", false).toBool());
    checkBy16->setChecked(cfg.value("by16", false).toBool());
    checkBy32->setChecked(cfg.value("by32", false).toBool());
    checkBy64->setChecked(cfg.value("by64", false).toBool());
    checkExternal->setChecked(cfg.value("useExternal", true).toBool());
}


void CToolOverviewGroupBox::buildCmd(QList<CShellCmd>& cmds, const QString& filename, const QString& resampling)
{
    if(isChecked())
    {
        QStringList args;
        if(checkExternal->isChecked())
        {
            args << "-ro";
        }

        args << "-r";
        args << resampling;

        args << filename;
        if(checkBy2->isChecked())
        {
            args << "2";
        }
        if(checkBy4->isChecked())
        {
            args << "4";
        }
        if(checkBy8->isChecked())
        {
            args << "8";
        }
        if(checkBy16->isChecked())
        {
            args << "16";
        }
        if(checkBy32->isChecked())
        {
            args << "32";
        }
        if(checkBy64->isChecked())
        {
            args << "64";
        }
        cmds << CShellCmd(IAppSetup::self().getGdaladdo(), args);
    }
}
