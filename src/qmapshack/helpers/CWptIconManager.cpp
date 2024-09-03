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

#include <QtWidgets>

#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "misc.h"
#include "setup/IAppSetup.h"

CWptIconManager* CWptIconManager::pSelf = nullptr;
const char* CWptIconManager::wptDefault = "://icons/waypoints/32x32/Default.png";

CWptIconManager::CWptIconManager(QObject* parent) {
  pSelf = this;

  init();
}

CWptIconManager::~CWptIconManager() {
  qDebug() << "CWptIconManager::~CWptIconManager()";
  removeNumberedBullets();
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

  wptIcons["Default"] = icon_t(wptDefault, 16, 16);
  wptIcons["City (Capitol)"] = icon_t("://icons/waypoints/32x32/CityCapitol.png", 16, 16);
  wptIcons["City (Large)"] = icon_t("://icons/waypoints/32x32/CityLarge.png", 16, 16);
  wptIcons["City (Medium)"] = icon_t("://icons/waypoints/32x32/CityMedium.png", 16, 16);
  wptIcons["City (Small)"] = icon_t("://icons/waypoints/32x32/CitySmall.png", 16, 16);
  wptIcons["Residence"] = icon_t("://icons/waypoints/32x32/Residence.png", 16, 16);
  wptIcons["Flag, Red"] = icon_t("://icons/waypoints/32x32/FlagRed.png", 0, 32);
  wptIcons["Flag, Blue"] = icon_t("://icons/waypoints/32x32/FlagBlue.png", 0, 32);
  wptIcons["Flag, Green"] = icon_t("://icons/waypoints/32x32/FlagGreen.png", 0, 32);
  wptIcons["Pin, Red"] = icon_t("://icons/waypoints/32x32/PinRed.png", 0, 32);
  wptIcons["Pin, Blue"] = icon_t("://icons/waypoints/32x32/PinBlue.png", 0, 32);
  wptIcons["Pin, Green"] = icon_t("://icons/waypoints/32x32/PinGreen.png", 0, 32);
  wptIcons["Block, Red"] = icon_t("://icons/waypoints/32x32/BoxRed.png", 16, 16);
  wptIcons["Block, Blue"] = icon_t("://icons/waypoints/32x32/BoxBlue.png", 16, 16);
  wptIcons["Block, Green"] = icon_t("://icons/waypoints/32x32/BoxGreen.png", 16, 16);
  wptIcons["Blue Diamond"] = icon_t("://icons/waypoints/32x32/DiamondBlue.png", 16, 16);
  wptIcons["Green Diamond"] = icon_t("://icons/waypoints/32x32/DiamondGreen.png", 16, 16);
  wptIcons["Red Diamond"] = icon_t("://icons/waypoints/32x32/DiamondRed.png", 16, 16);
  wptIcons["Parking Area"] = icon_t("://icons/cache/32x32/parking.png", 16, 16);
  wptIcons["Trailhead"] = icon_t("://icons/cache/32x32/trailhead.png", 16, 16);
  wptIcons["Waypoint"] = icon_t("://icons/waypoints/32x32/Waypoint.png", 16, 16);
  wptIcons["1st Category"] = icon_t("://icons/waypoints/32x32/1stCategory.png", 16, 16);
  wptIcons["2nd Category"] = icon_t("://icons/waypoints/32x32/2ndCategory.png", 16, 16);
  wptIcons["3rd Category"] = icon_t("://icons/waypoints/32x32/3rdCategory.png", 16, 16);
  wptIcons["4th Category"] = icon_t("://icons/waypoints/32x32/4thCategory.png", 16, 16);
  wptIcons["Danger"] = icon_t("://icons/waypoints/32x32/Danger.png", 16, 16);
  wptIcons["First Aid"] = icon_t("://icons/waypoints/32x32/FirstAid.png", 16, 16);
  wptIcons["Food"] = icon_t("://icons/waypoints/32x32/Food.png", 16, 16);
  wptIcons["Generic"] = icon_t("://icons/waypoints/32x32/Generic.png", 16, 16);
  wptIcons["Hors Category"] = icon_t("://icons/waypoints/32x32/HorsCategory.png", 16, 16);
  wptIcons["Left"] = icon_t("://icons/waypoints/32x32/Left.png", 16, 16);
  wptIcons["Right"] = icon_t("://icons/waypoints/32x32/Right.png", 16, 16);
  wptIcons["Sprint"] = icon_t("://icons/waypoints/32x32/Sprint.png", 16, 16);
  wptIcons["Straight"] = icon_t("://icons/waypoints/32x32/Straight.png", 16, 16);
  wptIcons["Summit"] = icon_t("://icons/waypoints/32x32/Summit.png", 16, 16);
  wptIcons["Valley"] = icon_t("://icons/waypoints/32x32/Valley.png", 16, 16);
  wptIcons["Water"] = icon_t("://icons/waypoints/32x32/Water.png", 16, 16);
  wptIcons["LeftFork"] = icon_t("://icons/waypoints/32x32/LeftFork.png", 16, 16);
  wptIcons["RightFork"] = icon_t("://icons/waypoints/32x32/RightFork.png", 16, 16);
  wptIcons["MiddleFork"] = icon_t("://icons/waypoints/32x32/MiddleFork.png", 16, 16);
  wptIcons["SlightLeft"] = icon_t("://icons/waypoints/32x32/SlightLeft.png", 16, 16);
  wptIcons["SharpLeft"] = icon_t("://icons/waypoints/32x32/SharpLeft.png", 16, 16);
  wptIcons["SlightRight"] = icon_t("://icons/waypoints/32x32/SlightRight.png", 16, 16);
  wptIcons["SharpRight"] = icon_t("://icons/waypoints/32x32/SharpRight.png", 16, 16);
  wptIcons["UTurn"] = icon_t("://icons/waypoints/32x32/UTurn.png", 16, 16);
  wptIcons["Start"] = icon_t("://icons/waypoints/32x32/Start.png", 16, 16);
  wptIcons["End"] = icon_t("://icons/waypoints/32x32/End.png", 16, 16);

  wptIcons["Bank"] = icon_t("://icons/poi/SJJB/png/money_bank.n.32.png", 16, 16);
  wptIcons["Gas Station"] = icon_t("://icons/poi/SJJB/png/transport_fuel.n.32.png", 16, 16);
  wptIcons["Restaurant"] = icon_t("://icons/poi/SJJB/png/food_restaurant.n.32.png", 16, 16);
  wptIcons["Bar"] = icon_t("://icons/poi/SJJB/png/food_bar.n.32.png", 16, 16);
  wptIcons["Shipwreck"] = icon_t("://icons/poi/SJJB/png/tourist_wreck.n.32.png", 16, 16);
  wptIcons["Boat Ramp"] = icon_t("://icons/poi/SJJB/png/transport_slipway.n.32.png", 16, 16);
  wptIcons["Campground"] = icon_t("://icons/poi/SJJB/png/accommodation_camping.n.32.png", 16, 16);
  wptIcons["Restroom"] = icon_t("://icons/poi/SJJB/png/amenity_toilets.n.32.png", 16, 16);
  wptIcons["Drinking Water"] = icon_t("://icons/poi/SJJB/png/food_drinkingtap.n.32.png", 16, 16);
  wptIcons["Telephone"] = icon_t("://icons/poi/SJJB/png/amenity_telephone.n.32.png", 16, 16);
  wptIcons["Medical Facility"] = icon_t("://icons/poi/SJJB/png/health_hospital.n.32.png", 16, 16);
  wptIcons["Information"] = icon_t("://icons/poi/SJJB/png/tourist_information.n.32.png", 16, 16);
  wptIcons["Parking Area"] = icon_t("://icons/poi/SJJB/png/transport_parking.n.32.png", 16, 16);
  wptIcons["Picnic Area"] = icon_t("://icons/poi/SJJB/png/tourist_picnic.n.32.png", 16, 16);
  wptIcons["Scenic Area"] = icon_t("://icons/poi/SJJB/png/tourist_view_point.n.32.png", 16, 16);
  wptIcons["Swimming Area"] = icon_t("://icons/poi/SJJB/png/sport_swimming_outdoor.n.32.png", 16, 16);
  wptIcons["Dam"] = icon_t("://icons/poi/SJJB/png/water_dam.n.32.png", 16, 16);
  wptIcons["Shopping Center"] = icon_t("://icons/poi/SJJB/png/shopping_supermarket.n.32.png", 16, 16);
  wptIcons["Lodging"] = icon_t("://icons/poi/SJJB/png/accommodation_hotel.n.32.png", 16, 16);
  wptIcons["Mine"] = icon_t("://icons/poi/SJJB/png/poi_mine.n.32.png", 16, 16);
  wptIcons["Car Rental"] = icon_t("://icons/poi/SJJB/png/transport_rental_car.n.32.png", 16, 16);
  wptIcons["Car Repair"] = icon_t("://icons/poi/SJJB/png/shopping_car_repair.n.32.png", 16, 16);
  wptIcons["Fast Food"] = icon_t("://icons/poi/SJJB/png/food_fastfood.n.32.png", 16, 16);
  wptIcons["Fitness Center"] = icon_t("://icons/poi/SJJB/png/sport_gym.n.32.png", 16, 16);
  wptIcons["Museum"] = icon_t("://icons/poi/SJJB/png/tourist_museum.n.32.png", 16, 16);
  wptIcons["Pharmacy"] = icon_t("://icons/poi/SJJB/png/health_pharmacy.n.32.png", 16, 16);
  wptIcons["Pizza"] = icon_t("://icons/poi/SJJB/png/food_pizza.n.32.png", 16, 16);
  wptIcons["Post Office"] = icon_t("://icons/poi/SJJB/png/amenity_post_office.n.32.png", 16, 16);
  wptIcons["RV Park"] = icon_t("://icons/poi/SJJB/png/accommodation_caravan_park.n.32.png", 16, 16);
  wptIcons["School"] = icon_t("://icons/poi/SJJB/png/education_school.n.32.png", 16, 16);
  wptIcons["Stadium"] = icon_t("://icons/poi/SJJB/png/sport_stadium.n.32.png", 16, 16);
  wptIcons["Department Store"] = icon_t("://icons/poi/SJJB/png/shopping_department_store.n.32.png", 16, 16);
  wptIcons["Zoo"] = icon_t("://icons/poi/SJJB/png/tourist_zoo.n.32.png", 16, 16);
  wptIcons["Convenience Store"] = icon_t("://icons/poi/SJJB/png/shopping_convenience.n.32.png", 16, 16);
  wptIcons["Live Theater"] = icon_t("://icons/poi/SJJB/png/tourist_theatre.n.32.png", 16, 16);
  wptIcons["Scales"] = icon_t("://icons/poi/SJJB/png/amenity_court.n.32.png", 16, 16);
  wptIcons["Church"] = icon_t("://icons/poi/SJJB/png/place_of_worship_unknown.n.32.png", 16, 16);
  wptIcons["Ghost Town"] = icon_t("://icons/poi/SJJB/png/tourist_ruin.n.32.png", 16, 16);
  wptIcons["Beach"] = icon_t("://icons/poi/SJJB/png/tourist_beach.n.32.png", 16, 16);
  wptIcons["Forest"] = icon_t("://icons/poi/SJJB/png/landuse_coniferous_and_deciduous.n.32.png", 16, 16);
  wptIcons["Airport"] = icon_t("://icons/poi/SJJB/png/transport_airport.n.32.png", 16, 16);
  wptIcons["Heliport"] = icon_t("://icons/poi/SJJB/png/transport_helicopter_pad.n.32.png", 16, 16);
  wptIcons["Tall Tower"] = icon_t("://icons/poi/SJJB/png/poi_tower_communications.n.32.png", 16, 16);
  wptIcons["Short Tower"] = icon_t("://icons/poi/SJJB/png/poi_tower_lookout.n.32.png", 16, 16);
  wptIcons["Library"] = icon_t("://icons/poi/SJJB/png/amenity_library.n.32.png", 16, 16);
  wptIcons["Ground Transportation"] = icon_t("://icons/poi/SJJB/png/transport_bus_stop.n.32.png", 16, 16);
  wptIcons["City Hall"] = icon_t("://icons/poi/SJJB/png/amenity_town_hall.n.32.png", 16, 16);
  wptIcons["Winery"] = icon_t("://icons/poi/SJJB/png/shopping_alcohol.n.32.png", 16, 16);
  wptIcons["Lodge"] = icon_t("://icons/poi/SJJB/png/accommodation_shelter.n.32.png", 16, 16);
  wptIcons["Railway"] = icon_t("://icons/poi/SJJB/png/transport_train_station.n.32.png", 16, 16);
  wptIcons["Parking, Pay"] = icon_t("://icons/poi/SJJB/png/transport_parking_car_paid.n.32.png", 16, 16);

  setWptIconByName("Traditional Cache", "://icons/geocaching/icons/traditional.png");
  setWptIconByName("Multi-cache", "://icons/geocaching/icons/multi.png");
  setWptIconByName("Unknown Cache", "://icons/geocaching/icons/unknown.png");
  setWptIconByName("Wherigo Cache", "://icons/geocaching/icons/wherigo.png");
  setWptIconByName("Event Cache", "://icons/geocaching/icons/event.png");
  setWptIconByName("Mega-Event Cache", "://icons/geocaching/icons/mega.png");
  setWptIconByName("Giga-Event Cache", "://icons/geocaching/icons/giga.png");
  setWptIconByName("Cache In Trash Out Event", "://icons/geocaching/icons/cito.png");
  setWptIconByName("Earthcache", "://icons/geocaching/icons/earth.png");
  setWptIconByName("Letterbox Hybrid", "://icons/geocaching/icons/letterbox.png");
  setWptIconByName("Virtual Cache", "://icons/geocaching/icons/virtual.png");
  setWptIconByName("Webcam Cache", "://icons/geocaching/icons/webcam.png");

  setWptIconByName("gray_Traditional Cache", createGrayscale("://icons/geocaching/icons/traditional.png"));
  setWptIconByName("gray_Multi-cache", createGrayscale("://icons/geocaching/icons/multi.png"));
  setWptIconByName("gray_Unknown Cache", createGrayscale("://icons/geocaching/icons/unknown.png"));
  setWptIconByName("gray_Wherigo Cache", createGrayscale("://icons/geocaching/icons/wherigo.png"));
  setWptIconByName("gray_Event Cache", createGrayscale("://icons/geocaching/icons/event.png"));
  setWptIconByName("gray_Mega-Event Cache", createGrayscale("://icons/geocaching/icons/mega.png"));
  setWptIconByName("gray_Giga-Event Cache", createGrayscale("://icons/geocaching/icons/giga.png"));
  setWptIconByName("gray_Cache In Trash Out Event", createGrayscale("://icons/geocaching/icons/cito.png"));
  setWptIconByName("gray_Earthcache", createGrayscale("://icons/geocaching/icons/earth.png"));
  setWptIconByName("gray_Letterbox Hybrid", createGrayscale("://icons/geocaching/icons/letterbox.png"));
  setWptIconByName("gray_Virtual Cache", createGrayscale("://icons/geocaching/icons/virtual.png"));
  setWptIconByName("gray_Webcam Cache", createGrayscale("://icons/geocaching/icons/webcam.png"));

  SETTINGS;
  QDir dirIcon(
      cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString());

  const QStringList& filenames = dirIcon.entryList(QString("*.bmp *.png").split(" "), QDir::Files);

  for (const QString& filename : filenames) {
    QFileInfo fi(filename);
    QString name = fi.completeBaseName();
    setWptIconByName(name, dirIcon.filePath(filename));
  }
}

