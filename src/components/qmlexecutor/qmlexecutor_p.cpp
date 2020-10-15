#include "qmlexecutor_p.h"
#include "canbusmodel.hpp"

#include <log.h>


QMLExecutorPrivate::QMLExecutorPrivate(QMLExecutor *q, QMLExecutorCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<QMLExecutorGuiInt>())
    , q_ptr(q)
{
    initProps();

    QObject::connect(&_ui, &QMLExecutorGuiInt::QMLLoaded, this, &QMLExecutorPrivate::QMLLoaded);

    QObject::connect(&_ui, &QMLExecutorGuiInt::dockUndock, [this] {
        _docked = !_docked;
        emit q_ptr->mainWidgetDockToggled(_ui.mainWidget());
    });
}

QMLExecutorPrivate::~QMLExecutorPrivate()
{
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

    _ui.loadQML(QUrl::fromLocalFile(fileName));
}

void QMLExecutorPrivate::setCANBusModel(CANBusModel* model)
{
    _model = model;
    _ui.setModel(model);
}

void QMLExecutorPrivate::QMLLoaded(const QUrl& url)
{
    _props[_fileProperty] = url.toLocalFile();
}

void QMLExecutorPrivate::startSimulation()
{
    if (_model)
    {
        emit _model->simulationStarted();
    }
}

void QMLExecutorPrivate::stopSimulation()
{
    if (_model)
    {
        emit _model->simulationStopped();
    }
}

void QMLExecutorPrivate::updateUIColor()
{
    _ui.updateUIColor();
}
