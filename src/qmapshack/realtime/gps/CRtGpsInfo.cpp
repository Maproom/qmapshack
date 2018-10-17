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

#include "realtime/gps/CRtGpsInfo.h"
#include "realtime/gps/CRtRfcomm.h"

#include <QtPositioning>
#include <QtWidgets>

CRtGpsInfo::CRtGpsInfo(CRtGps& source, QWidget * parent)
    : QWidget(parent)
    , source(source)

{
    setupUi(this);
    connect(comboSource, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CRtGpsInfo::slotSetSource);

    QStringList sources = QGeoPositionInfoSource::availableSources();

#ifdef Q_OS_LINUX
    sources += "rfcomm";
#endif

    comboSource->addItem(tr("choose one..."));
    for(const QString& source : sources)
    {
        comboSource->addItem(source);
    }
}

void CRtGpsInfo::loadSettings(QSettings& cfg)
{
    int idx = comboSource->findText(cfg.value("source","").toString());
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
    cfg.setValue("source", comboSource->currentText());
}

void CRtGpsInfo::slotSetSource(int idx)
{
    qDebug() << comboSource->currentText();
    frame->setEnabled(comboSource->currentIndex() != 0);

}
