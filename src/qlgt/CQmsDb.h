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

#ifndef CQMSDB_H
#define CQMSDB_H

#include <QObject>
#include <QMap>
#include "gis/db/IDB.h"

class CImportDatabase;
class IGisItem;
class CQlgtWpt;
class CQlgtTrack;

class CQmsDb : public QObject, private IDB
{
    public:
        CQmsDb(const QString& filename, CImportDatabase * parent);
        virtual ~CQmsDb();

        void addWpt(CQlgtWpt& wpt1);
        void addTrk(CQlgtTrack& trk1);

    private:
        quint64 store(IGisItem& item);

        CImportDatabase * gui;

        QMap<quint64, quint64> mapItemIDs;
};

#endif //CQMSDB_H

