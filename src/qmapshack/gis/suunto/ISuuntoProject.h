/**********************************************************************************************
    Copyright (C) 2017 Michel Durand <zero@cms123.fr>

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

#ifndef ISUUNTOPROJECT_H
#define ISUUNTOPROJECT_H

#include "gis/prj/IGisProject.h"
#include "gis/trk/CGisItemTrk.h"
#include <functional>

using fTrkPtSetVal = std::function<void(CTrackData::trkpt_t&, qreal)>;

#define NIL ///< this is to silence the MSVC compiler
#define ASSIGN_VALUE(var, op) \
    [](CTrackData::trkpt_t &pt, qreal val) \
    { \
        if(val != NOFLOAT) \
        { \
            pt.var = op(val); \
        } \
    } \

struct extension_t
{
    /// the tag as used in the xml file
    QString tag;
    /// a scale factor to be applied to the value stored in the xml file
    qreal scale;
    /// an offset to be applied to the value stored in the xml file
    qreal offset;
    /// an assignment function that assigns a value to a member of a trkpt_t object
    fTrkPtSetVal func;
};

class ISuuntoProject : public IGisProject
{
public:
    ISuuntoProject(type_e type, const QString &filename, CGisListWks *parent);

    struct sample_t
    {
        QDateTime time; // as UTC timestamp
        QMap<QString, qreal> data;

        qreal& operator[](const QString& key)
        {
            return data[key];
        }

        qreal operator[](const QString& key) const
        {
            return data[key];
        }
    };

    static void fillMissingData(const QString &dataField, QList<sample_t> &samplesList);

    static sample_t mergeSamples(QList<sample_t> samples, QList<extension_t> extensions);
    static void deleteSamplesWithDuplicateTimestamps(QList<sample_t> &samples, QList<extension_t> extensions);

    static void fillTrackPointsFromSamples(QList<sample_t> &samplesList, QList<QDateTime> &lapsList, CTrackData &trk, QList<extension_t> extensions);
};


#endif //CSUUNTOPROJECT_H

