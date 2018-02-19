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

    d_ptr->setSettings(json);
}

void CanRawPlayer::setConfig(const QObject& qobject)
{
    Q_D(CanRawPlayer);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject CanRawPlayer::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QObject> CanRawPlayer::getQConfig() const
{
    const Q_D(CanRawPlayer);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanRawPlayer::configChanged() 
{
    QString fileName = getQConfig()->property("file").toString();

    cds_info("File to open: '{}'", fileName.toStdString());

    d_ptr->loadTraceFile(fileName);
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
