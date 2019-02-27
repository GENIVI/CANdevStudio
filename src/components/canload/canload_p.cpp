#include "canload_p.h"
#include <log.h>

CanLoadPrivate::CanLoadPrivate(CanLoad* q, CanLoadCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{
    initProps();

    _bitrate = _props[_bitrateProperty].toInt();
    _period = _props[_periodProperty].toInt();

    connect(&_timer, &QTimer::timeout, [this] {
        if (_div) {
            uint8_t load = static_cast<uint8_t>((_totalBits * 100) / _div);
            _totalBits = 0;
            emit q_ptr->currentLoad(load);
        }
    });
}

void CanLoadPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }

    // Default value
    _props[_periodProperty] = "1000";
    _props[_bitrateProperty] = "500000";
}

ComponentInterface::ComponentProperties CanLoadPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanLoadPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanLoadPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        QString propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }

    _bitrate = _props[_bitrateProperty].toInt();
    _period = _props[_periodProperty].toInt();
}


