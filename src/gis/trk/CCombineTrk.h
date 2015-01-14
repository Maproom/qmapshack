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

#ifndef CCOMBINETRK_H
#define CCOMBINETRK_H

#include "gis/IGisItem.h"
#include "ui_ICombineTrk.h"
#include <QDialog>

class CGisItemTrk;
class IGisProject;

class CCombineTrk : public QDialog, private Ui::ICombineTrk
{
    Q_OBJECT
public:
    CCombineTrk(CGisItemTrk& trk, IGisProject &project, QWidget * parent);
    virtual ~CCombineTrk();

    const QList<IGisItem::key_t>& getTrackKeys()
    {
        return keys;
    }

public slots:
    void accept();

private slots:
    void slotSelectionChanged();
    void slotSelect();
    void slotRemove();
    void slotUp();
    void slotDown();

private:
    void updatePreview();
    CGisItemTrk& trk;
    IGisProject& project;

    QList<IGisItem::key_t> keys;
};

#endif //CCOMBINETRK_H

