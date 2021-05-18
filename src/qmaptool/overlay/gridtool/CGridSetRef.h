/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CGRIDSETREF_H
#define CGRIDSETREF_H

#include "ui_IGridSetRef.h"

class QSettings;

class CGridSetRef : public QWidget, private Ui::IGridSetRef
{
    Q_OBJECT
public:
    CGridSetRef(QWidget* parent);
    virtual ~CGridSetRef() = default;

    void saveSettings(QSettings& cfg);
    void loadSettings(QSettings& cfg);

    bool isOk();

    QString getProjection() const
    {
        return lineGridProj->text();
    }

    qreal getEasting() const
    {
        return lineEasting->text().toDouble();
    }

    qreal getNorthing() const
    {
        return lineNorthing->text().toDouble();
    }

    qreal getHorizSpacing() const
    {
        return lineHorizSpacing->text().toDouble();
    }

    qreal getVertSpacing() const
    {
        return lineVertSpacing->text().toDouble();
    }

signals:
    void sigChanged();

public slots:
    void slotReset();

private slots:
    void slotSetupGridProj();

private:
    template<typename T>
    void markWidget(T* w, bool isOk)
    {
        QPalette pal = T(this).palette();
        if(!isOk)
        {
            pal.setColor(QPalette::Base, 0xffffaa7f);
        }
        w->setPalette(pal);
    }
};

#endif //CGRIDSETREF_H

