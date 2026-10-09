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

#ifndef CSURFACE_H
#define CSURFACE_H

#include <QColor>
#include <QCoreApplication>
#include <QString>

/**
   @brief Classification of OpenStreetMap ways into surface classes and way types

   The classes follow what a walker or cyclist feels, like the "surfaces" and "way types" of
   popular route planners. A way's tags map to a detailed surface label, a surface class and a
   way type. Tags missing on a way are filled with the usual assumption for its highway type.
 */
class CSurface {
  Q_DECLARE_TR_FUNCTIONS(CSurface)
 public:
  enum class_e : qint8 {
    eClassPaved = 0,      ///< asphalt, concrete, cobbles/pavers, roads without surface tag
    eClassSemiPaved = 1,  ///< compacted, (fine) gravel, tracktype grade2
    eClassUnpaved = 2,    ///< dirt, ground, grass, sand, rock, grade3-5, paths without surface tag
    eClassUnknown = 3,    ///< tracks without surface and tracktype, or no way found
    eClassCount = 4
  };

  enum waytype_e : qint8 {
    eWayPath = 0,  ///< path, footway, bridleway
    eWayTrack,     ///< track (forest/field road)
    eWayCycleway,  ///< cycleway
    eWaySteps,     ///< steps
    eWayStreet,    ///< residential, living_street, pedestrian
    eWayRoad,      ///< unclassified, service, tertiary
    eWayBusyRoad,  ///< secondary, primary, trunk
    eWayOther,     ///< any other highway value
    eWayNoData,    ///< no way found close to the track
    eWayCount
  };

  enum label_e : qint8 {
    eLabelAsphalt = 0,
    eLabelPaved,
    eLabelConcrete,
    eLabelConcreteLanes,
    eLabelConcretePlates,
    eLabelPavingStones,
    eLabelCobblestone,
    eLabelMetal,
    eLabelWood,
    eLabelCompacted,
    eLabelFineGravel,
    eLabelGravel,
    eLabelPebblestone,
    eLabelGrassPaver,
    eLabelSemiPaved,
    eLabelUnpaved,
    eLabelDirt,
    eLabelGround,
    eLabelGrass,
    eLabelMud,
    eLabelSand,
    eLabelWoodchips,
    eLabelRock,
    eLabelSteppingStones,
    eLabelPavedAssumed,
    eLabelUnpavedAssumed,
    eLabelUnknown,
    eLabelCount
  };

  /// the OSM tags of a way the classification depends on
  struct tags_t {
    QString highway;
    QString surface;
    QString tracktype;
    QString footway;
  };

  /// the result of a classification
  struct info_t {
    label_e label = eLabelUnknown;
    class_e cls = eClassUnknown;
    waytype_e way = eWayNoData;

    bool operator==(const info_t& other) const { return label == other.label && cls == other.cls && way == other.way; }
    bool operator!=(const info_t& other) const { return !(*this == other); }
  };

  /// the info used for track parts without any way close by
  static info_t noData() { return {eLabelUnknown, eClassUnknown, eWayNoData}; }

  /**
     @brief Classify a way by its tags
     @param tags  the way's highway, surface, tracktype and footway tags
     @return label, class and way type
   */
  static info_t classify(const tags_t& tags);

  /// map a highway tag value to a way type
  static waytype_e wayType(const QString& highway);

  static QString className(class_e cls);
  static QString wayTypeName(waytype_e way);
  static QString labelName(label_e label);

  static QColor classColor(class_e cls);
  static QColor wayTypeColor(waytype_e way);
};

#endif  // CSURFACE_H
