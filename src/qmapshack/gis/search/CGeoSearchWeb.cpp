/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "helpers/CSettings.h"

#include <functional>
#include <QtWidgets>


CGeoSearchWeb* CGeoSearchWeb::pSelf = nullptr;
const QString CGeoSearchWeb::defaultIcon = "://icons/32x32/SearchWebDefault.png";

CGeoSearchWeb::CGeoSearchWeb(QObject* parent)
    : QObject(parent)
{
    pSelf = this;

    SETTINGS;
    cfg.beginGroup("Search");

    const qint32 N = cfg.beginReadArray("webServices");
    if(N == 0)
    {
        services = defaultServices();
    }
    else
    {
        for(int n = 0; n < N; ++n)
        {
            cfg.setArrayIndex(n);
            const QString& name = cfg.value("name").toString();
            const QString& url = cfg.value("url").toString();
            const QString& icon = cfg.value("icon").toString();
            services << service_t(name, url, icon);
        }
    }

    cfg.endArray(); // webServices
    cfg.endGroup(); // Search
}

QList<CGeoSearchWeb::service_t> CGeoSearchWeb::defaultServices()
{
    QList<service_t> tmp;
    tmp << service_t("PeakFinder", "https://www.peakfinder.org/?lat=%2&lng=%1&ele=%3&azi=0&zoom=5", "://icons/32x32/SearchWebPeakfinder.png");
    tmp << service_t("Waymarked Trails Hiking", "https://hiking.waymarkedtrails.org/#routelist?map=13!%2!%1", "://icons/32x32/SearchWebWaymarked.png");
    tmp << service_t("Wikiloc", "https://www.wikiloc.com/wikiloc/map.do?lt=%2&ln=%1&z=13", "://icons/32x32/SearchWebOutings.png");
    tmp << service_t("Wikiloc Skitours", "https://www.wikiloc.com/wikiloc/map.do?lt=%2&ln=%1&z=13&act=40,17", "://icons/32x32/SearchWebOutings.png");
    tmp << service_t("Webcam", "https://webcams.travel/map/#lat=%2&lng=%1&z=12", "://icons/32x32/SearchWebCam.png");
    tmp << service_t("Meteoblue 7 days", "https://www.meteoblue.com/en/weather/forecast/week/%2N%1E", "://icons/32x32/SearchWebMeteo.png");
    tmp << service_t("Meteoblue 5 days", "https://www.meteoblue.com/en/weather/forecast/meteogramfive/%2N%1E", "://icons/32x32/SearchWebMeteo.png");
    tmp << service_t("Meteoblue Multi. Mod.", "https://www.meteoblue.com/en/weather/forecast/multimodel/%2N%1E", "://icons/32x32/SearchWebMeteo.png");
    tmp << service_t("Meteoblue Maps", "https://www.meteoblue.com/en/weather/webmap/index/%2N%1E?level=surface&zoom=8", "://icons/32x32/SearchWebMeteo.png");
    tmp << service_t("Ventusky Map", "https://www.ventusky.com/?p=%2;%1;8&l=rain-3h", "://icons/32x32/SearchWebMeteo.png");

    return tmp;
}

CGeoSearchWeb::~CGeoSearchWeb()
{
    SETTINGS;
    cfg.beginGroup("Search");
    cfg.remove("webServices");
    cfg.beginWriteArray("webServices");

    const qint32 N = services.size();
    for(int n = 0; n < N; ++n)
    {
        const service_t& service = services[n];
        cfg.setArrayIndex(n);
        cfg.setValue("name", service.name);
        cfg.setValue("url", service.url);
        cfg.setValue("icon", service.icon);
    }

    cfg.endArray(); // webServices
    cfg.endGroup(); // Search
}

QMenu* CGeoSearchWeb::getMenu(const QPointF& pt, QWidget* parent, bool execute) const
{
    QMenu* menu = new QMenu(tr("Search Web for Position"), parent);
    menu->setIcon(QIcon("://icons/32x32/SearchWeb.png"));

    QAction* action;
    int serviceId = 0;
    for(const service_t& service : services)
    {
        action = menu->addAction(QIcon(service.icon), service.name);
        connect(action, &QAction::triggered, this, [this, serviceId, pt](){slotSearchWeb(serviceId, pt);});
        serviceId++;
    }

    menu->addSeparator();
    action = menu->addAction(QIcon("://icons/32x32/Apply.png"), tr("Configure Services"));
    connect(action, &QAction::triggered, this, &CGeoSearchWeb::slotConfigureServices);

    if(execute)
    {
        menu->exec(QCursor::pos());
        delete menu;
        return nullptr;
    }

    return menu;
}

void CGeoSearchWeb::search(const QPointF& pt)
{
    QMenu* menu = getMenu(pt, CMainWindow::self().getBestWidgetForParent());
    menu->exec(QCursor::pos());
    delete menu;
}

void CGeoSearchWeb::slotSearchWeb(int serviceId, const QPointF pt) const
{
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
    CGeoSearchWebConfigDialog dlg(services, CMainWindow::self().getBestWidgetForParent());
    dlg.exec();
}
