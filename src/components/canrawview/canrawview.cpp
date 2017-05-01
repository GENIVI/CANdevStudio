#include <QStandardItem>
#include <QCanBusFrame>
#include <QStringList>
#include <QList>
#include "canrawview.h"
#include "canrawview_p.h"

CanRawView::CanRawView(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CanRawViewPrivate())
{
    Q_D(CanRawView);

    d->setupUi(this);
}

CanRawView::~CanRawView()
{
}

void CanRawView::frameReceived(const QCanBusFrame &frame)
{
    Q_D(CanRawView);

    if(d->tvModel->rowCount() >= d->cMaxListSize) {
        clear();
    }

    auto payHex = frame.payload().toHex();
    for(int i = payHex.size(); i >= 2; i-=2) {
        payHex.insert(i, ' ');
    }

    QList<QStandardItem*> list;
    list.append(new QStandardItem(d->elapsedTime()));
    list.append(new QStandardItem("RX"));
    list.append(new QStandardItem("0x" + QString::number(frame.frameId(), 16)));
    list.append(new QStandardItem(QString::number(frame.payload().size())));
    list.append(new QStandardItem(QString::fromUtf8(payHex.data(), payHex.size())));

    d->tvModel->appendRow(list);
    d->tv->scrollToBottom();
}

void CanRawView::frameSent(bool status, const QCanBusFrame &frame, const QVariant &)
{
    Q_D(CanRawView);

    if(d->tvModel->rowCount() >= d->cMaxListSize) {
        clear();
    }

    if(status) {
        auto payHex = frame.payload().toHex();
        for(int i = payHex.size(); i >= 2; i-=2) {
            payHex.insert(i, ' ');
        }

        QList<QStandardItem*> list;
        list.append(new QStandardItem(d->elapsedTime()));
        list.append(new QStandardItem("TX"));
        list.append(new QStandardItem("0x" + QString::number(frame.frameId(), 16)));
        list.append(new QStandardItem(QString::number(frame.payload().size())));
        list.append(new QStandardItem(QString::fromUtf8(payHex.data(), payHex.size())));

        d->tvModel->appendRow(list);
        d->tv->scrollToBottom();
    }
}

void CanRawView::clear()
{
    Q_D(CanRawView);

    d->clear();
}

void CanRawView::start()
{
    Q_D(CanRawView);

    d->startTimer();
}
