#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include "candeviceqt.h"
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

    const QString _backendProperty = "backend";
    const QString _interfaceProperty = "interface";

    ComponentInterface::ComponentProperties _supportedProps = {
            {_backendProperty,   {QVariant::String, true}},
            {_interfaceProperty, {QVariant::String, true}}
    };

    std::map<QString, QVariant> _props;

};

#endif /* !__CANDEVICE_P_H */
