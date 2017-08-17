#include "canrawview.h"
#include "canrawview_p.h"
#include "log.hpp"
#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSerialBus/QCanBusFrame>

CanRawView::CanRawView()
    : d_ptr(new CanRawViewPrivate(this))
{
    Q_D(CanRawView);
}

CanRawView::CanRawView(CRVFactoryInterface& factory)
    : d_ptr(new CanRawViewPrivate(this, factory))
{
    Q_D(CanRawView);
}

CanRawView::~CanRawView() {}

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
