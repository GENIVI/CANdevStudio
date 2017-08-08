#ifndef CANDEVICEQT_HPP_JYBV8GIQ
#define CANDEVICEQT_HPP_JYBV8GIQ

#include "candeviceinterface.hpp"

#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusDevice>

struct CanDeviceQt : public CanDeviceInterface {
    CanDeviceQt(const QString& backend, const QString& iface)
    {
        _device = QCanBus::instance()->createDevice(backend.toUtf8(), iface);
        if (!_device) {
            throw std::runtime_error("Unable to create candevice");
        }
    }

    virtual void setFramesWrittenCbk(const framesWritten_t& cb) override
    {
        QObject::connect(_device, &QCanBusDevice::framesWritten, cb);
    }

    virtual void setFramesReceivedCbk(const framesReceived_t& cb) override
    {
        QObject::connect(_device, &QCanBusDevice::framesReceived, cb);
    }

    virtual void setErrorOccurredCbk(const errorOccurred_t& cb) override
    {
        QObject::connect(_device, &QCanBusDevice::errorOccurred, cb);
    }

    virtual bool writeFrame(const QCanBusFrame& frame) override { return _device->writeFrame(frame); }

    virtual bool connectDevice() override { return _device->connectDevice(); }
    virtual qint64 framesAvailable() override { return _device->framesAvailable(); }

    virtual QCanBusFrame readFrame() noexcept override { return _device->readFrame(); }

private:
    QCanBusDevice* _device = nullptr;
};

#endif /* end of include guard: CANDEVICEQT_HPP_JYBV8GIQ */
