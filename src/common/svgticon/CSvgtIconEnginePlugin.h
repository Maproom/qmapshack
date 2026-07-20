/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSVGTICONENGINEPLUGIN_H
#define CSVGTICONENGINEPLUGIN_H

#include <QIconEnginePlugin>

/**
   @brief Registers CSvgtIconEngine for the ".svgt" suffix.

   Static plugin (see CMakeLists.txt), imported via Q_IMPORT_PLUGIN in main.cpp; compiled into
   the executable, nothing to deploy.

   Key is "svgt", not "svg": Qt's qsvgicon owns "svg" and takes priority, so this engine only
   receives ".svgt" resources.
 */
class CSvgtIconEnginePlugin : public QIconEnginePlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QIconEngineFactoryInterface_iid FILE "svgticon.json")

 public:
  QIconEngine* create(const QString& fileName = QString()) override;
};

#endif  // CSVGTICONENGINEPLUGIN_H
