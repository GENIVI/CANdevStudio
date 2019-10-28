#include "cansignalsender_p.h"
#include <log.h>

CanSignalSenderPrivate::CanSignalSenderPrivate(CanSignalSender* q, CanSignalSenderCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanSignalSenderGuiInt>())
    , q_ptr(q)
{
    initProps();

    connect(
        &_db, &CanDbHandler::currentDbNameChanged, [this](const QString& name) { _props[_dbProperty] = name; });

    connect(&_db, &CanDbHandler::sendCanDbRequest, [this] {
        QJsonObject msg;
        msg["msg"] = BcastMsg::RequestCanDb;

        emit q_ptr->simBcastSnd(msg);
    });
}

void CanSignalSenderPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanSignalSenderPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalSenderPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanSignalSenderPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }
}
