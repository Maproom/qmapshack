/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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
        ,eTypeOpenSky
    };



    IRtSource(type_e type, bool singleInstanceOnly, QTreeWidget * parent);
    virtual ~IRtSource() = default;

    static IRtSource* create(int type, QTreeWidget * parent);

    static QMutex mutex;

    enum column_e
    {
        eColumnIcon = 0
        ,eColumnCheckBox = eColumnIcon
        ,eColumnDecoration = eColumnIcon
        ,eColumnName = 1
    };
    virtual void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) const = 0;

    virtual QString getDescription() const = 0;

    virtual void loadSettings(QSettings& cfg);
    virtual void saveSettings(QSettings& cfg) const;

    const type_e type;
    const bool singleInstanceOnly;
};

Q_DECLARE_METATYPE(IRtSource*)
#endif //IRTSOURCE_H

