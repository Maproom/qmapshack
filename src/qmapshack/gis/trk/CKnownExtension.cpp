/**********************************************************************************************
    Copyright (C) 2015-2016 Christian Eichler <code@christian-eichler.de>

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

#include "gis/trk/CKnownExtension.h"

#include <QStringBuilder>

#include "units/IUnit.h"

const QString CKnownExtension::internalSlope = "ql:slope";
const QString CKnownExtension::internalSpeedDist = "ql:speeddist";
const QString CKnownExtension::internalSpeedTime = "ql:speedtime";
const QString CKnownExtension::internalEle = "ql:ele";
const QString CKnownExtension::internalProgress = "ql:progress";
const QString CKnownExtension::internalTerrainSlope = "ql:terrainslope";

QHash<QString, CKnownExtension> CKnownExtension::knownExtensions;
QSet<QString> CKnownExtension::registeredNS;

static const int NOORDER = std::numeric_limits<int>::max();

static fTrkPtGetVal getExtensionValueFunc(const QString ext) {
  return [ext](const CTrackData::trkpt_t& p) {
    bool ok;
    qreal val = p.extensions.value(ext).toReal(&ok);
    return ok ? val : NOFLOAT;
  };
}

bool CKnownExtension::registerNS(const QString& ns) {
  if (!registeredNS.contains(ns)) {
    registeredNS.insert(ns);
    return true;
  }

  return false;
}

void CKnownExtension::initGarminTPXv1(const IUnit& units, const QString& ns) {
  if (!registerNS(ns)) {
    return;
  }

  // support for the Garmin TrackPointExtension v1
  //  https://www8.garmin.com/xmlschemas/TrackPointExtensionv1.xsd
  knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":atemp",
                         {tr("Air Temp.", "extShortName"), tr("Air Temperature", "extLongName"), 0, -100., 100., 1.,
                          "°C", "://icons/32x32/CSrcATemp.png", true, false,
                          getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":atemp")});

  knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":wtemp",
                         {tr("Water Temp.", "extShortName"), tr("Water Temperature", "extLongName"), 1, -100., 100., 1.,
                          "°C", "://icons/32x32/CSrcWTemp.png", true, false,
                          getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":wtemp")});

  knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":depth",
                         {tr("Depth", "extShortName"), tr("Depth", "extLongName"), 2, 0., 12000., units.elevationFactor,
                          units.elevationUnit, "://icons/32x32/CSrcDepth.png", true, false,
                          getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":depth")});

  knownExtensions.insert(
      ns % ":TrackPointExtension|" % ns % ":hr",
      {tr("Heart R.", "extShortName"), tr("Heart Rate", "extLongName"), 3, 0., 300., 1., "bpm",
       "://icons/32x32/CSrcHR.png", true, false, getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":hr")});

  knownExtensions.insert(
      ns % ":TrackPointExtension|" % ns % ":cad",
      {tr("Cadence", "extShortName"), tr("Cadence", "extLongName"), 4, 0., 500., 1., "rpm",
       "://icons/32x32/CSrcCAD.png", true, false, getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":cad")});

  knownExtensions.insert(ns % ":TrackPointExtension|" % ns % ":power",
                         {tr("Power", "extShortName"), tr("Power", "extLongName"), 5, 0., 1500., 1., "Watt",
                          "://icons/32x32/CSrcPower.png", true, false,
                          getExtensionValueFunc(ns % ":TrackPointExtension|" % ns % ":power")});
}

void CKnownExtension::initMioTPX(const IUnit& units) {
  // support for extensions used by MIO Cyclo ver. 4.2 (who needs xml namespaces?!)
  knownExtensions.insert("heartrate",
                         {tr("Heart R.", "extShortName"), tr("Heart Rate", "extLongName"), NOORDER, 0., 300., 1., "bpm",
                          "://icons/32x32/CSrcHR.png", true, false, getExtensionValueFunc("heartrate")});

  knownExtensions.insert("cadence",
                         {tr("Cadence", "extShortName"), tr("Cadence", "extLongName"), NOORDER, 0., 500., 1., "rpm",
                          "://icons/32x32/CSrcCAD.png", true, false, getExtensionValueFunc("cadence")});

  knownExtensions.insert(
      "speed", {tr("Speed", "extShortName"), tr("Speed", "extLongName"), NOORDER, 0., 600., units.speedFactor,
                units.speedUnit, "://icons/32x32/CSrcSpeed.png", true, false, getExtensionValueFunc("speed")});

  knownExtensions.insert("acceleration", {tr("Accel.", "extShortName"), tr("Acceleration", "extLongName"), NOORDER,
                                          std::numeric_limits<qreal>::lowest(), std::numeric_limits<qreal>::max(),
                                          units.baseFactor, units.baseUnit + "/s²", "://icons/32x32/CSrcAccel.png",
                                          true, false, getExtensionValueFunc("acceleration")});

  knownExtensions.insert("course",
                         {tr("Course", "extShortName"), tr("Course", "extLongName"), NOORDER, -3.2, 3.2, 1., "rad",
                          "://icons/32x32/CSrcCourse.png", true, false, getExtensionValueFunc("course")});
}

void CKnownExtension::initClueTrustTPXv1(const IUnit& units, const QString& ns) {
  knownExtensions.insert(ns % ":cadence",
                         {tr("Cadence", "extShortName"), tr("Cadence", "extLongName"), 0, 0., 500., 1., "rpm",
                          "://icons/32x32/CSrcCAD.png", true, false, getExtensionValueFunc(ns % ":cadence")});

  knownExtensions.insert(ns % ":temp",
                         {tr("Temp.", "extShortName"), tr("Temperature", "extLongName"), 1, -100., 100., 1., "°C",
                          "://icons/32x32/CSrcATemp.png", true, false, getExtensionValueFunc(ns % ":temp")});

  knownExtensions.insert(
      ns % ":distance",
      {tr("Dist.", "extShortName"), tr("Distance", "extLongName"), 2, 0., +100000000., units.baseFactor, units.baseUnit,
       "://icons/32x32/CSrcDistance.png", true, false, getExtensionValueFunc(ns % ":distance")});

  knownExtensions.insert(
      ns % ":altitude",
      {tr("Ele.", "extShortName"), tr("Elevation", "extLongName"), 3, -1000., +10000., units.elevationFactor,
       units.elevationUnit, "://icons/32x32/CSrcElevation.png", true, false, getExtensionValueFunc(ns % ":altitude")});

  knownExtensions.insert(ns % ":energy",
                         {tr("Energy", "extShortName"), tr("Energy", "extLongName"), 4, 0., 10000., 1., "kcal/min",
                          "://icons/32x32/CSrcEnergy.png", true, false, getExtensionValueFunc(ns % ":energy")});

  knownExtensions.insert(
      ns % ":seaLevelPressure",
      {tr("Sea Lev. Pres.", "extShortName"), tr("Sea Level Pressure", "extLongName"), 5, 0., 1500., 1., "hPa",
       "://icons/32x32/CSrcSeaLevelPressure.png", true, false, getExtensionValueFunc(ns % ":seaLevelPressure")});

  knownExtensions.insert(ns % ":speed", {tr("Speed", "extShortName"), tr("Speed", "extLongName"), 6, 0., 600.,
                                         units.speedFactor, units.speedUnit, "://icons/32x32/CSrcSpeed.png", true,
                                         false, getExtensionValueFunc(ns % ":speed")});

  knownExtensions.insert(
      ns % ":verticalSpeed",
      {tr("v. Speed", "extShortName"), tr("Vertical Speed", "extLongName"), 7, 0., 50., units.speedFactor,
       units.speedUnit, "://icons/32x32/CSrcVertSpeed.png", true, false, getExtensionValueFunc(ns % ":verticalSpeed")});
}

void CKnownExtension::initGarminFit(const IUnit& units, const QString& ns) {
  knownExtensions.insert(ns % ":speed", {tr("Speed", "extShortName"), tr("Speed (device)", "extLongName"), -1, 0.,
                                         NOFLOAT, units.speedFactor, units.speedUnit, "://icons/32x32/CSrcSpeed.png",
                                         true, false, getExtensionValueFunc(ns % ":speed")});

  knownExtensions.insert(
      ns % ":distance",
      {tr("Distance", "extShortName"), tr("Distance (device)", "extLongName"), -1, 0., NOFLOAT, units.baseFactor,
       units.baseUnit, "://icons/32x32/CSrcDistance.png", true, false, getExtensionValueFunc(ns % ":distance")});

  knownExtensions.insert(
      ns % ":respiration_rate",
      {tr("Resp. Rate", "extShortName"), tr("Respiration Rate", "extLongName"), -1, 0., NOFLOAT, 1.0, "brpm",
       "://icons/32x32/CSrcUnknown.png", true, false, getExtensionValueFunc(ns % ":respiration_rate")});
}

void CKnownExtension::init(const IUnit& units) {
  knownExtensions = {
      {internalSlope,
       {tr("Slope", "extShortName"), tr("Slope*"), -1, -90., 90., 1.,
        (IUnit::getSlopeMode() == IUnit::eSlopePercent) ? "%" : "°", "://icons/32x32/CSrcSlope.png", true, true,
        [](const CTrackData::trkpt_t& p) {
          return (IUnit::getSlopeMode() == IUnit::eSlopePercent) ? p.slope2 : p.slope1;
        }}},

      {internalSpeedDist,
       {tr("Speed", "extShortName"), tr("Speed over Distance*", "extLongName"), -1, 0., 600., units.speedFactor,
        units.speedUnit, "://icons/32x32/CSrcSpeed.png", true, true,
        [](const CTrackData::trkpt_t& p) { return p.speed; }}},

      {internalSpeedTime,
       {tr("Speed", "extShortName"), tr("Speed over Time*", "extLongName"), -1, 0., NOFLOAT, units.speedFactor,
        units.speedUnit, "://icons/32x32/CSrcSpeed.png", true, true,
        [](const CTrackData::trkpt_t& p) { return p.speed; }}},

      {internalEle,
       {tr("Ele.", "extShortName"), tr("Elevation*", "extLongName"), -1, 0., 100000., units.elevationFactor,
        units.elevationUnit, "://icons/32x32/CSrcElevation.png", true, true,
        [](const CTrackData::trkpt_t& p) { return (NOINT == p.ele) ? NOFLOAT : p.ele; }}},

      {internalProgress,
       {tr("Progress", "extShortName"), tr("Progress*", "extLongName"), -1, 0., NOFLOAT, units.baseFactor,
        units.baseUnit, "://icons/32x32/Progress.png", true, true,
        [](const CTrackData::trkpt_t& p) { return p.distance; }}},

      {internalTerrainSlope,
       {tr("Terr. Slope", "extShortName"), tr("Terrain Slope*", "extLongName"), -1, 0, 90., 1., "°",
        "://icons/32x32/CSrcSlope.png", true, false, getExtensionValueFunc(internalTerrainSlope)}}};

  initGarminTPXv1(units, "gpxtpx");
  initGarminTPXv1(units, "tp1");
  initMioTPX(units);
  initClueTrustTPXv1(units, "gpxdata");
  initGarminFit(units, "fit");
}

const CKnownExtension CKnownExtension::get(const QString& key) {
  CKnownExtension def("", "", NOORDER, -100000., 100000., 1., "", "://icons/32x32/CSrcUnknown.png", false, true,
                      getExtensionValueFunc(key));
  return knownExtensions.value(key, def);
}

bool CKnownExtension::isKnown(const QString& key) { return knownExtensions.contains(key); }

QString CKnownExtension::getName(const QString& altName) const {
  bool hasNoName = nameShortText.isEmpty();
  QString name = hasNoName ? altName : nameShortText;

  if (derivedQMS && !hasNoName) {
    name += "*";
  }

  return name;
}

QString CKnownExtension::toString(qreal value, bool withName, const QString& key) const {
  QString str;
  if (key == CKnownExtension::internalProgress) {
    return str;
  } else if (key.contains("speed")) {
    QString v, u;
    IUnit::self().meter2speed(value, v, u);
    str = v + u;
  } else if (key == CKnownExtension::internalEle) {
    QString v, u;
    IUnit::self().meter2elevation(value, v, u);
    str = v + u;
  } else if (key == CKnownExtension::internalSlope) {
    QString v, u;
    IUnit::self().slope2string(value, v, u);
    str = v + u;
  } else {
    str = QString("%1%2").arg(value * factor).arg(unit);
  }

  if (withName) {
    str = getName(key) + " " + str;
  }

  return str;
}
