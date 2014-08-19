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

#ifndef CPLOT_H
#define CPLOT_H

#include <QWidget>

class CPlot : public QWidget
{
    public:
        enum mode_e {eModeNormal, eModeIcon};

        CPlot(mode_e mode, QWidget * parent);
        virtual ~CPlot();

    protected:
        void paintEvent(QPaintEvent * e);
        void resizeEvent(QResizeEvent * e);
        void leaveEvent(QEvent * e);
        void enterEvent(QEvent * e);
        void mouseMoveEvent(QMouseEvent * e);

    private:
        void draw(QPainter& p);
        void draw();

        mode_e mode;
        bool needsRedraw;
        bool cursorFocus;
        QImage buffer;
        QPoint posMouse;
};

#endif //CPLOT_H

