#include "cansignalsender_p.h"
#include <log.h>

CanSignalSenderPrivate::CanSignalSenderPrivate(CanSignalSender* q, CanSignalSenderCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanSignalSenderGuiInt>())
    , q_ptr(q)
{
    initProps();

    connect(&_db, &CanDbHandler::sendCanDbRequest, [this] {
        QJsonObject msg;
        msg["msg"] = BcastMsg::RequestCanDb;

        emit q_ptr->simBcastSnd(msg);
    });

    connect(&_db, &CanDbHandler::dbDeleted, [this](const QUuid& id) {
        if (id == _db._currentDb) {
            _props[_dbProperty] = "";
            for (auto& iter : _db._dbNames) {
                if (!iter.first.isNull()) {
                    _props[_dbProperty] = iter.first.toString();
                    _db.updateCurrentDb(_props[_dbProperty]);
                }
            }
        }
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

    _db.updateCurrentDb(_props[_dbProperty]);
}
