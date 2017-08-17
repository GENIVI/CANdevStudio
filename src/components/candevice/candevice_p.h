#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include "candeviceinterface.hpp"
#include "canfactoryqt.hpp"
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusFrame>
#include <memory>

class CanDevicePrivate {
public:
    CanDevicePrivate()
        : CanDevicePrivate(mDefFactory)
    {
    }

    CanDevicePrivate(CanFactoryInterface& factory)
        : mFactory(factory)
    {
    }

    QString mBackend;
    QString mInterface;
    QVector<QCanBusFrame> mSendQueue;
    std::unique_ptr<CanDeviceInterface> canDevice;
    CanFactoryInterface& mFactory;

private:
    CanFactoryQt mDefFactory;
};

#endif /* !__CANDEVICE_P_H */
