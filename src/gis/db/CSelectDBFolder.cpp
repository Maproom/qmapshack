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

#include "gis/db/CDBFolderDatabase.h"
#include "gis/db/CSelectDBFolder.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CSelectDBFolder::CSelectDBFolder(quint64 &id, QString &db, QWidget *parent)
    : QDialog(parent)
    , id(id)
    , db(db)
{
    setupUi(this);

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    SETTINGS;
    QStringList names = cfg.value("Database/names").toStringList();
    QStringList files = cfg.value("Database/files").toStringList();

    const int N = names.count();
    for(int i = 0; i < N; i++)
    {
        new CDBFolderDatabase(files[i], names[i], treeWidget);
    }

    connect(treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)));
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));

    QApplication::setOverrideCursor(Qt::ArrowCursor);

}

CSelectDBFolder::~CSelectDBFolder()
{
    QApplication::restoreOverrideCursor();
}

void CSelectDBFolder::slotItemExpanded(QTreeWidgetItem * item)
{
    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(folder == 0)
    {
        return;
    }

    folder->expanding();
}


void CSelectDBFolder::slotItemSelectionChanged()
{
    IDBFolder * folder = dynamic_cast<IDBFolder*>(treeWidget->currentItem());
    if(folder)
    {
        id = folder->getId();
        db = folder->getDBName();
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else
    {
        id = -1;
        db.clear();
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}
