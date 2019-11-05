#include "cansignalencoder.h"
#include "cansignalencoder_p.h"
#include <confighelpers.h>
#include <log.h>

CanSignalEncoder::CanSignalEncoder()
    : d_ptr(new CanSignalEncoderPrivate(this))
{
}

CanSignalEncoder::CanSignalEncoder(CanSignalEncoderCtx&& ctx)
    : d_ptr(new CanSignalEncoderPrivate(this, std::move(ctx)))
{
}

CanSignalEncoder::~CanSignalEncoder()
{
}

QWidget* CanSignalEncoder::mainWidget()
{
    // Component does not have main widget
    return nullptr;
}

void CanSignalEncoder::setConfig(const QJsonObject& json)
{
    Q_D(CanSignalEncoder);

    d->setSettings(json);
}

void CanSignalEncoder::setConfig(const QWidget& qobject)
{
    Q_D(CanSignalEncoder);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject CanSignalEncoder::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanSignalEncoder::getQConfig() const
{
    const Q_D(CanSignalEncoder);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanSignalEncoder::configChanged()
{
}

bool CanSignalEncoder::mainWidgetDocked() const
{
    // Widget does not exist. Return always true
    return true;
}

ComponentInterface::ComponentProperties CanSignalEncoder::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanSignalEncoder::stopSimulation()
{
    Q_D(CanSignalEncoder);

    d->_simStarted = false;
}

void CanSignalEncoder::startSimulation()
{
    Q_D(CanSignalEncoder);

    d->_simStarted = true;
}

void CanSignalEncoder::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
