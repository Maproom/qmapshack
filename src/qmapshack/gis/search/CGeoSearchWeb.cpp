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

#include "CMainWindow.h"
#include "gis/search/CGeoSearchWeb.h"
#include "gis/search/CGeoSearchWebConfigDialog.h"

#include <QtWidgets>

CGeoSearchWeb * CGeoSearchWeb::pSelf = nullptr;

CGeoSearchWeb::CGeoSearchWeb(QObject * parent)
    : QObject(parent)
{
    pSelf = this;

    services << service_t(tr("PeakFinder"), "https://www.peakfinder.org/?lat=%2&lng=%1&ele=%3&azi=0&zoom=5");
    services << service_t(tr("Waymarked Trails Hiking"), "https://hiking.waymarkedtrails.org/#routelist?map=13!%2!%1");
    services << service_t(tr("Wikiloc"), "https://www.wikiloc.com/wikiloc/map.do?lt=%2&ln=%1&z=13");
    services << service_t(tr("Wikiloc Skitours"), "https://www.wikiloc.com/wikiloc/map.do?lt=%2&ln=%1&z=13&act=40,17");
    services << service_t(tr("Webcam"), "https://webcams.travel/map/#lat=%2&lng=%1&z=12");
}


QMenu * CGeoSearchWeb::getMenu(QObject * obj, const char* slot, QMenu * parent) const
{
    QMenu * menu = new QMenu(tr("Search Web for Position"), parent);
    menu->setIcon(QIcon("://icons/32x32/SearchWeb.png"));

    int serviceId = 0;
    for(const service_t& service : services)
    {
        QAction * action = menu->addAction(service.icon, service.name);
        action->setProperty("ServiceID", serviceId++);
        connect(action, SIGNAL(triggered(bool)), obj, slot);
    }
    menu->addSeparator();
    menu->addAction(QIcon("://icons/32x32/Apply.png"),tr("Configure Services"), this, &CGeoSearchWeb::slotConfigureServices);

    return menu;
}


void CGeoSearchWeb::search(const QPointF& pt, int serviceId) const
{
    if((serviceId < 0) || (serviceId >= services.size()))
    {
        return;
    }

    QString url = services[serviceId].url;
    url = url.replace("%1", QString::number(pt.x(), 'g', 8));
    url = url.replace("%2", QString::number(pt.y(), 'g', 8));

    qreal ele = CMainWindow::self().getElevationAt(pt);
    if(ele == NOFLOAT)
    {
        ele = 0;
    }
    url = url.replace("%3", QString::number(ele));

    QDesktopServices::openUrl(QUrl(url));
}

void CGeoSearchWeb::slotConfigureServices()
{
    CGeoSearchWebConfigDialog dlg(CMainWindow::self().getBestWidgetForParent());
    dlg.exec();
}
