/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/trk/CCutTrk.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CCutTrk::CCutTrk(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(radioKeepFirst,  &QRadioButton::toggled, this, &CCutTrk::slotClicked);
    connect(radioKeepBoth,   &QRadioButton::toggled, this, &CCutTrk::slotClicked);
    connect(radioKeepSecond, &QRadioButton::toggled, this, &CCutTrk::slotClicked);

    SETTINGS;
    cfg.beginGroup("TrackCut");
    checkCreateClone->setChecked(cfg.value("checkCreateClone", true).toBool());
    switch(cfg.value("mode", eModeKeepBoth).toInt())
    {
    case eModeKeepFirst:
        radioKeepFirst->setChecked(true);
        break;

    case eModeKeepBoth:
        radioKeepBoth->setChecked(true);
        break;

    case eModeKeepSecond:
        radioKeepSecond->setChecked(true);
        break;
    }

    switch(cfg.value("cutMode", eCutMode2).toInt())
    {
    case eCutMode1:
        radioCutMode1->setChecked(true);
        break;

    case eCutMode2:
        radioCutMode2->setChecked(true);
        break;
    }

    cfg.endGroup();
}


void CCutTrk::accept()
{
    SETTINGS;
    cfg.beginGroup("TrackCut");
    cfg.setValue("checkCreateClone", checkCreateClone->isChecked());
    cfg.setValue("mode", radioKeepFirst->isChecked() ? eModeKeepFirst : radioKeepBoth->isChecked() ? eModeKeepBoth : radioKeepSecond->isChecked() ? eModeKeepSecond : eModeNone);
    cfg.setValue("cutMode", radioCutMode1->isChecked() ? eCutMode1 : eCutMode2);
    cfg.endGroup();

    if(radioKeepFirst->isChecked())
    {
        mode = eModeKeepFirst;
    }
    else if(radioKeepBoth->isChecked())
    {
        mode = eModeKeepBoth;
    }
    else if(radioKeepSecond->isChecked())
    {
        mode = eModeKeepSecond;
    }

    if(radioCutMode1->isChecked())
    {
        cutMode = eCutMode1;
    }
    else
    {
        cutMode = eCutMode2;
    }

    QDialog::accept();
}

void CCutTrk::slotClicked()
{
    checkCreateClone->setEnabled(!radioKeepBoth->isChecked());
}
