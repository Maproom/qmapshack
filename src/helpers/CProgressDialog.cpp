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

#include "helpers/CProgressDialog.h"
#include "units/IUnit.h"

#include <QtWidgets>

QStack<CProgressDialog*> CProgressDialog::stackSelf;

CProgressDialog::CProgressDialog(const QString text, int min, int max, QWidget *parent)
    : QDialog(parent)
{
    stackSelf.push(this);

    setupUi(this);
    setWindowModality(Qt::WindowModal);

    label->setText(text);
    progressBar->setMinimum(min);
    progressBar->setMaximum(max);
    progressBar->setValue(0);

    time.start();
    labelTime->setText(tr("Elapsed time: %1").arg(time.elapsed()/1000));

    if(max == NOINT)
    {
        progressBar->hide();
        // add a timer to update the elapsed time
        QTimer * timer = new QTimer(this);
        timer->start(1000);
        connect(timer, &QTimer::timeout, this, [this]{setValue(0);});
    }
    hide();
    QTimer::singleShot(1000, this, SLOT(show()));
}

CProgressDialog * CProgressDialog::self()
{
    if(stackSelf.isEmpty())
    {
        return nullptr;
    }
    return stackSelf.top();
}

CProgressDialog::~CProgressDialog()
{
    stackSelf.pop();
}

void CProgressDialog::enableCancel(bool yes)
{
    buttonBox->setEnabled(yes);
}

void CProgressDialog::reject()
{
    setResult(QMessageBox::Abort);
    emit rejected();
}

void CProgressDialog::setValue(int val)
{
    if(time.elapsed() > 1000)
    {
        QApplication::processEvents();
    }
    progressBar->setValue(val);
    labelTime->setText(tr("Elapsed time: %1 seconds.").arg(time.elapsed()/1000.0, 0,'f',0));
}

bool CProgressDialog::wasCanceled()
{
    return result() == QMessageBox::Abort;
}
