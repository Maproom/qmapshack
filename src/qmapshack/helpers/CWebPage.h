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

#ifndef CWEBPAGE_H
#define CWEBPAGE_H

#include <QWebEnginePage>

class CWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    CWebPage(QObject* parent)
        : QWebEnginePage(parent)
    {
    }

    virtual ~CWebPage() = default;

    bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool) override
    {
        if (type == QWebEnginePage::NavigationTypeLinkClicked)
        {
            emit linkClicked(url);
            return false;
        }
        return true;
    }

signals:
    void linkClicked(const QUrl&);
};
#endif //CWEBPAGE_H

