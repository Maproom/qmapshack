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

#ifndef IWKSITEM_H
#define IWKSITEM_H

#include <QTreeWidgetItem>

class IWksItem : public QTreeWidgetItem {
 public:
  IWksItem(QTreeWidgetItem* parent, int type);
  IWksItem(QTreeWidget* parent, int type);
  virtual ~IWksItem() = default;

  enum mark_e { eMarkNone = 0, eMarkChanged = 0x00000001, eMarkNotPart = 0x00000002, eMarkNotInDB = 0x00000004 };

  enum type_e {
    eTypeNone = 0,
    // --- Start: do not change these numbers. It will break binary data files ----
    // items
    eTypeWpt = 1,
    eTypeTrk = 2,
    eTypeRte = 3,
    eTypeOvl = 4,
    eTypeMax = 5,
    // --- End: do not change these numbers. It will break binary data files ----
    // *** special items ***
    eTypeGeoSearch,
    // *** projects ***
    eTypeQms,
    eTypeGpx,
    eTypeDb,
    eTypeLostFound,
    eTypeTwoNav,
    eTypeSlf,
    eTypeFit,
    eTypeTcx,
    eTypeSml,
    eTypeLog,
    eTypeQlb,
    // *** devices ***
    eTypeGarmin,
    eTypeCompe,
    eTypeGarminMtp,
    eTypeVirtual,
    eTypeGenericMtp
  };

  enum class eBaseType { Project, Device, Item, GeoSearch, Unknown };

  eBaseType getBaseType() const;
  virtual const QString& getName() const { return name; }
  virtual const QString& getToolTipName() const { return toolTipName; }
  virtual const QString& getToolTipDecoration() const { return toolTipDecoration; }
  virtual const QPixmap& getIcon() const { return icon; }
  virtual const bool isVisible() const { return visible; }
  virtual const bool isAutoSave() const { return autoSave; }
  virtual const bool isAutoSyncToDev() const { return autoSyncToDev; }
  virtual const qint32 isOnDevice() const { return false; }
  virtual const bool canSave() const { return false; }
  virtual bool hasUserFocus() const = 0;

  /**
     @brief Check if there are any pending unsaved changes
     @return True if the are changes to be saved
   */
  virtual const bool isChanged() const { return (flagsDecoration & eMarkChanged) != 0; }
  quint32 getFlagsDecoration() const { return flagsDecoration; }

  virtual void setToolTipName(const QString& tip) { toolTipName = tip; }
  virtual void setVisibility(bool visible) {
    this->visible = visible;
    updateItem();
  }
  virtual void setAutoSave(bool on) {
    autoSave = on;
    updateItem();
  }
  virtual void setAutoSyncToDev(bool on) {
    autoSyncToDev = on;
    updateItem();
  }

  virtual void gainUserFocus(bool yes) = 0;
  void updateItem();

 protected:
  virtual void updateDecoration(quint32 enable, quint32 disable);

  QString name;
  QString toolTipName;
  QPixmap icon;

  QString toolTipDecoration;

  /// labeling the GisItems
  qreal rating = 0;
  QSet<QString> keywords;

 private:
  quint32 flagsDecoration = eMarkNone;
  bool visible = true;
  bool autoSave = false;       ///< flag to show if auto save is on or off
  bool autoSyncToDev = false;  ///< if set true sync the project with every device connected
};

#endif  // IWKSITEM_H
