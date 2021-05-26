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

#ifndef CGEOSEARCHWEB_H
#define CGEOSEARCHWEB_H

#include <QObject>

class QMenu;

class CGeoSearchWeb : public QObject
{
    Q_OBJECT
public:
    struct service_t
    {
        service_t(const QString& name, const QString& url, const QString& icon = defaultIcon)
            : name(name)
            , url(url)
            , icon(icon)
        {
        }

        QString name;
        QString url;
        QString icon;
    };


    static CGeoSearchWeb& self()
    {
        return *pSelf;
    }
    virtual ~CGeoSearchWeb();

    static const QString defaultIcon;
    QList<service_t> defaultServices();

    QMenu* getMenu(const QPointF& pt, QWidget* parent, bool execute = false) const;
    void search(const QPointF& pt);

private slots:
    void slotConfigureServices();
    void slotSearchWeb(int serviceId, const QPointF pt) const;

private:
    CGeoSearchWeb(QObject* parent);
    friend class CMainWindow;

    static CGeoSearchWeb* pSelf;

    QList<service_t> services;
};

#endif //CGEOSEARCHWEB_H

