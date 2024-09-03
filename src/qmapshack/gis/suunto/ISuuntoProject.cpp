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

#include "gis/suunto/ISuuntoProject.h"

#include "gis/CGisListWks.h"

ISuuntoProject::ISuuntoProject(type_e type, const QString& filename, CGisListWks* parent)
    : IGisProject(type, filename, parent) {}

void ISuuntoProject::fillTrackPointsFromSamples(QList<sample_t>& samplesList, QList<QDateTime>& lapsList,
                                                CTrackData& trk, const QList<extension_t>& extensions) {
  for (const extension_t& ext : extensions) {
    fillMissingData(ext.tag, samplesList);
  }

  deleteSamplesWithDuplicateTimestamps(samplesList, extensions);

  lapsList << samplesList.last().time.addSecs(
      1);  // a last dummy lap button push is added with timestamp = 1 s later than the last sample timestamp

  trk.segs.resize(lapsList.size());  // segments are created and each of them contains 1 lap

  int lap = 0;
  CTrackData::trkseg_t* seg = &(trk.segs[lap]);

  for (const sample_t& sample : std::as_const(samplesList)) {
    if (sample.time > lapsList[lap]) {
      lap++;
      seg = &(trk.segs[lap]);
    }

    CTrackData::trkpt_t trkpt;
    trkpt.time = sample.time;

    for (const extension_t& ext : extensions) {
      if (sample.data.contains(ext.tag)) {
        ext.func(trkpt, sample[ext.tag]);
      }
    }

    seg->pts.append(trkpt);
  }
}

void ISuuntoProject::fillMissingData(
    const QString& dataField, QList<sample_t>& samplesList) {  // Suunto samples contain lat/lon OR heart rate,
                                                               // elevation, etc.., each one with its own timestamp.
  // The purpose of the code below is to "spread" data among samples.
  // At the end each sample contains data, linearly interpolated from its neighbors according to timestamps.
  QList<sample_t> collect;
  QList<sample_t> result;
  sample_t previousSampleWithData;

  for (sample_t& sample : samplesList) {
    collect << sample;

    if (sample.data.contains(dataField)) {
      if (!previousSampleWithData.data.contains(
              dataField)) {  // case where, at the beginning, first samples have no data
        for (sample_t& collectedSample : collect) {
          collectedSample[dataField] = sample[dataField];
        }
      } else {  // case where linear interpolation can be applied
        qreal dT =
            ((qreal)(collect.last().time.toMSecsSinceEpoch() - previousSampleWithData.time.toMSecsSinceEpoch())) /
            1000.0;
        if (dT != 0)  // dT == 0 when samples have the same timestamps ; this is managed later in
                      // deleteSamplesWithDuplicateTimestamps function
        {
          qreal dY = collect.last().data[dataField] - previousSampleWithData.data[dataField];
          qreal slope = dY / dT;

          for (sample_t& collectedSample : collect) {  // apply interpolation to collected samples
            collectedSample[dataField] = previousSampleWithData.data[dataField] +
                                         slope * ((qreal)(collectedSample.time.toMSecsSinceEpoch() -
                                                          previousSampleWithData.time.toMSecsSinceEpoch()) /
                                                  1000.0);
          }
        }
      }

      previousSampleWithData = sample;
      result << collect;
      collect.clear();
    }
  }

  if (previousSampleWithData.data.contains(dataField)) {
    for (sample_t& collectedSample : collect) {  // processing last remaining collected samples without data
      collectedSample[dataField] = previousSampleWithData[dataField];
    }
  }

  result << collect;
  samplesList = result;
}

void ISuuntoProject::deleteSamplesWithDuplicateTimestamps(QList<sample_t>& samples, QList<extension_t> extensions) {
  QList<sample_t> result;
  QList<sample_t> collect;

  for (sample_t& sample : samples) {
    if (!collect.isEmpty()) {
      if (sample.time != collect.first().time) {
        result << mergeSamples(collect, extensions);
        collect.clear();
      }
    }
    collect << sample;
  }

  result << mergeSamples(collect, extensions);
  samples = result;
}

ISuuntoProject::sample_t ISuuntoProject::mergeSamples(QList<sample_t> samples, QList<extension_t> extensions) {
  if (samples.count() == 1) {
    return samples.first();
  }

  sample_t result;

  result.time = samples.first().time;

  for (const extension_t& ext : extensions) {
    qreal sum = 0;
    qint32 cnt = 0;

    for (const sample_t& sample : samples) {
      if (sample.data.contains(ext.tag)) {
        sum += sample[ext.tag];
        cnt++;
      }
    }

    if (cnt != 0) {
      result[ext.tag] = sum / cnt;  // averaged value is assigned to the merged sample
    }
  }

  return result;
}
