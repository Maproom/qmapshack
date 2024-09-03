/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/trk/CPropertyTrk.h"

#include <QtWidgets>

#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "plot/CPlot.h"
#include "units/IUnit.h"

CPropertyTrk::CPropertyTrk(CGisItemTrk& trk) : trk(trk) { setupData(); }

void CPropertyTrk::setupData() {
  properties.clear();

  property_t propNull{QString(), QString(), QString(), QIcon(), property_t::eAxisDistance,
                      nullptr,   QString(), QString(), 1.0,     nullptr};
  properties << propNull;

  const QStringList& keys = trk.getExistingDataSources();
  for (const QString& key : keys) {
    const CKnownExtension& ext = CKnownExtension::get(key);
    QString nameShortText = (ext.known ? ext.nameShortText : key);
    QString nameLongText = (ext.known ? ext.nameLongText : key);

    property_t property{key,
                        nameShortText,
                        nameLongText,
                        QIcon(ext.icon),
                        property_t::eAxisDistance,
                        [](const CTrackData::trkpt_t& p) { return p.distance; },
                        ext.unit,
                        ext.known ? QString("%1 [%2]").arg(nameShortText, ext.unit) : nameShortText,
                        ext.factor,
                        ext.valueFunc};

    // lame hack for properties off the usual scheme
    if ((key == CKnownExtension::internalProgress) || (key == CKnownExtension::internalSpeedTime)) {
      property.min = 0;
      property.axisType = property_t::eAxisTime;
      property.getX = [](const CTrackData::trkpt_t& p) { return p.time.isValid() ? p.time.toSecsSinceEpoch() : NOFLOAT; };
    }

    if (key == CKnownExtension::internalSpeedDist) {
      property.min = 0;
    }

    if (key == CKnownExtension::internalSpeedTime) {
      property.min = 0;
    }

    if (key == CKnownExtension::internalSlope) {
      qreal val;
      QString unit;
      IUnit::self().slope2unit(0, val, unit);
      property.unit = unit;
      property.yLabel = QString("%1 [%2]").arg(nameShortText, unit);
      property.getY = [](const CTrackData::trkpt_t& p) {
        qreal val;
        QString unit;
        IUnit::self().slope2unit(p.slope1, val, unit);
        return val;
      };
    }

    properties << property;
  }
}

const CPropertyTrk::property_t& CPropertyTrk::propBySource(const QString& source) const {
  for (const property_t& prop : properties) {
    if (prop.key == source) {
      return prop;
    }
  }

  return properties[0];
}

void CPropertyTrk::fillComboBox(QComboBox* box) const {
  box->clear();

  for (const property_t& p : properties) {
    if (p.key == CKnownExtension::internalEle) {
      // skip it as there is a dedicated profile plot
      continue;
    }
    box->addItem(p.icon, p.nameLongText, p.key);
  }
}

void CPropertyTrk::setupPlot(CPlot* plot, const QString& source) const {
  const property_t& p = propBySource(source);
  if (p.nameLongText.isEmpty()) {
    plot->clear();
    return;
  }

  plot->setup(p);
}
