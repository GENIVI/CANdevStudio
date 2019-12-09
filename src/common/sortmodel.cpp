#include "sortmodel.h"
#include "sortenums.h"
#include <log.h>


SortModel::SortModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , _currSortNdx(0)
    , _currSortOrder(Qt::AscendingOrder)
    , _filterActive(false)
{
}

bool SortModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QVariant leftData = sourceModel()->data(sourceModel()->index(left.row(), updatedSortNdx()));
    QVariant rightData = sourceModel()->data(sourceModel()->index(right.row(), updatedSortNdx()));
    QVariant userRole = sourceModel()->headerData(updatedSortNdx(), Qt::Horizontal, Qt::UserRole);

    switch (userRole.value<ColType>()) {
    case ColType::uint_type:
        return (leftData.toUInt() < rightData.toUInt());
    case ColType::double_type:
        return (leftData.toDouble() < rightData.toDouble());
    case ColType::hex_type:
        return ((leftData.toString().toUInt(nullptr, 16)) < (rightData.toString().toUInt(nullptr, 16)));
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

int SortModel::updatedSortNdx() const
{
    int updatedSortNdx = _currSortNdx;

    // Use ID column by default for sorting when in filtering mode. Using time causes "frame flickering"
    // TODO: Find something better without magic nubmers...
    if(isFilterActive() && _currSortNdx == 0) {
        updatedSortNdx = 2;
    }

    return updatedSortNdx;
}

bool SortModel::isFilterActive() const
{
    return _filterActive;
}

void SortModel::setFilterActive(bool enabled)
{
    _filterActive = enabled;
}

void SortModel::sort(int column, Qt::SortOrder order)
{
    _currSortNdx = column;
    _currSortOrder = order;

    cds_debug("Sort index: {}, Refreshed sort index: {}, sort ortder: {}", _currSortNdx, updatedSortNdx(), _currSortOrder);

    QSortFilterProxyModel::sort(updatedSortNdx(), _currSortOrder);
}
