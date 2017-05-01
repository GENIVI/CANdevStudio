#include <QList>
#include <QStandardItem>
#include <QString>
#include "cansignalview.h"
#include "cansignalview_p.h"

CanSignalView::CanSignalView(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CanSignalViewPrivate())
{
    Q_D(CanSignalView);

    d->setupUi(this);
}

CanSignalView::~CanSignalView()
{
}

void CanSignalView::signalReceived(const QString &name, const QByteArray &val)
{
    Q_D(CanSignalView);

    if(d->tvModel->rowCount() >= d->cMaxListSize) {
        clear();
    }

    QList<QStandardItem*> list;
    list.append(new QStandardItem(d->elapsedTime()));
    list.append(new QStandardItem("RX"));
    list.append(new QStandardItem(name));
    list.append(new QStandardItem(QString::number(val.toUInt())));
    
    d->tvModel->appendRow(list);
    d->tv->scrollToBottom();
}

void CanSignalView::signalSent(const QString &name, const QByteArray &val)
{
    Q_D(CanSignalView);

    if(d->tvModel->rowCount() >= d->cMaxListSize) {
        clear();
    }

    QList<QStandardItem*> list;
    list.append(new QStandardItem(d->elapsedTime()));
    list.append(new QStandardItem("TX"));
    list.append(new QStandardItem(name));
    list.append(new QStandardItem(QString::number(val.toUInt())));
    
    d->tvModel->appendRow(list);
    d->tv->scrollToBottom();
}

void CanSignalView::clear()
{
    Q_D(CanSignalView);

    d->clear();
}

void CanSignalView::start()
{
    Q_D(CanSignalView);

    d->startTimer();
}
