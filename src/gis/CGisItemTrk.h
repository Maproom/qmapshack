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

#ifndef CGISITEMTRK_H
#define CGISITEMTRK_H

#include "gis/IGisItem.h"

class QDomNode;
class CGisProject;

class CGisItemTrk : public IGisItem
{
    public:
        CGisItemTrk(const QDomNode &xml, CGisProject *parent);
        virtual ~CGisItemTrk();

    private:
        typedef wpt_t trkpt_t;

        struct trkseg_t
        {
            QVector<trkpt_t> trkpts;
            QMap<QString, QVariant> extensions;
        };


        QString key;
        // -- all gpx tags - start
        QString name;
        QString cmt;
        QString desc;
        QString src;
        QStringList links;
        quint64 number;
        QString type;
        QVector<trkseg_t> trksegs;
        // -- all gpx tags - stop
        QMap<QString, QVariant> extensions;
};

#endif //CGISITEMTRK_H

