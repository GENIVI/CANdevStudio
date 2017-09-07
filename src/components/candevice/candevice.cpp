#include "candevice.h"
#include "candevice_p.h"
#include "canfactory.hpp"
#include <QtCore/QQueue>

CanDevice::CanDevice(CanDeviceCtx *ctx)
    : d_ptr(new CanDevicePrivate(ctx))
{
}

CanDevice::~CanDevice()
{
}

bool CanDevice::init(const QString& backend, const QString& interface)
{
    Q_D(CanDevice);
    QString errorString;

    d->mBackend = backend;
    d->mInterface = interface;
    d->canDevice.init(backend, interface);

    d->canDevice.setFramesWrittenCbk(std::bind(&CanDevice::framesWritten, this, std::placeholders::_1));
    d->canDevice.setFramesReceivedCbk(std::bind(&CanDevice::framesReceived, this));
    d->canDevice.setErrorOccurredCbk(std::bind(&CanDevice::errorOccurred, this, std::placeholders::_1));

    initialized = true;

    return true;
}

void CanDevice::sendFrame(const QCanBusFrame& frame)
{
    Q_D(CanDevice);
    bool status = false;

    if (!initialized) {
        return;
    }

    // Success will be reported in framesWritten signal.
    // Sending may be buffered. Keep correlation between sending results and frame/context
    d->mSendQueue.push_back(frame);

    status = d->canDevice.writeFrame(frame);

    if (!status) {
        emit frameSent(status, frame);
        d->mSendQueue.takeFirst();
    }
}

bool CanDevice::start()
{
    Q_D(CanDevice);

    if (!initialized) {
        return false;
    }

    return d->canDevice.connectDevice();
}

void CanDevice::framesReceived()
{
    if (!initialized) {
        return;
    }
    Q_D(CanDevice);

    while (static_cast<bool>(d->canDevice.framesAvailable())) {
        const QCanBusFrame frame = d->canDevice.readFrame();
        emit frameReceived(frame);
    }
}

void CanDevice::framesWritten(qint64)
{
    Q_D(CanDevice);

    if (!d->mSendQueue.isEmpty()) {
        auto sendItem = d->mSendQueue.takeFirst();
        emit frameSent(true, sendItem);
    }
}

void CanDevice::errorOccurred(int error)
{
    Q_D(CanDevice);

    if (error == QCanBusDevice::WriteError && !d->mSendQueue.isEmpty()) {
        auto sendItem = d->mSendQueue.takeFirst();
        emit frameSent(false, sendItem);
    }
}
