#include "sortmodel.h"
#include "crs_enums.h"

SortModel::SortModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

bool SortModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    QVariant userRole = sourceModel()->headerData(left.column(), Qt::Horizontal, Qt::UserRole);

    switch (userRole.value<CRS_ColType>()) {
    case CRS_ColType::uint_type:
        return (leftData.toUInt() < rightData.toUInt());
    case CRS_ColType::double_type:
        return (leftData.toDouble() < rightData.toDouble());
    case CRS_ColType::hex_type:
        return ((leftData.toString().toUInt(nullptr, 16)) << (rightData.toString().toUInt(nullptr, 16)));
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    }
}
