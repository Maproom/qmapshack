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

#ifndef CWPTICONMANAGER_H
#define CWPTICONMANAGER_H

#include <QMap>
#include <QObject>
#include <QPoint>
#include <QString>

class QMenu;

class CWptIconManager : public QObject
{
public:
    virtual ~CWptIconManager() = default;
    static CWptIconManager& self(){return *pSelf;}

    struct icon_t
    {
        icon_t() : focus(16,16)
        {
        }
        icon_t(const QString& path, int x, int y) : path(path), focus(x,y)
        {
        }
        QString path;
        QPoint focus;
    };


    void init();
    QPixmap getWptIconByName(const QString& name, QPointF &focus, QString * src = nullptr);
    QString selectWptIcon(QWidget * parent);
    QMenu * getWptIconMenu(const QString& title, QObject * obj, const char * slot, QWidget * parent);
    QPixmap loadIcon(const QString& path);

    const QMap<QString, icon_t>& getWptIcons()
    {
        return wptIcons;
    }

private:
    friend class CMainWindow;
    CWptIconManager(QObject * parent);

    void setWptIconByName(const QString& name, const QString& filename);
    void setWptIconByName(const QString& name, const QPixmap& icon);

    static CWptIconManager * pSelf;
    static const char * wptDefault;

    QMap<QString, icon_t> wptIcons;
};

#endif //CWPTICONMANAGER_H

