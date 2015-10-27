/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

/** @brief A QWidget derived from QSpinBox, aiming to be tiny with regard to spaced consumed 
           on screen.

    By default, a readonly QTinySpinBox looks like an ordinary label, whereas a non-readonly
    QTinySpinBox contains blue, underlined text. As soon as the non-readonly widget receives
    the focus, the color is changed to black, the underline disappears and the modifiable text
    is selected.
 */

#ifndef QTINYSPINBOX_H
#define QTINYSPINBOX_H

#include <QColor>
#include <QPalette>
#include <QFont>
#include <QDebug>
#include <QTimer>
#include <QSpinBox>

class QTinySpinBox : public QSpinBox
{
Q_OBJECT

private:
    bool     initialized;

    QPalette paletteEdit;
    QPalette paletteRO;
    QPalette paletteRW;

    QFont    fontNoUnderline;
    QFont    fontUnderline;

    void initialize()
    {
        initialized  = true;

        paletteEdit = QPalette(palette());
        paletteRO   = QPalette(palette());
        paletteRW   = QPalette(palette());
        paletteRW.setColor(QPalette::Text, QColor(0, 0, 255));

        fontNoUnderline = QFont(font());
        fontUnderline   = QFont(font());
        fontUnderline.setUnderline(true);
    }

    void updateStyle()
    {
        if(!initialized)
        {
            initialize();
        }

        if(isReadOnly())
        {
            setPalette(paletteRO);
            setFont(fontNoUnderline);
        } else if(hasFocus()) {
            setPalette(paletteEdit);
            setFont(fontNoUnderline);
        } else {
            setPalette(paletteRW);
            setFont(fontUnderline);
        }
    }

public slots:
    void slotSelectAll() { selectAll(); }

public:
    QTinySpinBox(QWidget * parent = 0)
        : QSpinBox(parent)
    {
        // initialization has to be done deferred,
        // as the correct palette is set after construction
        initialized = false;
    }

    void setReadOnly(bool r)
    {
        QSpinBox::setReadOnly(r);
        updateStyle();
    }

protected:
    void focusInEvent(QFocusEvent *event)
    {
        updateStyle();
        if(!isReadOnly())
        {
            QTimer::singleShot(0, this, SLOT(slotSelectAll()));
        }

        QSpinBox::focusInEvent(event);
    }

    void focusOutEvent(QFocusEvent *event)
    {
        updateStyle();
        QSpinBox::focusOutEvent(event);
    }
};
#endif // QTINYSPINBOX_H
