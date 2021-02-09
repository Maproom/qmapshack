#ifndef CPOIICONCATEGORY_H
#define CPOIICONCATEGORY_H

#include <QMap>
#include <QPixmap>

class CPoiIconCategory
{
public:
    //Only to avoid compiler errors due to maps assignment
    CPoiIconCategory(){}
    CPoiIconCategory(const QPixmap& baseIcon, const QMap<QString, QPixmap>& subCategories = QMap<QString, QPixmap>()) : baseIcon(baseIcon), subCategories(subCategories){}
    const QPixmap getIcon(QStringList additionalTags) const;
private:
    QPixmap baseIcon;
    QMap<QString, QPixmap> subCategories;
};

#endif // CPOIICONCATEGORY_H
