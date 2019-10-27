#include "canrawfilter.h"
#include "canrawfilter_p.h"
#include <confighelpers.h>
#include <log.h>
#include <QCanBusFrame>

CanRawFilter::CanRawFilter()
    : d_ptr(new CanRawFilterPrivate(this))
{
}

CanRawFilter::CanRawFilter(CanRawFilterCtx&& ctx)
    : d_ptr(new CanRawFilterPrivate(this, std::move(ctx)))
{
}

CanRawFilter::~CanRawFilter()
{
}

QWidget* CanRawFilter::mainWidget()
{
    Q_D(CanRawFilter);

    return d->_ui.mainWidget();
}

void CanRawFilter::setConfig(const QJsonObject& json)
{
    Q_D(CanRawFilter);

    d->setSettings(json);
}

void CanRawFilter::setConfig(const QWidget& qobject)
{
    Q_D(CanRawFilter);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject CanRawFilter::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanRawFilter::getQConfig() const
{
    const Q_D(CanRawFilter);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanRawFilter::configChanged()
{
}

bool CanRawFilter::mainWidgetDocked() const
{
    return d_ptr->_docked;
}

ComponentInterface::ComponentProperties CanRawFilter::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanRawFilter::stopSimulation()
{
    Q_D(CanRawFilter);

    d->_simStarted = false;
}

void CanRawFilter::startSimulation()
{
    Q_D(CanRawFilter);

    d->_simStarted = true;
}

void CanRawFilter::txFrameIn(const QCanBusFrame& frame)
{
    Q_D(CanRawFilter);

    if(d->acceptTxFrame(frame) && d->_simStarted) {
        emit txFrameOut(frame);
    }
}

void CanRawFilter::rxFrameIn(const QCanBusFrame& frame)
{
    Q_D(CanRawFilter);

    if(d->acceptRxFrame(frame) && d->_simStarted) {
        emit rxFrameOut(frame);
    }
}

void CanRawFilter::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
