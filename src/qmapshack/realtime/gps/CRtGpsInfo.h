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

#ifndef CRTGPSINFO_H
#define CRTGPSINFO_H

#include "ui_IRtGpsInfo.h"

class CRtGps;
class QSettings;

class CRtGpsInfo : public QWidget, private Ui::IRtGpsInfo
{
    Q_OBJECT
public:
    CRtGpsInfo(CRtGps& source, QWidget * parent);
    virtual ~CRtGpsInfo() = default;

    void loadSettings(QSettings& cfg);
    void saveSettings(QSettings& cfg) const;
private slots:
    void slotSetSource(int idx);

private:
    CRtGps& source;
};

#endif //CRTGPSINFO_H

