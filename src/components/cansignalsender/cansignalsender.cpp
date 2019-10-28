#include "cansignalsender.h"
#include "cansignalsender_p.h"
#include <QJsonDocument>
#include <bcastmsgs.h>
#include <confighelpers.h>
#include <log.h>

Q_DECLARE_METATYPE(CANmessages_t);

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

    QVariant vId = msg["id"];
    QUuid id;
    if (vId.isValid()) {
        id = qvariant_cast<QUuid>(vId);
    }

    QVariant vMsg = msg["msg"];
    if (vMsg.isValid() && vMsg.toString() == BcastMsg::CanDbUpdated) {
        CANmessages_t paramMsg;
        if (param.isValid()) {
            paramMsg = qvariant_cast<CANmessages_t>(param);
        }

        d->_candb[id] = paramMsg;
    } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::CanDbUpdated) {
        QVariant vName = msg["caption"];
        QString name;
        if (vName.isValid()) {
            name = vName.toString();
        }

        d->_dbNames[id] = name;
    } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::InitDone) {
        QJsonObject msg;
        msg["msg"] = BcastMsg::RequestCanDb;

        emit simBcastSnd(msg);
    }
}
