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
#include "canvas/CCanvas.h"
#include "gis/CGisListWks.h"
#include "gis/prj/IGisProject.h"
#include "helpers/CSelectProjectDialog.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

QString CSelectProjectDialog::lastkey;


CSelectProjectDialog::CSelectProjectDialog(QString &key, QString &name, type_e& type, QTreeWidget * parent)
    : QDialog(CMainWindow::getBestWidgetForParent())
    , key(key)
    , name(name)
    , type(type)
{
    setupUi(this);

    QListWidgetItem *lastSelectedItem = nullptr;

    if(parent)
    {
        for(int i = 0; i < parent->topLevelItemCount(); i++)
        {
            IGisProject * project = dynamic_cast<IGisProject*>(parent->topLevelItem(i));
            if(nullptr == project)
            {
                continue;
            }

            QListWidgetItem * item = new QListWidgetItem(project->icon(CGisListWks::eColumnIcon), project->text(CGisListWks::eColumnName),listWidget);
            item->setData(Qt::UserRole+0, project->getKey());
            item->setData(Qt::UserRole+1, project->getType());
            item->setData(Qt::UserRole+2, project->getName());

            if(project->getKey() == lastkey)
            {
                lastSelectedItem = item;
            }
        }
    }
    else
    {
        listWidget->hide();
        label1->hide();
    }

    frameType->setEnabled(listWidget->count() == 0);

    if(nullptr == lastSelectedItem)
    {
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
    }
    else
    {
        slotItemClicked(lastSelectedItem);
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }

    connect(listWidget,    &QListWidget::itemClicked,       this, &CSelectProjectDialog::slotItemClicked);
    connect(listWidget,    &QListWidget::itemDoubleClicked, this, &CSelectProjectDialog::slotItemDoubleClicked);
    connect(lineEdit,      &QLineEdit::textChanged,         this, &CSelectProjectDialog::slotProjectChanged);
    connect(lineEdit,      &QLineEdit::textEdited,          this, &CSelectProjectDialog::slotProjectEdited);
    connect(radioQms,      &QRadioButton::clicked,          this, &CSelectProjectDialog::slotTypeChanged);
    connect(radioGpx,      &QRadioButton::clicked,          this, &CSelectProjectDialog::slotTypeChanged);
    connect(radioDatabase, &QRadioButton::toggled,          this, &CSelectProjectDialog::slotTypeChanged);

    adjustSize();

    CCanvas::setOverrideCursor(Qt::ArrowCursor, "CSelectProjectDialog");
}

CSelectProjectDialog::~CSelectProjectDialog()
{
    CCanvas::restoreOverrideCursor("~CSelectProjectDialog");
}

void CSelectProjectDialog::accept()
{
    lastkey = key;
    QDialog::accept();
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
    lineEdit->setText(item->data(Qt::UserRole+2).toString());
    frameType->setEnabled(false);

    switch(item->data(Qt::UserRole+1).toInt())
    {
    case IGisProject::eTypeQms:
        radioQms->setChecked(true);
        break;

    case IGisProject::eTypeGpx:
        radioGpx->setChecked(true);
        break;

    case IGisProject::eTypeDb:
        radioDatabase->setChecked(true);
        break;
    }
}

void CSelectProjectDialog::slotItemDoubleClicked(QListWidgetItem * item)
{
    key = item->data(Qt::UserRole).toString();
    lineEdit->setText(item->data(Qt::UserRole+2).toString());
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

