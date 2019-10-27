#include "canload.h"
#include "canload_p.h"
#include <confighelpers.h>
#include <log.h>
#include <QCanBusFrame>

CanLoad::CanLoad()
    : d_ptr(new CanLoadPrivate(this))
{
}

CanLoad::CanLoad(CanLoadCtx&& ctx)
    : d_ptr(new CanLoadPrivate(this, std::move(ctx)))
{
}

CanLoad::~CanLoad()
{
}

QWidget* CanLoad::mainWidget()
{
    // Component does not have main widget
    return nullptr;
}

void CanLoad::setConfig(const QJsonObject& json)
{
    Q_D(CanLoad);

    d->setSettings(json);
}

void CanLoad::setConfig(const QWidget& qobject)
{
    Q_D(CanLoad);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);

    d->_bitrate = d->_props[d->_bitrateProperty].toInt();
    d->_period = d->_props[d->_periodProperty].toInt();
}

QJsonObject CanLoad::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanLoad::getQConfig() const
{
    const Q_D(CanLoad);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanLoad::configChanged()
{
}

bool CanLoad::mainWidgetDocked() const
{
    // Widget does not exist. Return always true
    return true;
}

ComponentInterface::ComponentProperties CanLoad::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanLoad::stopSimulation()
{
    Q_D(CanLoad);

    d->_timer.stop();

    d->_simStarted = false;

    emit currentLoad(0);
}

void CanLoad::startSimulation()
{
    Q_D(CanLoad);

    d->_totalBits = 0;
    d->_div = static_cast<uint32_t>(static_cast<float>(d->_bitrate) * static_cast<float>(d->_period) / 1000);

    d->_timer.setInterval(d->_period);
    d->_timer.start();

    d->_simStarted = true;
}

void CanLoad::frameIn(const QCanBusFrame& frame)
{
    if(d_ptr->_simStarted) {
        // Load calculation assumes worst case with bit stuffing. 
        // https://github.com/linux-can/can-utils/blob/master/canframelen.c
        d_ptr->_totalBits += (frame.hasExtendedFrameFormat() ? 80 : 55) + frame.payload().length() * 10;        
    }
}

void CanLoad::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
