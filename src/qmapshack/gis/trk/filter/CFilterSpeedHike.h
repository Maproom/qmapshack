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

#ifndef CFILTERSPEEDHIKE_H
#define CFILTERSPEEDHIKE_H

#include "ui_IFilterSpeedHike.h"

class CGisItemTrk;
class QSettings;

class CFilterSpeedHike : public QWidget, private Ui::IFilterSpeedHike
{
    Q_OBJECT
public:

    struct hiking_type_t
    {
        QString name;
        qreal plainSpeed;
        qreal ascending;
        qreal descending;

        inline bool isValid() const
        {
            if (!plainSpeed || !ascending || !descending)
            {
                return false;
            }
            return true;
        }
    };

    CFilterSpeedHike(QWidget * parent);
    virtual ~CFilterSpeedHike() = default;

    void loadSettings(QSettings& cfg);
    void saveSettings(QSettings& cfg);

    void apply(CGisItemTrk& trk);

private slots:
    void slotSetHikingType(int type);
    void slotSetPlainSpeed(double speed);
    void slotSetAscending(double ascending);
    void slotSetDescending(double descending);

private:
    const qint32 noOfFixTypes;
    const qint32 noOfCustomTypes;
    const QList<hiking_type_t> hikingTypeDefaults;
    QList <hiking_type_t> hikingTypes;
};

#endif //CFILTERSPEEDHIKE_H

