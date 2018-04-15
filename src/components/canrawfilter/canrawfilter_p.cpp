#include "canrawfilter_p.h"
#include <log.h>

CanRawFilterPrivate::CanRawFilterPrivate(CanRawFilter *q, CanRawFilterCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanRawFilterGuiInt>())
    , q_ptr(q)
{
    initProps();
}

void CanRawFilterPrivate::initProps()
{
    for (const auto& p: _supportedProps)
    {
        _props[p.first];
    }
}

ComponentInterface::ComponentProperties CanRawFilterPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanRawFilterPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanRawFilterPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        if (json.contains(p.first))
            _props[p.first] = json[p.first].toVariant();
    }
}
