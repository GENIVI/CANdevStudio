#include "cansignalencoder_p.h"
#include <confighelpers.h>
#include <log.h>

CanSignalEncoder::CanSignalEncoder()
    : d_ptr(new CanSignalEncoderPrivate(this))
{
}

CanSignalEncoder::~CanSignalEncoder() {}

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

    d->_db.updateCurrentDbFromProps();
}

QJsonObject CanSignalEncoder::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanSignalEncoder::getQConfig() const
{
    const Q_D(CanSignalEncoder);

    auto config = configHelpers::getQConfig(getSupportedProperties(), d->_props);

    auto iter = d->_props.find("color");
    if (iter != d->_props.end()) {
        config->setProperty("color", iter->second);
    }

    return config;
}

void CanSignalEncoder::configChanged() {}

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

    for (auto& timer : d_ptr->_cycleTimers) {
        timer->stop();
    }
}

void CanSignalEncoder::startSimulation()
{
    Q_D(CanSignalEncoder);

    d->initCacheAndTimers();

    d->_simStarted = true;

    for (auto& timer : d_ptr->_cycleTimers) {
        timer->start();
    }
}

void CanSignalEncoder::simBcastRcv(const QJsonObject& msg, const QVariant& param)
{
    Q_D(CanSignalEncoder);

    d->_db.processBcast(msg, param);
}

void CanSignalEncoder::rcvSignal(const QString& name, const QVariant& val)
{
    if (d_ptr->_simStarted) {
        cds_debug("Signal received: {}, {}", name.toStdString(), val.toString().toStdString());

        d_ptr->encodeSignal(name, val);
    }
}
