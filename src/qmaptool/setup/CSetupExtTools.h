/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSETUPEXTTOOLS_H
#define CSETUPEXTTOOLS_H

#include <functional>
#include <QDialog>

#include "ui_ISetupExtTools.h"

using fResetPath = std::function<void()>;
using fSetPath = std::function<void(const QString&)>;

class CSetupExtTools : public QDialog, private Ui::ISetupExtTools
{
    Q_OBJECT
public:
    CSetupExtTools(QWidget* parent);
    virtual ~CSetupExtTools() = default;

private slots:
    void slotSetPathXOverride(const QString& name, fSetPath setPath);
    void slotResetPathXOverride(fResetPath resetPath);

private:
    void setupGui();
};

#endif //CSETUPEXTTOOLS_H

