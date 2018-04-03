/**********************************************************************************************
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CGEOSEARCHCONFIG_H
#define CGEOSEARCHCONFIG_H

#include <QObject>

class QPixmap;

class CGeoSearchConfig : public QObject
{
    Q_OBJECT
public:
    enum service_e
    {
        eServiceNone,
        eServiceGoogle,
        eServiceGeonamesSearch,
        eServiceGeonamesAddress,
        eServiceNominatim
    };

    CGeoSearchConfig(QObject* parent);
    virtual ~CGeoSearchConfig();

    void load();
    void save() const;

    const QIcon getCurrentIcon() const;

signals:
    void sigConfigChanged() const;

private:
    service_e currentService;
    QString googleApiKey;
    QString geonamesUsername;
    QString nominatimEmail;
    QString symbolName;
    int nominatimLimit;

    void emitChanged() const;

    friend class CGeoSearchConfigDialog;
    friend class CGeoSearch;
};

#endif
