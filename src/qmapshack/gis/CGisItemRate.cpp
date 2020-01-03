#include "CGisItemRate.h"
#include "IGisItem.h"
#include "ui_IGisItemRate.h"

IGisItemRate::IGisItemRate(QWidget *parent, const QSet<QString>& commonKeywords, qreal rating) :
    QDialog(parent),
    commonKeywords(commonKeywords)
{
    setupUi(this);
    keywordPlainTextEdit->setPlainText(QStringList(commonKeywords.toList()).join("; "));
    ratingSlider->setValue(rating);
    connect(ratingSlider, &QSlider::valueChanged, this, &IGisItemRate::slotRatingChanged);
}

IGisItemRate::~IGisItemRate()
{
}

qreal IGisItemRate::getRating()
{
    return ratingSlider->value();
}

bool IGisItemRate::getRatingChanged()
{
    return ratingChanged;
}

QSet<QString> IGisItemRate::getAddedKeywords()
{
    QSet<QString> keywords = keywordPlainTextEdit->toPlainText().split(QRegExp("\w*;\w*"), QString::SkipEmptyParts).toSet();
    return keywords.subtract(commonKeywords);
}

QSet<QString> IGisItemRate::getRemovedKeywords()
{
    QSet<QString> keywords = keywordPlainTextEdit->toPlainText().split(";").toSet();
    //Copy-construct the common keywords to keep things clean
    QSet<QString> removedKeywords = QSet<QString>(commonKeywords).subtract(keywords);
    return removedKeywords;
}

void IGisItemRate::slotRatingChanged()
{
    ratingChanged=true;
}
