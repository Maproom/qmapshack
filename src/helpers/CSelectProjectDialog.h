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

#ifndef CSELECTPROJECTDIALOG_H
#define CSELECTPROJECTDIALOG_H

#include "ui_ISelectProjectDialog.h"
#include "gis/prj/IGisProject.h"
#include <QDialog>

class QTreeWidget;

class CSelectProjectDialog : public QDialog, private Ui::ISelectProjectDialog
{
    Q_OBJECT
public:
    CSelectProjectDialog(QString& key, QString& name, IGisProject::type_e& type, QTreeWidget *parent);
    virtual ~CSelectProjectDialog();

public slots:
    void accept() override;
    void reject() override;

private slots:
    void slotItemClicked(QListWidgetItem * item);
    void slotItemDoubleClicked(QListWidgetItem * item);
    void slotProjectChanged(const QString& text);
    void slotProjectEdited(const QString& text);
    void slotTypeChanged();

private:
    void setType(IGisProject::type_e& t);

    static QString lastkey;
    QString& key;
    QString& name;
    IGisProject::type_e& type;
};

#endif //CSELECTPROJECTDIALOG_H

