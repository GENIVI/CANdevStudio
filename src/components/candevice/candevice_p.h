#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include "candeviceqt.hpp"
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusFrame>
#include <memory>

class CanDevicePrivate {
public:
    CanDevicePrivate(CanDeviceCtx *ctx = new CanDeviceCtx(new CanDeviceQt))
        : canDevice(ctx->get<CanDeviceInterface>())
        , _ctx(ctx)
    {
    }

    QString mBackend;
    QString mInterface;
    QVector<QCanBusFrame> mSendQueue;
    CanDeviceInterface &canDevice;

private:
    std::unique_ptr<CanDeviceCtx> _ctx;
};

#endif /* !__CANDEVICE_P_H */
