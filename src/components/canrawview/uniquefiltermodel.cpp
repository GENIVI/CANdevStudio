#include "uniquefiltermodel.h"

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

    QString clickedColumnName = sourceModel()->headerData(left.column(), Qt::Horizontal).toString();
    QString userRole = sourceModel()->headerData(left.column(), Qt::Horizontal, Qt::UserRole).toString();

    if (userRole == "uint") {
        return (leftData.toUInt() < rightData.toUInt());
    } else if (userRole == "double") {
        return (leftData.toDouble() < rightData.toDouble());
    } else if (userRole == "hex") {
        return ((leftData.toString().toUInt(nullptr, 16)) < (rightData.toString().toUInt(nullptr, 16)));
    } else {
        return (leftData < rightData);
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
