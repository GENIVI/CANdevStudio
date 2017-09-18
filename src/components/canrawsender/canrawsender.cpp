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

void CanRawSender::setConfig(const QObject& qobject)
{
    // FIXME: implement
}

std::shared_ptr<QObject> CanRawSender::getQConfig() const
{
    throw std::runtime_error("Not implemented"); // FIXME: implement
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

void CanRawSender::setConfig(QJsonObject&)
{
    // TODO
}

QJsonObject CanRawSender::getConfig() const
{
    QJsonObject config;

    d_ptr->saveSettings(config);

    return config;
}

void CanRawSender::setDockUndockClbk(const std::function<void()>& cb)
{
    Q_D(CanRawSender);

    d->_ui.setDockUndockCbk(cb);
}

bool CanRawSender::mainWidgetDocked() const
{
    return d_ptr->docked;
}
