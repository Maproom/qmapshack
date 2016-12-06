/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/db/CExportDialog.h"
#include "gis/db/macros.h"
#include "helpers/CSettings.h"

#include <QtSql>
#include <QtWidgets>

CExportDialog::CExportDialog(quint64 id, QSqlDatabase &db, QWidget *parent)
    : QDialog(parent)
    , id(id)
    , db(db)
{
    setupUi(this);

    SETTINGS;
    cfg.beginGroup("ExportDB");
    labelPath->setText(cfg.value("path","-").toString());
    checkGpx11->setChecked(cfg.value("asGpx11", false).toBool());
    cfg.endGroup();

    connect(toolPath, &QToolButton::clicked, this, &CExportDialog::slotSetPath);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::pressed, this, &CExportDialog::slotApply);
}

CExportDialog::~CExportDialog()
{
    SETTINGS;
    cfg.beginGroup("ExportDB");
    cfg.setValue("path", labelPath->text());
    cfg.setValue("asGpx11", checkGpx11->isChecked());
    cfg.endGroup();
}

void CExportDialog::stdOut(const QString& str)
{
    textBrowser->setTextColor(Qt::black);
    textBrowser->append(str);
}


void CExportDialog::stdErr(const QString& str)
{
    textBrowser->setTextColor(Qt::red);
    textBrowser->append(str);
}


void CExportDialog::slotSetPath()
{
    QString path = labelPath->text();

    path = QFileDialog::getExistingDirectory(CMainWindow::self().getBestWidgetForParent(), tr("Select export path..."), path);
    if(path.isEmpty())
    {
        return;
    }
    labelPath->setText(path);
}

void CExportDialog::slotApply()
{
    stdOut("xxx");
}
