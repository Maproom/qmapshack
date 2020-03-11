/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler oliver.eichler@gmx.de

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

#include "ui_IMainWindow.h"
#include <QMainWindow>

class CToolBox;
class CToolAddOverview;
class CToolCutMap;
class CToolRefMap;
class CToolPalettize;
class CCanvas;
class CToolGrid;
class CItemRefMap;
class CToolExport;
class CHelp;

class CMainWindow : public QMainWindow, private Ui::IMainWindow
{
    Q_OBJECT
public:
    static CMainWindow& self()
    {
        return *pSelf;
    }

    virtual ~CMainWindow();

    static QString getUser();

    CCanvas * getCanvas() const
    {
        return canvas;
    }

    const QFont& getMapFont() const
    {
        return mapFont;
    }

    bool flipMouseWheel() const
    {
        return actionFlipMouseWheel->isChecked();
    }

    QAction * showToolHelp() const
    {
        return actionShowToolHelp;
    }

    void makeShellVisible();

    void startGridTool(CItemRefMap * item);
    void showToolBox();

private slots:
    void slotAbout();
    void slotSetupExtTools();
    void slotSetupUnits();
    void slotSetupCoordFormat();
    void slotSetupChanged();
    void slotHelp();

private:
    friend int main(int argc, char ** argv);
    CMainWindow();
    static CMainWindow * pSelf;

    void prepareMenuForMac();

    QFont mapFont;

    CToolBox  * toolBox;
    CToolGrid * toolGrid;

    CToolAddOverview * toolAddOverview;
    CToolCutMap * toolCutMap;
    CToolRefMap * toolRefMap;
    CToolPalettize * toolPalettize;
    CToolExport * toolExport;

    QPointer<CHelp> help;
};

#endif //CMAINWINDOW_H

