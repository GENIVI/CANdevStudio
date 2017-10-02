#ifndef SORTMODEL_H
#define SORTMODEL_H

#include <QSortFilterProxyModel>

/**
*   @brief This class provides a filter model between source model and table view, which passes through only frames with
*          newest unique ID and direction values
*/
class SortModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit SortModel(QObject* parent = 0);

protected:
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
};
#endif
