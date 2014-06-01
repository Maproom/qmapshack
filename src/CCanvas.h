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

#ifndef CCANVAS_H
#define CCANVAS_H

#include <QWidget>
class CMap;
class QSettings;

class CCanvas : public QWidget
{
    Q_OBJECT
    public:
        CCanvas(QWidget * parent);
        virtual ~CCanvas();

        void saveConfig(QSettings& cfg);
        void loadConfig(QSettings& cfg);

    protected:
        void resizeEvent(QResizeEvent * e);
        void paintEvent(QPaintEvent * e);

    private:      
        /// set true to initiate a complete redraw of the screen content
        bool needsRedraw;
        /// the map object attached to this canvas
        CMap * map;

};

#endif //CCANVAS_H

