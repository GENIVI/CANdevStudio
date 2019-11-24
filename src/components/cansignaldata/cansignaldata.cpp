#include "cansignaldata.h"
#include "cansignaldata_p.h"
#include <bcastmsgs.h>
#include <confighelpers.h>
#include <log.h>

CanSignalData::CanSignalData()
    : d_ptr(new CanSignalDataPrivate(this))
{
}

CanSignalData::CanSignalData(CanSignalDataCtx&& ctx)
    : d_ptr(new CanSignalDataPrivate(this, std::move(ctx)))
{
}

CanSignalData::~CanSignalData() {}

QWidget* CanSignalData::mainWidget()
{
    Q_D(CanSignalData);

    return d->_ui.mainWidget();
}

void CanSignalData::setConfig(const QJsonObject& json)
{
    d_ptr->setSettings(json);
}

void CanSignalData::setConfig(const QWidget& qobject)
{
    Q_D(CanSignalData);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject CanSignalData::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanSignalData::getQConfig() const
{
    const Q_D(CanSignalData);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanSignalData::configChanged()
{
    QString fileName = getQConfig()->property("file").toString();

    cds_info("File to open: '{}'", fileName.toStdString());

    d_ptr->loadDbc(fileName.toStdString());
}

bool CanSignalData::mainWidgetDocked() const
{
    return d_ptr->_docked;
}

ComponentInterface::ComponentProperties CanSignalData::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanSignalData::stopSimulation()
{
    Q_D(CanSignalData);

    d->_simStarted = false;
}

void CanSignalData::startSimulation()
{
    Q_D(CanSignalData);

    d->_simStarted = true;
}

void CanSignalData::simBcastRcv(const QJsonObject& msg, const QVariant& param)
{
    Q_UNUSED(param);

    Q_D(CanSignalData);

    QVariant vMsg = msg["msg"];
    if ((vMsg.isValid() && vMsg.toString() == BcastMsg::RequestCanDb)
        || (vMsg.isValid() && vMsg.toString() == BcastMsg::InitDone)) {
        d->sendCANdbUpdated();
    }
}
