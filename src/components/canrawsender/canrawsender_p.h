#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "ui_canrawsender.h"
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>
#include <memory>

namespace Ui {
class CanRawSenderPrivate;
}

class CanRawSenderPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawSender)

public:
    CanRawSenderPrivate(CanRawSender* q)
        : ui(std::make_unique<Ui::CanRawSenderPrivate>())
        , q_ptr(q)
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({ "id", "value", "" });
        ui->tv->setModel(&tvModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->setSelectionBehavior(QAbstractItemView::SelectRows);

        connect(ui->pbAdd, &QPushButton::pressed, this, &CanRawSenderPrivate::addNewItem);
        connect(ui->pbRemove, &QPushButton::pressed, this, &CanRawSenderPrivate::removeRowsSelectedByMouse);
        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawSenderPrivate::dockUndock);
    }

    ~CanRawSenderPrivate() {}

private:
    std::unique_ptr<Ui::CanRawSenderPrivate> ui;
    QStandardItemModel tvModel;
    CanRawSender* q_ptr;

private slots:
    /**
     * @brief removeRowsSelectedByMouse
     *
     *  This function is used to remove selected (selected by mouse) rows.
     *
     */
    void removeRowsSelectedByMouse()
    {
        QModelIndexList IndexList = ui->tv->selectionModel()->selectedRows();
        std::list<QModelIndex> tmp = IndexList.toStdList();
        tmp.sort(); // List must to be sorted and reversed because erasing started from last row
        tmp.reverse();
        for (QModelIndex n : tmp) {
            tvModel.removeRow(n.row());
        }
    }

    void addNewItem()
    {
        QStandardItem* id = new QStandardItem();
        QStandardItem* value = new QStandardItem();
        QList<QStandardItem*> list{ id, value };
        tvModel.appendRow(list);

        QPushButton* pbSend = new QPushButton("Send");
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, 2), pbSend);

        connect(pbSend, &QPushButton::pressed, this, [this, id, value] {
            Q_Q(CanRawSender);

            if (id->text().size() && value->text().size()) {
                quint32 val;
                if (id->text().startsWith("0x"))
                    val = id->text().toUInt(nullptr, 16);
                else
                    val = id->text().toUInt(nullptr, 10);

                QCanBusFrame frame;
                frame.setFrameId(val);
                frame.setPayload(QByteArray::fromHex(value->text().toUtf8()));
                emit q->sendFrame(frame);
            }
        });
    }

    void dockUndock()
    {
        Q_Q(CanRawSender);
        emit q->dockUndock();
    }
};

#endif // CANRAWSENDER_P_H
