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

#ifndef CINVALIDTRK_H
#define CINVALIDTRK_H

#include "ui_IInvalidTrk.h"

class CGisItemTrk;

class CInvalidTrk : public QDialog, private Ui::IInvalidTrk
{
    Q_OBJECT
public:
    CInvalidTrk(CGisItemTrk& trk, QWidget* parent);
    virtual ~CInvalidTrk();

public slots:
    void accept() override;

private slots:
    void slotShowDetails();
    void slotDoNotAskAgain(bool checked);

private:
    CGisItemTrk& trk;
};

#endif //CINVALIDTRK_H

