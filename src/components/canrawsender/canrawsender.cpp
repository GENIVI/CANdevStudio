#include "canrawsender.h"
#include "canrawsender_p.h"
#include <cassert>

CanRawSender::CanRawSender()
    : d_ptr(new CanRawSenderPrivate(this))
{
}

CanRawSender::CanRawSender(CRSFactoryInterface& factory)
    : d_ptr(new CanRawSenderPrivate(this, factory))
{
}

CanRawSender::~CanRawSender()
{
}

void CanRawSender::startSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->SetSimulationState(true);
}

void CanRawSender::stopSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->SetSimulationState(false);
}

QWidget* CanRawSender::getMainWidget()
{
    Q_D(CanRawSender);

    return d->mUi->getMainWidget();
}
