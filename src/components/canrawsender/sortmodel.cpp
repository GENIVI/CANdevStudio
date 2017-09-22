#include "sortmodel.h"

SortModel::SortModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

bool SortModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    QVariant userRole = sourceModel()->headerData(left.column(), Qt::Horizontal, Qt::UserRole);

    QString colName = sourceModel()->headerData(left.column(), Qt::Horizontal).toString();

    if (colName == "Id") {
        return ((leftData.toString().toUInt(nullptr, 16)) < (rightData.toString().toUInt(nullptr, 16)));
    } else if (colName == "Data") {
        return ((leftData.toString().toUInt(nullptr, 16)) < (rightData.toString().toUInt(nullptr, 16)));
    } else if (colName == "Interval") {
        return (leftData.toUInt() < rightData.toUInt());
    } else {
        return (leftData < rightData);
    }
}
