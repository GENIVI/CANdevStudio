#include "uniquefiltermodel.h"

UniqueFilterModel::UniqueFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void UniqueFilterModel::updateFilter(int frameID, double time, QString direction)
{
    QPair<int, QString> value(frameID, direction);

    if ((!uniques.contains(value)) || (time > uniques[value])) {
        uniques[std::move(value)] = time;
    }

    invalidateFilter();
}

bool UniqueFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const int frameID = sourceModel()->index(sourceRow, 3, sourceParent).data().toInt();
    const double time = sourceModel()->index(sourceRow, 1, sourceParent).data().toDouble();
    const QString direction = sourceModel()->index(sourceRow, 5, sourceParent).data().toString();

    QPair<int, QString> value(std::move(frameID), std::move(direction));

    return ((uniques[value] == time) || (false == filterActive ));

}

void UniqueFilterModel::clearFilter() { uniques.clear(); }

void UniqueFilterModel::toggleFilter()
{
    filterActive = !filterActive;
    invalidateFilter();
}

bool UniqueFilterModel::isFilterActive() { return filterActive; }
