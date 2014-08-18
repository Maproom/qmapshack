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

#include "helpers/CSelectProjectDialog.h"
#include "gis/CGisProject.h"

#include <QtWidgets>

CSelectProjectDialog::CSelectProjectDialog(QString &key, QString &name, QTreeWidget * parent)
    : QDialog(parent)
    , key(key)
    , name(name)
{
    setupUi(this);

    for(int i = 0; i < parent->topLevelItemCount(); i++)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(parent->topLevelItem(i));
        if(project == 0)
        {
         continue;
        }

        QListWidgetItem * item = new QListWidgetItem(project->icon(0), project->text(0),listWidget);
        item->setData(Qt::UserRole, project->getKey());
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
    connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotProjectChanged(QString)));
    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotProjectEdited(QString)));

}

CSelectProjectDialog::~CSelectProjectDialog()
{

}

void CSelectProjectDialog::reject()
{
    key.clear();
    name.clear();

    QDialog::reject();
}

void CSelectProjectDialog::slotItemClicked(QListWidgetItem * item)
{

    key = item->data(Qt::UserRole).toString();
    lineEdit->setText(item->text());
}

void CSelectProjectDialog::slotProjectChanged(const QString& text)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
    name = text;
}

void CSelectProjectDialog::slotProjectEdited(const QString& text)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
    key.clear();
    name = text;
}