void CWptIconManager::setWptIconByName(const QString& name, const QString& filename) {
  QImage icon(filename);
  wptIcons[name] = (icon.offset() == QPoint(0, 0)) ? icon_t(filename, icon.width() >> 1, icon.height() >> 1)
                                                   : icon_t(filename, icon.offset().x(), icon.offset().y());
}

void CWptIconManager::setWptIconByName(const QString& name, const QPixmap& icon) {
  SETTINGS;
  QDir dirIcon(
      cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString());
  QString filename = dirIcon.filePath(name + ".png");

  icon.save(filename);
  wptIcons[name] = icon_t(filename, icon.width() >> 1, icon.height() >> 1);
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

  // Limit icon size to 22 pixel max.
  if (icon.width() > 22 || icon.height() > 22) {
    qreal s;
    if (icon.width() > icon.height()) {
      s = 22.0 / icon.width();
    } else {
      s = 22.0 / icon.height();
    }

    focus = focus * s;
    icon = icon.scaled(icon.size() * s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  return icon;
}

QString CWptIconManager::selectWptIcon(QWidget* parent) {
  QString icon;

  QMenu* menu = getWptIconMenu("", nullptr, "", parent);
  QAction* action = menu->exec(QCursor::pos());

  if (action != nullptr) {
    icon = action->property("iconName").toString();
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

QMenu* CWptIconManager::getWptIconMenu(const QString& title, QObject* obj, const char* slot, QWidget* parent) {
  QMenu* menu = new QMenu(title, parent);
  menu->setIcon(QIcon("://icons/waypoints/32x32/PinBlue.png"));

  const QMap<QString, icon_t>& wptIcons = getWptIcons();
  QStringList keys = wptIcons.keys();

  std::sort(keys.begin(), keys.end(), sortByString);

  for (const QString& key : std::as_const(keys)) {
    const QString& icon = wptIcons[key].path;
    QPixmap pixmap = loadIcon(icon);

    QAction* action = menu->addAction(pixmap, key);
    action->setProperty("iconName", key);
    if (obj != nullptr) {
      QAction::connect(action, SIGNAL(triggered(bool)), obj, slot);
    }
  }

  return menu;
}
