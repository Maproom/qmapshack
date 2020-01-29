/**********************************************************************************************
    Copyright (C) 2020 Henri Hornburg hrnbg@t-online.de

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

#ifndef CGISITEMRATE_H
#define CGISITEMRATE_H

#include "ui_IGisItemRate.h"
#include <QDialog>

class IGisItem;
class CGisItemRate : public QDialog, private Ui::IGisItemRate
{
    Q_OBJECT

public:
    CGisItemRate(QWidget *parent = nullptr, const QSet<QString>& commonKeywords = QSet<QString>(), qreal rating = 0);
    ~CGisItemRate();

    qreal getRating();
    bool getRatingChanged();
    //Don't return a reference since this ends in a null reference when handed further
    QSet<QString> getKeywords() const;
    QSet<QString> getAddedKeywords() const;
    QSet<QString> getRemovedKeywords() const;

private:
    void ratingLabelClicked(int labelNumber);
    void updateStars();

    const QSet<QString>& commonKeywords;
    bool ratingChanged = false;
    //keeping the rating in qreal if the wish for half stars emerges
    qreal rating = 0;
};

#endif // CGISITEMRATE_H
