#include "CGisItemRate.h"
#include "IGisItem.h"
#include "QPushButton"
#include "ui_IGisItemRate.h"

IGisItemRate::IGisItemRate(QWidget *parent, const QSet<QString>& commonKeywords, qreal rating) :
    QDialog(parent),
    commonKeywords(commonKeywords),
    rating(rating)
{
    setupUi(this);
    keywordPlainTextEdit->setPlainText(QStringList(commonKeywords.toList()).join(", "));

    pushButtonR1->setIcon(QPixmap(rating < 1 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR2->setIcon(QPixmap(rating < 2 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR3->setIcon(QPixmap(rating < 3 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR4->setIcon(QPixmap(rating < 4 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR5->setIcon(QPixmap(rating < 5 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));

    pushButtonR1->setFixedSize(32, 32);
    pushButtonR2->setFixedSize(32, 32);
    pushButtonR3->setFixedSize(32, 32);
    pushButtonR4->setFixedSize(32, 32);
    pushButtonR5->setFixedSize(32, 32);

    connect(pushButtonR1, &QPushButton::clicked, this, [this] { ratingLabelClicked(1); });
    connect(pushButtonR2, &QPushButton::clicked, this, [this] { ratingLabelClicked(2); });
    connect(pushButtonR3, &QPushButton::clicked, this, [this] { ratingLabelClicked(3); });
    connect(pushButtonR4, &QPushButton::clicked, this, [this] { ratingLabelClicked(4); });
    connect(pushButtonR5, &QPushButton::clicked, this, [this] { ratingLabelClicked(5); });
}

IGisItemRate::~IGisItemRate()
{
}

qreal IGisItemRate::getRating()
{
    return rating;
}

bool IGisItemRate::getRatingChanged()
{
    return ratingChanged;
}

QSet<QString> IGisItemRate::getAddedKeywords()
{
    QSet<QString> keywords = keywordPlainTextEdit->toPlainText().split(QRegExp("\w*,s\w*"), QString::SkipEmptyParts).toSet();
    return keywords.subtract(commonKeywords);
}

QSet<QString> IGisItemRate::getRemovedKeywords()
{
    QSet<QString> keywords = keywordPlainTextEdit->toPlainText().split(",").toSet();
    //Copy-construct the common keywords to keep things clean
    QSet<QString> removedKeywords = QSet<QString>(commonKeywords).subtract(keywords);
    return removedKeywords;
}

void IGisItemRate::ratingLabelClicked(int labelNumber)
{
    ratingChanged=true;

    //Comparing like this since one is a floating point and one an integer
    if(rating > labelNumber || rating < labelNumber)
    {
        rating=labelNumber;
    }
    else
    {
        //The icon is already a star, if you click it again, the star goes away
        rating=labelNumber-1;
    }

    pushButtonR1->setIcon(QPixmap(rating < 1 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR2->setIcon(QPixmap(rating < 2 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR3->setIcon(QPixmap(rating < 3 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR4->setIcon(QPixmap(rating < 4 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
    pushButtonR5->setIcon(QPixmap(rating < 5 ? "://icons/cache/32x32/star_empty.png" :  "://icons/cache/32x32/star.png"));
}
