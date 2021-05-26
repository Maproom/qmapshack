/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2015 Christian Eichler <code@christian-eichler.de>

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

#ifndef CFILTERSPLITSEGMENT_H
#define CFILTERSPLITSEGMENT_H

#include "ui_IFilterSplitSegment.h"
#include <QWidget>

class CGisItemTrk;

class CFilterSplitSegment : public QWidget, private Ui::IFilterSplitSegment
{
    Q_OBJECT
public:
    CFilterSplitSegment(CGisItemTrk& trk, QWidget* parent);
    virtual ~CFilterSplitSegment() = default;

private slots:
    void slotApply();

private:
    CGisItemTrk& trk;
};

#endif // CFILTERSPLITSEGMENT_H

