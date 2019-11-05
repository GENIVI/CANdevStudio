#include "cansignalencoder_p.h"
#include <log.h>

CanSignalEncoderPrivate::CanSignalEncoderPrivate(CanSignalEncoder *q, CanSignalEncoderCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{
    initProps();

    connect(&_db, &CanDbHandler::sendCanDbRequest, q_ptr, &CanSignalEncoder::simBcastSnd);
    connect(&_db, &CanDbHandler::requestRedraw, q_ptr, &CanSignalEncoder::requestRedraw);
}

void CanSignalEncoderPrivate::initProps()
{
    for (const auto& p: _supportedProps)
    {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanSignalEncoderPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalEncoderPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanSignalEncoderPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }

    _db.updateCurrentDbFromProps();
}
