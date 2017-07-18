#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "ui_canrawsender.h"
#include <memory>
#include <QtSerialBus/QCanBusFrame>
#include <QtGui/QStandardItemModel>

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

        tvModel.setHorizontalHeaderLabels({"id", "value",""});
        ui->tv->setModel(&tvModel);

        connect(ui->pbAdd, &QPushButton::pressed, this, &CanRawSenderPrivate::addNewItem);
    }

    ~CanRawSenderPrivate()
    {
    }

private:
    std::unique_ptr<Ui::CanRawSenderPrivate> ui;
    QStandardItemModel tvModel;
    CanRawSender* q_ptr;

private slots:
    void addNewItem()
    {
        QStandardItem* id = new QStandardItem();
        QStandardItem* value = new QStandardItem();
        QList<QStandardItem*> list {id, value};
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
                QVariant ctx = 0;
                emit q->sendFrame(frame, ctx);
            }
        });
    }
};

#endif // CANRAWSENDER_P_H
