
#include "canrawview.h"
#include "canrawview_p.h"

#include <QCloseEvent>
#include <QtSerialBus/QCanBusFrame>


void CanRawView::startSimulation()
{
    Q_D(CanRawView);

    d->timer->restart();
    d->simStarted = true;
}

void CanRawView::stopSimulation()
{
    Q_D(CanRawView);

    d->simStarted = false;
}

void CanRawView::frameReceived(const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    d->frameView(frame, "RX");
}

void CanRawView::frameSent(bool status, const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    if (status) {
        d->frameView(frame, "TX");
    }
}

void CanRawView::closeEvent(QCloseEvent* e)
{
    Q_D(CanRawView);

    auto parent = d->backend().parentWidget();

    if (parent) {
        parent->hide();
    } else {
        d->backend().hide();
    }

    e->ignore();
}

