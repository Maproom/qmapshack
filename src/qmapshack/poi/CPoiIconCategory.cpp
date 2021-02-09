#include "CPoiIconCategory.h"

const QPixmap CPoiIconCategory::getIcon(QStringList additionalTags) const
{
    for(const QString& tag : additionalTags)
    {
        if(subCategories.keys().contains(tag))
        {
            return subCategories[tag];
        }
    }
    return baseIcon;
}
