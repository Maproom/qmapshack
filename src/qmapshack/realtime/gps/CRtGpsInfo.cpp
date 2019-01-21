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
#include "realtime/gps/IRtGpsDevice.h"

#include <QtPositioning>
#include <QtWidgets>

CRtGpsInfo::CRtGpsInfo(CRtGps& source, QWidget * parent)
    : QWidget(parent)
    , source(source)

{
    setupUi(this);
    connect(&source, &CRtGps::sigChanged, this, &CRtGpsInfo::slotUpdate);
    connect(comboSource, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CRtGpsInfo::slotSetSource);
    connect(toolHelp, &QToolButton::clicked, this, &CRtGpsInfo::slotShowHelp);
    connect(toolConfig, &QToolButton::clicked, this, &CRtGpsInfo::slotConfigure);

    comboSource->addItem(tr("choose one..."));
    for(const QString& source : source.getDevices())
    {
        comboSource->addItem(source);
    }
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
}

void CRtGpsInfo::saveSettings(QSettings& cfg) const
{
    cfg.setValue("device", comboSource->currentText());
}

void CRtGpsInfo::slotSetSource(int idx)
{
    frame->setEnabled(source.setDevice(comboSource->currentText()));
}

void CRtGpsInfo::slotUpdate()
{
    const QGeoPositionInfoSource * device = source.getDevice();
    // try to get point to internally defined device that implements IRtGpsDevice API
    const IRtGpsDevice * intDevice = dynamic_cast<const IRtGpsDevice*>(device);
    if(intDevice != nullptr)
    {
        toolConfig->setEnabled(intDevice->hasConfig());
        toolHelp->setEnabled(true);
        labelConfig->setText(intDevice->getConfig());
    }
    else
    {
        toolConfig->setEnabled(false);
        toolHelp->setEnabled(device != nullptr);
        labelConfig->setText("-");
    }

}

void CRtGpsInfo::slotShowHelp()
{
    IRtGpsDevice * intDevice = dynamic_cast<IRtGpsDevice*>(source.getDevice());
    if(intDevice != nullptr)
    {
        QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"), intDevice->getHelp());
    }
    else
    {
        QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
                                 tr("Qt Positioning\n"
                                    "This is a Qt internal plugin using the positioning "
                                    "service of your host system. It's dependent on your "
                                    "operating system and your Qt installation. Please "
                                    "refer to the Qt documentation for more details."));
    }
}

void CRtGpsInfo::slotConfigure()
{
    IRtGpsDevice * intDevice = dynamic_cast<IRtGpsDevice*>(source.getDevice());
    if(intDevice != nullptr)
    {
        intDevice->configure();
        slotUpdate();
    }
}

