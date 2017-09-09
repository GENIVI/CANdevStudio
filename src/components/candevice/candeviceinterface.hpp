#ifndef CANDEVICEINTERFACE_HPP_DNXOI7PW
#define CANDEVICEINTERFACE_HPP_DNXOI7PW

#include <QtCore/QtGlobal>
#include <QtSerialBus/QCanBusFrame>
#include <functional>

struct CanDeviceInterface {
    virtual ~CanDeviceInterface()
    {
    }

    typedef std::function<void(qint64)> framesWritten_t;
    typedef std::function<void()> framesReceived_t;
    typedef std::function<void(int)> errorOccurred_t;

    virtual void setFramesWrittenCbk(const framesWritten_t& cb) = 0;
    virtual void setFramesReceivedCbk(const framesReceived_t& cb) = 0;
    virtual void setErrorOccurredCbk(const errorOccurred_t& cb) = 0;

    virtual bool init(const QString& backend, const QString& iface) = 0;
    virtual bool writeFrame(const QCanBusFrame& frame) = 0;
    virtual bool connectDevice() = 0;
    virtual qint64 framesAvailable() = 0;

    virtual QCanBusFrame readFrame() = 0;
};

#endif /* end of include guard: CANDEVICEINTERFACE_HPP_DNXOI7PW */
