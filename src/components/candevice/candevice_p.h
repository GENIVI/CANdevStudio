#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include "candeviceqt.hpp"
#include <QtCore/QVector>

class CanDevicePrivate {
public:
    CanDevicePrivate(CanDeviceCtx&& ctx = CanDeviceCtx(new CanDeviceQt))
        : _ctx(std::move(ctx))
        , _canDevice(_ctx.get<CanDeviceInterface>())
    {
    }

    CanDeviceCtx _ctx;
    QVector<QCanBusFrame> _sendQueue;
    CanDeviceInterface& _canDevice;
    bool _initialized{ false };
};

#endif /* !__CANDEVICE_P_H */
