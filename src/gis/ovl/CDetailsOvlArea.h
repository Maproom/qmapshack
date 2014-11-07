/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CDETAILSOVLAREA_H
#define CDETAILSOVLAREA_H

#include <QDialog>
#include "ui_IDetailsOvlArea.h"

class CGisItemOvlArea;

class CDetailsOvlArea : public QDialog, private Ui::IDetailsOvlArea
{
    Q_OBJECT
    public:
        CDetailsOvlArea(CGisItemOvlArea &area, QWidget * parent);
        virtual ~CDetailsOvlArea();

    private slots:
        void slotSetColor(int idx);
        void slotSetWidth(int idx);
        void slotSetStyle(int idx);
        void slotOpyacity(bool yes);
        void slotChangeReadOnlyMode(bool on);
        void slotLinkActivated(const QUrl& url);
        void slotLinkActivated(const QString& link);
        void setupGui();

    private:        
        enum columns_t
        {
            eColNum
            ,eColPosition
            ,eColMax
        };

        CGisItemOvlArea& area;

        bool originator;
};

#endif //CDETAILSOVLAREA_H

