#ifndef CANSIGNALSENDERTABLEMODEL_H
#define CANSIGNALSENDERTABLEMODEL_H

#include <QStandardItemModel>

class CanSignalSenderTableModel : public QStandardItemModel {
    Q_OBJECT

public:
    explicit CanSignalSenderTableModel(QObject* parent = nullptr);
    ~CanSignalSenderTableModel() override = default;

    bool dropMimeData(
        const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
    QStringList mimeTypes() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

signals:
    void droppedItems(const QList<QList<QString>>& dropItems);
};

#endif // CANSIGNALSENDERTABLEMODEL_H
