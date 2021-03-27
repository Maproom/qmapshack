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

#include "CMainWindow.h"
#include "gis/proj_x.h"
#include "helpers/mitab.h"
#include "overlay/refmap/CProjWizard.h"

#include <QtWidgets>

struct mitab_entry_t
{
    QString name;
    int idx;
};

static bool mitabLessThan(const mitab_entry_t &s1, const mitab_entry_t &s2)
{
    return s1.name < s2.name;
}

CProjWizard::CProjWizard(QLineEdit &line, QWidget * parent)
    : QDialog(parent)
    , line(line)
{
    setupUi(this);
    QList<mitab_entry_t> list;
    int idx = 0;

    for(const MapInfoDatumInfo& di : asDatumInfoList)
    {
        mitab_entry_t entry;
        entry.name  = di.pszOGCDatumName;
        if(!entry.name.isEmpty())
        {
            for(const MapInfoSpheroidInfo& si : asSpheroidInfoList)
            {
                if(si.nMapInfoId == di.nEllipsoid)
                {
                    entry.name += tr(" (Spheroid: %1)").arg(si.pszMapinfoName);
                }
            }
        }
        entry.idx   = idx;
        list << entry;
        ++idx;
    }
    qSort(list.begin(), list.end(), mitabLessThan);

    for(const mitab_entry_t &entry : qAsConst(list))
    {
        comboDatum->addItem(entry.name, entry.idx);
    }

    comboHemisphere->addItem(tr("north"), "");
    comboHemisphere->addItem(tr("south"), "+south");

    connect(radioMercator,      &QRadioButton::clicked,  this, &CProjWizard::slotChange);
    connect(radioWorldMercator, &QRadioButton::clicked,  this, &CProjWizard::slotChange);
    connect(radioUPSNorth,      &QRadioButton::clicked,  this, &CProjWizard::slotChange);
    connect(radioUPSSouth,      &QRadioButton::clicked,  this, &CProjWizard::slotChange);
    connect(radioUTM,           &QRadioButton::clicked,  this, &CProjWizard::slotChange);
    connect(radioUserDef,       &QRadioButton::clicked,  this, &CProjWizard::slotChange);
    connect(lineUserDef,        &QLineEdit::textChanged, this, &CProjWizard::slotChange);

    connect(spinUTMZone,        static_cast<void (QSpinBox::*)(int) >(&QSpinBox::valueChanged),         this, &CProjWizard::slotChange);
    connect(comboDatum,         static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CProjWizard::slotChange);
    connect(comboHemisphere,    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CProjWizard::slotChange);

    QString projstr = line.text();
    QRegExp re2("\\s*\\+proj=merc \\+a=6378137 \\+b=6378137 \\+lat_ts=0.001 \\+lon_0=0.0 \\+x_0=0.0 \\+y_0=0 \\+k=1.0 \\+units=m \\+nadgrids=@null \\+no_defs");
    QRegExp re3("\\s*\\+proj=merc\\s(.*)");
    QRegExp re4("\\s*\\+proj=utm \\+zone=([0-9]+)\\s(.*)");

    if(re2.exactMatch(projstr))
    {
        radioWorldMercator->setChecked(true);
    }
    else if(re3.exactMatch(projstr))
    {
        radioMercator->setChecked(true);
        findDatum(re3.cap(1));
    }
    else if(re4.exactMatch(projstr))
    {
        radioUTM->setChecked(true);
        spinUTMZone->setValue(re4.cap(1).toInt());

        QString datum = re4.cap(2);
        if(datum.startsWith("+south "))
        {
            datum = datum.mid(7);
            comboHemisphere->setCurrentIndex(1);
        }

        findDatum(datum);
    }

    slotChange();
}

CProjWizard::~CProjWizard()
{
}


void CProjWizard::findDatum(const QString& str)
{
    QString cmp;
    for(const MapInfoDatumInfo& di : asDatumInfoList)
    {
        cmp.clear();
        if(di.pszOGCDatumName != QString())
        {
            for(const MapInfoSpheroidInfo& si : asSpheroidInfoList)
            {
                if(si.nMapInfoId == di.nEllipsoid)
                {
                    cmp += QString("+a=%1 +b=%2 ").arg(si.dfA, 0, 'f', 4).arg(si.dfA * (1.0 - (1.0 / si.dfInvFlattening)), 0, 'f', 4);
                    cmp += QString("+towgs84=%1,%2,%3,%4,%5,%6,%7 ").arg(di.dfShiftX).arg(di.dfShiftY).arg(di.dfShiftZ).arg(di.dfDatumParm0).arg(di.dfDatumParm1).arg(di.dfDatumParm2).arg(di.dfDatumParm3);
                    cmp += "+units=m  +no_defs";
                    break;
                }
            }
        }

        if(cmp == str)
        {
            comboDatum->setCurrentIndex(comboDatum->findText(di.pszOGCDatumName));
            break;
        }
    }
}


void CProjWizard::slotChange()
{
    QString str;
    if(radioMercator->isChecked())
    {
        str += "+proj=merc ";
    }
    else if(radioWorldMercator->isChecked())
    {
        str += "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.001 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +no_defs";
        labelResult->setText(str);
        return;
    }
    else if(radioUPSNorth->isChecked())
    {
        str += "EPSG:32661";
    }
    else if(radioUPSSouth->isChecked())
    {
        str += "EPSG:32761";
    }
    else if(radioUTM->isChecked())
    {
        str += QString("+proj=utm +zone=%1 %2 ").arg(spinUTMZone->value()).arg(comboHemisphere->itemData(comboHemisphere->currentIndex()).toString());
    }
    else if(radioUserDef->isChecked())
    {
        str += lineUserDef->text() + " ";
    }

    int idx = comboDatum->itemData(comboDatum->currentIndex()).toInt();
    const MapInfoDatumInfo& di = asDatumInfoList[idx];
    if(di.pszOGCDatumName != QString())
    {
        for(const MapInfoSpheroidInfo& si : asSpheroidInfoList)
        {
            if(si.nMapInfoId == di.nEllipsoid)
            {
                str += QString("+a=%1 +b=%2 ").arg(si.dfA, 0, 'f', 4).arg(si.dfA * (1.0 - (1.0 / si.dfInvFlattening)), 0, 'f', 4);
                str += QString("+towgs84=%1,%2,%3,%4,%5,%6,%7 ").arg(di.dfShiftX).arg(di.dfShiftY).arg(di.dfShiftZ).arg(di.dfDatumParm0).arg(di.dfDatumParm1).arg(di.dfDatumParm2).arg(di.dfDatumParm3);
                str += "+units=m  +no_defs";
                break;
            }
        }
    }

    labelResult->setText(str);
}


void CProjWizard::accept()
{
    if (CProjWizard::validProjStr(labelResult->text()))
    {
        line.setText(labelResult->text());
        line.setCursorPosition(0);
        QDialog::accept();
    }
}


bool CProjWizard::validProjStr(const QString projStr)
{
    return CProj::validProjStr(projStr, true, [](const QString& msg){
        QMessageBox::warning(&CMainWindow::self(), tr("Error..."), msg, QMessageBox::Abort, QMessageBox::Abort);
    });
}


