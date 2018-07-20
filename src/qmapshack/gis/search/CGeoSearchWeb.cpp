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

#include <QtWidgets>

CGeoSearchWeb * CGeoSearchWeb::pSelf = nullptr;

CGeoSearchWeb::CGeoSearchWeb(QObject * parent)
    : QObject(parent)
{
    pSelf = this;

    //services << service_t(tr("PeakFinder"), "https://www.peakfinder.org/?lat=%3&amp;lng=%2&amp;off=20&amp;azi=0&amp;zoom=4");
    services << service_t(tr("PeakFinder"), "https://www.peakfinder.org/?lat=%3&lng=%2&ele=%1&azi=0&zoom=5");
    services << service_t(tr("Waymarked Trails Hiking"), "https://hiking.waymarkedtrails.org/#routelist?map=13!%3!%2");
}


QMenu * CGeoSearchWeb::getMenu(QObject * obj, const char* slot, QMenu * parent) const
{
    QMenu * menu = new QMenu(tr("Search Web"), parent);
    menu->setIcon(QIcon("://icons/32x32/SearchWeb.png"));

    int idx = 0;
    for(const service_t& service : services)
    {
        QAction * action = menu->addAction(service.name);
        action->setProperty("ID", idx++);
        connect(action, SIGNAL(triggered(bool)), obj, slot);
    }

    return menu;
}


void CGeoSearchWeb::open(const QPointF& pt, int idx) const
{
    if((idx < 0) || (idx >= services.size()))
    {
        return;
    }

    QString url = services[idx].url;
    url = url.replace("%2", QString::number(pt.x(), 'g', 8));
    url = url.replace("%3", QString::number(pt.y(), 'g', 8));

    qreal ele = CMainWindow::self().getElevationAt(pt);
    if(ele == NOFLOAT)
    {
        ele = 0;
    }
    url = url.replace("%1", QString::number(ele));

    qDebug() << url;
    qDebug() << QUrl(url);
    QDesktopServices::openUrl(QUrl(url));
}

