/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CRTWORKSPACE_H
#define CRTWORKSPACE_H

#include "ui_IRtWorkspace.h"

class CRtDraw;

class CRtWorkspace : public QWidget, private Ui::IRtWorkspace
{
    Q_OBJECT
public:
    static CRtWorkspace& self()
    {
        return *pSelf;
    }
    virtual ~CRtWorkspace();

    /**
       @brief Draw all loaded data in the workspace that is visible

       This method is called from The CGisDraw thread. The thread has to make sure
       that everything is thread safe.

       @param p         the painter to be used
       @param viewport  the viewport in units of rad
       @param rt       the draw context to be used
     */
    void draw(QPainter& p, const QPolygonF &viewport, CRtDraw *rt);

    /**
       @brief Draw all data that is time variant and can't wait for a full update

       This method is called directly from the main thread's paintEvent() method.

       @param p         the painter to be used
       @param viewport  the viewport in units of rad
       @param rt       the draw context to be used
     */
    void fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt);


signals:
    void sigChanged();

private slots:
    void slotItemChanged(QTreeWidgetItem * item, int column);

private:
    friend class CMainWindow;
    CRtWorkspace(QWidget * parent);

    static CRtWorkspace * pSelf;
};

#endif //CRTWORKSPACE_H

