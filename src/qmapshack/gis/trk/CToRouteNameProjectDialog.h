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

#ifndef CTOROUTENAMEPROJECTDIALOG_H
#define CTOROUTENAMEPROJECTDIALOG_H

#include "gis/prj/IGisProject.h"
#include "gis/CGisWorkspace.h"
#include "ui_IToRouteNameProjectDialog.h"
#include <QDialog>

class QTreeWidget;

/*
 * Dialog Class for the track to route conversion.
 */
class CToRouteNameProjectDialog : public QDialog, private Ui::IToRouteNameProjectDialog
{
    Q_OBJECT
public:
    /**
       @brief Shows the Dialog and preselects the project and route name
     */
    CToRouteNameProjectDialog(QString& projectName, QString& routeName, IGisProject::type_e& type);
    virtual ~CToRouteNameProjectDialog();

    /**
       @brief Returns the selected or new project

       This will either return the selected project or a newly created project for the selected
       route.

       @return  A project.
     */
    IGisProject* getProject();

    /**
      @brief Returns whether the sub-points shall be saved during route conversion

      @return True if sub-points shall be saved
    */
    bool getSaveSubPoints()
    {
        return saveSubPoints;
    }

public slots:
    void reject() override;
    void accept() override;

private slots:
    void slotItemClicked(QListWidgetItem* item);
    void slotItemDoubleClicked(QListWidgetItem* item);
    void slotProjectChanged(const QString& text);
    void slotProjectEdited(const QString& text);
    void slotTypeChanged();
    void slotRouteChanged(const QString& text);

private:
    void setType(IGisProject::type_e& t);
    void buttonBoxEnabled();

    QString key;
    bool saveSubPoints;
    QString& routeName;
    QString& projectName;
    IGisProject::type_e& type;
};

#endif //CSELECTPROJECTDIALOG_H

