#ifndef UNIQUEFILTERMODEL_H
#define UNIQUEFILTERMODEL_H

#include <QSortFilterProxyModel>

class UniqueFilterModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit UniqueFilterModel(QObject* parent = 0);

    void updateFilter(int frameID, double time, QString direction);
    void clearFilter();
    bool isFilterActive();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

signals:

public slots:
    void toggleFilter();

private:
    QMap<QPair<int, QString>, double> uniques;
    bool filterActive = false;
};
#endif
