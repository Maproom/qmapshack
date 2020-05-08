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

#ifndef CWPTICONDIALOG_H
#define CWPTICONDIALOG_H

#include "ui_IWptIconDialog.h"
#include <QDialog>

class QToolButton;
class QAction;
class QListWidgetItem;
class CMainWindow;

class CWptIconDialog : public QDialog, private Ui::IWptIconDialog
{
    Q_OBJECT
public:
    CWptIconDialog(CMainWindow * parent);
    virtual ~CWptIconDialog();

private slots:
    void slotSetupPath();

private:
    void setupList(QObject *obj);
    void setupSignals();

    QToolButton * button = nullptr;
    QAction     * action = nullptr;
};

#endif //CWPTICONDIALOG_H

