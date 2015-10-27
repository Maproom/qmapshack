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
