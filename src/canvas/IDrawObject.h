/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef IDRAWOBJECT_H
#define IDRAWOBJECT_H

#include <QObject>

class QSettings;

class IDrawObject : public QObject
{
    Q_OBJECT
    public:
        IDrawObject(QObject * parent);
        virtual ~IDrawObject();

        virtual void saveConfig(QSettings& cfg);

        virtual void loadConfig(QSettings& cfg);

        /**
           @brief Read opacity value
           @return Return the opacity in a range of 0..100(full opacity)
         */
        int getOpacity(){return opacity;}

        /**
           @brief Read the minimum scale factor the object should be displayed
           @return A factor or NOFLOAT if no minimum has been set
         */
        qreal getMinScale(){return minScale;}
        /**
           @brief Read the maximum scale factor the object should be displayed
           @return A factor or NOFLOAT if no maximum has been set
         */
        qreal getMaxScale(){return maxScale;}
        /**
           @brief Write the minimum scale the object should be displayed at.
           @param s A factor or NOFLOAT if no minimum should be set
         */
        void setMinScale(qreal s);
        /**
           @brief Write the maximum scale the object should be displayed at.
           @param s A factor or NOFLOAT if no maximum should be set
         */
        void setMaxScale(qreal s);

    public slots:
        /**
           @brief Write opacity value
           @param value must be in the range of 0..100(full opacity)
         */
        void slotSetOpacity(int value){opacity = value;}

    signals:
        /**
           @brief Emitted every time a property of the object is changed
         */
        void sigPropertiesChanged();

    protected:
        /**
           @brief Test if the given scale is out of the min/max scale
           @param scale A scale factor for x and y axis
           @return True if x scale is out of the min/max range
         */
        bool isOutOfScale(const QPointF& scale);


    private:
        qreal opacity;

        qreal minScale;

        qreal maxScale;

};

#endif //IDRAWOBJECT_H

