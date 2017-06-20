#ifndef CANDEVICEQT_HPP_JYBV8GIQ
#define CANDEVICEQT_HPP_JYBV8GIQ

#include "candeviceinterface.hpp"

#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusDevice>

struct CanDeviceQt : public CanDeviceInterface {
    CanDeviceQt(const QString& backend, const QString& iface)
    {
        QString errorString;
        _device = QCanBus::instance()->createDevice(backend, iface, &errorString);
        if (!_device) {
            throw std::runtime_error("Unable to create candevice");
        }
    }

    virtual void framesWritten(const framesWritten_t& cb) override
    {
        QObject::connect(_device, &QCanBusDevice::framesWritten, cb);
    }

    virtual void framesReceived(const framesReceived_t& cb) override
    {
        QObject::connect(_device, &QCanBusDevice::framesReceived, cb);
    }

    virtual void errorOccurred(const errorOccurred_t& cb) override
    {
        QObject::connect(_device, &QCanBusDevice::errorOccurred, cb);
    }

    virtual bool writeFrame(const QCanBusFrame& frame) override
    {
        return _device->writeFrame(frame);
    }

    virtual bool connectDevice() override
    {
        return _device->connectDevice();
    }
    virtual qint64 framesAvailable()
    {
        return _device->framesAvailable();
    }

    virtual QCanBusFrame readFrame() noexcept
    {
        return _device->readFrame();
    }

private:
    QCanBusDevice* _device = nullptr;
};

#endif /* end of include guard: CANDEVICEQT_HPP_JYBV8GIQ */
