/**********************************************************************************************
    Copyright (C) 2014-2018 Michel Durand zero@cms123.fr

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

#ifndef CCOLORTRK_H
#define CCOLORTRK_H

#include <QCoreApplication>
#include <QIcon>
#include <QMap>
#include <QMenu>
#include <QPointer>
#include <QVector>
//class CGisItemTrk;

class CColorTrk
{
    Q_DECLARE_TR_FUNCTIONS(CColorTrk)
public:
    virtual ~CColorTrk() = default;

    static quint32 selectColor(QWidget *parent);



private:


};

#endif //CCOLORTRK_H

