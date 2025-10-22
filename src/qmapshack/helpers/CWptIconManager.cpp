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
#include "misc.h"
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
  wptIcons.clear();

  wptIcons["Default"] = icon_t(wptDefault, 16, 16, {tr("markers")}, {});
  wptIcons["City (Capitol)"] = icon_t("://icons/waypoints/32x32/CityCapitol.png", 16, 16, {tr("markers")}, {tr("dot")});
  wptIcons["City (Large)"] = icon_t("://icons/waypoints/32x32/CityLarge.png", 16, 16, {tr("markers")}, {tr("dot")});
  wptIcons["City (Medium)"] = icon_t("://icons/waypoints/32x32/CityMedium.png", 16, 16, {tr("markers")}, {tr("dot")});
  wptIcons["City (Small)"] = icon_t("://icons/waypoints/32x32/CitySmall.png", 16, 16, {tr("markers")}, {tr("dot")});
  wptIcons["Flag, Red"] = icon_t("://icons/waypoints/32x32/FlagRed.png", 0, 32, {tr("markers")}, {});
  wptIcons["Flag, Blue"] = icon_t("://icons/waypoints/32x32/FlagBlue.png", 0, 32, {tr("markers")}, {});
  wptIcons["Flag, Green"] = icon_t("://icons/waypoints/32x32/FlagGreen.png", 0, 32, {tr("markers")}, {});
  wptIcons["Pin, Red"] = icon_t("://icons/waypoints/32x32/PinRed.png", 0, 32, {tr("markers")}, {});
  wptIcons["Pin, Blue"] = icon_t("://icons/waypoints/32x32/PinBlue.png", 0, 32, {tr("markers")}, {});
  wptIcons["Pin, Green"] = icon_t("://icons/waypoints/32x32/PinGreen.png", 0, 32, {tr("markers")}, {});
  wptIcons["Block, Red"] = icon_t("://icons/waypoints/32x32/BoxRed.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Block, Blue"] = icon_t("://icons/waypoints/32x32/BoxBlue.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Block, Green"] = icon_t("://icons/waypoints/32x32/BoxGreen.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Circle, Red"] = icon_t("://icons/waypoints/32x32/CircleRed.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Circle, Blue"] = icon_t("://icons/waypoints/32x32/CircleBlue.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Circle, Green"] =
      icon_t("://icons/waypoints/32x32/CircleGreen.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Diamond, Red"] = icon_t("://icons/waypoints/32x32/DiamondRed.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Diamond, Blue"] =
      icon_t("://icons/waypoints/32x32/DiamondBlue.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Diamond, Green"] =
      icon_t("://icons/waypoints/32x32/DiamondGreen.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Oval, Red"] = icon_t("://icons/waypoints/32x32/OvalRed.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Oval, Blue"] = icon_t("://icons/waypoints/32x32/OvalBlue.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Oval, Green"] = icon_t("://icons/waypoints/32x32/OvalGreen.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Rectangle, Red"] =
      icon_t("://icons/waypoints/32x32/RectangleRed.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Rectangle, Blue"] =
      icon_t("://icons/waypoints/32x32/RectangleBlue.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Rectangle, Green"] =
      icon_t("://icons/waypoints/32x32/RectangleGreen.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Square, Red"] = icon_t("://icons/waypoints/32x32/SquareRed.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Square, Blue"] = icon_t("://icons/waypoints/32x32/SquareBlue.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Square, Green"] =
      icon_t("://icons/waypoints/32x32/SquareGreen.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Triangle, Red"] =
      icon_t("://icons/waypoints/32x32/TriangleRed.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Triangle, Blue"] =
      icon_t("://icons/waypoints/32x32/TriangleBlue.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Triangle, Green"] =
      icon_t("://icons/waypoints/32x32/TriangleGreen.png", 16, 16, {tr("markers")}, {tr("shape")});
  wptIcons["Letter A, Red"] =
      icon_t("://icons/waypoints/32x32/LetterARed.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter A, Blue"] =
      icon_t("://icons/waypoints/32x32/LetterABlue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter A, Green"] =
      icon_t("://icons/waypoints/32x32/LetterAGreen.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter B, Red"] =
      icon_t("://icons/waypoints/32x32/LetterBRed.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter B, Blue"] =
      icon_t("://icons/waypoints/32x32/LetterBBlue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter B, Green"] =
      icon_t("://icons/waypoints/32x32/LetterBGreen.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter C, Red"] =
      icon_t("://icons/waypoints/32x32/LetterCRed.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter C, Blue"] =
      icon_t("://icons/waypoints/32x32/LetterCBlue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter C, Green"] =
      icon_t("://icons/waypoints/32x32/LetterCGreen.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter D, Red"] =
      icon_t("://icons/waypoints/32x32/LetterDRed.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter D, Blue"] =
      icon_t("://icons/waypoints/32x32/LetterDBlue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Letter D, Green"] =
      icon_t("://icons/waypoints/32x32/LetterDGreen.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 0, Red"] =
      icon_t("://icons/waypoints/32x32/Number0Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 0, Blue"] =
      icon_t("://icons/waypoints/32x32/Number0Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 0, Green"] =
      icon_t("://icons/waypoints/32x32/Number0Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 1, Red"] =
      icon_t("://icons/waypoints/32x32/Number1Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 1, Blue"] =
      icon_t("://icons/waypoints/32x32/Number1Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 1, Green"] =
      icon_t("://icons/waypoints/32x32/Number1Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 2, Red"] =
      icon_t("://icons/waypoints/32x32/Number2Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 2, Blue"] =
      icon_t("://icons/waypoints/32x32/Number2Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 2, Green"] =
      icon_t("://icons/waypoints/32x32/Number2Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 3, Red"] =
      icon_t("://icons/waypoints/32x32/Number3Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 3, Blue"] =
      icon_t("://icons/waypoints/32x32/Number3Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 3, Green"] =
      icon_t("://icons/waypoints/32x32/Number3Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 4, Red"] =
      icon_t("://icons/waypoints/32x32/Number4Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 4, Blue"] =
      icon_t("://icons/waypoints/32x32/Number4Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 4, Green"] =
      icon_t("://icons/waypoints/32x32/Number4Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 5, Red"] =
      icon_t("://icons/waypoints/32x32/Number5Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 5, Blue"] =
      icon_t("://icons/waypoints/32x32/Number5Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 5, Green"] =
      icon_t("://icons/waypoints/32x32/Number5Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 6, Red"] =
      icon_t("://icons/waypoints/32x32/Number6Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 6, Blue"] =
      icon_t("://icons/waypoints/32x32/Number6Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 6, Green"] =
      icon_t("://icons/waypoints/32x32/Number6Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 7, Red"] =
      icon_t("://icons/waypoints/32x32/Number7Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 7, Blue"] =
      icon_t("://icons/waypoints/32x32/Number7Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 7, Green"] =
      icon_t("://icons/waypoints/32x32/Number7Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 8, Red"] =
      icon_t("://icons/waypoints/32x32/Number8Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 8, Blue"] =
      icon_t("://icons/waypoints/32x32/Number8Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 8, Green"] =
      icon_t("://icons/waypoints/32x32/Number8Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 9, Red"] =
      icon_t("://icons/waypoints/32x32/Number9Red.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 9, Blue"] =
      icon_t("://icons/waypoints/32x32/Number9Blue.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Number 9, Green"] =
      icon_t("://icons/waypoints/32x32/Number9Green.png", 16, 16, {tr("markers")}, {tr("alphanumerical")});
  wptIcons["Residence"] = icon_t("://icons/waypoints/32x32/Residence.png", 16, 16, {tr("markers")}, {tr("building")});
  wptIcons["Favorite"] = icon_t("://icons/waypoints/32x32/Favorite.png", 16, 16, {tr("markers")}, {});
  wptIcons["Flag"] = icon_t("://icons/waypoints/32x32/FlagRed.png", 0, 32, {tr("markers")}, {});
  wptIcons["Trail Head"] = icon_t("://icons/cache/32x32/trailhead.png", 16, 16, {tr("markers")}, {});
  wptIcons["Contact, Smiley"] = icon_t("://icons/cache/32x32/found.png", 16, 16, {tr("markers")}, {});
  wptIcons["Sad Face"] = icon_t("://icons/cache/32x32/dnf.png", 16, 16, {tr("markers")}, {});
  wptIcons["Waypoint"] = icon_t("://icons/waypoints/32x32/Waypoint.png", 16, 16, {tr("markers")}, {tr("dot")});

  wptIcons["1st Category"] = icon_t("://icons/waypoints/32x32/1stCategory.png", 16, 16, {tr("cycling")}, {});
  wptIcons["2nd Category"] = icon_t("://icons/waypoints/32x32/2ndCategory.png", 16, 16, {tr("cycling")}, {});
  wptIcons["3rd Category"] = icon_t("://icons/waypoints/32x32/3rdCategory.png", 16, 16, {tr("cycling")}, {});
  wptIcons["4th Category"] = icon_t("://icons/waypoints/32x32/4thCategory.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Danger"] = icon_t("://icons/waypoints/32x32/Danger.png", 16, 16, {tr("cycling")}, {});
  wptIcons["First Aid"] = icon_t("://icons/waypoints/32x32/FirstAid.png", 16, 16, {tr("cycling")}, {tr("help")});
  wptIcons["Food"] = icon_t("://icons/waypoints/32x32/Food.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Generic"] = icon_t("://icons/waypoints/32x32/Generic.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Hors Category"] = icon_t("://icons/waypoints/32x32/HorsCategory.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Left"] = icon_t("://icons/waypoints/32x32/Left.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Right"] = icon_t("://icons/waypoints/32x32/Right.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Sprint"] = icon_t("://icons/waypoints/32x32/Sprint.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Straight"] = icon_t("://icons/waypoints/32x32/Straight.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Summit"] = icon_t("://icons/waypoints/32x32/Summit.png", 16, 16, {tr("cycling")}, {tr("land")});
  wptIcons["Valley"] = icon_t("://icons/waypoints/32x32/Valley.png", 16, 16, {tr("cycling")}, {tr("land")});
  wptIcons["Water"] = icon_t("://icons/waypoints/32x32/Water.png", 16, 16, {tr("cycling")}, {tr("food")});
  wptIcons["LeftFork"] = icon_t("://icons/waypoints/32x32/LeftFork.png", 16, 16, {tr("cycling")}, {});
  wptIcons["RightFork"] = icon_t("://icons/waypoints/32x32/RightFork.png", 16, 16, {tr("cycling")}, {});
  wptIcons["MiddleFork"] = icon_t("://icons/waypoints/32x32/MiddleFork.png", 16, 16, {tr("cycling")}, {});
  wptIcons["SlightLeft"] = icon_t("://icons/waypoints/32x32/SlightLeft.png", 16, 16, {tr("cycling")}, {});
  wptIcons["SharpLeft"] = icon_t("://icons/waypoints/32x32/SharpLeft.png", 16, 16, {tr("cycling")}, {});
  wptIcons["SlightRight"] = icon_t("://icons/waypoints/32x32/SlightRight.png", 16, 16, {tr("cycling")}, {});
  wptIcons["SharpRight"] = icon_t("://icons/waypoints/32x32/SharpRight.png", 16, 16, {tr("cycling")}, {});
  wptIcons["UTurn"] = icon_t("://icons/waypoints/32x32/UTurn.png", 16, 16, {tr("cycling")}, {});
  wptIcons["Start"] = icon_t("://icons/waypoints/32x32/Start.png", 16, 16, {tr("cycling")}, {});
  wptIcons["End"] = icon_t("://icons/waypoints/32x32/End.png", 16, 16, {tr("cycling")}, {});

  wptIcons["Bank"] = icon_t("://icons/poi/SJJB/png/money_bank.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Church"] =
      icon_t("://icons/poi/SJJB/png/place_of_worship_unknown.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Mine"] = icon_t("://icons/poi/SJJB/png/poi_mine.n.32.png", 16, 16, {tr("poi")}, {});
  wptIcons["Tall Tower"] =
      icon_t("://icons/poi/SJJB/png/poi_tower_communications.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Short Tower"] =
      icon_t("://icons/poi/SJJB/png/poi_tower_lookout.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Gas Station"] = icon_t("://icons/poi/SJJB/png/transport_fuel.n.32.png", 16, 16,
                                   {tr("poi"), tr("transport"), tr("marine")}, {tr("building")});
  wptIcons["Parking Area"] =
      icon_t("://icons/poi/SJJB/png/transport_parking.n.32.png", 16, 16, {tr("poi"), tr("transport")}, {});
  wptIcons["Parking, Pay"] =
      icon_t("://icons/poi/SJJB/png/transport_parking_car_paid.n.32.png", 16, 16, {tr("poi"), tr("transport")}, {});
  wptIcons["Car Rental"] =
      icon_t("://icons/poi/SJJB/png/transport_rental_car.n.32.png", 16, 16, {tr("poi"), tr("transport")}, {});
  wptIcons["Boat Ramp"] =
      icon_t("://icons/poi/SJJB/png/transport_slipway.n.32.png", 16, 16, {tr("poi"), tr("marine")}, {});
  wptIcons["Marina"] = icon_t("://icons/poi/SJJB/png/transport_marina.n.32.png", 16, 16, {tr("poi"), tr("marine")}, {});
  wptIcons["Ground Transportation"] =
      icon_t("://icons/poi/SJJB/png/transport_bus_stop.n.32.png", 16, 16, {tr("poi"), tr("transport")}, {});
  wptIcons["Railway"] =
      icon_t("://icons/poi/SJJB/png/transport_train_station.n.32.png", 16, 16, {tr("poi"), tr("transport")}, {});
  wptIcons["Airport"] =
      icon_t("://icons/poi/SJJB/png/transport_airport.n.32.png", 16, 16, {tr("poi"), tr("transport")}, {});
  wptIcons["Heliport"] =
      icon_t("://icons/poi/SJJB/png/transport_helicopter_pad.n.32.png", 16, 16, {tr("poi"), tr("transport")}, {});
  wptIcons["Campground"] = icon_t("://icons/poi/SJJB/png/accommodation_camping.n.32.png", 16, 16,
                                  {tr("poi"), tr("outdoors"), tr("marine"), tr("accommodation")}, {});
  wptIcons["Lodge"] = icon_t("://icons/poi/SJJB/png/accommodation_shelter.n.32.png", 16, 16,
                             {tr("poi"), tr("accommodation")}, {tr("building")});
  wptIcons["Lodging"] = icon_t("://icons/poi/SJJB/png/accommodation_hotel.n.32.png", 16, 16,
                               {tr("poi"), tr("accommodation")}, {tr("building")});
  wptIcons["RV Park"] = icon_t("://icons/poi/SJJB/png/accommodation_caravan_park.n.32.png", 16, 16,
                               {tr("poi"), tr("outdoors"), tr("accommodation")}, {});
  wptIcons["Dam"] = icon_t("://icons/poi/SJJB/png/water_dam.n.32.png", 16, 16, {tr("poi")}, {tr("landmark")});
  wptIcons["Bar"] =
      icon_t("://icons/poi/SJJB/png/food_bar.n.32.png", 16, 16, {tr("poi")}, {tr("building"), tr("food")});
  wptIcons["Drinking Water"] =
      icon_t("://icons/poi/SJJB/png/food_drinkingtap.n.32.png", 16, 16, {tr("poi")}, {tr("food")});
  wptIcons["Fast Food"] = icon_t("://icons/poi/SJJB/png/food_fastfood.n.32.png", 16, 16, {tr("poi")}, {tr("food")});
  wptIcons["Pizza"] = icon_t("://icons/poi/SJJB/png/food_pizza.n.32.png", 16, 16, {tr("poi")}, {tr("food")});
  wptIcons["Restaurant"] = icon_t("://icons/poi/SJJB/png/food_restaurant.n.32.png", 16, 16,
                                  {tr("poi"), tr("transport"), tr("marine")}, {tr("building"), tr("food")});
  wptIcons["Amusement Park"] =
      icon_t("://icons/poi/SJJB/png/tourist_theme_park.n.32.png", 16, 16, {tr("poi")}, {tr("landmark")});
  wptIcons["Beach"] = icon_t("://icons/poi/SJJB/png/tourist_beach.n.32.png", 16, 16, {tr("poi")}, {});
  wptIcons["Ghost Town"] =
      icon_t("://icons/poi/SJJB/png/tourist_ruin.n.32.png", 16, 16, {tr("poi")}, {tr("building"), tr("landmark")});
  wptIcons["Information"] = icon_t("://icons/poi/SJJB/png/tourist_information.n.32.png", 16, 16, {tr("poi")}, {});
  wptIcons["Live Theater"] =
      icon_t("://icons/poi/SJJB/png/tourist_theatre.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Museum"] = icon_t("://icons/poi/SJJB/png/tourist_museum.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Picnic Area"] = icon_t("://icons/poi/SJJB/png/tourist_picnic.n.32.png", 16, 16, {tr("poi")}, {tr("food")});
  wptIcons["Scenic Area"] = icon_t("://icons/poi/SJJB/png/tourist_view_point.n.32.png", 16, 16, {tr("poi")}, {});
  wptIcons["Shipwreck"] =
      icon_t("://icons/poi/SJJB/png/tourist_wreck.n.32.png", 16, 16, {tr("poi"), tr("marine")}, {tr("landmark")});
  wptIcons["Zoo"] = icon_t("://icons/poi/SJJB/png/tourist_zoo.n.32.png", 16, 16, {tr("poi")}, {tr("landmark")});
  wptIcons["City Hall"] =
      icon_t("://icons/poi/SJJB/png/amenity_town_hall.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Library"] = icon_t("://icons/poi/SJJB/png/amenity_library.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Police Station"] =
      icon_t("://icons/poi/SJJB/png/amenity_police.n.32.png", 16, 16, {tr("poi")}, {tr("building"), tr("help")});
  wptIcons["Post Office"] =
      icon_t("://icons/poi/SJJB/png/amenity_post_office.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Restroom"] = icon_t("://icons/poi/SJJB/png/amenity_toilets.n.32.png", 16, 16,
                                {
                                    tr("poi"),
                                    tr("outdoors"),
                                    tr("transport"),
                                },
                                {tr("building")});
  wptIcons["Scales"] = icon_t("://icons/poi/SJJB/png/amenity_court.n.32.png", 16, 16, {tr("poi")}, {});
  wptIcons["Telephone"] = icon_t("://icons/poi/SJJB/png/amenity_telephone.n.32.png", 16, 16, {tr("poi")}, {});
  wptIcons["Medical Facility"] =
      icon_t("://icons/poi/SJJB/png/health_hospital.n.32.png", 16, 16, {tr("poi")}, {tr("help")});
  wptIcons["Pharmacy"] = icon_t("://icons/poi/SJJB/png/health_pharmacy.n.32.png", 16, 16, {tr("poi")}, {tr("help")});
  wptIcons["School"] = icon_t("://icons/poi/SJJB/png/education_school.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Fitness Center"] = icon_t("://icons/poi/SJJB/png/sport_gym.n.32.png", 16, 16, {tr("poi"), tr("sport")},
                                      {tr("building"), tr("sport")});
  wptIcons["Golf Course"] =
      icon_t("://icons/poi/SJJB/png/sport_golf.n.32.png", 16, 16, {tr("poi"), tr("sport")}, {tr("sport")});
  wptIcons["Ice Skating"] =
      icon_t("://icons/poi/SJJB/png/sport_iceskating.n.32.png", 16, 16, {tr("poi"), tr("sport")}, {tr("sport")});
  wptIcons["Skiing Area"] = icon_t("://icons/poi/SJJB/png/sport_skiing_crosscountry.n.32.png", 16, 16,
                                   {tr("poi"), tr("sport")}, {tr("sport")});
  wptIcons["Ski Resort"] =
      icon_t("://icons/poi/SJJB/png/sport_skiing_downhill.n.32.png", 16, 16, {tr("poi"), tr("sport")}, {tr("sport")});
  wptIcons["Stadium"] =
      icon_t("://icons/poi/SJJB/png/sport_stadium.n.32.png", 16, 16, {tr("poi"), tr("sport")}, {tr("sport")});
  wptIcons["Swimming Area"] =
      icon_t("://icons/poi/SJJB/png/sport_swimming_outdoor.n.32.png", 16, 16, {tr("poi"), tr("sport")}, {tr("sport")});
  wptIcons["Car Repair"] = icon_t("://icons/poi/SJJB/png/shopping_car_repair.n.32.png", 16, 16,
                                  {tr("poi"), tr("transport")}, {tr("building")});
  wptIcons["Convenience Store"] =
      icon_t("://icons/poi/SJJB/png/shopping_convenience.n.32.png", 16, 16, {tr("poi")}, {tr("building"), tr("food")});
  wptIcons["Department Store"] = icon_t("://icons/poi/SJJB/png/shopping_department_store.n.32.png", 16, 16, {tr("poi")},
                                        {tr("building"), tr("food")});
  wptIcons["Shopping Center"] =
      icon_t("://icons/poi/SJJB/png/shopping_supermarket.n.32.png", 16, 16, {tr("poi")}, {tr("building")});
  wptIcons["Winery"] = icon_t("://icons/poi/SJJB/png/shopping_alcohol.n.32.png", 16, 16, {tr("poi")}, {tr("food")});
  wptIcons["Forest"] =
      icon_t("://icons/poi/SJJB/png/landuse_coniferous_and_deciduous.n.32.png", 16, 16, {tr("poi")}, {tr("land")});
  wptIcons["Park"] = icon_t("://icons/poi/SJJB/png/landuse_deciduous.n.32.png", 16, 16, {tr("poi")}, {tr("land")});

  setWptIconByName("Traditional Cache", "://icons/geocaching/icons/traditional.png", {tr("geocache")}, {});
  setWptIconByName("Multi-Cache", "://icons/geocaching/icons/multi.png", {tr("geocache")}, {});
  setWptIconByName("Unknown Cache", "://icons/geocaching/icons/unknown.png", {tr("geocache")}, {});
  setWptIconByName("Wherigo Cache", "://icons/geocaching/icons/wherigo.png", {tr("geocache")}, {});
  setWptIconByName("Event Cache", "://icons/geocaching/icons/event.png", {tr("geocache")}, {});
  setWptIconByName("Mega-Event Cache", "://icons/geocaching/icons/mega.png", {tr("geocache")}, {});
  setWptIconByName("Giga-Event Cache", "://icons/geocaching/icons/giga.png", {tr("geocache")}, {});
  setWptIconByName("Cache In Trash Out Event", "://icons/geocaching/icons/cito.png", {tr("geocache")}, {});
  setWptIconByName("Earthcache", "://icons/geocaching/icons/earth.png", {tr("geocache")}, {});
  setWptIconByName("Letterbox Cache", "://icons/geocaching/icons/letterbox.png", {tr("geocache")}, {});
  setWptIconByName("Virtual Cache", "://icons/geocaching/icons/virtual.png", {tr("geocache")}, {});
  setWptIconByName("Webcam Cache", "://icons/geocaching/icons/webcam.png", {tr("geocache")}, {});

  setWptIconByName("gray_Traditional Cache", createGrayscale("://icons/geocaching/icons/traditional.png"),
                   {tr("geocache")}, {});
  setWptIconByName("gray_Multi-Cache", createGrayscale("://icons/geocaching/icons/multi.png"), {tr("geocache")}, {});
  setWptIconByName("gray_Unknown Cache", createGrayscale("://icons/geocaching/icons/unknown.png"), {tr("geocache")},
                   {});
  setWptIconByName("gray_Wherigo Cache", createGrayscale("://icons/geocaching/icons/wherigo.png"), {tr("geocache")},
                   {});
  setWptIconByName("gray_Event Cache", createGrayscale("://icons/geocaching/icons/event.png"), {tr("geocache")}, {});
  setWptIconByName("gray_Mega-Event Cache", createGrayscale("://icons/geocaching/icons/mega.png"), {tr("geocache")},
                   {});
  setWptIconByName("gray_Giga-Event Cache", createGrayscale("://icons/geocaching/icons/giga.png"), {tr("geocache")},
                   {});
  setWptIconByName("gray_Cache In Trash Out Event", createGrayscale("://icons/geocaching/icons/cito.png"),
                   {tr("geocache")}, {});
  setWptIconByName("gray_Earthcache", createGrayscale("://icons/geocaching/icons/earth.png"), {tr("geocache")}, {});
  setWptIconByName("gray_Letterbox Cache", createGrayscale("://icons/geocaching/icons/letterbox.png"), {tr("geocache")},
                   {});
  setWptIconByName("gray_Virtual Cache", createGrayscale("://icons/geocaching/icons/virtual.png"), {tr("geocache")},
                   {});
  setWptIconByName("gray_Webcam Cache", createGrayscale("://icons/geocaching/icons/webcam.png"), {tr("geocache")}, {});

  SETTINGS;
  QDir dirIcon(
      cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString());

  const QStringList& filenames = dirIcon.entryList(QString("*.bmp *.png").split(" "), QDir::Files);

  for (const QString& filename : filenames) {
    QFileInfo fi(filename);
    QString name = fi.completeBaseName();
    setWptIconByName(name, dirIcon.filePath(filename), {tr("external")}, {});
  }

  emit sigChanged();
}

