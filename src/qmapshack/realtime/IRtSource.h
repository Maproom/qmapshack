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

#ifndef IRTSOURCE_H
#define IRTSOURCE_H

#include <QCoreApplication>
#include <QDebug>
#include <QMutex>
#include <QObject>
#include <QTreeWidgetItem>

class CRtDraw;
class QSettings;

class IRtSource : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    enum type_e
    {
        eTypeNone
        , eTypeOpenSky
        , eTypeGpsTether
    };

    IRtSource(type_e type, bool singleInstanceOnly, QTreeWidget * parent);
    virtual ~IRtSource() = default;

    /**
       @brief Create new IRtSource items by type id

       @param type      the type id of the new item
       @param parent    the tree widget the item belongs to (can be nullptr, too)

       @return The pointer to the item or nullptr if type is unknown
     */
    static IRtSource* create(int type, QTreeWidget * parent);

    /**
       @brief Do everything necessary to setup item after it has been reparented to tree widget

       This will give you the chance to register widgets with sub-items and to setup singnal/slot
       connections.

     */
    virtual void registerWithTreeWidget() = 0;
    /**
       @brief Load item configuration from QSettings

       @param cfg   the QSettings object with proper group focus
     */
    virtual void loadSettings(QSettings& cfg);
    /**
       @brief Save item configuration to QSettings

       @param cfg   the QSettings object with proper group focus
     */
    virtual void saveSettings(QSettings& cfg) const;
    /**
       @brief Get string with description

       @return A descriptive text.
     */
    virtual QString getDescription() const = 0;

    virtual void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) = 0;

    virtual void fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt) = 0;

    virtual void mouseMove(const QPointF& pos)
    {
    }

    /**
       @brief Add actions to the context menu for this source

       @param menu  the context menu
     */
    virtual void contextMenu(QMenu * menu) const
    {
    }

    /**
       @brief Add actions to the context menu for a child of this source

       @param child     the child
       @param menu      the context menu
     */
    virtual void contextMenuChild(QTreeWidgetItem * child, QMenu * menu) const
    {
    }


    /**
       @brief This source item has been clicked

       @param column    the item's column that was clicked
     */
    virtual void itemClicked(int column) const
    {
    }

    /**
       @brief A child item of the source has been clicked

       @param child     the child item
       @param column    the item's column that was clicked
     */
    virtual void itemClicked(QTreeWidgetItem * child, int column) const
    {
    }

    /// the global mutex. Has to be used for all operations on a IRtItem
    static QMutex mutex;
    /// the item's type id
    const type_e type;
    /// set true if only one instance at a time can exist
    const bool singleInstanceOnly;

    /// used to identify tre widget columns
    enum column_e
    {
        eColumnIcon = 0
        , eColumnCheckBox = eColumnIcon
        , eColumnDecoration = eColumnIcon
        , eColumnName = 1
        , eColumnWidget = eColumnName
    };

signals:
    void sigChanged();
};

Q_DECLARE_METATYPE(IRtSource*)
#endif //IRTSOURCE_H

