#include "CSearchModel.h"

CSearchModel::CSearchModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QStringList stringOnlyProperties = {"name","attributes","full text"};
    QStringList stringSearchTypes = {"with", "without", "contains", "regex"};
    for(const QString& propertyString:stringOnlyProperties)
    {
        //append an empty item first, to make sure pointers are correct
        root.children.prepend(searchItem_t());
        searchItem_t& property = root.children[0];
        property.parent=&root;
        property.value=propertyString;

        for(const QString& typeString:stringSearchTypes)
        {
            property.children.prepend(searchItem_t());
            searchItem_t& type = property.children[0];
            type.parent=&property;
            type.value=typeString;
        }
    }

    QList<QString> otherProperties = CSearch::getSearchPropertyKeywords().toSet().subtract(stringOnlyProperties.toSet()).toList();
    for(const QString& propertyString:otherProperties)
    {
        //append an empty item first, to make sure pointers are correct
        root.children.prepend(searchItem_t());
        searchItem_t& property = root.children[0];
        property.parent=&root;
        property.value=propertyString;

        for(const QString& typeString:CSearch::getSearchTypeKeywords())
        {
            property.children.prepend(searchItem_t());
            searchItem_t& type = property.children[0];
            type.parent=&property;
            type.value=typeString;
        }
    }
}

QVariant CSearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    searchItem_t *item = static_cast<searchItem_t*>(index.internalPointer());

    return item->value;
}

Qt::ItemFlags CSearchModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return 0;
    }

    return QAbstractItemModel::flags(index);
}

QModelIndex CSearchModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    const searchItem_t *parentItem;

    if (!parent.isValid())
    {
        parentItem = &root;
    }
    else
    {
        parentItem = static_cast<searchItem_t*>(parent.internalPointer());
    }

    if (row < parentItem->children.count())
    {
        return createIndex(&row, &column, parentItem->children[row]);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex CSearchModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    searchItem_t *thisItem = static_cast<searchItem_t*>(index.internalPointer());
    searchItem_t *parentItem = thisItem->parent;

    if (parentItem == &root)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->parent->children.indexOf(*parentItem), 0, parentItem);
}

int CSearchModel::rowCount(const QModelIndex &parent) const
{
    const searchItem_t* parentItem;

    if (!parent.isValid())
    {
        parentItem = &root;
    }
    else
    {
        parentItem = static_cast<searchItem_t*>(parent.internalPointer());
    }

    return parentItem->children.count();
}
