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

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include "ui_IMainWindow.h"

class QListWidget;
class QLabel;

class CMainWindow : public QMainWindow, private Ui::IMainWindow
{
    Q_OBJECT
    public:
        static CMainWindow& self(){return *pSelf;}
        virtual ~CMainWindow();


        void addMapList(QListWidget * list, const QString& name);
        void delMapList(QListWidget * list);

        bool isScaleVisible();
        const QFont& getMapFont(){return mapFont;}

    private slots:
        void slotAddCanvas();
        void slotTabCloseRequest(int i);
        void slotCurrentTabCanvas(int i);
        void slotCurrentTabMaps(int i);
        void slotMousePosition(const QPointF& pos);
        void slotSetupScale();
        void slotSetupMapFont();

    private:
        friend int main(int argc, char ** argv);
        CMainWindow();

        static CMainWindow * pSelf;

        QLabel * lblPosition;

        QFont mapFont;
};

#endif //CMAINWINDOW_H

