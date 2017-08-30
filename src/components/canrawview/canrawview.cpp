
#include "canrawview.h"
#include "canrawview_p.h"

#include <QCloseEvent>
#include <QtSerialBus/QCanBusFrame>
#include <QWidget>

#include <cassert> // assert


void CanRawView::startSimulation()
{
    Q_D(CanRawView);

    d->_timer->restart();
    d->_simStarted = true;
}

void CanRawView::stopSimulation()
{
    Q_D(CanRawView);

    d->_simStarted = false;
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

    assert(nullptr != d->backend().getMainWidget());

    auto widget = d->backend().getMainWidget();
    auto parent = widget->parentWidget();

    if (parent) {
        parent->hide();
    } else {
        widget->hide();
    }

    e->ignore();
}

