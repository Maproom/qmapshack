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

#include "CMainWindow.h"
#include "CSettings.h"
#include "CCanvas.h"
#include "GeoMath.h"
#include "map/CMap.h"
#include "map/CMapList.h"
#include "units/IUnit.h"
#include "version.h"

#include <QtGui>
#include <QtWidgets>

CMainWindow * CMainWindow::pSelf = 0;

CMainWindow::CMainWindow()
{
    pSelf = this;
    qDebug() << WHAT_STR;
    setupUi(this);
    setWindowTitle(WHAT_STR);

    IUnit::self().setUnitType(IUnit::eTypeMetric, this);

    SETTINGS;
    // start ---- restore window geometry -----
    if ( cfg.contains("MainWindow/geometry"))
    {
        restoreGeometry(cfg.value("MainWindow/geometry").toByteArray());
    }
    else
    {
        setGeometry(0,0,800,600);
    }

    if ( cfg.contains("MainWindow/state"))
    {
        restoreState(cfg.value("MainWindow/state").toByteArray());
    }
    // end ---- restore window geometry -----


    connect(actionAddCanvas, SIGNAL(triggered()), this, SLOT(slotAddCanvas()));
    connect(actionShowScale, SIGNAL(changed()), this, SLOT(slotUpdateCurrentWidget()));
    connect(actionShowGrid, SIGNAL(changed()), this, SLOT(slotUpdateCurrentWidget()));
    connect(actionSetupMapFont, SIGNAL(triggered()), this, SLOT(slotSetupMapFont()));
    connect(actionSetupGrid, SIGNAL(triggered()), this, SLOT(slotSetupGrid()));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(slotTabCloseRequest(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabCanvas(int)));
    connect(tabMaps, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabMaps(int)));

    cfg.beginGroup("Canvas");
    int N = cfg.value("numberOfCanvas").toInt();
    for(int i = 0; i < N; i++)
    {
        CCanvas * canvas = new CCanvas(tabWidget);
        tabWidget->addTab(canvas, canvas->objectName());
        connect(canvas, SIGNAL(sigMousePosition(QPointF)), this, SLOT(slotMousePosition(QPointF)));

        cfg.beginGroup(QString("Canvas%1").arg(i));
        canvas->loadConfig(cfg);
        cfg.endGroup();
    }

    actionShowScale->setChecked(cfg.value("isScaleVisible", true).toBool());
    actionShowGrid->setChecked(cfg.value("isGridVisible", true).toBool());
    mapFont = cfg.value("mapFont", font()).value<QFont>();
    cfg.endGroup(); // Canvas

    QStatusBar * status = statusBar();
    lblPosWGS84 = new QLabel(status);
    status->addPermanentWidget(lblPosWGS84);
    lblPosGrid = new QLabel(status);
    status->addPermanentWidget(lblPosGrid);

    menuWindow->addAction(dockMaps->toggleViewAction());
}

CMainWindow::~CMainWindow()
{
    int cnt = 0;

    SETTINGS;
    cfg.setValue("MainWindow/state", saveState());
    cfg.setValue("MainWindow/geometry", saveGeometry());

    cfg.beginGroup("Canvas");
    for(int i = 0; i < tabWidget->count(); i++)
    {
        CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->widget(i));
        if(canvas == 0)
        {
            continue;
        }
        cnt++;
        cfg.beginGroup(QString("Canvas%1").arg(i));
        canvas->saveConfig(cfg);
        cfg.endGroup();
    }

    cfg.setValue("numberOfCanvas", cnt);
    cfg.setValue("isScaleVisible", actionShowScale->isChecked());
    cfg.setValue("isGridVisible", actionShowGrid->isChecked());
    cfg.setValue("mapFont", mapFont);

    cfg.endGroup(); // Canvas

}

bool CMainWindow::isScaleVisible()
{
    return actionShowScale->isChecked();
}

bool CMainWindow::isGridVisible()
{
    return actionShowGrid->isChecked();
}

void CMainWindow::addMapList(CMapList * list, const QString &name)
{
    tabMaps->addTab(list,name);
}

void CMainWindow::delMapList(CMapList * list)
{
    for(int i = 0; i < tabMaps->count(); i++)
    {
        QWidget * w = tabMaps->widget(i);
        if(w == list)
        {
            tabMaps->removeTab(i);
            delete w;
            return;
        }
    }
}


void CMainWindow::slotAddCanvas()
{
    CCanvas * canvas = new CCanvas(tabWidget);
    tabWidget->addTab(canvas, canvas->objectName());
    connect(canvas, SIGNAL(sigMousePosition(QPointF)), this, SLOT(slotMousePosition(QPointF)));
}

void CMainWindow::slotTabCloseRequest(int i)
{
    QWidget * w = tabWidget->widget(i);
    tabWidget->removeTab(i);
    delete w;
}

void CMainWindow::slotCurrentTabCanvas(int i)
{
    QString name = tabWidget->tabText(i);
    for(int n = 0; n < tabMaps->count(); n++)
    {
        if(tabMaps->tabText(n) == name)
        {
            tabMaps->setCurrentIndex(n);
            return;
        }
    }
}

void CMainWindow::slotCurrentTabMaps(int i)
{
    QString name = tabMaps->tabText(i);
    for(int n = 0; n < tabWidget->count(); n++)
    {
        if(tabWidget->tabText(n) == name)
        {
            tabWidget->setCurrentIndex(n);
            return;
        }
    }
}

void CMainWindow::slotMousePosition(const QPointF& pos)
{
    QString str;
    GPS_Math_Deg_To_Str(pos.x(), pos.y(), str);
    lblPosWGS84->setText(str);

    if(actionShowGrid->isChecked())
    {
        CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->currentWidget());
        if(canvas)
        {
            QString str;
            lblPosGrid->show();
            canvas->convertGridPos2Str(pos, str);
            lblPosGrid->setText(str);
        }
    }
    else
    {
        lblPosGrid->hide();
    }
}

void CMainWindow::slotUpdateCurrentWidget()
{
    QWidget * w = tabWidget->currentWidget();
    if(w)
    {
        w->update();
    }
}


void CMainWindow::slotSetupMapFont()
{
    bool ok = false;
    QFont f = QFontDialog::getFont(&ok, mapFont, this);
    if(ok)
    {
        mapFont = f;
        QWidget * w = tabWidget->currentWidget();
        if(w)
        {
            w->update();
        }
    }

}

void CMainWindow::slotSetupGrid()
{
    CCanvas * canvas = dynamic_cast<CCanvas*>(tabWidget->currentWidget());
    if(canvas == 0)
    {
        return;
    }
    canvas->setupGrid();
}
