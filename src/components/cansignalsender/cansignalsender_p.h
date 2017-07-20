#ifndef CANSIGNALSENDER_P_H
#define CANSIGNALSENDER_P_H

#include "cansignalsender.h"
#include "ui_cansignalsender.h"
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>
#include <memory>

namespace Ui {
class CanSignalSenderPrivate;
}

class CanSignalSenderPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalSender)

public:
    CanSignalSenderPrivate(CanSignalSender* q)
    : q_ptr(q) 
    , ui(std::make_unique<Ui::CanSignalSenderPrivate>())
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({"id", "value",""});
        ui->tv->setModel(&tvModel);

        connect(ui->pbAdd, &QPushButton::pressed, this, &CanSignalSenderPrivate::addNewItem);

        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanSignalSenderPrivate::dockUndock);

    }

    ~CanSignalSenderPrivate()
    {
    }


private:
    CanSignalSender* q_ptr;
    std::unique_ptr<Ui::CanSignalSenderPrivate> ui;
    QStandardItemModel tvModel;

private slots:
    void addNewItem()
    {
        QStandardItem* name = new QStandardItem();
        QStandardItem* value = new QStandardItem();
        QList<QStandardItem*> list{ name, value };
        tvModel.appendRow(list);
        QPushButton* pbSend = new QPushButton("Send");
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, 2), pbSend);

        connect(pbSend, &QPushButton::pressed, [this, name, value]() {
            Q_Q(CanSignalSender);

            if (name->text().size() && value->text().size()) {
                quint32 val;
                if (value->text().startsWith("0x"))
                    val = value->text().toUInt(nullptr, 16);
                else
                    val = value->text().toUInt(nullptr, 10);

                QByteArray ba = QByteArray::number(val);
                emit q->sendSignal(name->text(), ba);
            }
        });
    }

    void dockUndock()
    {
        Q_Q(CanSignalSender);
        emit q->dockUndock();
    }

};

#endif // CANSIGNALSENDER_P_H
