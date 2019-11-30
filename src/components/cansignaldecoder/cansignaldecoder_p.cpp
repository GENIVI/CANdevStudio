#include "cansignaldecoder_p.h"
#include <log.h>

CanSignalDecoderPrivate::CanSignalDecoderPrivate(CanSignalDecoder *q, CanSignalDecoderCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{
    initProps();

    connect(&_db, &CanDbHandler::sendCanDbRequest, q_ptr, &CanSignalDecoder::simBcastSnd);
    connect(&_db, &CanDbHandler::requestRedraw, q_ptr, &CanSignalDecoder::requestRedraw);
}

void CanSignalDecoderPrivate::initProps()
{
    for (const auto& p: _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanSignalDecoderPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalDecoderPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanSignalDecoderPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }

    _db.updateCurrentDbFromProps();
}
