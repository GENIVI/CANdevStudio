#include "uniquefiltermodel.h"
#include "crv_enums.h"


UniqueFilterModel::UniqueFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void UniqueFilterModel::updateFilter(QString frameID, QString time, QString direction)
{
    QPair<QString, QString> value(frameID, direction);

    if ((!_uniques.contains(value)) || (time.toDouble() > _uniques[value].toDouble())) {
        _uniques[std::move(value)] = time;
    }

    invalidateFilter();
}

bool UniqueFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QString time = sourceModel()->index(sourceRow, 1, sourceParent).data().toString();
    QString frameID = sourceModel()->index(sourceRow, 2, sourceParent).data().toString();
    QString direction = sourceModel()->index(sourceRow, 3, sourceParent).data().toString();

    QPair<QString, QString> value(std::move(frameID), std::move(direction));

    return ((_uniques[value] == time) || (false == _filterActive));
}

bool UniqueFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    QVariant userRole = sourceModel()->headerData(left.column(), Qt::Horizontal, Qt::UserRole);

    switch (userRole.value<CRV_ColType>()) {
    case CRV_ColType::uint_type:
        return (leftData.toUInt() < rightData.toUInt());
    case CRV_ColType::double_type:
        return (leftData.toDouble() < rightData.toDouble());
    case CRV_ColType::hex_type:
        return ((leftData.toString().toUInt(nullptr, 16)) << (rightData.toString().toUInt(nullptr, 16)));
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

void UniqueFilterModel::clearFilter()
{
    _uniques.clear();
}

void UniqueFilterModel::toggleFilter()
{
    _filterActive = !_filterActive;
    invalidateFilter();
}

bool UniqueFilterModel::isFilterActive()
{
    return _filterActive;
}
