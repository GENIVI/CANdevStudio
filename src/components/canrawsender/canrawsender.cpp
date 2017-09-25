#include "canrawsender.h"
#include "canrawsender_p.h"
#include <cassert>

CanRawSender::CanRawSender()
    : d_ptr(new CanRawSenderPrivate(this))
{
}

CanRawSender::CanRawSender(CanRawSenderCtx&& ctx)
    : d_ptr(new CanRawSenderPrivate(this, std::move(ctx)))
{
}

CanRawSender::~CanRawSender()
{
}

void CanRawSender::setConfig(const QObject& qobject)
{
    Q_D(CanRawSender);

    for (const auto& p: getSupportedProperties())
    {
        QVariant v = qobject.property(p.first.toStdString().c_str());
        if (!v.isValid() || v.type() != p.second.first)
            continue;

        d->_props[p.first] = v;
    }
}

std::shared_ptr<QObject> CanRawSender::getQConfig() const
{
    const Q_D(CanRawSender);

    std::shared_ptr<QObject> q = std::make_shared<QObject>();

    QStringList props;
    for (auto& p: getSupportedProperties())
    {
        if (!p.second.second) // property not editable
            continue;

        props.push_back(p.first);
        q->setProperty(p.first.toStdString().c_str(), d->_props.at(p.first));
    }

    q->setProperty("exposedProperties", props);

    return q;
}

void CanRawSender::startSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->setSimulationState(true);
}

void CanRawSender::stopSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->setSimulationState(false);
}

QWidget* CanRawSender::mainWidget()
{
    Q_D(CanRawSender);

    return d->_ui.mainWidget();
}
int CanRawSender::getLineCount() const
{
    assert(d_ptr != nullptr);
    return d_ptr->getLineCount();
}

void CanRawSender::setConfig(const QJsonObject& json)
{
    assert(d_ptr != nullptr);
    d_ptr->restoreConfiguration(json);
}

QJsonObject CanRawSender::getConfig() const
{
    QJsonObject config;

    d_ptr->saveSettings(config);

    return config;
}

bool CanRawSender::mainWidgetDocked() const
{
    return d_ptr->docked;
}

ComponentInterface::ComponentProperties CanRawSender::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}
