#include "candevice.h"
#include "candevice_p.h"
#include <QtCore/QQueue>

#include "canfactory.hpp"

CanDevice::CanDevice(CanFactoryInterface& factory)
    : d_ptr(new CanDevicePrivate)
    , mFactory(factory)
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
    d->canDevice.reset(mFactory.create(backend, interface));

    if (!d->canDevice) {
        return false;
    }

    d->canDevice->framesWritten(std::bind(&CanDevice::framesWritten, this, std::placeholders::_1));
    d->canDevice->framesReceived(std::bind(&CanDevice::framesReceived, this));
    d->canDevice->errorOccurred(std::bind(&CanDevice::errorOccurred, this, std::placeholders::_1));

    // connect

    return true;
}

void CanDevice::sendFrame(const QCanBusFrame& frame, const QVariant& context)
{
    Q_D(CanDevice);
    bool status = false;

    if (!d->canDevice) {
        return;
    }

    // Success will be reported in framesWritten signal.
    // Sending may be buffered. Keep correlation between sending results and frame/context
    d->mSendQueue.push_back({ frame, context });

    status = d->canDevice->writeFrame(frame);

    if (!status) {
        emit frameSent(status, frame, context);
        d->mSendQueue.takeFirst();
    }
}

bool CanDevice::start()
{
    Q_D(CanDevice);

    if(!d->canDevice) {
        return false;
    }

    return d->canDevice->connectDevice();

}

void CanDevice::framesReceived()
{
    Q_D(CanDevice);

    while (static_cast<bool>(d->canDevice->framesAvailable())) {
        const QCanBusFrame frame = d->canDevice->readFrame();
        emit frameReceived(frame);
    }
}

void CanDevice::framesWritten(qint64)
{
    Q_D(CanDevice);

    if (!d->mSendQueue.isEmpty()) {
        auto sendItem = d->mSendQueue.takeFirst();
        emit frameSent(true, sendItem.first, sendItem.second);
    }
}

void CanDevice::errorOccurred(int error)
{
    Q_D(CanDevice);

    if (error == QCanBusDevice::WriteError && !d->mSendQueue.isEmpty()) {
        auto sendItem = d->mSendQueue.takeFirst();
        emit frameSent(false, sendItem.first, sendItem.second);
    }
}
