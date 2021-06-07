/**********************************************************************************************
    Copyright (C) 2021 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CTRKTORTEDIALOG_H
#define CTRKTORTEDIALOG_H

#include "gis/prj/IGisProject.h"
#include "gis/CGisWorkspace.h"
#include "ui_ITrkToRteDialog.h"

class QTreeWidget;

/*
 * Dialog Class for the track to route conversion.
 */
class CTrkToRteDialog : public QDialog, private Ui::ITrkToRteDialog
{
    Q_OBJECT
public:
    /**
       @brief Shows the Dialog and preselects the project and route name
     */
    CTrkToRteDialog(IGisProject*& project, QString& routeName, bool& saveSubPoints);
    virtual ~CTrkToRteDialog();

public slots:
    void accept() override;

private slots:
    void slotProject();
    void slotRouteChanged(const QString& text);

private:
    void setType(IGisProject::type_e& t);
    void buttonBoxEnabled();

    IGisProject*& project;
    QString& routeName;
    bool& saveSubPoints;
};

#endif //CTRKTORTEDIALOG_H

