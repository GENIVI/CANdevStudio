#include "candevice.h"
#include "candevice_p.h"
#include "confighelpers.h"
#include <QtCore/QQueue>
#include <QVariant>
#include <iostream>

CanDevice::CanDevice()
    : d_ptr(new CanDevicePrivate())
{
}

CanDevice::CanDevice(CanDeviceCtx&& ctx)
    : d_ptr(new CanDevicePrivate(std::move(ctx)))
{
}

CanDevice::~CanDevice()
{
}

bool CanDevice::init(const QString& backend, const QString& interface, bool saveConfig)
{
    Q_D(CanDevice);
    QString errorString;

    if (saveConfig)
    {
        d->_props[d->_backendProperty] = backend;
        d->_props[d->_interfaceProperty] = interface;
    }

    if (d->_initialized)
        d->_canDevice.clearCallbacks();

    d->_initialized = false;

    if (d->_canDevice.init(backend, interface)) {
        d->_canDevice.setFramesWrittenCbk(std::bind(&CanDevice::framesWritten, this, std::placeholders::_1));
        d->_canDevice.setFramesReceivedCbk(std::bind(&CanDevice::framesReceived, this));
        d->_canDevice.setErrorOccurredCbk(std::bind(&CanDevice::errorOccurred, this, std::placeholders::_1));

        d->_initialized = true;
    }

    return d->_initialized;
}

bool CanDevice::init()
{
    Q_D(CanDevice);

    const auto& props   = d->_props;
    const auto& backend = d->_backendProperty;
    const auto& iface   = d->_interfaceProperty;

    // check if required properties are set
    const bool propertiesSet = (props.count(backend) == 1)
        && (props.count(iface) == 1);

    if (!propertiesSet)
        return d->_initialized;

    QString errorString;

    if (d->_initialized)
        d->_canDevice.clearCallbacks();

    d->_initialized = false;

    QString b = props.at(backend).toString();
    QString i = props.at(iface).toString();
    if (d->_canDevice.init(b, i)) {
        d->_canDevice.setFramesWrittenCbk(std::bind(&CanDevice::framesWritten, this, std::placeholders::_1));
        d->_canDevice.setFramesReceivedCbk(std::bind(&CanDevice::framesReceived, this));
        d->_canDevice.setErrorOccurredCbk(std::bind(&CanDevice::errorOccurred, this, std::placeholders::_1));

        d->_initialized = true;
    }

    return d->_initialized;
}

void CanDevice::sendFrame(const QCanBusFrame& frame)
{
    Q_D(CanDevice);
    bool status = false;

    if (!d->_initialized) {
        return;
    }

    // Success will be reported in framesWritten signal.
    // Sending may be buffered. Keep correlation between sending results and frame/context
    d->_sendQueue.push_back(frame);

    status = d->_canDevice.writeFrame(frame);

    if (!status) {
        emit frameSent(status, frame);
        d->_sendQueue.takeFirst();
    }
}

ComponentInterface::ComponentProperties CanDevice::getSupportedProperties() const
{
    return d_ptr->_supportedProps;
}

void CanDevice::framesReceived()
{
    Q_D(CanDevice);

    if (!d->_initialized) {
        return;
    }

    while (static_cast<bool>(d->_canDevice.framesAvailable())) {
        const QCanBusFrame frame = d->_canDevice.readFrame();
        emit frameReceived(frame);
    }
}

void CanDevice::framesWritten(qint64)
{
    Q_D(CanDevice);

    if (!d->_sendQueue.isEmpty()) {
        auto sendItem = d->_sendQueue.takeFirst();
        emit frameSent(true, sendItem);
    }
}

void CanDevice::errorOccurred(int error)
{
    Q_D(CanDevice);

    if (error == QCanBusDevice::WriteError && !d->_sendQueue.isEmpty()) {
        auto sendItem = d->_sendQueue.takeFirst();
        emit frameSent(false, sendItem);
    }
}

void CanDevice::setConfig(const QJsonObject&)
{
    // TODO
}

QJsonObject CanDevice::getConfig() const
{
    // TODO
    return {};
}

void CanDevice::setConfig(const QObject& qobject)
{
    Q_D(CanDevice);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

std::shared_ptr<QObject> CanDevice::getQConfig() const
{
    const Q_D(CanDevice);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanDevice::startSimulation()
{
    Q_D(CanDevice);

    if (!d->_initialized) {
        cds_info("CanDevice not initialized");
        return;
    }

    if (!d->_canDevice.connectDevice()) {
        cds_error("Failed to connect device");
    }
}

void CanDevice::stopSimulation()
{
    Q_D(CanDevice);

    if (!d->_initialized) {
        cds_info("CanDevice not initialized");
        return;
    }

    d->_canDevice.disconnectDevice();
}

QWidget* CanDevice::mainWidget()
{
    // Component does not have main widget
    return nullptr;
}

bool CanDevice::mainWidgetDocked() const
{
    // Widget does not exist. Return always true
    return true;
}

void CanDevice::configChanged()
{
    init();
}
