/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CCANVASSETUP_H
#define CCANVASSETUP_H

#include "ui_ICanvasSetup.h"
#include <QDialog>

class CCanvas;

class CCanvasSetup : public QDialog, private Ui::ICanvasSetup
{
    Q_OBJECT
public:
    CCanvasSetup(CCanvas *canvas);
    virtual ~CCanvasSetup();

public slots:
    void accept() override;


protected slots:
    void slotProjWizard();

protected:
    CCanvas * canvas;
};

#endif //CCANVASSETUP_H

