#include "candevice.h"
#include "candevice_p.h"
#include "confighelpers.h"
#include <QVariant>
#include <QtCore/QQueue>
#include <iostream>

CanDevice::CanDevice()
    : d_ptr(new CanDevicePrivate(this))
{
}

CanDevice::CanDevice(CanDeviceCtx&& ctx)
    : d_ptr(new CanDevicePrivate(this, std::move(ctx)))
{
}

CanDevice::~CanDevice() {}

bool CanDevice::init()
{
    Q_D(CanDevice);

    const auto& props = d->_props;
    const auto& backend = d->_backendProperty;
    const auto& iface = d->_interfaceProperty;

    // check if required properties are set. They always exists as are initialized in constructor
    if ((props.at(backend).toString().length() == 0) || (props.at(iface).toString().length() == 0)) {
        return d->_initialized;
    }

    QString errorString;

    if (d->_initialized) {
        d->_canDevice.clearCallbacks();
    }

    d->_initialized = false;

    QString b = props.at(backend).toString();
    QString i = props.at(iface).toString();
    if (d->_canDevice.init(b, i)) {
        d->_canDevice.setFramesWrittenCbk(std::bind(&CanDevice::framesWritten, this, std::placeholders::_1));
        d->_canDevice.setFramesReceivedCbk(std::bind(&CanDevice::framesReceived, this));
        d->_canDevice.setErrorOccurredCbk(std::bind(&CanDevice::errorOccurred, this, std::placeholders::_1));

        for (auto &item : d->getDevConfig()) {
            d->_canDevice.setConfigurationParameter(item.first, item.second);
        }

        d->_initialized = true;
    }

    return d->_initialized;
}

void CanDevice::sendFrame(const QCanBusFrame& frame)
{
    Q_D(CanDevice);

    if (!d->_initialized) {
        return;
    }

    // Success will be reported in framesWritten signal.
    // Sending may be buffered. Keep correlation between sending results and frame/context
    d->_sendQueue.push_back(frame);

    d->_canDevice.writeFrame(frame);

    // writeFrame status will be handled by framesWritten and errorOccured slots
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

void CanDevice::framesWritten(qint64 cnt)
{
    Q_D(CanDevice);

    while (cnt--) {
        if (!d->_sendQueue.isEmpty()) {
            auto sendItem = d->_sendQueue.takeFirst();
            emit frameSent(true, sendItem);
        } else {
            cds_warn("Send queue is empty!");
        }
    }
}

void CanDevice::errorOccurred(int error)
{
    Q_D(CanDevice);

    cds_warn("Error occurred. Send queue size {}", d->_sendQueue.count());

    if (error == QCanBusDevice::WriteError && !d->_sendQueue.isEmpty()) {
        auto sendItem = d->_sendQueue.takeFirst();
        emit frameSent(false, sendItem);
    }
}

void CanDevice::setConfig(const QJsonObject& json)
{
    assert(d_ptr != nullptr);
    d_ptr->restoreConfiguration(json);
}

QJsonObject CanDevice::getConfig() const
{
    QJsonObject config;

    d_ptr->saveSettings(config);

    return config;
}

void CanDevice::setConfig(const QWidget& qobject)
{
    Q_D(CanDevice);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

std::shared_ptr<QWidget> CanDevice::getQConfig() const
{
    const Q_D(CanDevice);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanDevice::startSimulation()
{
    Q_D(CanDevice);

    d->_simStarted = true;

    if (!d->_initialized) {
        cds_info("CanDevice not initialized");
        return;
    }

    if (!d->_canDevice.connectDevice()) {
        cds_error("Failed to connect device. Trying to init the device again...");

        // Cannelloni plugin fails to reconnect after initial disconnection.
        // This workaround reinit the device before reconnection
        // TODO: Findout why cannelloni plugin fails.
        if (init()) {
            cds_info("Re-init successful");

            if (!d->_canDevice.connectDevice()) {
                cds_error("Failed to re-connect device");
            } else {
                cds_info("Re-connection successful");
            }
        }
    }

    d->_sendQueue.clear();
}

void CanDevice::stopSimulation()
{
    Q_D(CanDevice);

    d->_simStarted = false;

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
    Q_D(CanDevice);

    init();

    if (d->_simStarted) {
        startSimulation();
    }
}

void CanDevice::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
