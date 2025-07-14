/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "poi/IPoiFile.h"

#include "poi/CPoiDraw.h"
#include "poi/CPoiPropSetup.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconManager.h"

QSize IPoiFile::poiSize;
QImage IPoiFile::poiHighlight;
QImage IPoiFile::poiHighlightScaled;

void IPoiFile::init() {
  poiHighlight = QImage("://cursors/poiHighlightRed.png");
  SETTINGS;
  // default POI icon size is "DEFAULTICONSIZE"
  setIconSize(cfg.value("Icons/sizePoi", DEFAULTICONSIZE).toInt());
}

IPoiFile::IPoiFile(CPoiDraw* parent)
    : IDrawObject(parent),
      poi(parent)
{}

IPoiProp* IPoiFile::getSetup() {
  if (setup.isNull()) {
    setup = new CPoiPropSetup(this, poi);
  }

  return setup;
}

void IPoiFile::setIconSize(int size) {
  poiSize = {size, size};
  poiHighlightScaled =
      poiHighlight.scaled(poiHighlight.size() * size / qreal(DEFAULTICONSIZE), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

