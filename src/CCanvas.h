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
#include <QPainter>
class CMap;
class QSettings;
class QPointF;
class CGrid;

inline  void USE_ANTI_ALIASING(QPainter& p, bool useAntiAliasing)
{
    p.setRenderHints(QPainter::TextAntialiasing|QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing, useAntiAliasing);
}


class CCanvas : public QWidget
{
    Q_OBJECT
    public:
        CCanvas(QWidget * parent);
        virtual ~CCanvas();

        void saveConfig(QSettings& cfg);
        void loadConfig(QSettings& cfg);

        static void drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color);
        static void drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color, const QFont& font);
        static void drawText(const QString& str, QPainter& p, const QRect& r, const QColor& color);

        void getPositionAsString(const QPointF& pos, QString& str);

    signals:
        void sigMousePosition(const QPointF& pos);

    protected:
        void resizeEvent(QResizeEvent * e);
        void paintEvent(QPaintEvent * e);
        void mousePressEvent(QMouseEvent * e);
        void mouseMoveEvent(QMouseEvent * e);
        void mouseReleaseEvent(QMouseEvent *e);
        void wheelEvent(QWheelEvent * e);

    private slots:
        void slotTriggerCompleteUpdate();

    private:
        void drawScale(QPainter& p);

        /// set true to initiate a complete redraw of the screen content
        bool needsRedraw;
        /// the map object attached to this canvas
        CMap * map;

        CGrid * grid;

};

#endif //CCANVAS_H

