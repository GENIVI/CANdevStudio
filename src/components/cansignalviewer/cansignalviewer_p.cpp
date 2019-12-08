#include "cansignalviewer_p.h"
#include <log.h>

CanSignalViewerPrivate::CanSignalViewerPrivate(CanSignalViewer *q, CanSignalViewerCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanSignalViewerGuiInt>())
    , q_ptr(q)
{
    initProps();
}

void CanSignalViewerPrivate::initProps()
{
    for (const auto& p: _supportedProps)
    {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanSignalViewerPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalViewerPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanSignalViewerPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }
}
