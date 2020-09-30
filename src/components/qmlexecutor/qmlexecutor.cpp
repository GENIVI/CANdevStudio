#include "qmlexecutor.h"
#include "qmlexecutor_p.h"
#include <confighelpers.h>

#include <functional>
#include <log.h>
#include <QtGlobal>


QMLExecutor::QMLExecutor()
    : d_ptr(new QMLExecutorPrivate(this))
{
}

QMLExecutor::QMLExecutor(QMLExecutorCtx&& ctx)
    : d_ptr(new QMLExecutorPrivate(this, std::move(ctx)))
{
}

QMLExecutor::~QMLExecutor()
{
}

QWidget* QMLExecutor::mainWidget()
{
    Q_D(QMLExecutor);

    return d->_ui.mainWidget();
}

void QMLExecutor::setConfig(const QJsonObject& json)
{
    Q_D(QMLExecutor);

    d->setSettings(json);
}

void QMLExecutor::setConfig(const QWidget& qobject)
{
    Q_D(QMLExecutor);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject QMLExecutor::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> QMLExecutor::getQConfig() const
{
    const Q_D(QMLExecutor);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void QMLExecutor::configChanged()
{
    d_ptr->configChanged();
}

bool QMLExecutor::mainWidgetDocked() const
{
    return d_ptr->_docked;
}

ComponentInterface::ComponentProperties QMLExecutor::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void QMLExecutor::stopSimulation()
{
    Q_D(QMLExecutor);

    d->stopSimulation();
}

void QMLExecutor::startSimulation()
{
    Q_D(QMLExecutor);

    d->startSimulation();
}

void QMLExecutor::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}

void QMLExecutor::setCANBusModel(CANBusModel* model)
{
    d_ptr->setCANBusModel(model);
}
