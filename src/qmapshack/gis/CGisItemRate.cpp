/**********************************************************************************************
    Copyright (C) 2020 Henri Hornburg <hrnbg@t-online.de>

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

#include "gis/CGisItemRate.h"

#include <QtGui>

#include "misc.h"

CGisItemRate::CGisItemRate(QWidget* parent, const QSet<QString>& commonKeywords, qreal rating)
    : QDialog(parent), commonKeywords(commonKeywords), rating(rating) {
  setupUi(this);

  QStringList stringList = QStringList(commonKeywords.values());
  std::sort(stringList.begin(), stringList.end(), sortByString);
  keywordPlainTextEdit->setPlainText(stringList.join(", "));

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

  updateStars();
}

CGisItemRate::~CGisItemRate() {}

qreal CGisItemRate::getRating() { return rating; }

bool CGisItemRate::getRatingChanged() { return ratingChanged; }

QSet<QString> CGisItemRate::getKeywords() const {
  static const QRegularExpression re("\\s*,\\s*");
  const QList<QString>& keywords = keywordPlainTextEdit->toPlainText().split(re, Qt::SkipEmptyParts);
  return {keywords.begin(), keywords.end()};
}

QSet<QString> CGisItemRate::getAddedKeywords() const {
  QSet<QString> keywords = getKeywords();
  return keywords.subtract(commonKeywords);
}

QSet<QString> CGisItemRate::getRemovedKeywords() const {
  QSet<QString> keywords = getKeywords();
  // Copy-construct the common keywords to keep things clean
  QSet<QString> removedKeywords = QSet<QString>(commonKeywords).subtract(keywords);
  return removedKeywords;
}

void CGisItemRate::ratingLabelClicked(int labelNumber) {
  ratingChanged = true;

  // Comparing like this since one is a floating point and one an integer
  if (rating > labelNumber || rating < labelNumber) {
    rating = labelNumber;
  } else {
    // The icon is already a star, if you click it again, the star goes away
    rating = labelNumber - 1;
  }
  updateStars();
}

void CGisItemRate::updateStars() {
  // The generic item rating uses the UI copies. The geocache D/T rating in CDetailsGeoCache
  // keeps its own stars under icons/cache/, so restyling chrome cannot touch geocache display.
  const QIcon starEmpty("://icons/RatingStarEmpty.svgt");
  const QIcon starFull("://icons/RatingStar.svgt");

  pushButtonR1->setIcon(rating < 1 ? starEmpty : starFull);
  pushButtonR2->setIcon(rating < 2 ? starEmpty : starFull);
  pushButtonR3->setIcon(rating < 3 ? starEmpty : starFull);
  pushButtonR4->setIcon(rating < 4 ? starEmpty : starFull);
  pushButtonR5->setIcon(rating < 5 ? starEmpty : starFull);
}
