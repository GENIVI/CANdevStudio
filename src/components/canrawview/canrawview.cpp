#include "canrawview.h"
#include "canrawview_p.h"
#include "confighelpers.h"
#include "log.h"
#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSerialBus/QCanBusFrame>

CanRawView::CanRawView()
    : d_ptr(new CanRawViewPrivate(this))
{
}

CanRawView::CanRawView(CanRawViewCtx&& ctx)
    : d_ptr(new CanRawViewPrivate(this, std::move(ctx)))
{
}

CanRawView::~CanRawView()
{
}

ComponentInterface::ComponentProperties CanRawView::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanRawView::startSimulation()
{
    Q_D(CanRawView);

    d->_timer.restart();
    d->_simStarted = true;
    d->clear();
}

void CanRawView::stopSimulation()
{
    Q_D(CanRawView);

    d->_simStarted = false;
}

void CanRawView::frameReceived(const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    d->frameView(frame, "RX");
}

void CanRawView::frameSent(bool status, const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    if (status) {
        d->frameView(frame, "TX");
    }
}

QWidget* CanRawView::mainWidget()
{
    Q_D(CanRawView);

    return d->_ui.mainWidget();
}

void CanRawView::setConfig(const QJsonObject& json)
{
    assert(d_ptr != nullptr);
    d_ptr->restoreConfiguration(json);
}

QJsonObject CanRawView::getConfig() const
{
    // Q_D(CanRawView);
    QJsonObject config;

    d_ptr->saveSettings(config);

    return config;
}

void CanRawView::setConfig(const QObject& qobject)
{
    Q_D(CanRawView);

    for (const auto& p: getSupportedProperties())
    {
        QVariant v = qobject.property(p.first.toStdString().c_str());
        if (v.isValid() && v.type() == p.second.first)
            d->_props[p.first] = v;
    }
}

std::shared_ptr<QObject> CanRawView::getQConfig() const
{
    const Q_D(CanRawView);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

bool CanRawView::mainWidgetDocked() const
{
    return d_ptr->docked;
}
