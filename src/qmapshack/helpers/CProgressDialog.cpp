/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "helpers/CProgressDialog.h"
#include "units/IUnit.h"

#include <QtWidgets>

QStack<CProgressDialog*> CProgressDialog::stackSelf;

#define DELAY 1000

CProgressDialog::CProgressDialog(const QString text, int min, int max, QWidget* parent)
    : QDialog(parent)
{
    stackSelf.push(this);
    int oldTopIndex = stackSelf.length() - 2;

    setupUi(this);
    setWindowModality(Qt::ApplicationModal);

    label->setText(text);
    progressBar->setMinimum(min);
    progressBar->setMaximum(max);
    progressBar->setValue(0);

    time.start();
    labelTime->setText(tr("Elapsed time: %1").arg(time.elapsed() / 1000));

    if(max == NOINT)
    {
        progressBar->hide();
        // add a timer to update the elapsed time
        QTimer* t = new QTimer(this);
        t->start(DELAY);
        connect(t, &QTimer::timeout, this, [this] {
            setValue(0);
        });
    }

    QDialog::hide();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this, oldTopIndex] {
        if(oldTopIndex > 0)
        {
            stackSelf[oldTopIndex]->pause();
        }
        show();
    });
    timer->start(DELAY);
}

CProgressDialog* CProgressDialog::self()
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
    if(!stackSelf.isEmpty())
    {
        stackSelf.top()->goOn();
    }
}

void CProgressDialog::pause()
{
    hide();
    timer->stop();
    timeElapsed = time.elapsed();
}

void CProgressDialog::goOn()
{
    //if goOn() is called from the dtor of another progress bar this progress bar was not necessarily paused.
    //Also, it the second of waiting might not have elapsed. Thus, if the timer is still running, we do nothing
    if(timer->isActive())
    {
        return;
    }
    if(timeElapsed <= DELAY)
    {
        timer->start(DELAY - timeElapsed);
    }
    else
    {
        show();
    }
}


void CProgressDialog::setAllVisible(bool yes)
{
    if(stackSelf.isEmpty())
    {
        return;
    }

    yes ? stackSelf.top()->goOn() : stackSelf.top()->pause();
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
    if(time.elapsed() > DELAY)
    {
        QApplication::processEvents();
    }
    progressBar->setValue(val);
    labelTime->setText(tr("Elapsed time: %1 seconds.").arg(time.elapsed() / 1000.0, 0, 'f', 0));
}

bool CProgressDialog::wasCanceled()
{
    return result() == QMessageBox::Abort;
}

void CProgressDialog::showEvent(QShowEvent*)
{
    // that is a workaround for canvas loosing mousetracking caused by CProgressDialog being modal:
    CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
    if (canvas != nullptr)
    {
        canvas->mouseTrackingLost();
    }
}
