#ifndef CSEARCHMODEL_H
#define CSEARCHMODEL_H

#include "CSearch.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QString>
#include <QVariant>

class CSearchModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    CSearchModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 1;
    }

private:
    class searchItem_t
    {
        searchItem_t* parent = 0;
        QList<searchItem_t> children;
        QString value;
        searchItem_t& getChild(int row)
        {
            return children[row];
        }
        int getRow()
        {
            return parent->children.indexOf(*this);
        }
    };

    searchItem_t root;
};

#endif // CSEARCHMODEL_H
