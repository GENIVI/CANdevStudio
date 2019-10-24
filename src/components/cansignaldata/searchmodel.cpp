#include "searchmodel.h"

bool SearchModel::isFilterActive() const 
{
    return _filter.length() > 0;
}

bool SearchModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    bool result = false;

    for(int i = 0; i < sourceModel()->columnCount(); ++i) {
        QString str = sourceModel()->index(sourceRow, i, sourceParent).data().toString().toLower();
        
        result |= str.contains(_filter);
    }

    return result;
}

void SearchModel::updateFilter(const QString& filter)
{
    _filter = filter.toLower();
    invalidate();
}
