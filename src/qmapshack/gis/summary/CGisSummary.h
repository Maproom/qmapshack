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

#ifndef CGISSUMMARY_H
#define CGISSUMMARY_H

#include "ui_IGisSummary.h"

class CGisSummaryDropZone;

class CGisSummary : public QWidget, private Ui::IGisSummary
{
    Q_OBJECT
public:
    CGisSummary(QWidget * parent);
    virtual ~CGisSummary();

    enum dropzone_e
    {
        eDropZone1 = 0
        , eDropZone2 = 1
        , eDropZone3 = 2
        , eDropZone4 = 3
        , eDropZoneMax = 4
    };

    struct folder_t
    {
        QString name;
        QString db;
        quint64 id = 0;
    };

    struct dropzone_t
    {
        QString name;
        QList<folder_t> folders;
        CGisSummaryDropZone * zone = nullptr;
    };

    dropzone_t& getDropZone(dropzone_e dropzone)
    {
        return dropZones[dropzone];
    }

private slots:
    void slotStartup();
    void slotSetup();

private:
    void setupDropZones();
    QVector<dropzone_t> dropZones;
};

#endif //CGISSUMMARY_H

