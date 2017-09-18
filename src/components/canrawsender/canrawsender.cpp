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

QWidget* CanRawSender::mainWidget()
{
    Q_D(CanRawSender);

    return d->_ui.mainWidget();
}
int CanRawSender::getLineCount() const
{
    assert(d_ptr != nullptr);
    return d_ptr->getLineCount();
}

void CanRawSender::setConfig(const QJsonObject& json)
{
    assert(d_ptr != nullptr);
    d_ptr->restoreConfiguration(json);
}

QJsonObject CanRawSender::getConfig() const
{
    QJsonObject config;

    d_ptr->saveSettings(config);

    return config;
}

bool CanRawSender::mainWidgetDocked() const
{
    return d_ptr->docked;
}
