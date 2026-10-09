/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#include "gis/trk/surface/CSurface.h"

#include <QHash>

namespace {
struct surface_t {
  CSurface::label_e label;
  CSurface::class_e cls;
};

// OSM surface=* values; anything not listed falls back to the highway's assumption
const QHash<QString, surface_t>& surfaces() {
  static const QHash<QString, surface_t> table = {
      {"asphalt", {CSurface::eLabelAsphalt, CSurface::eClassPaved}},
      {"paved", {CSurface::eLabelPaved, CSurface::eClassPaved}},
      {"chipseal", {CSurface::eLabelAsphalt, CSurface::eClassPaved}},
      {"concrete", {CSurface::eLabelConcrete, CSurface::eClassPaved}},
      {"concrete:lanes", {CSurface::eLabelConcreteLanes, CSurface::eClassPaved}},
      {"concrete:plates", {CSurface::eLabelConcretePlates, CSurface::eClassPaved}},
      {"paving_stones", {CSurface::eLabelPavingStones, CSurface::eClassPaved}},
      {"sett", {CSurface::eLabelCobblestone, CSurface::eClassPaved}},
      {"cobblestone", {CSurface::eLabelCobblestone, CSurface::eClassPaved}},
      {"unhewn_cobblestone", {CSurface::eLabelCobblestone, CSurface::eClassPaved}},
      {"bricks", {CSurface::eLabelPavingStones, CSurface::eClassPaved}},
      {"metal", {CSurface::eLabelMetal, CSurface::eClassPaved}},
      {"wood", {CSurface::eLabelWood, CSurface::eClassPaved}},
      {"compacted", {CSurface::eLabelCompacted, CSurface::eClassSemiPaved}},
      {"fine_gravel", {CSurface::eLabelFineGravel, CSurface::eClassSemiPaved}},
      {"gravel", {CSurface::eLabelGravel, CSurface::eClassSemiPaved}},
      {"pebblestone", {CSurface::eLabelPebblestone, CSurface::eClassSemiPaved}},
      {"grass_paver", {CSurface::eLabelGrassPaver, CSurface::eClassSemiPaved}},
      {"unpaved", {CSurface::eLabelUnpaved, CSurface::eClassUnpaved}},
      {"dirt", {CSurface::eLabelDirt, CSurface::eClassUnpaved}},
      {"earth", {CSurface::eLabelDirt, CSurface::eClassUnpaved}},
      {"ground", {CSurface::eLabelGround, CSurface::eClassUnpaved}},
      {"grass", {CSurface::eLabelGrass, CSurface::eClassUnpaved}},
      {"mud", {CSurface::eLabelMud, CSurface::eClassUnpaved}},
      {"sand", {CSurface::eLabelSand, CSurface::eClassUnpaved}},
      {"woodchips", {CSurface::eLabelWoodchips, CSurface::eClassUnpaved}},
      {"rock", {CSurface::eLabelRock, CSurface::eClassUnpaved}},
      {"stepping_stones", {CSurface::eLabelSteppingStones, CSurface::eClassUnpaved}},
  };
  return table;
}

const QHash<QString, surface_t>& tracktypes() {
  static const QHash<QString, surface_t> table = {
      {"grade1", {CSurface::eLabelPaved, CSurface::eClassPaved}},
      {"grade2", {CSurface::eLabelSemiPaved, CSurface::eClassSemiPaved}},
      {"grade3", {CSurface::eLabelUnpaved, CSurface::eClassUnpaved}},
      {"grade4", {CSurface::eLabelUnpaved, CSurface::eClassUnpaved}},
      {"grade5", {CSurface::eLabelUnpaved, CSurface::eClassUnpaved}},
  };
  return table;
}

const QHash<QString, CSurface::waytype_e>& waytypes() {
  static const QHash<QString, CSurface::waytype_e> table = {
      {"path", CSurface::eWayPath},
      {"footway", CSurface::eWayPath},
      {"bridleway", CSurface::eWayPath},
      {"steps", CSurface::eWaySteps},
      {"track", CSurface::eWayTrack},
      {"cycleway", CSurface::eWayCycleway},
      {"residential", CSurface::eWayStreet},
      {"living_street", CSurface::eWayStreet},
      {"pedestrian", CSurface::eWayStreet},
      {"unclassified", CSurface::eWayRoad},
      {"service", CSurface::eWayRoad},
      {"tertiary", CSurface::eWayRoad},
      {"tertiary_link", CSurface::eWayRoad},
      {"secondary", CSurface::eWayBusyRoad},
      {"secondary_link", CSurface::eWayBusyRoad},
      {"primary", CSurface::eWayBusyRoad},
      {"primary_link", CSurface::eWayBusyRoad},
      {"trunk", CSurface::eWayBusyRoad},
  };
  return table;
}
}  // namespace

CSurface::waytype_e CSurface::wayType(const QString& highway) { return waytypes().value(highway, eWayOther); }

