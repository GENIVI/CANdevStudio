#include "uniquefiltermodel.h"

UniqueFilterModel::UniqueFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void UniqueFilterModel::updateFilter(int frameID, double time, QString direction)
{
    QPair<int, QString> value(frameID, direction);

    if ((!uniques.contains(value)) || (time > uniques[value])) {
        uniques[value] = time;
    }

    invalidateFilter();
}

bool UniqueFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    int frameID = sourceModel()->index(sourceRow, 3, sourceParent).data().toInt();
    double time = sourceModel()->index(sourceRow, 1, sourceParent).data().toDouble();
    QString direction = sourceModel()->index(sourceRow, 5, sourceParent).data().toString();

    QPair<int, QString> value(frameID, direction);

    if ((uniques[value] == time) || (filterActive == false)) {
        return true;
    } else {
        return false;
    }
}

void UniqueFilterModel::clearFilter() { uniques.clear(); }

void UniqueFilterModel::toggleFilter()
{
    filterActive = !filterActive;
    invalidateFilter();
}

bool UniqueFilterModel::isFilterActive() { return filterActive; }
