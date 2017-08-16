#include "candevice.h"
#include "candevice_p.h"
#include <QtCore/QQueue>

#include "candeviceinterface.hpp"

CanDevice::CanDevice(CanDeviceInterface& canDevice)
    : d_ptr(new CanDevicePrivate)
    , canDevice_(canDevice)
{
    canDevice_.setFramesWrittenCbk(std::bind(&CanDevice::framesWritten, this, std::placeholders::_1));
    canDevice_.setFramesReceivedCbk(std::bind(&CanDevice::framesReceived, this));
    canDevice_.setErrorOccurredCbk(std::bind(&CanDevice::errorOccurred, this, std::placeholders::_1));
}

CanDevice::~CanDevice() {}

void CanDevice::sendFrame(const QCanBusFrame& frame)
{
    Q_D(CanDevice);
    bool status = false;

    // Success will be reported in framesWritten signal.
    // Sending may be buffered. Keep correlation between sending results and frame/context
    d->mSendQueue.push_back(frame);

    status = canDevice_.writeFrame(frame);

    if (!status) {
        emit frameSent(status, frame);
        d->mSendQueue.takeFirst();
    }
}

bool CanDevice::start() { return canDevice_.connectDevice(); }

void CanDevice::framesReceived()
{
    while (static_cast<bool>(canDevice_.framesAvailable())) {
        const QCanBusFrame frame = canDevice_.readFrame();
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
