#ifndef CGISITEMRATE_H
#define CGISITEMRATE_H

#include "ui_IGisItemRate.h"
#include <QDialog>

class IGisItem;
class IGisItemRate : public QDialog, private Ui::IGisItemRate
{
    Q_OBJECT

public:
    IGisItemRate(QWidget *parent = nullptr, const QSet<QString>& commonKeywords = QSet<QString>(), qreal rating = 0);
    ~IGisItemRate();

    qreal getRating();
    bool getRatingChanged();
    //Don't return a reference since this ends in a null reference when handed further
    QSet<QString> getAddedKeywords();
    QSet<QString> getRemovedKeywords();

private:
    void ratingLabelClicked(int labelNumber);

    const QSet<QString>& commonKeywords;
    bool ratingChanged = false;
    //keeping the rating in qreal if the wish for half stars emerges
    qreal rating;
};

#endif // CGISITEMRATE_H
