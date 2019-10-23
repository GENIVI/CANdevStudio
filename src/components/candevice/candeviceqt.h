#ifndef CANDEVICEQT_H_JYBV8GIQ
#define CANDEVICEQT_H_JYBV8GIQ

#include "candeviceinterface.h"
#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusDevice>
#include <log.h>

struct CanDeviceQt : public CanDeviceInterface {

    CanDeviceQt() = default;

    virtual void setParent(QObject* parent) override
    {
        _parent = parent;
    }

    virtual void setFramesWrittenCbk(const framesWritten_t& cb) override
    {
        if (_device) {
            QObject::connect(_device, &QCanBusDevice::framesWritten, cb);
        } else {
            cds_error("Calling connect on null CAN device");
            throw std::runtime_error("Calling connect on null CAN device");
        }
    }

    virtual void setFramesReceivedCbk(const framesReceived_t& cb) override
    {
        if (_device) {
            QObject::connect(_device, &QCanBusDevice::framesReceived, cb);
        } else {
            cds_error("Calling connect on null CAN device");
            throw std::runtime_error("Calling connect on null CAN device");
        }
    }

    virtual void setErrorOccurredCbk(const errorOccurred_t& cb) override
    {
        if (_device) {
            QObject::connect(_device, &QCanBusDevice::errorOccurred, cb);
        } else {
            cds_error("Calling connect on null CAN device");
            throw std::runtime_error("Calling connect on null CAN device");
        }
    }

    virtual bool init(const QString& backend, const QString& iface) override
    {
        delete _device;
        _device = QCanBus::instance()->createDevice(backend.toUtf8(), iface);

        if (!_device) {
            cds_error("Failed to create candevice");

            return false;
        }

        if (_parent && _parent->thread()) {
            _device->moveToThread(_parent->thread());
            _device->setParent(_parent);
        }

        return true;
    }

    virtual bool writeFrame(const QCanBusFrame& frame) override
    {
        if (_device) {
            return _device->writeFrame(frame);
        } else {
            cds_error("candevice is null. Call init first!");
            throw std::runtime_error("candevice is null. Call init first!");
        }

        return false;
    }

    virtual bool connectDevice() override
    {
        if (_device) {
            return _device->connectDevice();
        } else {
            cds_error("candevice is null. Call init first!");
            throw std::runtime_error("candevice is null. Call init first!");
        }
    }
    virtual qint64 framesAvailable() override
    {
        if (_device) {
            return _device->framesAvailable();
        } else {
            cds_error("candevice is null. Call init first!");
            throw std::runtime_error("candevice is null. Call init first!");
        }
    }

    virtual QCanBusFrame readFrame() override
    {
        if (_device) {
            return _device->readFrame();
        } else {
            cds_error("candevice is null. Call init first!");
            throw std::runtime_error("candevice is null. Call init first!");
        }
    }

    virtual void disconnectDevice() override
    {
        if (_device) {
            return _device->disconnectDevice();
        } else {
            cds_error("candevice is null. Call init first!");
            throw std::runtime_error("candevice is null. Call init first!");
        }
    }

    virtual void clearCallbacks() override
    {
        if (_device) {
            _device->disconnect();
        } else {
            cds_error("candevice is null. Call init first!");
            throw std::runtime_error("candevice is null. Call init first!");
        }
    }

    virtual void setConfigurationParameter(int key, const QVariant& value) override
    {
        if (_device) {
            _device->setConfigurationParameter(key, value);
        } else {
            cds_error("candevice is null. Call init first!");
            throw std::runtime_error("candevice is null. Call init first!");
        }
    }

private:
    QCanBusDevice* _device{ nullptr };
    QObject* _parent{ nullptr };
};

#endif /* end of include guard: CANDEVICEQT_H_JYBV8GIQ */
