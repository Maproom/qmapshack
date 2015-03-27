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

#ifndef CDETAILSTRK_H
#define CDETAILSTRK_H

#include "ui_IDetailsTrk.h"
#include <QWidget>

class CGisItemTrk;

class CDetailsTrk : public QWidget, private Ui::IDetailsTrk
{
    Q_OBJECT
public:
    CDetailsTrk(CGisItemTrk &trk, QWidget * parent);
    virtual ~CDetailsTrk();

    void setMouseFocus(const CGisItemTrk::trkpt_t * pt);
    void setMouseClickFocus(const CGisItemTrk::trkpt_t * pt);

public slots:
    void setupGui();

private slots:
    void slotShowPlots();
    void slotColorChanged(int idx);
    void slotChangeReadOnlyMode(bool on);
    void slotItemSelectionChanged();
    void slotLinkActivated(const QUrl& url);
    void slotLinkActivated(const QString& url);

private:
    enum columns_t
    {
        eColNum
        ,eColTime
        ,eColEle
        ,eColDelta
        ,eColDist
        ,eColSpeed
        ,eColSlope
        ,eColAscend
        ,eColDescend
        ,eColPosition
        ,eColMax
    };

    /**
       @brief Pointer to track item

       It is ok to store the pointer as this widget is created by the track item. The
       track item will destroy this object on it's own destruction.
     */
    CGisItemTrk& trk;

    bool originator;
};

#endif //CDETAILSTRK_H

