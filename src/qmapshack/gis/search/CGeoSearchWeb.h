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

#ifndef CGEOSEARCHWEB_H
#define CGEOSEARCHWEB_H

#include <QObject>

class CGeoSearchWeb : public QObject
{
public:    
    static CGeoSearchWeb& self()
    {
        return *pSelf;
    }
    virtual ~CGeoSearchWeb() = default;

private:
    friend class CMainWindow;
    CGeoSearchWeb(QObject *parent);
    static CGeoSearchWeb * pSelf;

    struct service_t
    {
        service_t(const QString& name, const QString& url)
            : name(name)
            , url(url)
        {

        }

        QString name;
        QString url;
    };

    QList<service_t> services;
};

#endif //CGEOSEARCHWEB_H

