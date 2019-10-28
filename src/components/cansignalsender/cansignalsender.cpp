#include "cansignalsender.h"
#include "cansignalsender_p.h"
#include <QJsonDocument>
#include <bcastmsgs.h>
#include <confighelpers.h>
#include <log.h>


CanSignalSender::CanSignalSender()
    : d_ptr(new CanSignalSenderPrivate(this))
{
}

CanSignalSender::CanSignalSender(CanSignalSenderCtx&& ctx)
    : d_ptr(new CanSignalSenderPrivate(this, std::move(ctx)))
{
}

CanSignalSender::~CanSignalSender() {}

QWidget* CanSignalSender::mainWidget()
{
    Q_D(CanSignalSender);

    return d->_ui.mainWidget();
}

void CanSignalSender::setConfig(const QJsonObject& json)
{
    Q_D(CanSignalSender);

    d->setSettings(json);
}

void CanSignalSender::setConfig(const QWidget& qobject)
{
    Q_D(CanSignalSender);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);

    d->_db.updateCurrentDb(d->_props[d->_dbProperty]);
}

QJsonObject CanSignalSender::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanSignalSender::getQConfig() const
{
    const Q_D(CanSignalSender);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanSignalSender::configChanged() {}

bool CanSignalSender::mainWidgetDocked() const
{
    return d_ptr->_docked;
}

ComponentInterface::ComponentProperties CanSignalSender::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanSignalSender::stopSimulation()
{
    Q_D(CanSignalSender);

    d->_simStarted = false;
}

void CanSignalSender::startSimulation()
{
    Q_D(CanSignalSender);

    d->_simStarted = true;
}

void CanSignalSender::simBcastRcv(const QJsonObject& msg, const QVariant& param)
{
    QJsonDocument doc(msg);

    cds_warn("{}", doc.toJson().toStdString());

    Q_D(CanSignalSender);

    d->_db.processBcast(msg, param);
}
