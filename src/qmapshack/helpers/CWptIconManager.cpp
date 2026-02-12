/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CWptIconManager.h"

#include <QWidget>

#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconDialog.h"
#include "setup/IAppSetup.h"

CWptIconManager* CWptIconManager::pSelf = nullptr;
const char* CWptIconManager::wptDefault = "://icons/waypoints/32x32/Default.png";

CWptIconManager::CWptIconManager(QObject* parent) : wptHighlight("://cursors/wptHighlightRed.png") {
  pSelf = this;

  init();

  SETTINGS;
  setIconSize(cfg.value("Icons/sizeWpt", DEFAULTICONSIZE).toInt());
}

CWptIconManager::~CWptIconManager() { removeNumberedBullets(); }

const QImage& CWptIconManager::iconHighlight() { return wptHighlightScaled; }

void CWptIconManager::setIconSize(int size) {
  wptSize = size;
  wptHighlightScaled = wptHighlight.scaled(wptHighlight.size() * size / qreal(DEFAULTICONSIZE), Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
}

void CWptIconManager::removeNumberedBullets() {
  for (const QString& filename : std::as_const(mapNumberedBullets)) {
    QFile::remove(filename);
  }
  mapNumberedBullets.clear();
}

QPixmap CWptIconManager::createGrayscale(QString path) {
  QPixmap pixmap(path);
  QBitmap alpha = pixmap.createHeuristicMask();
  QImage image = pixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
  QPixmap pixmap_gray = QPixmap::fromImage(image.convertToFormat(QImage::Format_ARGB32));
  pixmap_gray.setMask(alpha);
  return pixmap_gray;
}

void CWptIconManager::init() {
  // clang-format off
  wptIcons = {
    {
      {16,16}, "Default", wptDefault,
      {tr("markers")},
      {}
    },
    {
      {16,16}, "City (Capitol)", "://icons/waypoints/32x32/CityCapitol.png",
      {tr("markers")},
      {tr("dot"), tr("city")}
    },
    {
      {16,16}, "City (Large)", "://icons/waypoints/32x32/CityLarge.png",
      {tr("markers")},
      {tr("dot"), tr("city"), tr("etrex")}
    },
    {
      {16,16}, "City (Medium)", "://icons/waypoints/32x32/CityMedium.png",
      {tr("markers")},
      {tr("dot"), tr("city"), tr("etrex")}
    },
    {
      {16,16}, "City (Small)", "://icons/waypoints/32x32/CitySmall.png",
      {tr("markers")},
      {tr("dot"), tr("city"), tr("etrex")}
    },
    {
      {0,32}, "Flag, Red", "://icons/waypoints/32x32/FlagRed.png",
      {tr("markers")},
      {tr("flag"), tr("red"), tr("etrex")}
    },
    {
      {0,32}, "Flag, Blue", "://icons/waypoints/32x32/FlagBlue.png",
      {tr("markers")},
      {tr("flag"), tr("blue"), tr("etrex")}
    },
    {
      {0,32}, "Flag, Green", "://icons/waypoints/32x32/FlagGreen.png",
      {tr("markers")},
      {tr("flag"), tr("green"), tr("etrex")}
    },
    {
      {0,32}, "Pin, Red", "://icons/waypoints/32x32/PinRed.png",
      {tr("markers")},
      {tr("pin"), tr("red"), tr("etrex")}
    },
    {
      {0,32}, "Pin, Blue", "://icons/waypoints/32x32/PinBlue.png",
      {tr("markers")},
      {tr("pin"), tr("blue"), tr("etrex")}
    },
    {
      {0,32}, "Pin, Green", "://icons/waypoints/32x32/PinGreen.png",
      {tr("markers")},
      {tr("pin"), tr("green"), tr("etrex")}
    },
    {
      {16,16}, "Block, Red", "://icons/waypoints/32x32/BoxRed.png",
      {tr("markers")},
      {tr("block"), tr("red"),tr("shape"), tr("etrex")}
    },
    {
      {16,16}, "Block, Blue", "://icons/waypoints/32x32/BoxBlue.png",
      {tr("markers")},
      {tr("block"), tr("blue"),tr("shape"), tr("etrex")}
    },
    {
      {16,16}, "Block, Green", "://icons/waypoints/32x32/BoxGreen.png",
      {tr("markers")},
      {tr("block"), tr("green"),tr("shape"), tr("etrex")}
    },
    {
      {16,16}, "Circle, Red", "://icons/waypoints/32x32/CircleRed.png",
      {tr("markers")},
      {tr("circle"), tr("red"),tr("shape")}
    },
    {
      {16,16}, "Circle, Blue", "://icons/waypoints/32x32/CircleBlue.png",
      {tr("markers")},
      {tr("circle"), tr("blue"),tr("shape")}
    },
    {
      {16,16}, "Circle, Green", "://icons/waypoints/32x32/CircleGreen.png",
      {tr("markers")},
      {tr("circle"), tr("green"),tr("shape")}
    },
    {
      {16,16}, "Diamond, Red", "://icons/waypoints/32x32/DiamondRed.png",
      {tr("markers")},
      {tr("diamond"), tr("red"),tr("shape"), tr("etrex")}
    },
    {
      {16,16}, "Diamond, Blue", "://icons/waypoints/32x32/DiamondBlue.png",
      {tr("markers")},
      {tr("diamond"), tr("blue"),tr("shape")}
    },
    {
      {16,16}, "Diamond, Green", "://icons/waypoints/32x32/DiamondGreen.png",
      {tr("markers")},
      {tr("diamond"), tr("green"),tr("shape"), tr("etrex")}
    },
    {
      {16,16}, "Oval, Red", "://icons/waypoints/32x32/OvalRed.png",
      {tr("markers")},
      {tr("oval"), tr("red"),tr("shape")}
    },
    {
      {16,16}, "Oval, Blue", "://icons/waypoints/32x32/OvalBlue.png",
      {tr("markers")},
      {tr("oval"), tr("blue"),tr("shape")}
    },
    {
      {16,16}, "Oval, Green", "://icons/waypoints/32x32/OvalGreen.png",
      {tr("markers")},
      {tr("oval"), tr("green"),tr("shape")}
    },
    {
      {16,16}, "Rectangle, Red", "://icons/waypoints/32x32/RectangleRed.png",
      {tr("markers")},
      {tr("rectangle"), tr("red"),tr("shape")}
    },
    {
      {16,16}, "Rectangle, Blue", "://icons/waypoints/32x32/RectangleBlue.png",
      {tr("markers")},
      {tr("rectangle"), tr("blue"),tr("shape")}
    },
    {
      {16,16}, "Rectangle, Green", "://icons/waypoints/32x32/RectangleGreen.png",
      {tr("markers")},
      {tr("rectangle"), tr("green"),tr("shape")}
    },
    {
      {16,16}, "Square, Red", "://icons/waypoints/32x32/SquareRed.png",
      {tr("markers")},
      {tr("square"), tr("red"),tr("shape"), tr("etrex")}
    },
    {
      {16,16}, "Square, Blue", "://icons/waypoints/32x32/SquareBlue.png",
      {tr("markers")},
      {tr("square"), tr("blue"),tr("shape")}
    },
    {
      {16,16}, "Square, Green", "://icons/waypoints/32x32/SquareGreen.png",
      {tr("markers")},
      {tr("square"), tr("green"),tr("shape"), tr("etrex")}
    },
    {
      {16,16}, "Triangle, Red", "://icons/waypoints/32x32/TriangleRed.png",
      {tr("markers")},
      {tr("triangle"), tr("red"),tr("shape")}
    },
    {
      {16,16}, "Triangle, Blue", "://icons/waypoints/32x32/TriangleBlue.png",
      {tr("markers")},
      {tr("triangle"), tr("blue"),tr("shape")}
    },
    {
      {16,16}, "Triangle, Green", "://icons/waypoints/32x32/TriangleGreen.png",
      {tr("markers")},
      {tr("triangle"), tr("green"),tr("shape")}
    },
    {
      {16,16}, "Letter A, Red", "://icons/waypoints/32x32/LetterARed.png",
      {tr("markers")},
      {tr("letter"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter A, Blue", "://icons/waypoints/32x32/LetterABlue.png",
      {tr("markers")},
      {tr("letter"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter A, Green", "://icons/waypoints/32x32/LetterAGreen.png",
      {tr("markers")},
      {tr("letter"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter B, Red", "://icons/waypoints/32x32/LetterBRed.png",
      {tr("markers")},
      {tr("letter"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter B, Blue", "://icons/waypoints/32x32/LetterBBlue.png",
      {tr("markers")},
      {tr("letter"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter B, Green", "://icons/waypoints/32x32/LetterBGreen.png",
      {tr("markers")},
      {tr("letter"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter C, Red", "://icons/waypoints/32x32/LetterCRed.png",
      {tr("markers")},
      {tr("letter"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter C, Blue", "://icons/waypoints/32x32/LetterCBlue.png",
      {tr("markers")},
      {tr("letter"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter C, Green", "://icons/waypoints/32x32/LetterCGreen.png",
      {tr("markers")},
      {tr("letter"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter D, Red", "://icons/waypoints/32x32/LetterDRed.png",
      {tr("markers")},
      {tr("letter"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter D, Blue", "://icons/waypoints/32x32/LetterDBlue.png",
      {tr("markers")},
      {tr("letter"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Letter D, Green", "://icons/waypoints/32x32/LetterDGreen.png",
      {tr("markers")},
      {tr("letter"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 0, Red", "://icons/waypoints/32x32/Number0Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 0, Blue", "://icons/waypoints/32x32/Number0Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 0, Green", "://icons/waypoints/32x32/Number0Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 1, Red", "://icons/waypoints/32x32/Number1Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 1, Blue", "://icons/waypoints/32x32/Number1Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 1, Green", "://icons/waypoints/32x32/Number1Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 2, Red", "://icons/waypoints/32x32/Number2Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 2, Blue", "://icons/waypoints/32x32/Number2Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 2, Green", "://icons/waypoints/32x32/Number2Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 3, Red", "://icons/waypoints/32x32/Number3Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 3, Blue", "://icons/waypoints/32x32/Number3Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 3, Green", "://icons/waypoints/32x32/Number3Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 4, Red", "://icons/waypoints/32x32/Number4Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 4, Blue", "://icons/waypoints/32x32/Number4Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 4, Green", "://icons/waypoints/32x32/Number4Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 5, Red", "://icons/waypoints/32x32/Number5Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 5, Blue", "://icons/waypoints/32x32/Number5Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 5, Green", "://icons/waypoints/32x32/Number5Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 6, Red", "://icons/waypoints/32x32/Number6Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 6, Blue", "://icons/waypoints/32x32/Number6Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 6, Green", "://icons/waypoints/32x32/Number6Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 7, Red", "://icons/waypoints/32x32/Number7Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 7, Blue", "://icons/waypoints/32x32/Number7Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 7, Green", "://icons/waypoints/32x32/Number7Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 8, Red", "://icons/waypoints/32x32/Number8Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 8, Blue", "://icons/waypoints/32x32/Number8Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 8, Green", "://icons/waypoints/32x32/Number8Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 9, Red", "://icons/waypoints/32x32/Number9Red.png",
      {tr("markers")},
      {tr("number"), tr("red"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 9, Blue", "://icons/waypoints/32x32/Number9Blue.png",
      {tr("markers")},
      {tr("number"), tr("blue"), tr("alphanumerical")}
    },
    {
      {16,16}, "Number 9, Green", "://icons/waypoints/32x32/Number9Green.png",
      {tr("markers")},
      {tr("number"), tr("green"), tr("alphanumerical")}
    },
    {
      {16,16}, "Residence", "://icons/waypoints/32x32/Residence.png",
      {tr("markers")},
      {tr("residence"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Favorite", "://icons/waypoints/32x32/Favorite.png",
      {tr("markers")},
      {tr("favorite")}
    },
    {
      {16,16}, "Civil", "://icons/waypoints/32x32/Civil.png",
      {tr("markers")},
      {tr("flag"), tr("etrex")}
    },
    {
      {16,16}, "Crossing", "://icons/waypoints/32x32/Crossing.png",
      {tr("markers")},
      {tr("crossing"), tr("etrex")}
    },
    {
      {0,31}, "Flag", "://icons/waypoints/32x32/FlagRed.png",
      {tr("markers")},
      {tr("flag")}
    },
    {
      {16,16}, "Trail Head", "://icons/cache/32x32/trailhead.png",
      {tr("markers")},
      {tr("trail head"), tr("etrex")}
    },
    {
      {16,16}, "Bike Trail", "://icons/waypoints/32x32/BikeTrail.png",
      {tr("markers")},
      {tr("bike trail"), tr("etrex")}
    },
    {
      {16,16}, "ATV", "://icons/waypoints/32x32/ATV.png",
      {tr("markers")},
      {tr("atv"), tr("vehicle"), tr("etrex")}
    },
    {
      {16,16}, "Car", "://icons/waypoints/32x32/Car.png",
      {tr("markers")},
      {tr("car"), tr("vehicle"), tr("etrex")}
    },
    {
      {16,16}, "Truck", "://icons/waypoints/32x32/Truck.png",
      {tr("markers")},
      {tr("truck"), tr("vehicle"), tr("etrex")}
    },
    {
      {16,16}, "Wrecker", "://icons/waypoints/32x32/Wrecker.png",
      {tr("markers")},
      {tr("wrecker"), tr("vehicle"), tr("etrex")}
    },
    {
      {16,16}, "Water Hydrant", "://icons/waypoints/32x32/WaterHydrant.png",
      {tr("markers")},
      {tr("water hydrant")}
    },
    {
      {16,16}, "Mile Marker", "://icons/waypoints/32x32/MileMarker.png",
      {tr("markers")},
      {tr("mile marker"), tr("etrex")}
    },
    {
      {16,16}, "TracBack Point", "://icons/waypoints/32x32/TracBackPoint.png",
      {tr("markers")},
      {tr("tracback point")}
    },
    {
      {16,16}, "Ball Park", "://icons/waypoints/32x32/BallPark.png",
      {tr("markers")},
      {tr("circle"), tr("etrex")}
    },
    {
      {16,16}, "Contact, Smiley", "://icons/cache/32x32/found.png",
      {tr("markers"), tr("contact")},
      {tr("smiley")}
    },
    {
      {16,16}, "Sad Face", "://icons/cache/32x32/dnf.png",
      {tr("markers")},
      {tr("smiley")}
    },
    {
      {16,16}, "Dot, White", "://icons/waypoints/32x32/DotWhite.png",
      {tr("markers")},
      {tr("square"), tr("etrex")}
    },
    {
      {16,16}, "Exit", "://icons/waypoints/32x32/Exit.png",
      {tr("markers")},
      {tr("square")}
    },
    {
      {16,16}, "Waypoint", "://icons/waypoints/32x32/Waypoint.png",
      {tr("markers")},
      {tr("waypoint"), tr("dot"), tr("etrex")}
    },
    {
      {16,16}, "Anchor", "://icons/waypoints/32x32/Anchor.png",
      {tr("marine")},
      {tr("anchor"), tr("etrex")}
    },
    {
      {16,16}, "Anchor Prohibited", "://icons/waypoints/32x32/AnchorProhibited.png",
      {tr("marine")},
      {tr("anchor")}
    },
    {
      {16,16}, "Bell", "://icons/waypoints/32x32/Bell.png",
      {tr("marine")},
      {tr("bell"), tr("etrex")}
    },
    {
      {16,16}, "Horn", "://icons/waypoints/32x32/Horn.png",
      {tr("marine")},
      {tr("horn"), tr("etrex")}
    },
    {
      {16,16}, "Light", "://icons/waypoints/32x32/Light.png",
      {tr("marine")},
      {tr("light"), tr("etrex")}
    },
    {
      {16,16}, "Skull and Crossbones", "://icons/waypoints/32x32/SkullAndCrossbones.png",
      {tr("marine")},
      {tr("skull and crossbones"), tr("etrex")}
    },
    {
      {16,16}, "Buoy, White", "://icons/waypoints/32x32/BuoyWhite.png",
      {tr("marine")},
      {tr("buoy"), tr("white"), tr("etrex")}
    },
    {
      {16,16}, "Radio Beacon", "://icons/waypoints/32x32/RadioBeacon.png",
      {tr("marine")},
      {tr("radio"), tr("beacon"), tr("etrex")}
    },
    {
      {16,16}, "Beacon", "://icons/waypoints/32x32/Beacon.png",
      {tr("marine")},
      {tr("beacon")}
    },
    {
      {16,16}, "Coast Guard", "://icons/waypoints/32x32/CoastGuard.png",
      {tr("marine")},
      {tr("flag")}
    },
    {
      {16,16}, "Diver Down Flag 1", "://icons/waypoints/32x32/DiverDownFlag1.png",
      {tr("marine")},
      {tr("diver down flag"), tr("etrex")}
    },
    {
      {16,16}, "Diver Down Flag 2", "://icons/waypoints/32x32/DiverDownFlag2.png",
      {tr("marine")},
      {tr("diver down flag"), tr("etrex")}
    },
    {
      {16,16}, "Man Overboard", "://icons/waypoints/32x32/ManOverboard.png",
      {tr("marine")},
      {tr("man overboard"), tr("etrex")}
    },
    {
      {16,16}, "Controlled Area", "://icons/waypoints/32x32/ControlledArea.png",
      {tr("marine")},
      {tr("area"), tr("etrex")}
    },
    {
      {16,16}, "Danger Area", "://icons/waypoints/32x32/DangerArea.png",
      {tr("marine")},
      {tr("area"), tr("etrex")}
    },
    {
      {16,16}, "Restricted Area", "://icons/waypoints/32x32/RestrictedArea.png",
      {tr("marine")},
      {tr("area"), tr("etrex")}
    },
    {
      {16,16}, "Reef", "://icons/waypoints/32x32/Reef.png",
      {tr("marine")},
      {tr("reef")}
    },
    {
      {16,16}, "Weed Bed", "://icons/waypoints/32x32/WeedBed.png",
      {tr("marine")},
      {tr("weed bed")}
    },
    {
      {16,16}, "Dropoff", "://icons/waypoints/32x32/Dropoff.png",
      {tr("marine")},
      {tr("dropoff")}
    },
    {
      {16,16}, "Dock", "://icons/waypoints/32x32/Dock.png",
      {tr("marine")},
      {tr("dock")}
    },
    {
      {16,16}, "Bait and Tackle", "://icons/waypoints/32x32/BaitAndTackle.png",
      {tr("marine")},
      {tr("bait and tackle")}
    },
    {
      {16,16}, "Stump", "://icons/waypoints/32x32/Stump.png",
      {tr("marine")},
      {tr("stump")}
    },

    {
      {16,16}, "Circle with X", "://icons/waypoints/32x32/CircleWithX.png",
      {tr("aerial")},
      {tr("circle")}
    },
    {
      {16,16}, "Private Field", "://icons/waypoints/32x32/PrivateField.png",
      {tr("aerial")},
      {tr("circle"), tr("etrex")}
    },
    {
      {16,16}, "Soft Field", "://icons/waypoints/32x32/SoftField.png",
      {tr("aerial")},
      {tr("circle"), tr("etrex")}
    },
    {
      {16,16}, "Glider Area", "://icons/waypoints/32x32/GliderArea.png",
      {tr("aerial")},
      {tr("glider area"), tr("etrex")}
    },
    {
      {16,16}, "Ultralight Area", "://icons/waypoints/32x32/UltralightArea.png",
      {tr("aerial")},
      {tr("ultralight area"), tr("etrex")}
    },
    {
      {16,16}, "Parachute Area", "://icons/waypoints/32x32/ParachuteArea.png",
      {tr("aerial")},
      {tr("parachute area"), tr("etrex")}
    },

    {
      {16,16}, "Big Game", "://icons/waypoints/32x32/BigGame.png",
      {tr("hunting")},
      {tr("big game"), tr("etrex")}
    },
    {
      {16,16}, "Blind", "://icons/waypoints/32x32/Blind.png",
      {tr("hunting")},
      {tr("blind"), tr("etrex")}
    },
    {
      {16,16}, "Blood Trail", "://icons/waypoints/32x32/BloodTrail.png",
      {tr("hunting")},
      {tr("blood trail"), tr("etrex")}
    },
    {
      {16,16}, "Cover", "://icons/waypoints/32x32/Cover.png",
      {tr("hunting")},
      {tr("cover"), tr("etrex")}
    },
    {
      {16,16}, "Covey", "://icons/waypoints/32x32/Covey.png",
      {tr("hunting")},
      {tr("covey"), tr("etrex")}
    },
    {
      {16,16}, "Food Source", "://icons/waypoints/32x32/FoodSource.png",
      {tr("hunting")},
      {tr("food source"), tr("etrex")}
    },
    {
      {16,16}, "Furbearer", "://icons/waypoints/32x32/Furbearer.png",
      {tr("hunting")},
      {tr("furbearer"), tr("etrex")}
    },
    {
      {16,16}, "Small Game", "://icons/waypoints/32x32/SmallGame.png",
      {tr("hunting")},
      {tr("small game"), tr("etrex")}
    },
    {
      {16,16}, "Animal Tracks", "://icons/waypoints/32x32/AnimalTracks.png",
      {tr("hunting")},
      {tr("animal tracks"), tr("etrex")}
    },
    {
      {16,16}, "Treed Quarry", "://icons/waypoints/32x32/TreedQuarry.png",
      {tr("hunting")},
      {tr("treed quarry"), tr("etrex")}
    },
    {
      {16,16}, "Tree Stand", "://icons/waypoints/32x32/TreeStand.png",
      {tr("hunting")},
      {tr("tree stand"), tr("etrex")}
    },
    {
      {16,16}, "Upland Game", "://icons/waypoints/32x32/UplandGame.png",
      {tr("hunting")},
      {tr("upland game"), tr("etrex")}
    },
    {
      {16,16}, "Waterfowl", "://icons/waypoints/32x32/Waterfowl.png",
      {tr("hunting")},
      {tr("waterfowl"), tr("etrex")}
    },
    {
      {16,16}, "Water Source", "://icons/waypoints/32x32/WaterSource.png",
      {tr("hunting")},
      {tr("water source"), tr("etrex")}
    },
    {
      {16,16}, "Dog Running", "://icons/waypoints/32x32/DogRunning.png",
      {tr("hunting")},
      {tr("dog running")}
    },
    {
      {16,16}, "Dog Pointing", "://icons/waypoints/32x32/DogPointing.png",
      {tr("hunting")},
      {tr("dog pointing")}
    },
    {
      {16,16}, "Dog Treed", "://icons/waypoints/32x32/DogTreed.png",
      {tr("hunting")},
      {tr("dog treed")}
    },
    {
      {16,16}, "Dog Sitting", "://icons/waypoints/32x32/DogSitting.png",
      {tr("hunting")},
      {tr("dog sitting")}
    },
    {
      {16,16}, "Dog Unknown", "://icons/waypoints/32x32/DogUnknown.png",
      {tr("hunting")},
      {tr("dog unknown")}
    },

    {
      {16,16}, "Navaid, Amber", "://icons/waypoints/32x32/NavaidAmber.png",
      {tr("navaid")},
      {tr("amber"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Black", "://icons/waypoints/32x32/NavaidBlack.png",
      {tr("navaid")},
      {tr("black"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Blue", "://icons/waypoints/32x32/NavaidBlue.png",
      {tr("navaid")},
      {tr("blue"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Green", "://icons/waypoints/32x32/NavaidGreen.png",
      {tr("navaid")},
      {tr("green"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Green/Red", "://icons/waypoints/32x32/NavaidGreenRed.png",
      {tr("navaid")},
      {tr("green"), tr("red"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Green/White", "://icons/waypoints/32x32/NavaidGreenWhite.png",
      {tr("navaid")},
      {tr("green"), tr("white"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Orange", "://icons/waypoints/32x32/NavaidOrange.png",
      {tr("navaid")},
      {tr("orange"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Red", "://icons/waypoints/32x32/NavaidRed.png",
      {tr("navaid")},
      {tr("red"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Red/Green", "://icons/waypoints/32x32/NavaidRedGreen.png",
      {tr("navaid")},
      {tr("red"), tr("green"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Red/White", "://icons/waypoints/32x32/NavaidRedWhite.png",
      {tr("navaid")},
      {tr("red"), tr("white"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, Violet", "://icons/waypoints/32x32/NavaidViolet.png",
      {tr("navaid")},
      {tr("violet"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, White", "://icons/waypoints/32x32/NavaidWhite.png",
      {tr("navaid")},
      {tr("white"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, White/Green", "://icons/waypoints/32x32/NavaidWhiteGreen.png",
      {tr("navaid")},
      {tr("white"), tr("green"), tr("etrex")}
    },
    {
      {16,16}, "Navaid, White/Red", "://icons/waypoints/32x32/NavaidWhiteRed.png",
      {tr("navaid")},
      {tr("white"), tr("red"), tr("etrex")}
    },

    {
      {16,16}, "1st Category", "://icons/waypoints/32x32/1stCategory.png",
      {tr("cycling")},
      {tr("category")}
    },
    {
      {16,16}, "2nd Category", "://icons/waypoints/32x32/2ndCategory.png",
      {tr("cycling")},
      {tr("category")}
    },
    {
      {16,16}, "3rd Category", "://icons/waypoints/32x32/3rdCategory.png",
      {tr("cycling")},
      {tr("category")}
    },
    {
      {16,16}, "4th Category", "://icons/waypoints/32x32/4thCategory.png",
      {tr("cycling")},
      {tr("category")}
    },
    {
      {16,16}, "Danger", "://icons/waypoints/32x32/Danger.png",
      {tr("cycling")},
      {tr("danger")}
    },
    {
      {16,16}, "First Aid", "://icons/waypoints/32x32/FirstAid.png",
      {tr("cycling")},
      {tr("first aid")}
    },
    {
      {16,16}, "Food", "://icons/waypoints/32x32/Food.png",
      {tr("cycling")},
      {tr("food")}
    },
    {
      {16,16}, "Generic", "://icons/waypoints/32x32/Generic.png",
      {tr("cycling")},
      {tr("generic")}
    },
    {
      {16,16}, "Hors Category", "://icons/waypoints/32x32/HorsCategory.png",
      {tr("cycling")},
      {tr("hors category")}
    },
    {
      {16,16}, "Left", "://icons/waypoints/32x32/Left.png",
      {tr("cycling")},
      {tr("left")}
    },
    {
      {16,16}, "Right", "://icons/waypoints/32x32/Right.png",
      {tr("cycling")},
      {tr("right")}
    },
    {
      {16,16}, "Sprint", "://icons/waypoints/32x32/Sprint.png",
      {tr("cycling")},
      {tr("sprint")}
    },
    {
      {16,16}, "Straight", "://icons/waypoints/32x32/Straight.png",
      {tr("cycling")},
      {tr("straight")}
    },
    {
      {16,16}, "Summit", "://icons/waypoints/32x32/Summit.png",
      {tr("cycling")},
      {tr("summit"), tr("etrex")}
    },
    {
      {16,16}, "Valley", "://icons/waypoints/32x32/Valley.png",
      {tr("cycling")},
      {tr("valley")}
    },
    {
      {16,16}, "Water", "://icons/waypoints/32x32/Water.png",
      {tr("cycling")},
      {tr("water")}
    },
    {
      {16,16}, "LeftFork", "://icons/waypoints/32x32/LeftFork.png",
      {tr("cycling")},
      {tr("left fork")}
    },
    {
      {16,16}, "RightFork", "://icons/waypoints/32x32/RightFork.png",
      {tr("cycling")},
      {tr("right fork")}
    },
    {
      {16,16}, "MiddleFork", "://icons/waypoints/32x32/MiddleFork.png",
      {tr("cycling")},
      {tr("middle fork")}
    },
    {
      {16,16}, "SlightLeft", "://icons/waypoints/32x32/SlightLeft.png",
      {tr("cycling")},
      {tr("slight left")}
    },
    {
      {16,16}, "SharpLeft", "://icons/waypoints/32x32/SharpLeft.png",
      {tr("cycling")},
      {tr("sharp left")}
    },
    {
      {16,16}, "SlightRight", "://icons/waypoints/32x32/SlightRight.png",
      {tr("cycling")},
      {tr("slight right")}
    },
    {
      {16,16}, "SharpRight", "://icons/waypoints/32x32/SharpRight.png",
      {tr("cycling")},
      {tr("sharp right")}
    },
    {
      {16,16}, "UTurn", "://icons/waypoints/32x32/UTurn.png",
      {tr("cycling")},
      {tr("uturn")}
    },
    {
      {16,16}, "Start", "://icons/waypoints/32x32/Start.png",
      {tr("cycling")},
      {tr("start")}
    },
    {
      {16,16}, "End", "://icons/waypoints/32x32/End.png",
      {tr("cycling")},
      {tr("end")}
    },
    {
      {16,16}, "Bank", "://icons/poi/SJJB/png/money_bank.n.32.png",
      {tr("poi")},
      {tr("bank"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Bank, Euro", "://icons/poi/png/bank_euro.png",
      {tr("poi")},
      {tr("bank"), tr("building")}
    },
    {
      {16,16}, "Bank, Pound", "://icons/poi/png/bank_pound.png",
      {tr("poi")},
      {tr("bank"), tr("building")}
    },
    {
      {16,16}, "Bank, Yen", "://icons/poi/png/bank_yen.png",
      {tr("poi")},
      {tr("bank"), tr("building")}
    },
    {
      {16,16}, "Bridge", "://icons/poi/png/bridge.png",
      {tr("poi"), tr("transport")},
      {tr("bridge"), tr("etrex")}
    },
    {
      {16,16}, "Building", "://icons/poi/png/building.png",
      {tr("poi")},
      {tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Church", "://icons/poi/SJJB/png/place_of_worship_unknown.n.32.png",
      {tr("poi")},
      {tr("church"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Cemetery", "://icons/poi/png/cemetery.png",
      {tr("poi")},
      {tr("cemetery"), tr("etrex")}
    },
    {
      {16,16}, "Military", "://icons/poi/png/military.png",
      {tr("poi")},
      {tr("military"), tr("etrex")}
    },
    {
      {16,16}, "Mine", "://icons/poi/SJJB/png/poi_mine.n.32.png",
      {tr("poi")},
      {tr("mine"), tr("landmark"), tr("etrex")}
    },
    {
      {16,16}, "Oil Field", "://icons/poi/png/oil_field.png",
      {tr("poi")},
      {tr("oil field"), tr("etrex")}
    },
    {
      {16,16}, "Tall Tower", "://icons/poi/SJJB/png/poi_tower_communications.n.32.png",
      {tr("poi"), tr("aerial")},
      {tr("tower"), tr("landmark"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Short Tower", "://icons/poi/SJJB/png/poi_tower_lookout.n.32.png",
      {tr("poi"), tr("aerial")},
      {tr("tower"), tr("landmark"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Toll Booth", "://icons/poi/png/toll_booth.png",
      {tr("poi"), tr("transport")},
      {tr("toll booth"), tr("etrex")}
    },
    {
      {16,16}, "Tunnel", "://icons/poi/png/tunnel.png",
      {tr("poi"), tr("transport")},
      {tr("tunnel"), tr("etrex")}
    },
    {
      {16,16}, "Gas Station", "://icons/poi/SJJB/png/transport_fuel.n.32.png",
      {tr("poi"), tr("transport"), tr("marine")},
      {tr("gas station"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Parking Area", "://icons/poi/SJJB/png/transport_parking.n.32.png",
      {tr("poi"), tr("transport")},
      {tr("parking"), tr("etrex")}
    },
    {
      {16,16}, "Parking, Pay", "://icons/poi/SJJB/png/transport_parking_car_paid.n.32.png",
      {tr("poi"), tr("transport")},
      {tr("parking")}
    },
    {
      {16,16}, "Parking, Euro", "://icons/poi/png/parking_euro.png",
      {tr("poi"), tr("transport")},
      {tr("parking")}
    },
    {
      {16,16}, "Parking, Euro Pay", "://icons/poi/png/parking_euro_pay.png",
      {tr("poi"), tr("transport")},
      {tr("parking")}
    },
    {
      {16,16}, "Car Rental", "://icons/poi/SJJB/png/transport_rental_car.n.32.png",
      {tr("poi"), tr("transport")},
      {tr("car rental"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Truck Stop", "://icons/poi/png/truck_stop.png",
      {tr("poi"), tr("transport")},
      {tr("truck stop"), tr("etrex")}
    },
    {
      {16,16}, "Boat Ramp", "://icons/poi/SJJB/png/transport_slipway.n.32.png",
      {tr("poi"), tr("marine")},
      {tr("boat ramp"), tr("etrex")}
    },
    {
      {16,16}, "Marina", "://icons/poi/SJJB/png/transport_marina.n.32.png",
      {tr("poi"), tr("marine")},
      {tr("marina")}
    },
    {
      {16,16}, "Ferry", "://icons/poi/SJJB/png/transport_port.n.32.png",
      {tr("poi"), tr("transport"), tr("marine")},
      {tr("ferry")}
    },
    {
      {16,16}, "Seaplane Base", "://icons/poi/png/seaplane_base.png",
      {tr("poi"), tr("transport"), tr("marine")},
      {tr("seaplane")}
    },
    {
      {16,16}, "Ground Transportation", "://icons/poi/SJJB/png/transport_bus_stop.n.32.png",
      {tr("poi"), tr("transport")},
      {tr("transportation"), tr("etrex")}
    },
    {
      {16,16}, "Funicular", "://icons/poi/png/funicular.png",
      {tr("poi"), tr("transport")},
      {tr("funicular")}
    },
    {
      {16,16}, "Railway", "://icons/poi/SJJB/png/transport_train_station.n.32.png",
      {tr("poi"), tr("transport")},
      {tr("railway")}
    },
    {
      {16,16}, "Airport", "://icons/poi/SJJB/png/transport_airport.n.32.png",
      {tr("poi"), tr("transport")},
      {tr("airport"), tr("etrex")}
    },
    {
      {16,16}, "Heliport", "://icons/poi/SJJB/png/transport_helicopter_pad.n.32.png",
      {tr("poi"), tr("transport")},
      {tr("heliport"), tr("etrex")}
    },
    {
      {16,16}, "Campground", "://icons/poi/SJJB/png/accommodation_camping.n.32.png",
      {tr("poi"), tr("outdoors"), tr("marine"), tr("accommodation")},
      {tr("campground"), tr("etrex")}
    },
    {
      {16,16}, "Lodge", "://icons/poi/SJJB/png/accommodation_shelter.n.32.png",
      {tr("poi"), tr("accommodation")},
      {tr("lodge"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Lodging", "://icons/poi/SJJB/png/accommodation_hotel.n.32.png",
      {tr("poi"), tr("accommodation")},
      {tr("lodging"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "RV Park", "://icons/poi/SJJB/png/accommodation_caravan_park.n.32.png",
      {tr("poi"), tr("outdoors"), tr("accommodation")},
      {tr("rv park"), tr("etrex")}
    },
    {
      {16,16}, "Dam", "://icons/poi/SJJB/png/water_dam.n.32.png",
      {tr("poi")},
      {tr("dam"), tr("landmark"), tr("etrex")}
    },
    {
      {16,16}, "Levee", "://icons/poi/png/levee.png",
      {tr("poi")},
      {tr("levee"), tr("etrex")}
    },
    {
      {16,16}, "Bar", "://icons/poi/SJJB/png/food_bar.n.32.png",
      {tr("poi")},
      {tr("bar"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Drinking Water", "://icons/poi/SJJB/png/food_drinkingtap.n.32.png",
      {tr("poi"), tr("outdoors")},
      {tr("drinking water"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Fast Food", "://icons/poi/SJJB/png/food_fastfood.n.32.png",
      {tr("poi")},
      {tr("fast food"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Pizza", "://icons/poi/SJJB/png/food_pizza.n.32.png",
      {tr("poi")},
      {tr("pizza"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Restaurant", "://icons/poi/SJJB/png/food_restaurant.n.32.png",
      {tr("poi"), tr("transport"), tr("marine")},
      {tr("restaurant"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Amusement Park", "://icons/poi/SJJB/png/tourist_theme_park.n.32.png",
      {tr("poi")},
      {tr("amusement park"), tr("landmark"), tr("etrex")}
    },
    {
      {16,16}, "Beach", "://icons/poi/SJJB/png/tourist_beach.n.32.png",
      {tr("poi"), tr("outdoors")},
      {tr("beach"), tr("etrex")}
    },
    {
      {16,16}, "Ghost Town", "://icons/poi/SJJB/png/tourist_ruin.n.32.png",
      {tr("poi")},
      {tr("ghost town"), tr("building"), tr("landmark"), tr("etrex")}
    },
    {
      {16,16}, "Information", "://icons/poi/SJJB/png/tourist_information.n.32.png",
      {tr("poi")},
      {tr("information"), tr("etrex")}
    },
    {
      {16,16}, "Live Theater", "://icons/poi/SJJB/png/tourist_theatre.n.32.png",
      {tr("poi")},
      {tr("live theatre"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Museum", "://icons/poi/SJJB/png/tourist_museum.n.32.png",
      {tr("poi")},
      {tr("museum"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Picnic Area", "://icons/poi/SJJB/png/tourist_picnic.n.32.png",
      {tr("poi")},
      {tr("picnic area"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Scenic Area", "://icons/poi/SJJB/png/tourist_view_point.n.32.png",
      {tr("poi")},
      {tr("scenic area"), tr("etrex")}
    },
    {
      {16,16}, "Shipwreck", "://icons/poi/SJJB/png/tourist_wreck.n.32.png",
      {tr("poi"), tr("marine")},
      {tr("scenic area"), tr("landmark"), tr("etrex")}
    },
    {
      {16,16}, "Zoo", "://icons/poi/SJJB/png/tourist_zoo.n.32.png",
      {tr("poi")},
      {tr("zoo"), tr("landmark"), tr("etrex")}
    },
    {
      {16,16}, "City Hall", "://icons/poi/SJJB/png/amenity_town_hall.n.32.png",
      {tr("poi")},
      {tr("city hall"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Library", "://icons/poi/SJJB/png/amenity_library.n.32.png",
      {tr("poi")},
      {tr("library"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Movie Theater", "://icons/poi/png/movie_theater.png",
      {tr("poi")},
      {tr("movie theater"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Police Station", "://icons/poi/SJJB/png/amenity_police.n.32.png",
      {tr("poi")},
      {tr("police station"), tr("building"), tr("help"), tr("etrex")}
    },
    {
      {16,16}, "Post Office", "://icons/poi/SJJB/png/amenity_post_office.n.32.png",
      {tr("poi")},
      {tr("post office"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Restroom", "://icons/poi/SJJB/png/amenity_toilets.n.32.png",
      {tr("poi"), tr("outdoors"), tr("transport")},
      {tr("restroom"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Shower", "://icons/poi/png/shower.png",
      {tr("poi")},
      {tr("shower"), tr("etrex")}
    },
    {
      {16,16}, "Scales", "://icons/poi/SJJB/png/amenity_court.n.32.png",
      {tr("poi")},
      {tr("scales"), tr("court"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Telephone", "://icons/poi/SJJB/png/amenity_telephone.n.32.png",
      {tr("poi")},
      {tr("telephone"), tr("etrex")}
    },
    {
      {16,16}, "Medical Facility", "://icons/poi/SJJB/png/health_hospital.n.32.png",
      {tr("poi")},
      {tr("medical facility"), tr("hospital"), tr("help"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Pharmacy", "://icons/poi/SJJB/png/health_pharmacy.n.32.png",
      {tr("poi")},
      {tr("pharmacy"), tr("help"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Hospital, Euro", "://icons/poi/png/hospital_euro.png",
      {tr("poi")},
      {tr("hospital euro"), tr("hospital"), tr("help"), tr("building")}
    },
    {
      {16,16}, "School", "://icons/poi/SJJB/png/education_school.n.32.png",
      {tr("poi")},
      {tr("school"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Bowling", "://icons/poi/png/bowling.png",
      {tr("poi"), tr("sport")},
      {tr("bowling"), tr("etrex")}
    },
    {
      {16,16}, "Fishing Area", "://icons/poi/png/fishing_area.png",
      {tr("poi"), tr("sport")},
      {tr("fishing area"), tr("etrex")}
    },
    {
      {16,16}, "Fishing Hot Spot Facility", "://icons/poi/png/fishing_hot_spot_facility.png",
      {tr("poi"), tr("sport")},
      {tr("fishing hot sport facility"), tr("etrex")}
    },
    {
      {16,16}, "Hunting Area", "://icons/poi/png/hunting_area.png",
      {tr("poi"), tr("sport"), tr("hunting")},
      {tr("hunting area")}
    },
    {
      {16,16}, "Fitness Center", "://icons/poi/SJJB/png/sport_gym.n.32.png",
      {tr("poi"), tr("sport")},
      {tr("fitness center"), tr("gym"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Golf Course", "://icons/poi/SJJB/png/sport_golf.n.32.png",
      {tr("poi"), tr("sport")},
      {tr("golf course"), tr("etrex")}
    },
    {
      {16,16}, "Ice Skating", "://icons/poi/SJJB/png/sport_iceskating.n.32.png",
      {tr("poi"), tr("sport")},
      {tr("ice skating"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Skiing Area", "://icons/poi/SJJB/png/sport_skiing_crosscountry.n.32.png",
      {tr("poi"), tr("sport")},
      {tr("skiing area"), tr("etrex")}
    },
    {
      {16,16}, "Ski Resort", "://icons/poi/SJJB/png/sport_skiing_downhill.n.32.png",
      {tr("poi"), tr("sport")},
      {tr("ski resort"), tr("etrex")}
    },
    {
      {16,16}, "Stadium", "://icons/poi/SJJB/png/sport_stadium.n.32.png",
      {tr("poi"), tr("sport")},
      {tr("stadium"), tr("building"), tr("landmark"), tr("etrex")}
    },
    {
      {16,16}, "Swimming Area", "://icons/poi/SJJB/png/sport_swimming_outdoor.n.32.png",
      {tr("poi"), tr("sport")},
      {tr("stadium"), tr("etrex")}
    },
    {
      {16,16}, "Car Repair", "://icons/poi/SJJB/png/shopping_car_repair.n.32.png",
      {tr("poi")},
      {tr("car repair"), tr("building"), tr("etrex")}
    },
    {
      {16,16}, "Convenience Store", "://icons/poi/SJJB/png/shopping_convenience.n.32.png",
      {tr("poi")},
      {tr("convenience store"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Department Store", "://icons/poi/SJJB/png/shopping_department_store.n.32.png",
      {tr("poi")},
      {tr("department store"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Shopping Center", "://icons/poi/SJJB/png/shopping_supermarket.n.32.png",
      {tr("poi")},
      {tr("shopping center"), tr("supermarket"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Winery", "://icons/poi/SJJB/png/shopping_alcohol.n.32.png",
      {tr("poi")},
      {tr("winery"),tr("bottle shop"), tr("building"), tr("food"), tr("etrex")}
    },
    {
      {16,16}, "Forest", "://icons/poi/SJJB/png/landuse_coniferous_and_deciduous.n.32.png",
      {tr("poi")},
      {tr("forest"), tr("land"), tr("etrex")}
    },
    {
      {16,16}, "Park", "://icons/poi/SJJB/png/landuse_deciduous.n.32.png",
      {tr("poi")},
      {tr("park"), tr("land"), tr("etrex")}
    },

    {
      {16,16}, "Geocache", "://icons/waypoints/32x32/Geocache.png",
      {tr("geocache")},
      {tr("geocache"), tr("etrex")}
    },
    {
      {16,16}, "Geocache Found", "://icons/waypoints/32x32/GeocacheFound.png",
      {tr("geocache")},
      {tr("geocache found"), tr("etrex")}
    },
    {
      {16,16}, "Puzzle Cache", "://icons/waypoints/32x32/PuzzleCache.png",
      {tr("geocache")},
      {tr("puzzle cache")}
    },
    {
      {16,16}, "Traditional Cache", "://icons/geocaching/icons/traditional.png",
      {tr("geocache")},
      {tr("traditional cache")}
    },
    {
      {16,16}, "Multi-cache", "://icons/geocaching/icons/multi.png",
      {tr("geocache")},
      {tr("multi cache")}
    },
    {
      {16,16}, "Unknown Cache", "://icons/geocaching/icons/unknown.png",
      {tr("geocache")},
      {tr("unknown cache")}
    },
    {
      {16,16}, "Letterbox Hybrid", "://icons/geocaching/icons/letterbox.png",
      {tr("geocache")},
      {tr("letterbox cache")}
    },
    {
      {16,16}, "Wherigo Cache", "://icons/geocaching/icons/wherigo.png",
      {tr("geocache")},
      {tr("wherigo cache")}
    },
    {
      {16,16}, "Earthcache", "://icons/geocaching/icons/earth.png",
      {tr("geocache")},
      {tr("earthcache")}
    },
    {
      {16,16}, "Virtual Cache", "://icons/geocaching/icons/virtual.png",
      {tr("geocache")},
      {tr("virtual cache")}
    },
    {
      {16,16}, "Webcam Cache", "://icons/geocaching/icons/webcam.png",
      {tr("geocache")},
      {tr("webcam cache")}
    },
    {
      {16,16}, "Event Cache", "://icons/geocaching/icons/event.png",
      {tr("geocache")},
      {tr("event cache")}
    },
    {
      {16,16}, "Mega-Event Cache", "://icons/geocaching/icons/mega.png",
      {tr("geocache")},
      {tr("mega-event cache")}
    },
    {
      {16,16}, "Giga-Event Cache", "://icons/geocaching/icons/giga.png",
      {tr("geocache")},
      {tr("giga-event cache")}
    },
    {
      {16,16}, "Cache In Trash Out Event", "://icons/geocaching/icons/cito.png",
      {tr("geocache")},
      {tr("cache in trash out event")}
    },
    {
      {16,16}, "Final Location", "://icons/32x32/waypoint_final.png",
      {tr("geocache")},
      {tr("Final Location")}
    },
    {
      {16,16}, "Physical Stage", "://icons/32x32/waypoint_physical.png",
      {tr("geocache")},
      {tr("Physical Stage")}
    },
    {
      {16,16}, "Reference Point", "://icons/32x32/waypoint_reference.png",
      {tr("geocache")},
      {tr("Reference Waypoint")}
    },
    {
      {16,16}, "Trailhead", "://icons/32x32/waypoint_trailhead.png",
      {tr("geocache")},
      {tr("Trailhead")}
    },
    {
      {16,16}, "Virtual Stage", "://icons/32x32/waypoint_virtual.png",
      {tr("geocache")},
      {tr("Virtual Stage")}
    },
    {
      {16,16}, "gray_Traditional Cache", "://icons/geocaching/icons/gray_Traditional Cache.png",
      {tr("geocache")},
      {tr("traditional cache")}
    },
    {
      {16,16}, "gray_Multi-cache", "://icons/geocaching/icons/gray_Multi-Cache.png",
      {tr("geocache")},
      {tr("multi cache")}
    },
    {
      {16,16}, "gray_Unknown Cache", "://icons/geocaching/icons/gray_Unknown Cache.png",
      {tr("geocache")},
      {tr("unknown cache")}
    },
    {
      {16,16}, "gray_Wherigo Cache", "://icons/geocaching/icons/gray_Wherigo Cache.png",
      {tr("geocache")},
      {tr("wherigo cache")}
    },
    {
      {16,16}, "gray_Event Cache", "://icons/geocaching/icons/gray_Event Cache.png",
      {tr("geocache")},
      {tr("event cache")}
    },
    {
      {16,16}, "gray_Mega-Event Cache", "://icons/geocaching/icons/gray_Mega-Event Cache.png",
      {tr("geocache")},
      {tr("mega-event cache")}
    },
    {
      {16,16}, "gray_Giga-Event Cache", "://icons/geocaching/icons/gray_Giga-Event Cache.png",
      {tr("geocache")},
      {tr("giga-event cache")}
    },
    {
      {16,16}, "gray_Cache In Trash Out Event", "://icons/geocaching/icons/gray_Cache In Trash Out Event.png",
      {tr("geocache")},
      {tr("cache in trash out event")}
    },
    {
      {16,16}, "gray_Earthcache", "://icons/geocaching/icons/gray_Earthcache.png",
      {tr("geocache")},
      {tr("earthcache")}
    },
    {
      {16,16}, "gray_Letterbox Cache", "://icons/geocaching/icons/gray_Letterbox Cache.png",
      {tr("geocache")},
      {tr("letterbox cache")}
    },
    {
      {16,16}, "gray_Virtual Cache", "://icons/geocaching/icons/gray_Virtual Cache.png",
      {tr("geocache")},
      {tr("virtual cache")}
    },
    {
      {16,16}, "gray_Webcam Cache", "://icons/geocaching/icons/gray_Webcam Cache.png",
      {tr("geocache")},
      {tr("webcam cache")}
    },
    {
      {16,16}, "Contact, Afro", "://icons/waypoints/32x32/ContactAfro.png",
      {tr("contact")},
      {tr("afro")}
    },
    {
      {16,16}, "Contact, Alien", "://icons/waypoints/32x32/ContactAlien.png",
      {tr("contact")},
      {tr("alien")}
    },
    {
      {16,16}, "Contact, Ball Cap", "://icons/waypoints/32x32/ContactBallCap.png",
      {tr("contact")},
      {tr("ball cap")}
    },
    {
      {16,16}, "Contact, Big Ears", "://icons/waypoints/32x32/ContactBigEars.png",
      {tr("contact")},
      {tr("big ears")}
    },
    {
      {16,16}, "Contact, Biker", "://icons/waypoints/32x32/ContactBiker.png",
      {tr("contact")},
      {tr("biker")}
    },
    {
      {16,16}, "Contact, Bug", "://icons/waypoints/32x32/ContactBug.png",
      {tr("contact")},
      {tr("bug")}
    },
    {
      {16,16}, "Contact, Cat", "://icons/waypoints/32x32/ContactCat.png",
      {tr("contact")},
      {tr("cat")}
    },
    {
      {16,16}, "Contact, Dog", "://icons/waypoints/32x32/ContactDog.png",
      {tr("contact")},
      {tr("dog")}
    },
    {
      {16,16}, "Contact, Dreadlocks", "://icons/waypoints/32x32/ContactDreadlocks.png",
      {tr("contact")},
      {tr("dreadlocks")}
    },
    {
      {16,16}, "Contact, Female1", "://icons/waypoints/32x32/ContactFemale1.png",
      {tr("contact")},
      {tr("female1")}
    },
    {
      {16,16}, "Contact, Female2", "://icons/waypoints/32x32/ContactFemale2.png",
      {tr("contact")},
      {tr("female2")}
    },
    {
      {16,16}, "Contact, Female3", "://icons/waypoints/32x32/ContactFemale3.png",
      {tr("contact")},
      {tr("female3")}
    },
    {
      {16,16}, "Contact, Goatee", "://icons/waypoints/32x32/ContactGoatee.png",
      {tr("contact")},
      {tr("goatee")}
    },
    {
      {16,16}, "Contact, Kung-Fu", "://icons/waypoints/32x32/ContactKungFu.png",
      {tr("contact")},
      {tr("kung-fu")}
    },
    {
      {16,16}, "Contact, Pig", "://icons/waypoints/32x32/ContactPig.png",
      {tr("contact")},
      {tr("pig")}
    },
    {
      {16,16}, "Contact, Pirate", "://icons/waypoints/32x32/ContactPirate.png",
      {tr("contact")},
      {tr("pirate")}
    },
    {
      {16,16}, "Contact, Ranger", "://icons/waypoints/32x32/ContactRanger.png",
      {tr("contact")},
      {tr("ranger")}
    },
    {
      {16,16}, "Contact, Spike", "://icons/waypoints/32x32/ContactSpike.png",
      {tr("contact")},
      {tr("spike")}
    },
    {
      {16,16}, "Contact, Sumo", "://icons/waypoints/32x32/ContactSumo.png",
      {tr("contact")},
      {tr("sumo")}
    },
    {
      {16,16}, "Contact, Blonde", "://icons/waypoints/32x32/ContactBlonde.png",
      {tr("contact")},
      {tr("blonde")}
    },
    {
      {16,16}, "Contact, Clown", "://icons/waypoints/32x32/ContactClown.png",
      {tr("contact")},
      {tr("clown")}
    },
    {
      {16,16}, "Contact, Glasses", "://icons/waypoints/32x32/ContactGlasses.png",
      {tr("contact")},
      {tr("glasses")}
    },
    {
      {16,16}, "Contact, Panda", "://icons/waypoints/32x32/ContactPanda.png",
      {tr("contact")},
      {tr("panda")}
    },
  };
  // clang-format on

  SETTINGS;
  QDir dirIcon(
      cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString());

  QDirIterator it(dirIcon.path(), {"*.bmp", "*.png"}, QDir::Files,
                  QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
  while (it.hasNext()) {
    it.next();
    const QString& name = QFileInfo(it.fileName()).completeBaseName();
    const QString& path = it.filePath();
    const QString& relpath = dirIcon.relativeFilePath(path);
    const QStringList& items = relpath.split("/");
    QStringList categories = {tr("external")};
    if (items.size() > 1) {
      categories << items.at(0).split(",");
    }
    QStringList tags = {};
    if (items.size() > 2) {
      tags << items.at(1).split(",");
    }
    setWptIconByName(name, path, categories, tags);
  }

  emit sigChanged();
}

CWptIconManager::icon_t& CWptIconManager::findIconByName(const QString& name, bool& ok) {
  for (icon_t& icon : wptIcons) {
    if (icon.name == name) {
      ok = true;
      return icon;
    }
  }
  ok = false;
  qDebug() << name;
  return wptIcons.front();
}

void CWptIconManager::setWptIconByName(const QString& name, const QString& filename, const QStringList& categories,
                                       const QStringList& tags) {
  QImage symbol(filename);

  icon_t newIcon;
  if (symbol.offset() == QPoint(0, 0)) {
    newIcon = {{symbol.width() >> 1, symbol.height() >> 1}, name, filename, categories, tags};
  } else {
    newIcon = {{symbol.offset().x(), symbol.offset().y()}, name, filename, categories, tags};
  }

  bool ok = false;
  icon_t& icon = findIconByName(name, ok);
  if (ok) {
    icon = newIcon;
  } else {
    wptIcons.append(newIcon);
  }
}

QPixmap CWptIconManager::loadIcon(const QString& path) {
  QFileInfo finfo(path);
  if (finfo.completeSuffix() != "bmp") {
    return QPixmap(path);
  } else {
    QImage img = QPixmap(path).toImage().convertToFormat(QImage::Format_Indexed8);
    img.setColor(0, qRgba(0, 0, 0, 0));
    return QPixmap::fromImage(img);
  }
}

QPixmap CWptIconManager::getWptIconByName(const QString& name, QPointF& focus, QString* src) {
  bool ok = false;
  icon_t& iconRef = findIconByName(name, ok);

  focus = iconRef.focus;

  if (src) {
    *src = iconRef.path;
  }

  QPixmap icon = loadIcon(iconRef.path);

  // Limit icon size to "DEFAULTICONSIZE" pixel max.
  int maxValue = qMax(icon.width(), icon.height());
  if (maxValue > DEFAULTICONSIZE) {
    qreal scale = qreal(DEFAULTICONSIZE) / maxValue;
    focus = focus * scale;
    icon = icon.scaled(icon.size() * scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  return icon;
}

QPixmap CWptIconManager::getWptIconScaledByName(const QString& name, QPointF& focus) {
  bool ok;
  icon_t& iconRef = findIconByName(name, ok);

  focus = iconRef.focus;

  QPixmap icon = loadIcon(iconRef.path);

  // First limit icon size to "DEFAULTICONSIZE" pixel max.
  // Then apply requested icon size
  int maxValue = qMax(icon.width(), icon.height());
  qreal scale = maxValue > DEFAULTICONSIZE ? wptSize / qreal(maxValue) : wptSize / qreal(DEFAULTICONSIZE);

  focus = focus * scale;
  icon = icon.scaled(icon.size() * scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  return icon;
}

QString CWptIconManager::selectWptIcon(QWidget* parent) {
  QString icon;

  // create a menu of the last N selected icons
  QPointer<QMenu> menu = getWptIconMenu(parent);

  // add an action that summons the wayoint icon dialog
  menu->addSeparator();
  QAction* more = menu->addAction(QIcon(":/icons/32x32/SetupWptSym.png"), tr("More..."));
  connect(more, &QAction::triggered, this, [&icon, this](bool) {
    CWptIconDialog dlg(&CMainWindow::self());
    connect(&dlg, &CWptIconDialog::sigSelectedIcon, this, [&icon](const QString& name) { icon = name; });
    dlg.exec();
  });

  QAction* clear = menu->addAction(QIcon(":/icons/32x32/Cancel.png"), tr("Clear List"));
  connect(clear, &QAction::triggered, this, [](bool) {
    SETTINGS;
    cfg.setValue("Icons/lastIcons", {"Waypoint"});
  });

  // display the menu and block until mouse click
  QAction* action = menu->exec(QCursor::pos());

  // the action pointer is null if no menu item has been clicked
  // ignore the "more..." action
  if (action != nullptr && action != more) {
    icon = action->property("iconName").toString();
  }

  // prepend the list of last icons by the new icon, if any
  if (!icon.isEmpty()) {
    SETTINGS;
    QStringList lastIcons = cfg.value("Icons/lastIcons", {"Waypoint"}).toStringList();
    if (!lastIcons.contains(icon)) {
      lastIcons.push_front(icon);
    } else {
      lastIcons.removeAll(icon);
      lastIcons.push_front(icon);
    }
    if (lastIcons.size() > 10) {
      lastIcons.pop_back();
    }
    cfg.setValue("Icons/lastIcons", lastIcons);
  }
  return icon;
}

QString CWptIconManager::getNumberedBullet(qint32 n) {
  const QFont& font = CMainWindow::self().getMapFont();
  if (mapNumberedBullets.contains(n) && (lastFont == font)) {
    return mapNumberedBullets[n];
  }

  if (lastFont != font) {
    removeNumberedBullets();
    lastFont = font;
  }

  const QString& filename = QDir::temp().absoluteFilePath(QString("Bullet%1.png").arg(n));
  mapNumberedBullets[n] = filename;

  const QPixmap& pixmap = CDraw::number(n, Qt::black);
  pixmap.save(filename);

  return filename;
}

QMenu* CWptIconManager::getWptIconMenu(QWidget* parent) {
  QMenu* menu = new QMenu(parent);

  SETTINGS;
  const QStringList& lastIcons = cfg.value("Icons/lastIcons", {"Waypoint"}).toStringList();

  for (const QString& name : lastIcons) {
    bool ok = false;
    const icon_t& icon = findIconByName(name, ok);
    if (!ok) {
      continue;
    }
    const QPixmap& pixmap = loadIcon(icon.path);
    QAction* action = menu->addAction(pixmap, name);
    action->setProperty("iconName", name);
  }

  return menu;
}
