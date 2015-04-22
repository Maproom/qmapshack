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

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/prj/IGisProject.h"
#include "helpers/CSelectProjectDialog.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CSelectProjectDialog::CSelectProjectDialog(QString &key, QString &name, type_e& type, QTreeWidget * parent)
    : QDialog(&CMainWindow::self())
    , key(key)
    , name(name)
    , type(type)
{
    setupUi(this);

    if(parent)
    {
        for(int i = 0; i < parent->topLevelItemCount(); i++)
        {
            IGisProject * project = dynamic_cast<IGisProject*>(parent->topLevelItem(i));
            if(project == 0)
            {
                continue;
            }

            QListWidgetItem * item = new QListWidgetItem(project->icon(CGisListWks::eColumnIcon), project->text(CGisListWks::eColumnName),listWidget);
            item->setData(Qt::UserRole, project->getKey());
        }
    }
    else
    {
        listWidget->hide();
        label1->hide();
    }
    frameType->setEnabled(listWidget->count() == 0);

    SETTINGS;
    QString filter = cfg.value("Paths/lastGisFilter", "GPS Exchange Format (*.qms)").toString();
    if(filter.contains("qms"))
    {
        radioQms->setChecked(true);
        type = eTypeQms;
    }
    else if(filter.contains("gpx"))
    {
        radioGpx->setChecked(true);
        type = eTypeGpx;
    }
    else
    {
        radioQms->setChecked(true);
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotItemDoubleClicked(QListWidgetItem*)));
    connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotProjectChanged(QString)));
    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotProjectEdited(QString)));
    connect(radioQms, SIGNAL(clicked()), this, SLOT(slotTypeChanged()));
    connect(radioGpx, SIGNAL(clicked()), this, SLOT(slotTypeChanged()));
    connect(radioDatabase, SIGNAL(clicked()), this, SLOT(slotTypeChanged()));

    adjustSize();
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
    frameType->setEnabled(false);
}

void CSelectProjectDialog::slotItemDoubleClicked(QListWidgetItem * item)
{
    key = item->data(Qt::UserRole).toString();
    lineEdit->setText(item->text());
    frameType->setEnabled(false);

    QDialog::accept();
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
    frameType->setEnabled(true);
}

void CSelectProjectDialog::slotTypeChanged()
{
    if(radioQms->isChecked())
    {
        type = eTypeQms;
    }
    else if(radioGpx->isChecked())
    {
        type = eTypeGpx;
    }
    else if(radioDatabase->isChecked())
    {
        type = eTypeDb;
    }
}
