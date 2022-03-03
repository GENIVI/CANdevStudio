#include "searchmodel.h"
#include "log.h"

#include <QDataStream>
#include <QMimeData>
#include <QUrl>

bool SearchModel::isFilterActive() const
{
    return _filter.length() > 0;
}

QMimeData* SearchModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* pData = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    QString type = objectName();
    int row = -1;
    for (int i = 0; indexes.size() > i; i++) {
        if (false == indexes[i].isValid()) {
            continue;
        }

        if (indexes[i].row() != row) {
            row = indexes[i].row();

            QString id = data(index(row, 0)).toString();
            QString sig = "SignalModel" == type ? data(index(row, 1)).toString() : "";
            stream << id << sig;
        }
    }

    pData->setData("fromCanDB", encoded);
    return pData;
}

QStringList SearchModel::mimeTypes() const
{
    return QStringList{ "fromCanDB" };
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
