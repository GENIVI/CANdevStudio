#include "candevice.h"
#include "candevice_p.h"
#include <QtCore/QQueue>

CanDevice::CanDevice()
    : d_ptr(new CanDevicePrivate)
{
}

CanDevice::~CanDevice()
{
}

bool CanDevice::init(const QString& backend, const QString& iface)
{
    Q_D(CanDevice);
    QString errorString;

    d->mBackend = backend;
    d->mInterface = iface;
    d->mDevice.reset(QCanBus::instance()->createDevice(backend, iface, &errorString));

    if (!d->mDevice) {
        return false;
    }

    connect(d->mDevice.data(), &QCanBusDevice::framesWritten, this, &CanDevice::framesWritten);
    connect(d->mDevice.data(), &QCanBusDevice::framesReceived, this, &CanDevice::framesReceived);
    connect(d->mDevice.data(), &QCanBusDevice::errorOccurred, this, &CanDevice::errorOccurred);

    return true;
}

void CanDevice::sendFrame(const QCanBusFrame& frame, const QVariant& context)
{
    Q_D(CanDevice);
    bool status = false;

    if (!d->mDevice) {
        return;
    }

    // Success will be reported in framesWritten signal.
    // Sending may be buffered. Keep correlation between sending results and frame/context
    d->mSendQueue.push_back({ frame, context });

    status = d->mDevice->writeFrame(frame);

    if (!status) {
        emit frameSent(status, frame, context);
        d->mSendQueue.takeFirst();
    }
}

bool CanDevice::start()
{
    Q_D(CanDevice);

    if (d->mDevice) {
        return d->mDevice->connectDevice();
    }

    return false;
}

void CanDevice::framesReceived()
{
    Q_D(CanDevice);

    while (static_cast<bool>(d->mDevice->framesAvailable())) {
        const QCanBusFrame frame = d->mDevice->readFrame();

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

void CanDevice::errorOccurred(QCanBusDevice::CanBusError error)
{
    Q_D(CanDevice);

    if (error == QCanBusDevice::WriteError && !d->mSendQueue.isEmpty()) {
        auto sendItem = d->mSendQueue.takeFirst();
        emit frameSent(false, sendItem.first, sendItem.second);
    }
}
