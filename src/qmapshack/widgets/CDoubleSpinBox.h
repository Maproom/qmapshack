/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

#ifndef CDOUBLESPINBOX_H
#define CDOUBLESPINBOX_H

#include <QSpinBox>

class CDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public slots:
    void slotSelectAll()
    {
        selectAll();
    }

signals:
    void valueChangedByStep(double val);

public:
    CDoubleSpinBox(QWidget * parent = nullptr);

    void stepBy(int steps) override;

protected:
    void focusInEvent(QFocusEvent *event) override;
};
#endif // CDOUBLESPINBOX_H
