#include "cansignalsendertablemodel.h"
#include "log.h"

#include <QEvent>
#include <QMimeData>

CanSignalSenderTableModel::CanSignalSenderTableModel(QObject* parent)
    : QStandardItemModel(parent)
{
}

bool CanSignalSenderTableModel::dropMimeData(
    const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    cds_info("{} action: {}, row: {}, column: {}", Q_FUNC_INFO, action, row, column);
    Q_UNUSED(parent)

    if (nullptr == data) {
        return false;
    }

    if (Qt::DropAction::CopyAction != action) {
        return false;
    }

    QByteArray encoded = data->data("fromCanDB");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QList<QList<QString>> dropItems;

    while (false == stream.atEnd()) {
        QString id, sig;
        stream >> id >> sig;
        dropItems.push_back(QList<QString>{ id, sig });
    }

    emit droppedItems(dropItems);

    return true;
}

QStringList CanSignalSenderTableModel::mimeTypes() const
{
    return QStringList{ "fromCanDB" };
}

Qt::ItemFlags CanSignalSenderTableModel::flags(const QModelIndex& index) const
{
    if (false == index.isValid()) {
        return QStandardItemModel::flags(index);
    }

    return QStandardItemModel::flags(index) | Qt::ItemFlag::ItemIsDropEnabled;
}
