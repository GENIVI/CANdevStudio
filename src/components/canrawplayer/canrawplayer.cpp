#include "canrawplayer.h"
#include "canrawplayer_p.h"
#include <QTimer>
#include <QtSerialBus/QCanBusFrame>
#include <confighelpers.h>
#include <log.h>

CanRawPlayer::CanRawPlayer()
    : d_ptr(new CanRawPlayerPrivate(this))
{
}

CanRawPlayer::CanRawPlayer(CanRawPlayerCtx&& ctx)
    : d_ptr(new CanRawPlayerPrivate(this, std::move(ctx)))
{
}

CanRawPlayer::~CanRawPlayer() {}

QWidget* CanRawPlayer::mainWidget()
{
    // Component does not have main widget
    return nullptr;
}

void CanRawPlayer::setConfig(const QJsonObject& json)
{
    Q_D(CanRawPlayer);

    d->setSettings(json);
}

void CanRawPlayer::setConfig(const QWidget& qobject)
{
    Q_D(CanRawPlayer);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject CanRawPlayer::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanRawPlayer::getQConfig() const
{
    const Q_D(CanRawPlayer);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanRawPlayer::configChanged()
{
    QString fileName = getQConfig()->property(d_ptr->_fileProperty.toStdString().c_str()).toString();

    cds_info("File to open: '{}'", fileName.toStdString());

    d_ptr->loadTraceFile(fileName);

    QString tick = getQConfig()->property(d_ptr->_tickProperty.toStdString().c_str()).toString();
    d_ptr->_tick = tick.toUInt();
    cds_debug("Tick set to {}", d_ptr->_tick);
}

bool CanRawPlayer::mainWidgetDocked() const
{
    // Widget does not exist. Return always true
    return true;
}

ComponentInterface::ComponentProperties CanRawPlayer::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanRawPlayer::stopSimulation()
{
    Q_D(CanRawPlayer);

    d->_simStarted = false;
    d->stopPlayback();
}

void CanRawPlayer::startSimulation()
{
    Q_D(CanRawPlayer);

    d->_simStarted = true;
    d->startPlayback();
}

void CanRawPlayer::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
