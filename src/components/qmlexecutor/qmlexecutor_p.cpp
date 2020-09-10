#include "qmlexecutor_p.h"
#include <log.h>


QMLExecutorPrivate::QMLExecutorPrivate(QMLExecutor *q, QMLExecutorCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<QMLExecutorGuiInt>())
    , q_ptr(q)
{
    initProps();

    QObject::connect(&_ui, &QMLExecutorGuiInt::QMLLoaded, this, &QMLExecutorPrivate::QMLLoaded);
}

void QMLExecutorPrivate::initProps()
{
    for (const auto& p: _supportedProps)
    {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties QMLExecutorPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject QMLExecutorPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void QMLExecutorPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }
}

void QMLExecutorPrivate::configChanged()
{
    QString fileName = _props[_fileProperty].toString();

    QUrl url = QUrl::fromLocalFile(fileName);

    _ui.loadQML(url);
}

void QMLExecutorPrivate::startSimulation()
{
}

void QMLExecutorPrivate::stopSimulation()
{
}

void QMLExecutorPrivate::setCANBusModel(CANBusModel* model)
{
    _ui.setModel(model);
}

void QMLExecutorPrivate::QMLLoaded(const QUrl& url)
{
    _props[_fileProperty] = url.toLocalFile();
}
