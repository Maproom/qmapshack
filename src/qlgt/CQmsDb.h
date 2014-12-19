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
class CQlgtFolder;
class CQlgtWpt;
class CQlgtTrack;
class CQlgtRoute;
class IQlgtOverlay;

class CQmsDb : public QObject, private IDB
{
    public:
        CQmsDb(const QString& filename, CImportDatabase * parent);
        virtual ~CQmsDb();

        void addFolder2FolderRelation(quint64 parent, quint64 child);
        void addFolder2ItemRelation(quint64 parent, quint64 child);

        void addFolder(CQlgtFolder &folder);
        void addWpt(CQlgtWpt &wpt1);
        void addTrk(CQlgtTrack &trk1);
        void addRte(CQlgtRoute& rte1);
        void addArea(IQlgtOverlay& ovl1);

        bool isValid(){return valid;}
    private:
        bool valid;

        quint64 store(IGisItem &item);

        CImportDatabase * gui;

        QMap<int, int> mapFolderTypes;

        QMap<quint64, quint64> mapFolderIDs;
        QMap<quint64, quint64> mapItemIDs;
};

#endif //CQMSDB_H

