#ifndef CANDEVICEINTERFACE_HPP_DNXOI7PW
#define CANDEVICEINTERFACE_HPP_DNXOI7PW

#include <functional>
#include <QtCore/QtGlobal>
#include <QtSerialBus/QCanBusFrame>

struct CanDeviceInterface {
    virtual ~CanDeviceInterface() {}

    typedef std::function<void(qint64)> framesWritten_t;
    typedef std::function<void()> framesReceived_t;
    typedef std::function<void(int)> errorOccurred_t;

    virtual void framesWritten(const framesWritten_t& cb) = 0;
    virtual void framesReceived(const framesReceived_t& cb) = 0;
    virtual void errorOccurred(const errorOccurred_t& cb) = 0;

    virtual bool writeFrame(const QCanBusFrame& frame) = 0;
    virtual bool connectDevice() = 0;
    virtual qint64 framesAvailable() = 0;

    virtual QCanBusFrame readFrame() noexcept = 0;
};

#endif /* end of include guard: CANDEVICEINTERFACE_HPP_DNXOI7PW */
