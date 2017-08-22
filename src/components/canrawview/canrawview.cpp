
#include "canrawview.h"
#include "canrawview_p.h"
#include "log.hpp"
#include "uibackend.hpp"

#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSerialBus/QCanBusFrame>

#include <memory>


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

QWidget* CanRawView::getMainWidget()
{
    Q_D(CanRawView);

    return d->mUi->getMainWidget();
}

