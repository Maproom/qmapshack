/**********************************************************************************************
    Copyright (C) 2017 Michel Durand zero@cms123.fr

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

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/suunto/ISuuntoProject.h"

using fTrkPtSetVal = std::function<void(CTrackData::trkpt_t&, qreal)>;

struct extension_t
{
    /// the tag as used in the SML/LOG file
    QString tag;
    /// a scale factor to be applied to the value stored in the SML/LOG file
    qreal scale;
    /// an offset to be applied to the value stored in the SML/LOG file
    qreal offset;
    /// an assignment function that assigns a value to a member of a trkpt_t object
    fTrkPtSetVal func;
};
#define NIL ///< this is to silence the MSVC compiler
#define ASSIGN_VALUE(var, op) \
    [](CTrackData::trkpt_t &pt, qreal val) \
    { \
        if(val != NOFLOAT) \
        { \
            pt.var = op(val); \
        } \
    } \

static const QList<extension_t> extensions =
{
    {"Latitude",            RAD_TO_DEG,     0.0,        ASSIGN_VALUE(lat,NIL)}  // unit [°]
    ,{"Longitude",          RAD_TO_DEG,     0.0,        ASSIGN_VALUE(lon,NIL)}  // unit [°]
    ,{"Altitude",           1.0,            0.0,        ASSIGN_VALUE(ele,NIL)}  // unit [m]
    ,{"VerticalSpeed",      1.0,            0.0,        ASSIGN_VALUE(extensions["gpxdata:verticalSpeed"],NIL)}                  // unit [m/h]
    ,{"HR",                 60.0,           0.0,        ASSIGN_VALUE(extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"],qRound)}   // unit [bpm]
    ,{"Cadence",            60.0,           0.0,        ASSIGN_VALUE(extensions["gpxdata:cadence"],NIL)}                        // unit [bpm]
    ,{"Temperature",        1.0,            -273.15,    ASSIGN_VALUE(extensions["gpxdata:temp"],NIL)}                           // unit [°C]
    ,{"SeaLevelPressure",   0.01,           0.0,        ASSIGN_VALUE(extensions["gpxdata:seaLevelPressure"],NIL)}               // unit [hPa]
    ,{"Speed",              1.0,            0.0,        ASSIGN_VALUE(extensions["gpxdata:speed"],NIL)}                          // unit [m/s]
    ,{"EnergyConsumption",  60.0 / 4184.0,  0.0,        ASSIGN_VALUE(extensions["gpxdata:energy"],NIL)}                         // unit [kCal/min]
};


ISuuntoProject::ISuuntoProject(type_e type, const QString &filename, CGisListWks *parent)
    : IGisProject(type, filename, parent)
{
}

void ISuuntoProject::fillMissingData(const QString &dataField, QList<suunto_sample_t> &samplesList)
{   // Suunto samples contain lat/lon OR heart rate, elevation, etc.., each one with its own timestamp.
    // The purpose of the code below is to "spread" data among samples.
    // At the end each sample contains data, linearly interpolated from its neighbors according to timestamps.
    QList<suunto_sample_t> collect;
    QList<suunto_sample_t> result;
    suunto_sample_t previousSampleWithData;

    for(suunto_sample_t& sample : samplesList)
    {
        collect << sample;

        if (sample.data.contains(dataField))
        {
            if (!previousSampleWithData.data.contains(dataField))
            {   // case where, at the beginning, first samples have no data
                for(suunto_sample_t& collectedSample : collect)
                {
                    collectedSample[dataField] = sample[dataField];
                }
            }
            else
            {   // case where linear interpolation can be applied
                qreal dT = ((qreal)(collect.last().time.toMSecsSinceEpoch() - previousSampleWithData.time.toMSecsSinceEpoch())) / 1000.0;
                if (dT != 0) // dT == 0 when samples have the same timestamps ; this is managed later in deleteSamplesWithDuplicateTimestamps function
                {
                    qreal dY = collect.last().data[dataField] - previousSampleWithData.data[dataField];
                    qreal slope = dY / dT;

                    for(suunto_sample_t& collectedSample : collect)
                    {   // apply interpolation to collected samples
                        collectedSample[dataField] = previousSampleWithData.data[dataField] + slope * ( (qreal)(collectedSample.time.toMSecsSinceEpoch() - previousSampleWithData.time.toMSecsSinceEpoch()) / 1000.0 );
                    }
                }
            }

            previousSampleWithData = sample;
            result << collect;
            collect.clear();
        }
    }

    if (previousSampleWithData.data.contains(dataField))
    {
        for(suunto_sample_t& collectedSample : collect)
        {   // processing last remaining collected samples without data
            collectedSample[dataField] = previousSampleWithData[dataField];
        }
    }

    result << collect;
    samplesList = result;
}


void ISuuntoProject::deleteSamplesWithDuplicateTimestamps(QList<suunto_sample_t> &samples)
{
    QList<suunto_sample_t> result;
    QList<suunto_sample_t> collect;


    for(suunto_sample_t& sample : samples)
    {
        if(!collect.isEmpty())
        {
            if(sample.time != collect.first().time)
            {
                result << mergeSamples(collect);
                collect.clear();
            }
        }
        collect << sample;
    }

    result << mergeSamples(collect);
    samples = result;
}


ISuuntoProject::suunto_sample_t ISuuntoProject::mergeSamples(QList<suunto_sample_t> samples)
{
    if(samples.count() == 1)
    {
        return samples.first();
    }

    suunto_sample_t result;

    result.time = samples.first().time;

    for (const extension_t& ext  : extensions)
    {
        qreal sum = 0;
        qint32 cnt = 0;

        for(const suunto_sample_t& sample : samples)
        {
            if(sample.data.contains(ext.tag))
            {
                sum += sample[ext.tag];
                cnt++;
            }
        }

        if(cnt != 0)
        {
            result[ext.tag] = sum/cnt; // averaged value is assigned to the merged sample
        }
    }

    return result;
}