void CWptIconManager::setWptIconByName(const QString& name, const QString& filename, const QStringList& categories,
                                       const QStringList& tags) {
  QImage icon(filename);
  wptIcons[name] = (icon.offset() == QPoint(0, 0))
                       ? icon_t(filename, icon.width() >> 1, icon.height() >> 1, categories, tags)
                       : icon_t(filename, icon.offset().x(), icon.offset().y(), categories, tags);
}

void CWptIconManager::setWptIconByName(const QString& name, const QPixmap& icon, const QStringList& categories,
                                       const QStringList& tags) {
  SETTINGS;
  QDir dirIcon(
      cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString());
  QString filename = dirIcon.filePath(name + ".png");

  icon.save(filename);
  wptIcons[name] = icon_t(filename, icon.width() >> 1, icon.height() >> 1, categories, tags);
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
  QPixmap icon;
  QString path;

  if (wptIcons.contains(name)) {
    focus = wptIcons[name].focus;
    path = wptIcons[name].path;
  } else {
    focus = wptIcons["Default"].focus;
    path = wptIcons["Default"].path;
  }

  if (path.isEmpty()) {
    path = wptDefault;
  }

  if (src) {
    *src = path;
  }

  icon = loadIcon(path);

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
  QPixmap icon;
  QString path;

  if (wptIcons.contains(name)) {
    focus = wptIcons[name].focus;
    path = wptIcons[name].path;
  } else {
    focus = wptIcons["Default"].focus;
    path = wptIcons["Default"].path;
  }

  if (path.isEmpty()) {
    path = wptDefault;
  }

  icon = loadIcon(path);

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
  QAction* more = menu->addAction(tr("more..."));
  connect(more, &QAction::triggered, this, [&icon, this](bool) {
    CWptIconDialog dlg(&CMainWindow::self());
    connect(&dlg, &CWptIconDialog::sigSelectedIcon, this, [&icon](const QString& name) { icon = name; });
    dlg.exec();
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
  const QMap<QString, icon_t>& wptIcons = getWptIcons();

  for (const QString& key : lastIcons) {
    const QString& icon = wptIcons[key].path;
    const QPixmap& pixmap = loadIcon(icon);

    QAction* action = menu->addAction(pixmap, key);
    action->setProperty("iconName", key);
  }

  return menu;
}
