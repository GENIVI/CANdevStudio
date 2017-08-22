
#include "canrawview.h"
#include "canrawview_p.h"
#include "log.hpp"
#include "uibackend.hpp"

#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSerialBus/QCanBusFrame>

#include <cassert> // assert
#include <memory> // unique_ptr


CanRawView::CanRawView()
    : d_ptr{new CanRawViewPrivate{this}}
{
    Q_D(CanRawView);
}

CanRawView::CanRawView(std::unique_ptr<UIBackend<CanRawView>> backend)
    : d_ptr{new CanRawViewPrivate{this, std::move(backend)}}
{
    Q_D(CanRawView);
}

void CanRawView::startSimulation()
{
    Q_D(CanRawView);

    assert(nullptr != d);
    assert(nullptr != d->timer);

    d->timer->restart();
    d->simStarted = true;
}

void CanRawView::stopSimulation()
{
    Q_D(CanRawView);

    assert(nullptr != d);

    d->simStarted = false;
}

void CanRawView::frameReceived(const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    assert(nullptr != d);

    d->frameView(frame, "RX");
}

void CanRawView::frameSent(bool status, const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    assert(nullptr != d);

    if (status) {
        d->frameView(frame, "TX");
    }
}

QWidget* CanRawView::getMainWidget()
{
    Q_D(CanRawView);

    assert(nullptr != d);
    assert(nullptr != d->backend);

    return d->backend->getMainWidget();
}

