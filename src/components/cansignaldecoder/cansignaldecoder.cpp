#include "cansignaldecoder.h"
#include "cansignaldecoder_p.h"
#include <confighelpers.h>
#include <log.h>

CanSignalDecoder::CanSignalDecoder()
    : d_ptr(new CanSignalDecoderPrivate(this))
{
}

CanSignalDecoder::CanSignalDecoder(CanSignalDecoderCtx&& ctx)
    : d_ptr(new CanSignalDecoderPrivate(this, std::move(ctx)))
{
}

CanSignalDecoder::~CanSignalDecoder()
{
}

QWidget* CanSignalDecoder::mainWidget()
{
    // Component does not have main widget
    return nullptr;
}

void CanSignalDecoder::setConfig(const QJsonObject& json)
{
    Q_D(CanSignalDecoder);

    d->setSettings(json);
}

void CanSignalDecoder::setConfig(const QWidget& qobject)
{
    Q_D(CanSignalDecoder);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject CanSignalDecoder::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanSignalDecoder::getQConfig() const
{
    const Q_D(CanSignalDecoder);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanSignalDecoder::configChanged()
{
}

bool CanSignalDecoder::mainWidgetDocked() const
{
    // Widget does not exist. Return always true
    return true;
}

ComponentInterface::ComponentProperties CanSignalDecoder::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanSignalDecoder::stopSimulation()
{
    Q_D(CanSignalDecoder);

    d->_simStarted = false;
}

void CanSignalDecoder::startSimulation()
{
    Q_D(CanSignalDecoder);

    d->_simStarted = true;
}

void CanSignalDecoder::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
