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

#include "ui_IMainWindow.h"
#include <QMainWindow>

class CMapList;
class CDemList;
class QLabel;
class CGisWidget;
class CCanvas;
struct SGisLine;


class CMainWindow : public QMainWindow, private Ui::IMainWindow
{
    Q_OBJECT
public:
    static CMainWindow& self()
    {
        return *pSelf;
    }

    static QWidget * getBestWidgetForParent();

    static QString getUser();

    virtual ~CMainWindow();

    void addMapList(CMapList *list, const QString& name);
    void addDemList(CDemList *list, const QString& name);
    void addWidgetToTab(QWidget * w);

    bool isScaleVisible()  const;
    bool isGridVisible()   const;
    bool isNight()         const;
    bool isPOIText()       const;
    bool isMapToolTip()    const;
    bool flipMouseWheel()  const;
    bool profileIsWindow() const;
    const QFont& getMapFont() const
    {
        return mapFont;
    }

    void zoomCanvasTo(const QRectF rect);
    /**
       @brief Read the elevation from DEM data attached to the currently visible canvas for a given location
       @param pos   a position in units of [rad]
       @return If no elevation value can be found for the position NOFLOAT is returned.
     */
    qreal getElevationAt(const QPointF &pos) const;
    void  getElevationAt(const QPolygonF& pos, QPolygonF &ele) const;
    void  getElevationAt(SGisLine &line) const;
    /**
       @brief Get pointer to the currently visible canvas object.
       @return If the currently visible tab does not contain a CCanvas object 0 is returned.
     */
    CCanvas* getVisibleCanvas() const;

    const qint32 id;
protected:
#ifdef WIN32
    bool CMainWindow::nativeEvent(const QByteArray & eventType, void * message, long * result);
#endif // WIN32
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;


private slots:
    void slotAbout();
    void slotHelp();
    void slotAddCanvas();
    void slotCloneCanvas();
    void slotTabCloseRequest(int i);
    void slotCurrentTabCanvas(int i);
    void slotCurrentTabMaps(int i);
    void slotCurrentTabDem(int i);
    void slotMousePosition(const QPointF& pos, qreal ele);
    void slotUpdateCurrentWidget();
    void slotSetupMapFont();
    void slotSetupGrid();
    void slotSetupMapPath();
    void slotSetupDemPath();
    void slotSetupMapView();
    void slotSetupTimeZone();
    void slotSetupUnits();
    void slotSetupWorkspace();
    void slotSetupCoordFormat();
    void slotImportDatabase();
    void slotLoadGISData();
    void slotBuildVrt();
    void slotStoreView();
    void slotLoadView();
    void slotSetProfileMode(bool on);
    void slotCreateRoutinoDatabase();
    void slotPrintMap();
    void slotPreferences();

private:
    friend int main(int argc, char ** argv);
    CMainWindow();
    void loadGISData(const QStringList& filenames);
    void prepareMenuForMac();

    static CMainWindow * pSelf;

    /// status bar label
    QLabel * lblPosWGS84;
    QLabel * lblElevation;
    QLabel * lblPosGrid;

    QFont mapFont;

    CGisWidget * gisWidget;
};

#endif //CMAINWINDOW_H