CSurface::info_t CSurface::classify(const tags_t& tags) {
  const waytype_e way = wayType(tags.highway);

  auto surface = surfaces().constFind(tags.surface);
  if (surface != surfaces().constEnd()) {
    return {surface->label, surface->cls, way};
  }

  if (tags.highway == "track") {
    auto tracktype = tracktypes().constFind(tags.tracktype);
    if (tracktype != tracktypes().constEnd()) {
      return {tracktype->label, tracktype->cls, way};
    }
    return {eLabelUnknown, eClassUnknown, way};
  }

  if (tags.highway == "path" || tags.highway == "bridleway" ||
      (tags.highway == "footway" && tags.footway != "sidewalk")) {
    return {eLabelUnpavedAssumed, eClassUnpaved, way};
  }

  // roads, streets, sidewalks: asphalt in practice
  return {eLabelPavedAssumed, eClassPaved, way};
}

QString CSurface::className(class_e cls) {
  switch (cls) {
    case eClassPaved:
      return tr("paved", "surface class");
    case eClassSemiPaved:
      return tr("semi-paved", "surface class");
    case eClassUnpaved:
      return tr("unpaved", "surface class");
    default:
      return tr("unknown", "surface class");
  }
}

QString CSurface::wayTypeName(waytype_e way) {
  switch (way) {
    case eWayPath:
      return tr("path", "way type");
    case eWayTrack:
      return tr("forest/field track", "way type");
    case eWayCycleway:
      return tr("cycleway", "way type");
    case eWaySteps:
      return tr("steps", "way type");
    case eWayStreet:
      return tr("street", "way type");
    case eWayRoad:
      return tr("road", "way type");
    case eWayBusyRoad:
      return tr("busy road", "way type");
    case eWayOther:
      return tr("other", "way type");
    default:
      return tr("no data", "way type");
  }
}

QString CSurface::labelName(label_e label) {
  switch (label) {
    case eLabelAsphalt:
      return tr("asphalt", "surface");
    case eLabelPaved:
      return tr("paved", "surface");
    case eLabelConcrete:
      return tr("concrete", "surface");
    case eLabelConcreteLanes:
      return tr("concrete lanes", "surface");
    case eLabelConcretePlates:
      return tr("concrete plates", "surface");
    case eLabelPavingStones:
      return tr("paving stones", "surface");
    case eLabelCobblestone:
      return tr("cobblestones", "surface");
    case eLabelMetal:
      return tr("metal", "surface");
    case eLabelWood:
      return tr("wood", "surface");
    case eLabelCompacted:
      return tr("compacted", "surface");
    case eLabelFineGravel:
      return tr("fine gravel", "surface");
    case eLabelGravel:
      return tr("gravel", "surface");
    case eLabelPebblestone:
      return tr("pebblestone", "surface");
    case eLabelGrassPaver:
      return tr("grass pavers", "surface");
    case eLabelSemiPaved:
      return tr("semi-paved", "surface");
    case eLabelUnpaved:
      return tr("unpaved", "surface");
    case eLabelDirt:
      return tr("dirt", "surface");
    case eLabelGround:
      return tr("natural ground", "surface");
    case eLabelGrass:
      return tr("grass", "surface");
    case eLabelMud:
      return tr("mud", "surface");
    case eLabelSand:
      return tr("sand", "surface");
    case eLabelWoodchips:
      return tr("woodchips", "surface");
    case eLabelRock:
      return tr("rock", "surface");
    case eLabelSteppingStones:
      return tr("stepping stones", "surface");
    case eLabelPavedAssumed:
      return tr("paved (assumed)", "surface");
    case eLabelUnpavedAssumed:
      return tr("unpaved (assumed)", "surface");
    default:
      return tr("unknown", "surface");
  }
}

QColor CSurface::classColor(class_e cls) {
  switch (cls) {
    case eClassPaved:
      return QColor(0x54, 0x6e, 0x7a);  // blue grey
    case eClassSemiPaved:
      return QColor(0xf9, 0xa8, 0x25);  // amber
    case eClassUnpaved:
      return QColor(0x79, 0x55, 0x48);  // brown
    default:
      return QColor(0xe0, 0x40, 0xfb);  // magenta: clearly "no information"
  }
}

QColor CSurface::wayTypeColor(waytype_e way) {
  switch (way) {
    case eWayPath:
      return QColor(0x2e, 0x7d, 0x32);  // green
    case eWayTrack:
      return QColor(0x8d, 0x6e, 0x63);  // light brown
    case eWayCycleway:
      return QColor(0x1e, 0x88, 0xe5);  // blue
    case eWaySteps:
      return QColor(0x6a, 0x1b, 0x9a);  // purple
    case eWayStreet:
      return QColor(0xfd, 0xd8, 0x35);  // yellow
    case eWayRoad:
      return QColor(0xfb, 0x8c, 0x00);  // orange
    case eWayBusyRoad:
      return QColor(0xd3, 0x2f, 0x2f);  // red
    case eWayOther:
      return QColor(0x75, 0x75, 0x75);  // grey
    default:
      return QColor(0xe0, 0x40, 0xfb);  // magenta, as the unknown surface class
  }
}
