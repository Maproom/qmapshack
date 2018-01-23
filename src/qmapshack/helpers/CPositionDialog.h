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

#ifndef CPOSITIONDIALOG_H
#define CPOSITIONDIALOG_H

#include "ui_IPositionDialog.h"
#include <QDialog>

class QPointF;

class CPositionDialog : public QDialog, private Ui::IPositionDialog
{
    Q_OBJECT
public:
    CPositionDialog(QWidget * parent, QPointF &pos);
    virtual ~CPositionDialog();

    static bool getPosition(QPointF& pt, const QString &str);

public slots:
    void accept() override;

private slots:
    void slotEdit(const QString& str);

private:
    QPointF& pos;
};

#endif //CPOSITIONDIALOG_H

