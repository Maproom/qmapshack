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

#ifndef CSEARCHGOOGLE_H
#define CSEARCHGOOGLE_H

#include <gis/IGisProject.h>

#include <QObject>
#include <QNetworkAccessManager>

class CGisListWks;
class QLineEdit;

class CSearchGoogle : public QObject, public IGisProject
{
    Q_OBJECT
    public:
        CSearchGoogle(CGisListWks * parent);
        virtual ~CSearchGoogle();

    private slots:
        void slotChangeSymbol();
        void slotStartSearch();
        void slotRequestFinished(QNetworkReply* reply);

    private:
        QLineEdit * edit;

        QAction * actSymbol;

        QNetworkAccessManager networkAccessManager;
};

#endif //CSEARCHGOOGLE_H

