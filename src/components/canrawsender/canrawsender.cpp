#include "canrawsender.h"
#include "canrawsender_p.h"
#include <cassert>

CanRawSender::CanRawSender()
    : d_ptr(new CanRawSenderPrivate(this))
{
}

CanRawSender::CanRawSender(CanRawSenderCtx&& ctx)
    : d_ptr(new CanRawSenderPrivate(this, std::move(ctx)))
{
}

CanRawSender::~CanRawSender()
{
}

void CanRawSender::startSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->setSimulationState(true);
}

void CanRawSender::stopSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->setSimulationState(false);
}

QWidget* CanRawSender::getMainWidget()
{
    Q_D(CanRawSender);

    return d->_ui.getMainWidget();
}
int CanRawSender::getLineCount() const
{
    assert(d_ptr != nullptr);
    return d_ptr->getLineCount();
}

void CanRawSender::saveSettings(QJsonObject& json) const
{
    assert(d_ptr != nullptr);
    return d_ptr->saveSettings(json);
}
