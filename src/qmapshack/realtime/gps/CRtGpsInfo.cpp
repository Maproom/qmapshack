/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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
#include "realtime/gps/CRtGps.h"
#include "realtime/gps/CRtGpsInfo.h"
#include "realtime/gps/CRtGpsTether.h"

#include <QtPositioning>
#include <QtWidgets>

CRtGpsInfo::CRtGpsInfo(CRtGps& source, QWidget * parent)
    : QWidget(parent)
    , source(source)

{
    setupUi(this);
    connect(comboSource, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CRtGpsInfo::slotSetSource);
    comboSource->addItem(tr("choose one..."));
    comboSource->addItem(tr("GPS Tether"));

    stackedWidget->addWidget(new CRtGpsTether(stackedWidget));
}

void CRtGpsInfo::loadSettings(QSettings& cfg)
{
    int idx = comboSource->findText(cfg.value("device","").toString());
    if(idx > 0)
    {
        comboSource->setCurrentIndex(idx);
    }
    else
    {
        comboSource->setCurrentIndex(0);
    }

    cfg.beginGroup("devices");
    const int N = stackedWidget->count();
    for(int n = 1; n < N; n++)
    {
        auto device = dynamic_cast<IRtGpsDevice*>(stackedWidget->widget(n));
        if(device != nullptr)
        {
            device->loadSettings(cfg);
        }
    }
    cfg.endGroup();
}

void CRtGpsInfo::saveSettings(QSettings& cfg) const
{
    cfg.setValue("device", comboSource->currentText());

    cfg.beginGroup("devices");
    const int N = stackedWidget->count();
    for(int n = 1; n < N; n++)
    {
        auto device = dynamic_cast<IRtGpsDevice*>(stackedWidget->widget(n));
        if(device != nullptr)
        {
            device->saveSettings(cfg);
        }
    }
    cfg.endGroup();
}

void CRtGpsInfo::slotSetSource(int idx)
{
    stackedWidget->setCurrentIndex(idx);
}


void CRtGpsInfo::slotShowHelp()
{
//    IRtGpsDevice * intDevice = dynamic_cast<IRtGpsDevice*>(source.getDevice());
//    if(intDevice != nullptr)
//    {
//        QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"), intDevice->getHelp());
//    }
//    else
//    {
//        QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
//                                 tr("Qt Positioning\n"
//                                    "This is a Qt internal plugin using the positioning "
//                                    "service of your host system. It's dependent on your "
//                                    "operating system and your Qt installation. Please "
//                                    "refer to the Qt documentation for more details."));
//    }
}

