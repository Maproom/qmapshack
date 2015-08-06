/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CPROGRESSDIALOG_H
#define CPROGRESSDIALOG_H

#include "ui_IProgressDialog.h"
#include <QDialog>
#include <QTime>


#define PROGRESS_SETUP(lbl, min, max, parent) \
    CProgressDialog progress(lbl, min, max, parent);


#define PROGRESS(x, cmd) \
    progress.setValue(x); \
    if (progress.wasCanceled()) { cmd; } \

class CProgressDialog : public QDialog, private Ui::IProgressDialog
{
    Q_OBJECT
public:
    CProgressDialog(const QString text, int min, int max, QWidget * parent);
    virtual ~CProgressDialog();

    void setValue(int val);

    bool wasCanceled();

public slots:
    void reject();

private:
    QTime time;
};

#endif //CPROGRESSDIALOG_H

