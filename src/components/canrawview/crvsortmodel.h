#ifndef CRVSORTMODEL_H
#define CRVSORTMODEL_H

#include <QSortFilterProxyModel>

class CRVSortModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit CRVSortModel(QObject* parent = 0);

    void setFilterActive(bool enabled);
    bool isFilterActive() const;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

protected:
    /**
    *   @brief  Function compares two items while sorting table view
    *   @param  left index of first item to compare
    *   @param  right index of second item to compare
    *   @return true if left item is smaller than right, false if otherwise
    */
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    int updatedSortNdx() const;

    int _currSortNdx;
    Qt::SortOrder _currSortOrder;
    bool _filterActive = false;
};
#endif
