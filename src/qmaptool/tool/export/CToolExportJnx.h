/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CTOOLEXPORTJNX_H
#define CTOOLEXPORTJNX_H

#include "ui_IToolExportJnx.h"

class CToolExportJnx : public QWidget, private Ui::IToolExportJnx
{
    Q_OBJECT
public:
    CToolExportJnx();
    virtual ~CToolExportJnx() = default;

    QString getProductName() const
    {
        return lineProductName->text();
    }

    QString getDescription() const
    {
        return lineDescription->text();
    }

    QString getCopyright() const
    {
        return lineCopyrightNotice->text();
    }

    QString getProductId() const
    {
        return QString::number(spinProductId->value());
    }

    QString getJpegQuality() const
    {
        return QString::number(spinJpegQuality->value());
    }

    QString getJpegSubsampling() const
    {
        return comboJpegSubSampling->currentText();
    }

    QString getZOrder() const
    {
        return QString::number(spinZOrder->value());
    }
};

#endif //CTOOLEXPORTJNX_H

