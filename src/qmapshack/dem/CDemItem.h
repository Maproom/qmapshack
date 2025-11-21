/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CDEMITEM_H
#define CDEMITEM_H

#include <QMutex>
#include <QPointer>
#include <QTreeWidgetItem>

#include "widgets/CMapItemWidget.h"

class IDem;
class CDemDraw;
class QSettings;

class CDemItem : public QObject, public QTreeWidgetItem {
  Q_OBJECT
 public:
  CDemItem(CDemDraw* dem);
  virtual ~CDemItem();

  /**
   * @brief Set the map's absolute filename
   *
   * The hash key is derived from the file's conetnt. If the file
   * is missing the fallback key is used.
   *
   * @param name
   * @param fallbackKey
   */
  void setFilename(const QString& name, const QString& fallbackKey);
  /**
   * @brief Set the map's name in the CMapItemWidget
   * @param text
   */
  void setName(const QString& Name);
  /**
   * @brief Set the status in the CMapItemWidget
   * @param status
   */
  void setStatus(CMapItemWidget::eStatus status);
  /**
   * @brief Save the map's configuration into the given QSettings object
   *
   * If the map is active the configuration is read from the loaded
   * map file object. If not the shadow config is used.
   *
   * @param cfg
   */
  void saveConfig(QSettings& cfg) const;
  /**
   * @brief Load the map's configuration from the given QSettings object
   *
   * If the map is active the map file object will read the configuration.
   * Additionally the configuration is stored in the shadow config
   *
   * Depending on the configuration and availablility of a map the status
   * is updated and the map is activated if configured.
   *
   * @param cfg
   * @param triggerActivation  set true to trigger activation/deactivation according to the config
   */
  void loadConfig(QSettings& cfg, bool triggerActivation);

  /**
     @brief As the drawing thread is using the list widget to iterate of all maps to draw, all access has to be
     synchronized.
   */
  static QRecursiveMutex mutexActiveDems;

  /**
     @brief Query if dem objects are loaded
     @return True if the internal list of dem objects is not empty.
   */
  bool isActivated();

  /**
     @brief Set item's icon according to map type and state
   */
  void updateIcon();

  /**
     @brief Show or hide child treewidget items
     @param yes set true to add children, false will remove all children and delete the attached widgets
   */
  void showChildren(bool yes);

  /**
   * @brief Get the map's name as displayed
   * @return
   */
  QString getName() const { return widget->getName(); }

  /**
   * @brief Get the map's full filename
   * @return
   */
  const QString& getFilename() const { return filename; }

  /**
   * @brief Get access to the internal map file instance
   * @return
   */
  QPointer<IDem>& getDemFile() { return demfile; }

  /**
   * @brief Get the map's hash/key derived from the map file content
   * @return
   */
  const QString& getKey() { return key; }

  /**
   * @brief Get the items activity status
   * @return
   */
  CMapItemWidget::eStatus getStatus() const { return widget->getStatus(); }

  /**
   * @brief Get the tre widget item's widget.
   *
   * This will always return a valid pointer. If the widget
   * has been delete by the tree widget a new one will be
   * created on-th-fly
   *
   * @return
   */
  QWidget* itemWidget();

  /**
   * @brief Load all internal map objects
   * @return Return true on success.
   */
  bool activate();

 signals:
  void sigChanged();
  // emitted if the tree widget item's widget was destroyed
  void sigUpdateWidget(CDemItem*);

 public slots:
  void slotActivate(bool yes);

 private:
  friend class CDemTreeWidget;
  /**
     @brief Delete all internal map objects
   */
  void deactivate();
  /**
   * @brief Copy the config in to the shadow config
   * @param cfg
   */
  void configToShadowConfig(const QSettings& cfg);
  /**
   * @brief Copy the shadow config into the config
   * @param cfg
   */
  void shadowConfigToConfig(QSettings& cfg) const;
  /**
   * @brief Check for map settings in the shadow config
   *
   * There are always to entries expected ("isActive" and "filename"). Therefor
   * if the are two or less entries the shadow config is empty.
   *
   * @return
   */
  bool noShadowConfig() const { return shadowConfig.size() <= 2; }

  CDemDraw* dem;
  /**
     @brief A MD5 hash over the first 1024 bytes of the map file, to identify the map
   */
  QString key;
  /**
     @brief List of map files forming that particular map
   */
  QString filename;
  /**
     @brief List of loaded map objects when map is activated.
   */
  QPointer<IDem> demfile;

  /**
   * @brief The map item's own widget to be shown in the tree widget
   */
  QPointer<CMapItemWidget> widget;

  /**
   * @brief The shadow config of the map's configuration
   *
   * This is used to keep the state for inactive maps (no mapfile object)
   */
  QMap<QString, QVariant> shadowConfig;
};

#endif  // CDEMITEM_H
