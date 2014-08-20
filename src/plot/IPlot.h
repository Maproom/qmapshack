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

#ifndef IPLOT_H
#define IPLOT_H

#include <QWidget>
#include "plot/CPlotData.h"

class CGisItemTrk;

class IPlot : public QWidget
{
    public:
        enum mode_e {eModeNormal, eModeIcon};

        IPlot(CGisItemTrk * trk, CPlotData::axistype_e type, mode_e mode, QWidget * parent);
        virtual ~IPlot();

    protected:
        void paintEvent(QPaintEvent * e);
        void resizeEvent(QResizeEvent * e);
        void leaveEvent(QEvent * e);
        void enterEvent(QEvent * e);
        void mouseMoveEvent(QMouseEvent * e);


        void draw(QPainter& p);
        void draw();

        // different draw modes
        mode_e mode;
        // buffer needs update
        bool needsRedraw;

        bool showScale;
        bool thinLine;

        QImage buffer;
        QPoint posMouse;

        /**
           @brief The track this plot is attached to

           @note It is save to store the pointer to the track item because
                 the plot objects registers/unregisters with the track during
                 construction and destruction.

                 See CGisItem::registeredPlots for details.
         */
        CGisItemTrk * trk;
        CPlotData * data;

};

#endif //IPLOT_H

