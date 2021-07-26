#ifndef __SEARCHMODEL_H
#define __SEARCHMODEL_H

#include <QSortFilterProxyModel>

class SearchModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    bool isFilterActive() const;

    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    QStringList mimeTypes() const override;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public slots:
    void updateFilter(const QString& filter);

private:
    QString _filter;
};

#endif /* !__SEARCHMODEL_H */
