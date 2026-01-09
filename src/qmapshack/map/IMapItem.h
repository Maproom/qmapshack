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

#ifndef IMAPITEM_H
#define IMAPITEM_H

#include <QString>

class IMapItem {
 public:
  enum class eStatus {
    Inactive,
    Active,
    Missing,
    Unused,
  };

  virtual const QString& getKey() = 0;

  /**
   * @brief Set the dem's name
   * @param text
   */
  void setName(const QString& name) { this->name = name; };
  /**
   * @brief Get the map's name as displayed
   * @return
   */
  QString getName() const { return name; };

  /**
   * @brief Activate or deactivate map
   * @param yes   set true for activation and false for deactivation
   */
  virtual void activate(bool yes) = 0;

  /**
   * @brief Set the status
   * @param status
   */
  void setStatus(eStatus status) { this->status = status; }
  /**
   * @brief Get the items activity status
   * @return
   */
  eStatus getStatus() const { return status; }

  /**
   * @brief True if map is out of scale and not visible
   */
  bool isOutOfScale() const { return outOfScale; }

 protected:
  /**
   * @brief Status of the map item
   */
  eStatus status{eStatus::Unused};
  /**
   * @brief Status flag if map is visible at current scale
   */
  bool outOfScale = true;
  /**
   * @brief Name as displayed in the tree widget
   */
  QString name;

  static constexpr Qt::GlobalColor kColorOut = Qt::lightGray;
  static constexpr Qt::GlobalColor kColorIn = Qt::darkGreen;
};

#endif  // IMAPITEM_H
