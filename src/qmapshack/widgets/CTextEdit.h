/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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

#ifndef CTEXTEDIT_H
#define CTEXTEDIT_H

#include <QTextEdit>

class CTextEdit : public QTextEdit
{
    Q_OBJECT

public slots:
    void paste();

public:
    CTextEdit(QWidget *parent = nullptr);

    void setPastePlain(bool plain);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    bool pastePlain;
};
#endif // CTEXTEDIT_H
