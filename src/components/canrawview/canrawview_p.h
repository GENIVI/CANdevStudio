#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "log.hpp"
#include "ui_canrawview.h"
#include <QtCore/QElapsedTimer>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>
#include <memory>

namespace Ui {
class CanRawViewPrivate;
}

class QElapsedTimer;

class CanRawViewPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:
    CanRawViewPrivate(CanRawView* q)
        : ui(std::make_unique<Ui::CanRawViewPrivate>())
        , timer(std::make_unique<QElapsedTimer>())
        , simStarted(false)
        , q_ptr(q)
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({ "time", "id", "dir", "dlc", "data" });
        ui->tv->setModel(&tvModel);
        ui->tv->setColumnWidth(0, 36);
        ui->tv->setColumnWidth(1, 92);
        ui->tv->setColumnWidth(2, 27);
        ui->tv->setColumnWidth(3, 25);
        ui->tv->setColumnWidth(4, 178);

        connect(ui->pbClear, &QPushButton::pressed, this, &CanRawViewPrivate::clear);
        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawViewPrivate::dockUndock);
    }

    ~CanRawViewPrivate() {}

    void frameView(const QCanBusFrame& frame, const QString& direction)
    {
        if (!simStarted) {
            cds_debug("send/received frame while simulation stopped");
            return;
        }

        auto payHex = frame.payload().toHex();
        for (int ii = payHex.size(); ii >= 2; ii -= 2) {
            payHex.insert(ii, ' ');
        }

        QList<QStandardItem*> list;
        list.append(new QStandardItem(QString::number((double)timer->elapsed() / 1000, 'f', 2)));
        list.append(new QStandardItem("0x" + QString::number(frame.frameId(), 16)));
        list.append(new QStandardItem(direction));
        list.append(new QStandardItem(QString::number(frame.payload().size())));
        list.append(new QStandardItem(QString::fromUtf8(payHex.data(), payHex.size())));

        tvModel.appendRow(list);

        if (ui->freezeBox->isChecked() == false) {
            ui->tv->scrollToBottom();
        }
    }

    std::unique_ptr<Ui::CanRawViewPrivate> ui;
    std::unique_ptr<QElapsedTimer> timer;
    QStandardItemModel tvModel;
    bool simStarted;

private:
    CanRawView* q_ptr;

private slots:
    void clear() {}

    void dockUndock()
    {
        Q_Q(CanRawView);
        emit q->dockUndock();
    }
};
#endif // CANRAWVIEW_P_H
