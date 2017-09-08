#include "candevice.h"
#include "candevice_p.h"
#include <QtCore/QQueue>

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

bool CanDevice::init(const QString& backend, const QString& interface)
{
    Q_D(CanDevice);
    QString errorString;

    d->_initialized = false;

    if (d->_canDevice.init(backend, interface)) {
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

bool CanDevice::start()
{
    Q_D(CanDevice);

    if (!d->_initialized) {
        return false;
    }

    return d->_canDevice.connectDevice();
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
