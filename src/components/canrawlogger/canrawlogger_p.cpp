#include "canrawlogger_p.h"
#include <log.h>

CanRawLoggerPrivate::CanRawLoggerPrivate(CanRawLogger* q, CanRawLoggerCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{
    initProps();
}

void CanRawLoggerPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[p.first];
    }

    _props[_dirProperty] = ".";
}

ComponentInterface::ComponentProperties CanRawLoggerPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanRawLoggerPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanRawLoggerPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        if (json.contains(p.first))
            _props[p.first] = json[p.first].toVariant();
    }
}

void CanRawLoggerPrivate::logFrame(const QCanBusFrame& frame, const QString& dir) 
{
    if(_file.exists()) {

    }
}
