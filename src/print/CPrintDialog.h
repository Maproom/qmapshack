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

#ifndef CPRINTDIALOG_H
#define CPRINTDIALOG_H

#include "ui_IPrintDialog.h"
#include <QDialog>
#include <QPrinter>

class CCanvas;

class CPrintDialog : public QDialog, private Ui::IPrintDialog
{
    Q_OBJECT
public:
    enum type_e
    {
        eTypePrint
        ,eTypeImage
    };

    CPrintDialog(type_e type, const QRectF &area, CCanvas * source);
    virtual ~CPrintDialog();


protected:
    void resizeEvent(QResizeEvent * e);

private slots:
    void slotGetPrinter();
    void slotUpdateMetrics();
    void slotPrint();
    void slotSave();

private:
    void updateMetrics();

    type_e type;

    CCanvas * canvas;

    /// the selected area in coordinated of lon/lat
    QRectF rectSelArea;
    /// the selected area in coordinated of pixel
    QRectF rectSelAreaPixel;
    /// the page rectangle in pixel
    QRectF rectPrinterPage;

    QPrinter printer;

    /// number of pages in x dimension
    qreal xPages;
    /// number of pages in y dimension
    qreal yPages;
};

#endif //CPRINTDIALOG_H

